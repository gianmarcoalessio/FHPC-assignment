#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="ex2"
#SBATCH -n 128
#SBATCH -N 1
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --exclusive
#SBATCH --time=02:00:00

echo "loading modules"

module load architecture/AMD
module load mkl
module load openBLAS/0.3.23-omp
export LD_LIBRARY_PATH=/u/dssc/galess00/final_assignment_FHPC/exercise2/myblis_epyc/lib:$LD_LIBRARY_PATH

location=$(pwd)

cd ../../..
make clean loc=$location
make cpu loc=$location

size=10000

cd $location
policy=spread
arch=EPYC #architecture

export OMP_PLACES=cores
export OMP_PROC_BIND=$policy

# libs=("openblas" "mkl" "blis")
libs=("blis")

for lib in "${libs[@]}"; do
  for prec in float double; do
    file="${lib}_${prec}.csv"
    if [ ! -f $file ]; then
     echo "#cores,time_mean(s),time_sd,GFLOPS_mean,GFLOPS_sd" > $file
    fi
  done
done

for cores in $(seq 2 2 128)
do
  export OMP_NUM_THREADS=$cores
  for lib in "${libs[@]}"; do
    for prec in float double; do
      echo -n "${cores}," >> ${lib}_${prec}.csv
      ./${lib}_${prec}.x $size $size $size
    done
  done
done

cd ../../..
make clean loc=$location
module purge
