#!/bin/bash -l
#
#SBATCH --gres=gpu:a40:1
#SBATCH --time=01:00:00
#SBATCH --export=NONE

unset SLURM_EXPORT_ENV
module load nvhpc
module load cmake
rm -r -f build/
mkdir build/

cd build/
cmake ..
make 

./lbm

mkdir VTU_files
mv *.vtu VTU_files/
cd VTU_files/

