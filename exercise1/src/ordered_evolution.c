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

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // get the total number of processes

    int local_size = my_chunk * xsize * sizeof(unsigned char);

    unsigned char *top_adjacent_row = (unsigned char *)malloc(xsize * sizeof(unsigned char));
    unsigned char *bottom_adjacent_row = (unsigned char *)malloc(xsize * sizeof(unsigned char));

    int top_neighbor = (rank - 1 + size) % size; // always rank-1 except for when rank==0, where
                                                 // the result of the modulus is size-1 (the last process in rank order is the top neighbor of process 0)

    int bottom_neighbor = (rank + 1) % size; // (rank + 1) / size is always 0, so the modulus will always
                                             // be rank+1, except for when rank == size-1 (maximum) where the modulus is 0 (process 0 is the bottom
                                             // neighbor of the last process)

    for (int step = 1; step <= n; step++)
    {

        MPI_Request request[4];
        MPI_Status status[4];

        // Send top ghost row to previous process and receive from next process
        MPI_Isend(&local_playground[0], xsize, MPI_UNSIGNED_CHAR, top_neighbor, 1, MPI_COMM_WORLD, &request[0]);
        MPI_Irecv(bottom_adjacent_row, xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 1, MPI_COMM_WORLD, &request[1]);
        MPI_Isend(&local_playground[(xsize - 1) * xsize], xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 0, MPI_COMM_WORLD, &request[2]);
        MPI_Irecv(top_adjacent_row, xsize, MPI_UNSIGNED_CHAR, top_neighbor, 0, MPI_COMM_WORLD, &request[3]);

        // Calculate the range of rows to be processed by the current process
        int chunk_size = xsize / size;
        int remainder = xsize % size;
        int start = rank * chunk_size + ((rank < remainder) ? rank : remainder);
        int end = start + chunk_size + (rank < remainder);

        // Start computation that does not depend on the data being communicated
        // #pragma omp parallel for collapse(2)
        for (int i = start; i < end; i++)
        {

            for (int j = 0; j < xsize; j++)
            {

                // if (i != 0 && i != xsize - 1)
                // {
                // temp_local_playground[i * k + j] = upgrade_cell_ordered(i, j, k, local_playground, top_adjacent_row, bottom_adjacent_row);
                update_cell_ordered(top_adjacent_row, bottom_adjacent_row, local_playground, xsize, xsize, j, i);
                // }
            }
        }

        // Wait for the communication to finish
        MPI_Waitall(4, request, status);

        // // Continue with the computation that depends on the data being communicated
        // #pragma omp parallel for collapse(2)
        //         for (int i = 0; i < xsize; i++)
        //         {
        //             for (int j = 0; j < xsize; j++)
        //             {
        //                 if (i == 0 || i == xsize - 1)
        //                 {
        //                     // temp_local_playground[i * k + j] = upgrade_cell_ordered(i, j, k, local_playground, top_adjacent_row, bottom_adjacent_row);
        //                     update_cell_ordered(top_adjacent_row, bottom_adjacent_row, local_playground, xsize, xsize, j, i);
        //                 }
        //             }
        //         }

        if (step % s == 0)
        {
            write_snapshot(local_playground, 255, xsize, my_chunk, "ssnapshot", step, my_offset);
        }
    }

    free(top_adjacent_row);
    free(bottom_adjacent_row);

    if (s != n)
    {
        write_snapshot(local_playground, 255, xsize, my_chunk, "ssnapshot", n, my_offset);
    }
}