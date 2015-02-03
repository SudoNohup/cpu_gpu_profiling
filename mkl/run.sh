for size in $(seq 32 32 512)
do
  #echo $devId
  #echo $size
  #nvcc -DSIZE=size dgemm.c -o dgemm -lcublas
  #./test_dgemm_mkl.x $size
  #./test_dsyrk_mkl.x $size
  #./test_dtrsm_mkl.x $size
  ./test_dpotrf_mkl.x $size
done
