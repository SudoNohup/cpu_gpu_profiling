#!/bin/bash

#for size in $(seq 1024 1024 8192)
#do
#  echo $devId
#  echo $size
#  #nvcc -DSIZE=size dgemm.c -o dgemm -lcublas
#  ./test_dgemm_mkl.x $size
#  #./test_dsyrk_mkl.x $size
#  #./test_dtrsm_mkl.x $size
#  #./test_dpotrf_mkl.x $size
#done

#./test_dgemm_mkl.x 4096
./test_dgemm_mkl.x 4097
