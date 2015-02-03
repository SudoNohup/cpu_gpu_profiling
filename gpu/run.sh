for devId in 1
do
  for size in $(seq 32 32 512)
  do
	#echo $devId
	#echo $size
	#nvcc -DSIZE=size dgemm.c -o dgemm -lcublas
	#echo $devId $size
	#./test_dgemm_cublas.x $devId $size
	#./test_dsyrk_cublas.x $devId $size
	./test_dtrsm_cublas.x $devId $size
  done
done
