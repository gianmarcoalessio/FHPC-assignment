#include <mpi.h>
#include <omp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "read_write_pgm.h"

#define MAXVAL 255

void update_cell_ordered(unsigned char *top_adjacent_row, unsigned char *bottom_adjacent_row, unsigned char *local_playground, int xsize, int ysize, int x, int y)
{

#pragma omp critical 
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
    MPI_Isend(&local_playground[0], xsize, MPI_UNSIGNED_CHAR, top_neighbor, 0, MPI_COMM_WORLD, &reqs[0]);
    MPI_Irecv(bottom_adjacent_row, xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 0, MPI_COMM_WORLD, &reqs[1]);
    MPI_Isend(&local_playground[(my_chunk - 1) * xsize], xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 1, MPI_COMM_WORLD, &reqs[2]);
    MPI_Irecv(top_adjacent_row, xsize, MPI_UNSIGNED_CHAR, top_neighbor, 1, MPI_COMM_WORLD, &reqs[3]);
    MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);

    // A nice idea could be parallelize the update of the next evolution while finish the current one, so as soon the first chunk is updated, the next one is already being updated and so on

    for (int step = 1; step <= n; step++){

        // Ensure that the processes execute in order
        for (int i = 0; i < size; i++) {
            if (i == rank) {       
    #pragma omp parallel for collapse(2)

                for (int y = 0; y < my_chunk; y++)
                {
                    for (int x = 0; x < xsize; x++){
                    #pragma omp ordered
                        {
                            update_cell_ordered((y == 0 ? top_adjacent_row : &local_playground[(y - 1) * xsize]),
                                            (y == my_chunk - 1 ? bottom_adjacent_row : &local_playground[(y + 1) * xsize]),local_playground, xsize, xsize, x, y);
                        }
                    }
                }
            }
        // Ensure that all processes have finished this iteration before moving on to the next one
        MPI_Barrier(MPI_COMM_WORLD);
        }
        
    }

    free(top_adjacent_row);
    free(bottom_adjacent_row);

    if (s != n)
    {
        write_snapshot(local_playground, 255, xsize, my_chunk, "ssnapshot", n, my_offset);
    }
}