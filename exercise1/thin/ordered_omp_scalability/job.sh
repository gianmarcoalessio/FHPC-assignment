#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="openMP_scal"
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

size=25000

datafile=$loc/timing.csv # 3 sockets
echo "threads_per_socket, ordered_mean" > $datafile

mpirun main.x -i -k $size -f "playground.pgm"


for socket in $(seq 2 1 4)
do
for th_socket in $(seq 1 1 12)
do
	export OMP_NUM_THREADS=$th_socket
	echo -n "${th_socket}," >> $datafile
	mpirun -np ${socket} --map-by node --bind-to socket main.x -r -f "playground.pgm" -e 0 -n 10 -s 0 -k $size
	echo >> $datafile # add a new line
done
done


cd ../..
make clean location=$loc
module purge
cd $loc