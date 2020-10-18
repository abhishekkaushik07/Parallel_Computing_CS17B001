#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define rows 5               /* number of rows=cols in matrix A */

#define MASTER 0               /* rank of master processor */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */

int main (int argc, char *argv[])
{
   int	np;             /* number of tasks in partition */
	int rank;                  /* a processor identifier */
	int numworkers,            /* number of worker tasks except master = np-1 */
	source_send,                /* task id of message source */
	worker,                  /* task id of message destination */
	mtype,                 /* message type */
	averow, offset, /* used to determine rows sent to each worker */
	i, j, k, rc;           /* misc */
   int	Ak[rows][rows],           /* matrix Ak */
   	A[rows][rows];           /* matrix A is adjacency matrix */ 
   MPI_Status status;

   int length = 1;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&np);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   
   
   
   numworkers = np-1;  // number of worker tasks except master = np-1


   // A is adjacency matrix
   for (i=0; i<rows; i++)
   {
      for (j=0; j<rows; j++)
      {
         
         if(i==j)
         {
            A[i][i] = 0;
            continue;
         }
         A[i][j] = rand()%2;
         A[j][i] = A[i][j];
      }
      
   }

   int src = 0; // source node in graph
   int dest = 3; // destination node in graph

   int shortest_dist = A[src][dest]; // this and length has to be sent to each processes





   if (rank == MASTER)  // Ak is changed in master only
   {
     

      // Set Ak = A
      for (i=0; i<rows; i++)
      {
         for (j=0; j<rows; j++)
         {
            Ak[i][j]= A[i][j]; 
         }
      }

      printf("Adjacency matrix A \n\n");

      

      for(i=0;i<rows;i++)
      {
         for(j=0;j<rows;j++)
         {
            printf("%d ",A[i][j]);
         }
         printf("\n");
      }

      printf("\n\n");

      while(Ak[src][dest] == 0 && length != rows)
      {

         
         averow = rows/(numworkers+1);
         offset = 0;
         mtype = FROM_MASTER;

         for (worker=1; worker<=numworkers; worker++) /* Send matrix data to the worker tasks */
         {
             
            MPI_Send(&offset, 1, MPI_INT, worker, mtype, MPI_COMM_WORLD);
            MPI_Send(&averow, 1, MPI_INT, worker, mtype, MPI_COMM_WORLD);
            MPI_Send(&Ak[offset][0], averow*rows, MPI_INT, worker, mtype,MPI_COMM_WORLD);
            offset = offset + averow;
         }

         // Remaining multiplication is done by Master(that is rank 0 in this case).
         for (int cc=averow*(numworkers); cc<rows; cc++)
         {
            for (int dd=0; dd<rows; dd++)
            {
               int val = 0;
               for (int kk=0; kk<rows; kk++)
               {
                  val += Ak[cc][kk] * A[kk][dd];
               }

               Ak[cc][dd] = val;
            }
         }
            

         /* Receive results from worker tasks */
         mtype = FROM_WORKER;
         for (i=1; i<=numworkers; i++)
         {
            source_send = i;
            MPI_Recv(&offset, 1, MPI_INT, source_send, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&averow, 1, MPI_INT, source_send, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&Ak[offset][0], averow*rows, MPI_INT, source_send, mtype, 
                     MPI_COMM_WORLD, &status);
            
         }

        
         length+=1;
         shortest_dist = Ak[src][dest];
         
         // Send the latest length , Ak[src][dst] value as shortest distance to all processes 
         for (worker=1; worker<=numworkers; worker++)
         {
            MPI_Send(&shortest_dist, 1, MPI_INT, worker, mtype, MPI_COMM_WORLD);
            MPI_Send(&length, 1, MPI_INT, worker, mtype, MPI_COMM_WORLD);
         }


      }

      /* Print results */
      if(length == rows)
      {
         printf("No path between the source:%d and destination:%d\n",src,dest);
      }
      else printf("\nShortest distance from src: %d to dest: %d is %d\n",src,dest,length);



   }
   else if (rank != MASTER) //Master is 0
   {
      while(length != rows && shortest_dist == 0)
      {

         mtype = FROM_MASTER;
         MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&averow, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&Ak, averow*rows, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        
         for (k=0; k<rows; k++)
         {
            for (i=0; i<averow; i++)
            {
               int val = 0;
               for (j=0; j<rows; j++)
               {
                  val +=  Ak[i][j] * A[j][k];
               }

               Ak[i][k] = val;
            }
         }
            
         mtype = FROM_WORKER;
         MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
         MPI_Send(&averow, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
         MPI_Send(&Ak, averow*rows, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);

         MPI_Recv(&shortest_dist, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
         MPI_Recv(&length, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);

        

      }


   }


   MPI_Finalize();





}

// Reference program to multiply two matrix taken from
// https://computing.llnl.gov/tutorials/mpi/samples/C/mpi_mm.c


// Algorithm used from shortest_path.py