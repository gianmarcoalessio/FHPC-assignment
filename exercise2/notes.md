# inside the folder myblis
> remember that you need to make sure that everything is linked inside the myblis repository

srun --partition=EPYC --ntasks=1 --account=dssc ./configure --enable-cblas --enable-threading=openmp --prefix=/u/dssc/galess00/final_assignment_FHPC/exercise2/myblis_epyc auto
srun --account=dssc --partition=EPYC --ntasks=1 --cpus-per-task=128 make -j 128 &
make install

srun --partition=THIN --ntasks=1 --account=dssc ./configure --enable-cblas --enable-threading=openmp --prefix=/u/dssc/galess0/final_assignment_FHPC/exercise2/myblis_thin auto
srun --account=dssc --partition=THIN --ntasks=1 --cpus-per-task=24 make -j 24 &
make install

# run a job 

sbatch --account=dssc job.sh

