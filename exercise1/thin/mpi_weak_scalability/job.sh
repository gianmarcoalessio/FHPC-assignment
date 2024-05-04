#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="MPI_weak"
#SBATCH --partition=THIN
#SBATCH -N 3
#SBATCH -n 36
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
for size in 10000 14143 17321 20000 22361 24495
do
	mpirun -np 1 -N 1 --map-by socket main.x -i -f "playground_${size}.pgm" -k $size
done


echo -n "10000, 1," >> $datafile
# mpirun -np 1 -N 3 --map-by socket main.x -r -n 10 -e 0 -s 0 -f "playground_10000.pgm" -k 10000
mpirun -np 1 -N 3 --map-by socket main.x -r -n 100 -e 1 -s 0 -f "playground_10000.pgm" -k 10000
echo >> $datafile # add a new line

echo -n "14143, 2," >> $datafile
# mpirun -np 2 -N 3 --map-by socket main.x -r -n 10 -e 0 -s 0 -f "playground_14143.pgm" -k 14143
mpirun -np 2 -N 3 --map-by socket main.x -r -n 100 -e 1 -s 0 -f "playground_14143.pgm" -k 14143
echo >> $datafile # add a new line

echo -n "17321, 3," >> $datafile
# mpirun -np 3 -N 3 --map-by socket main.x -r -n 10 -e 0 -s 0 -f "playground_17321.pgm" -k 17321
mpirun -np 3 -N 3 --map-by socket main.x -r -n 100 -e 1 -s 0 -f "playground_17321.pgm" -k 17321
echo >> $datafile # add a new line

echo -n "20000, 4," >> $datafile
# mpirun -np 4 -N 3 --map-by socket main.x -r -n 10 -e 0 -s 0 -f "playground_20000.pgm" -k 20000
mpirun -np 4 -N 3 --map-by socket main.x -r -n 100 -e 1 -s 0 -f "playground_20000.pgm" -k 20000
echo >> $datafile # add a new line

echo -n "22361, 5," >> $datafile
# mpirun -np 5 -N 3 --map-by socket main.x -r -n 10 -e 0 -s 0 -f "playground_22361.pgm" -k 22361
mpirun -np 5 -N 3 --map-by socket main.x -r -n 50 -e 1 -s 0 -f "playground_22361.pgm" -k 22361
echo >> $datafile # add a new line

echo -n "24495, 6," >> $datafile
# mpirun -np 6 -N 3 --map-by socket main.x -r -n 10 -e 0 -s 0 -f "playground_24495.pgm" -k 24495
mpirun -np 6 -N 3 --map-by socket main.x -r -n 50 -e 1 -s 0 -f "playground_24495.pgm" -k 24495
echo >> $datafile # add a new line


cd ../..
make clean location=$loc
module purge
cd $loc