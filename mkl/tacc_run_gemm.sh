#!/bin/bash
#SBATCH -J gemm_job
#SBATCH -o gemm_output.txt
#SBATCH -p gpu
#SBATCH -t 00:03:00
#SBATCH -n 1
#SBATCH -N 1
#SBATCH -A TRAINING-HPC 
export OMP_NUM_THREADS=10

ibrun tacc_affinity run_gemm.sh
