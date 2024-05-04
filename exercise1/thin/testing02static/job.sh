#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="MPI_weak"
#SBATCH --partition=THIN
#SBATCH -N 1
#SBATCH -n 12
#SBATCH --exclusive
#SBATCH --time=02:00:00

module load openMPI/4.1.5/gnu/12.2.1 
policy=close
export OMP_PLACES=cores
export OMP_PROC_BIND=$policy

loc=$(pwd)
cd ../..
make par location=$loc
cd $loc

mpirun -np 1 -N 1 --map-by socket main.x -i -f "starting.pgm" -k 4
mpirun -np 1 -N 1 --map-by socket main.x -r -n 3 -e 1 -s 1 -f "starting.pgm" -k 4


cd ../..
# make clean location=$loc
module purge
cd $loc