#export OMP_NUM_THREADS=5
#export OMP_NUM_THREADS=4
export OMP_NUM_THREADS=1
#export OMP_NUM_THREADS=8
#./test_dgemm_openblas.x 5000 5000 5000
#./test_dgemm_openblas.x 1000 1000 1000
./test_sgemm_openblas.x 1000 1000 1000
