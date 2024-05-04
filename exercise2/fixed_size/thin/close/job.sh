#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="ex2"
#SBATCH -n 24
#SBATCH -N 1
#SBATCH --get-user-env
#SBATCH --partition=THIN
#SBATCH --exclusive
#SBATCH --time=02:00:00

module load architecture/Intel
module load mkl
module load openBLAS/0.3.23-omp
export LD_LIBRARY_PATH=/u/dssc/galess00/final_assignment_FHPC/exercise2/myblis_thin/lib:$LD_LIBRARY_PATH

location=$(pwd)

cd ../../..
make clean loc=$location
make cpu loc=$location

size=10000


cd $location
policy=close
arch=THIN #architecture

export OMP_PLACES=cores
export OMP_PROC_BIND=$policy

# Define an array of libraries
# libraries=("mkl" "blis" "openblas")
libraries=("openblas")

for lib in "${libraries[@]}"; do
  for prec in float double; do
    file="${lib}_${prec}.csv"
    echo "#cores,time_mean(s),time_sd,GFLOPS_mean,GFLOPS_sd" > $file
  done
done

export OMP_NUM_THREADS=1
  for lib in "${libraries[@]}"; do
    for prec in float double; do
      echo -n "1," >> ${lib}_${prec}.csv
      ./${lib}_${prec}.x $size $size $size
    done
  done


for cores in $(seq 1 1 24)
do
  export BLIS_NUM_THREADS=$cores
  export OMP_NUM_THREADS=$cores
  for lib in "${libraries[@]}"; do
    for prec in float double; do
      echo -n "${cores}," >> ${lib}_${prec}.csv
      ./${lib}_${prec}.x $size $size $size
    done
  done
done

cd ../../..
make clean loc=$location
module purge
