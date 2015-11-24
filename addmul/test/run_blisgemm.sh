#!/bin/bash
export DYLD_LIBRARY_PATH=/opt/intel/lib:/opt/intel/mkl/lib

#m=4097
#n=4097
#r=10
#kmax=600
#
#echo "run_blisgemm=["
#for (( k=4; k<kmax; k+=31 ))
#do
#  ./test_dblisgemm.x     $m $n $k $r
#  ./test_dblisgemm_stl.x $m $n $k $r
#  ./test_sblisgemm.x     $m $n $k $r
#  ./test_sblisgemm_stl.x $m $n $k $r
#done
#echo "];"

#m=16
#n=16
#r=10
#k=8
#./test_dblisgemm.x     $m $n $k $r

#m=2048
#n=2048
#r=2048
#k=2048
#./test_dblisgemm.x     $m $n $k $r


#m=4097
#n=4097
#k=4097
#./test_dblisgemm.x     $m $n $k
#
#
#
#
#m=4098
#n=4098
#k=4098
#./test_dblisgemm.x     $m $n $k



#m=4
#n=4
#k=4
#./test_dblisgemm.x     $m $n $k

#m=500
#n=500
#k=500
#./test_dblisgemm.x     $m $n $k

#m=1000
#n=1000
#k=1000
#./test_dblisgemm.x     $m $n $k
#
##m=1500
##n=1500
##k=1500
##./test_dblisgemm.x     $m $n $k
#
#m=2000
#n=2000
#k=2000
#./test_dblisgemm.x     $m $n $k
#
#m=3000
#n=3000
#k=3000
#./test_dblisgemm.x     $m $n $k
#
#m=4000
#n=4000
#k=4000
#./test_dblisgemm.x     $m $n $k


#for (( size=1024; size<9000; size+=1024 ))
#do
#  ./test_dblisgemm.x     $size $size $size
#done


#for (( size=1000; size<9000; size+=1000 ))
#do
#  ./test_dblisgemm.x     $size $size $size
#done

#for (( size=512; size<4500; size+=512 ))
#do
#  ./test_dblisgemm.x     $size $size $size
#done

#size=1536
#./test_dblisgemm.x     $size $size $size

m=4096
n=4096
#for (( k=4; k<1024; k+=32 ))
for (( k=4; k<1030; k+=16 ))
#for (( k=16; k<1030; k+=16 ))
do
  ./test_dblisgemm.x $m $n $k
done

#k=1028
#./test_dblisgemm.x $m $n $k


