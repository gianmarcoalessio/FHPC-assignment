#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="strongmpi_45"
#SBATCH --partition=THIN
#SBATCH -N 2
#SBATCH -n 48
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --nodelist=thin[002-003]

module load openMPI/4.1.5/gnu/12.2.1 
policy=close
export OMP_PLACES=cores
export OMP_PROC_BIND=$policy

loc=$(pwd)
cd ../..
make par location=$loc
cd $loc


datafile=$loc/timing_all.csv

if [ ! -f $datafile ]; then
    echo "size, procs, static_mean" > $datafile
fi

## initialize a playground
export OMP_NUM_THREADS=1 #1 thread per core to look at the MPI scalability

for size in 55000 65000
do
mpirun -np 1 -N 1 --map-by socket main.x -i -f "playground_${size}.pgm" -k $size
for procs in 1 $(seq 2 2 48)
do
	echo -n "${size}," >> $datafile
	echo -n "${procs},">> $datafile
	#mpirun -np $procs -N 2 --map-by core main.x -r -f "playground_${size}.pgm" -e 0 -n 3 -s 0 -k $size
	mpirun -np $procs -N 2 --map-by core main.x -r -f "playground_${size}.pgm" -e 1 -n 30 -s 0 -k $size
	echo >> $datafile # add a new line
done
done


cd ../..
make clean location=$loc
module purge
cd $loc