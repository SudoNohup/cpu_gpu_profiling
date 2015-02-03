for size in $(seq 20 20 1000)
do
  #echo $devId
  #echo $size
  #nvcc -DSIZE=size dgemm.c -o dgemm -lcublas
  ./mkl_sgemm $size
done
