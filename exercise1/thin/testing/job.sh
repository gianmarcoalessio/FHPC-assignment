#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="MPI_weak"
#SBATCH --partition=THIN
#SBATCH -N 2
#SBATCH -n 24
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

datafile=$loc/timing.csv
echo "size, procs, static_mean" > $datafile

## initialize a playgrounds
export OMP_NUM_THREADS=12
for size in 10000 14143 17321 
do
	mpirun -np 1 -N 1 --map-by socket main.x -i -f "playground_${size}.pgm" -k $size
done

# -np is the number of CPU-socket inside a node, thin and epyc has both 2 sockets maximum, so if we request just one node we would get an error anyway

echo -n "10000, 1," >> $datafile
mpirun -np 1 -N 1 --map-by socket main.x -r -n 10 -e 1 -s 0 -f "playground_10000.pgm" -k 10000
echo >> $datafile # add a new line

echo -n "14143, 2," >> $datafile
mpirun -np 2 -N 1 --map-by socket main.x -r -n 10 -e 1 -s 0 -f "playground_14143.pgm" -k 14143
echo >> $datafile # add a new line

echo -n "17321, 2," >> $datafile
mpirun -np 2 -N 2 --map-by socket main.x -r -n 10 -e 1 -s 0 -f "playground_17321.pgm" -k 17321
echo >> $datafile # add a new line

echo -n "17321, 2," >> $datafile
mpirun -np 2 -N 2 --map-by socket main.x -r -n 10 -e 1 -s 0 -f "playground_17321.pgm" -k 17321
echo >> $datafile # add a new line



cd ../..
make clean location=$loc
module purge
cd $loc