Name :- Abhishek Kaushik

Roll No. :- CS17B001

Problem 1:-

Adjacency matrix can be allocated to a single process. 
This is an MPI based program to find the shortest distance between a given src(0-index based) and a destination. It outputs the shortest distance if it exists otherwise it prints No Path between src and dest.

Working:-

I have taken an Adjacency matrix as A in 1.c that is visible to all processes(np) it can be dumped to a file also that will be visible to all np, for now, I am putting on the terminal.

I made processor 0 as master, it will send the total_rows/np number of rows to different processes and these processes after receiving total_rows/np rows multiply with A(adjacency matrix) and send back to master(rank 0 ) it repeats until the distance from src to dest becomes non-zero or length is not equal to total_rows. In order to implement the while loop in master and non-master processes, I send the length and Ak[src][dest] of master(rank 0) to all other np's(processors).

When it breaks length will be our desired answer. If length is equal to rows than No path exists.

Time Complexity:-

Let N be the total_rows in the Adjacency matrix.

Let K = total_rows/total_processes(np).

Let M is the shortest distance between src and dest.

Complexity = O( K* N^2 * M) (Maximum value of M can be N when there is No path from src to dst)

as in each iteration( 1 to M ):-

  a processor would multiply in O( K * N^2) 
  
  it happens in parallel for np number of processors.

Problem 2:-

Adjacency matrix cannot be allocated to a single process. Everything in this program will be the same as above apart from passing matrix data multiple times. Each process will have to send row and columns to different process many time as adjacency matrix is split across processes.

Time complexity will be the same as of above here if exclude time of passing matrix data across processes.
