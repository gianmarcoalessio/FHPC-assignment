
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "mpi.h"
#include <omp.h>
#include <getopt.h>
#include <math.h>
// define inside include file the function prototypes
#include "ordered_evolution.h"
#include "read_write_pgm.h"
#include "static_evolution.h"

struct timeval start_time, end_time;
#define XWIDTH 256
#define YWIDTH 256
#define MAXVAL 255
#define INIT 1
#define RUN 2
#define K_DFLT 100
#define ORDERED 0
#define STATIC 1

/*Each character in the optstring represents a single-character
    option that the program accepts. If a character is followed by a colon (:),
    it indicates that the option requires an argument.
    -i: No argument required. Initialize playground.
    -r: No argument required. Run a playground.
    -k: Requires an argument (e.g., -k 100). Playground size.
    -e: Requires an argument (e.g., -e 1). Evolution type.
    -f: Requires an argument (e.g., -f filename.pgm).
    Name of the file to be either read or written
    -n: Requires an argument (e.g., -n 10000). Number of steps.
    -s: Requires an argument (e.g., -s 1). Frequency of dump.*/

int main(int argc, char **argv)
{

    int action = 0;
    int k = K_DFLT;  // size of the squared  playground
    int e = ORDERED; // evolution type [0\1]
    int n = 10;      // number of iterations (default 10000)
    int s = 1;       // every how many steps a dump of the system is saved on a file
                     // 0 meaning only at the end.
    char *fname = NULL;
    char *optstring = "irk:e:f:n:s:";
    int maxval = 255;

    int c;
    /*When the getopt function is called in the while loop,
        it processes the command-line arguments according to
        the format specified in optstring.
        The getopt function returns the next option character or -1 if there are no more options
    */
    while ((c = getopt(argc, argv, optstring)) != -1)
    {
        switch (c)
        {
        case 'i':
            action = INIT;
            break;
        case 'r':
            action = RUN;
            break;
        case 'k':
            k = atoi(optarg);
            break;
        case 'e':
            e = atoi(optarg);
            break;
        case 'f':
            fname = (char *)malloc(sizeof(optarg) + 1);
            sprintf(fname, "%s", optarg);
            break;
        case 'n':
            n = atoi(optarg);
            break;
        case 's':
            s = atoi(optarg);
            break;
        default:
            printf("argument -%c not known\n", c);
            break;
        }
    }

    if (action == INIT)
    {
        void *ptr = init_playground(k, k);
        write_pgm_image(ptr, 255, k, k, fname);
    }

    if (action == RUN)
    {

        double mean_time;
        double time_elapsed;
        MPI_Init(NULL, NULL);
        int rank;
        int size;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        int color_depth = 1 + (MAXVAL > 255);

        // int chunk_size = k / num_procs;
        // int rest = k % num_procs;

        // int start = rank * chunk_size + ((rank < rest) ? rank : rest);
        // int end = start + chunk_size + (rank < rest);

        int chunk = k / size; // chuck is the number of rows processed in parallel
        int rest = k % size;

        int rank_index = rank * chunk + (rank < rest ? rank : rest);
        int processed_chunck = chunk + (rank < rest); // consider the rest of the division, this is the real chunck processed by the ranked process

        // int processed_bytes = processed_chunck * k * color_depth;
        int processed_bytes = processed_chunck * k * color_depth;
        int header_size = 23;
        // int my_offset = header_size + rank_index * k * color_depth;
        int my_offset = header_size + rank_index * k;

        if (e == ORDERED)
        {
            unsigned char *playground_o = (unsigned char *)malloc(processed_bytes * sizeof(unsigned char));
            read_pgm_image((void **)&playground_o, &maxval, &k, &k, fname);

            // PRINTING
            // printf("Playground reading for %s:\n", fname);
            // for (int i = 0; i < k; i++)
            // {
            //     for (int j = 0; j < k; j++)
            //     {
            //         printf("%d ", playground_o[i * k + j] == MAXVAL ? 1 : 0);
            //     }
            //     printf("\n");
            // }

            if (s >= 0)
            {

                gettimeofday(&start_time, NULL);
                // Use ternary operator to decide the last argument
                ordered_evolution(playground_o, k, processed_chunck, my_offset, n, s > 0 ? s : n);

                MPI_Finalize();

                gettimeofday(&end_time, NULL);

                time_elapsed = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1e6;
                mean_time = time_elapsed / n;

                if (rank == 0)
                {
                    FILE *fp = fopen("timing.csv", "a");
                    fprintf(fp, "%f,", mean_time);
                    fclose(fp);
                    free(playground_o);
                }
            }
            else
            {
                printf("Error!");
            }
        }
        else if (e == STATIC)
        {
            unsigned char *playground_s = (unsigned char *)malloc(processed_bytes * sizeof(unsigned char));

            read_pgm_image((void **)&playground_s, &maxval, &k, &k, fname);

            // PRINTING
            // printf("Playground reading for %s:\n", fname);
            // for (int i = 0; i < k; i++)
            // {
            //     for (int j = 0; j < k; j++)
            //     {
            //         printf("%d ", playground_s[i * k + j] == MAXVAL ? 1 : 0);
            //     }
            //     printf("\n");
            // }

            if (s >= 0)
            {

                gettimeofday(&start_time, NULL);

                // Use ternary operator to decide the last argument
                static_evolution(playground_s, k, processed_chunck, my_offset, n, s > 0 ? s : n);

                MPI_Finalize();
                gettimeofday(&end_time, NULL);

                time_elapsed = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1e6;

                mean_time = time_elapsed / n;

                if (rank == 0)
                {
                    FILE *fp = fopen("timing.csv", "a");
                    fprintf(fp, "%f,", mean_time);
                    fclose(fp);
                    free(playground_s);
                }
            }
            else
            {
                printf("Error!");
            }
        }

        if (fname != NULL)
        {
            free(fname);
        }
        return 0;
    }
}