#include <mpi.h>
#include <omp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "read_write_pgm.h"

#define MAXVAL 255

void update_cell_ordered(unsigned char *top_adjacent_row, unsigned char *bottom_adjacent_row, unsigned char *local_playground, int xsize, int ysize, int x, int y)
{
    int max_y = ysize - 1;
    int alive_neighbors = 0;
    int nx, ny = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {

            if (i == 0 && j == 0)
                continue;

            nx = (x + i + xsize) % xsize;
            ny = (y + j + ysize) % ysize;
            unsigned char cell_value;

            if (ny == max_y && y == 0)
            {
                cell_value = top_adjacent_row[nx];
            }
            else if (ny == 0 && y == max_y)
            {
                cell_value = bottom_adjacent_row[nx];
            }
            else
            {
                cell_value = local_playground[ny * xsize + nx];
            }

            alive_neighbors += (cell_value == MAXVAL);
        }
    }

    int cell_index = y * xsize + x;

    local_playground[cell_index] = (((local_playground[cell_index] == MAXVAL) && (alive_neighbors == 2 || alive_neighbors == 3)) || ((local_playground[cell_index] == 0) && alive_neighbors == 3)) ? MAXVAL : 0;
}

void ordered_evolution(unsigned char *local_playground, int xsize, int my_chunk, int my_offset, int n, int s)
{

   int rank, size, nthreads;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // get the total number of processes

    int mpi_order = 0; // Used to manage the ordered evolution of the playground


    unsigned char *top_adjacent_row = (unsigned char *)malloc(xsize * sizeof(unsigned char));
    unsigned char *bottom_adjacent_row = (unsigned char *)malloc(xsize * sizeof(unsigned char));

    int top_neighbor = (rank - 1 + size) % size; // always rank-1 except for when rank==0, where
                                                 // the result of the modulus is size-1 (the last process in rank order is the top neighbor of process 0)

    int bottom_neighbor = (rank + 1) % size; // (rank + 1) / size is always 0, so the modulus will always
                                             // be rank+1, except for when rank == size-1 (maximum) where the modulus is 0 (process 0 is the bottom
                                             // neighbor of the last process)

    MPI_Request reqs[4]; // Array of requests
    MPI_Send(&local_playground[0], xsize, MPI_UNSIGNED_CHAR, top_neighbor, 1, MPI_COMM_WORLD);
    MPI_Recv(bottom_adjacent_row, xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&local_playground[(my_chunk - 1) * xsize], xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 0, MPI_COMM_WORLD);
    MPI_Recv(top_adjacent_row, xsize, MPI_UNSIGNED_CHAR, top_neighbor, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


    for (int step = 1; step <= n; step++)
    {
        while(rank != mpi_order) {
            MPI_Recv(&mpi_order, 1, MPI_INT, top_neighbor, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(top_adjacent_row, xsize, MPI_UNSIGNED_CHAR, top_neighbor, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if (rank == mpi_order) {
            
            
            if(step != 1 || rank == size-1) {
            	MPI_Irecv(bottom_adjacent_row, xsize, MPI_UNSIGNED_CHAR,bottom_neighbor, 5, MPI_COMM_WORLD, &reqs[0]);
            	MPI_Status status;
            	MPI_Wait(&reqs[0], &status);
            }
            
            int nthreads;
            int th_chunk;
            int th_mod;
            int order;

            #pragma omp parallel
            {
                #pragma omp single
                {
                    nthreads = omp_get_num_threads();
                    th_chunk = my_chunk / nthreads;
                    th_mod = my_chunk % nthreads;
                }
            }

            order = 0;
            #pragma omp parallel
            {
                int me = omp_get_thread_num();
                int done = 0;
                int th_my_first = th_chunk * me + ((me < th_mod) ? me : th_mod);
                int th_my_chunk = th_chunk + (th_mod > 0) * (me < th_mod);

                while (!done) {
                    #pragma omp critical
                    {
                        if (order == me) {
                            for (int y = th_my_first; y < th_my_first + th_my_chunk; y++) {
                                for (int x = 0; x < xsize; x++) {
                                    update_cell_ordered(top_adjacent_row, bottom_adjacent_row, local_playground, xsize, my_chunk, x, y);
                                }
                            }
                            order++;
                            done = 1;
                        }
                    }
                }
            }
        }

        if(rank != size-1) {
            mpi_order++;
            MPI_Send(&mpi_order, 1, MPI_INT, bottom_neighbor, 0, MPI_COMM_WORLD);
            MPI_Send(&local_playground[(my_chunk - 1) * xsize], xsize, MPI_UNSIGNED_CHAR, 
             			 bottom_neighbor, 4, MPI_COMM_WORLD);

	    if ( rank == 0 || step != n ) { 
            MPI_Isend(&local_playground[0], xsize, MPI_UNSIGNED_CHAR, top_neighbor, 5, MPI_COMM_WORLD, &reqs[1]);
           } 
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
     
        if(step % s == 0) {

            write_snapshot(local_playground, 255, xsize, my_chunk, "osnapshot", step, offset);
	 }
       
        if(rank == size-1 && step != n) {
            mpi_order = 0;
            MPI_Send(&mpi_order, 1, MPI_INT, bottom_neighbor, 0, MPI_COMM_WORLD); // last process sends to the first one
            MPI_Send(&local_playground[(my_chunk - 1) * xsize], xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 4, MPI_COMM_WORLD);
            MPI_Isend(&local_playground[0], xsize, MPI_UNSIGNED_CHAR, top_neighbor, 5, MPI_COMM_WORLD, &reqs[2]);
        } 
    }

    free(top_adjacent_row);
    free(bottom_adjacent_row);

    if (s != n)
    {
        write_snapshot(local_playground, 255, xsize, my_chunk, "ssnapshot", n, my_offset);
    }
}