# Exercise 1

The Game of Life (GoL) is a simulation where cells on a grid are marked as either ``alive'' or ``dead''. The game starts with an initial arrangement of these cells and evolves over time according to specific rules that determine whether cells live or die. The game does not require player interaction after the initial setup (for more information about the game here the reference \parencite{gol}). 

- Ordered Evolution: Cells are updated one at a time, in a sequential order from left to right, proceeding row by row.
- Static Evolution: The entire grid is evaluated at once, and all cell updates are applied simultaneously after all cells have been assessed.

This directory contains the following:

- `include/`: Contains the header files for the project.
- `media_serial/`: Contains the iterations snapshot obtained from the serial implementation. And also a script in order to cretate a video from the images.
- `src/`: Contains the source files for the project.
  - ordered_evolution.c: Contains the implementation of the ordered evolution algorithm.
  - static_evolution.c: Contains the implementation of the static evolution algorithm.
  - read_write_pgm.c: Contains the implementation of the read and write functions for the pgm format.
- `obj/`: Contains the object files for the project.
- `thin/`: Contains the data collected on the thin partition and contains `job.sh` file, `.csv` files that hold the performance results
  - `mpi_strong_scalability/`: Contains the data collected for the strong scalability test.
  - `mpi_weak_scalability/`: Contains the data collected for the weak scalability test.
  - `omp_scalability/`: Contains the data collected for the OpenMP scalability test.
  - `ordered_omp_scalability/`: Contains the data collected for the ordered OpenMP scalability test.
  - `serial/`: Contains the data collected for the serial implementation.
  - `testing*/`: Contains the data collected for the testing of the different implementations.
- `exclude_giammy.txt`: Contains the files that are excluded in the synchronization with the repository to my computer with bash file `sync_me.sh`.
- `exclude_orfeo.txt`: Contains the files that are excluded in the synchronization with my computer to the repository with bash file `sync_me.sh`.
- `Makefile`: Used to compile the code.
- `main.c`: Contains the main function that parses the input arguments and calls the functions to initialize and run the game.
- `serial.c`: Contains the implementation of the serial version of the game.
  

## How to compile the code

- Compile the code with the following command:
```sh
sbatch job.sh
```
In the job file, the version of MPI being loaded is openMPI/4.1.5/gnu/12.2.1. The program is executed using mpirun. Below are the input parameters:

- -f: This specifies the file name to write/read from (always required)
- -k: This sets the size of the world that you want to initialize
- -s: Determines the number of steps after which you want to save a snapshot of the world (default value is 0, which only saves the output)
- -i: Initializes the playground
- -r: Runs the game
- -n: Sets the number of generations
- -e: Chooses the evolution policy. The options are:
    - 0 for ordered evolution
    - 1 for static evolution
