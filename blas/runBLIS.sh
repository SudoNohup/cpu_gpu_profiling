export OMP_NUM_THREADS=4
export BLIS_JR_NT=1
export BLIS_IC_NT=4
#export BLIS_IC_NT=1
#./test_dgemm_blis.x 5000 5000 5000
./test_dgemm_blis.x 1000 1000 1000
#./test_dgemm_blis.x 100 100 100
#./test_dgemm_blis.x 5000

