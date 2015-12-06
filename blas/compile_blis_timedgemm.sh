#static: *.a
#gcc test_dgemm_openblas.c /home/jianyu/lib/blis/lib/libblis.a -o test_dgemm_blis.x -fopenmp -lm
#gcc test_dgemm_openblas.c /home/jianyu/lib/openblas/lib/libopenblas.a -o test_dgemm_openblas.x -fopenmp

#gcc -o test_dgemm_mkl.x -std=c99 -O2 -L/home/jianyu/lib/openblas/lib -I/home/jianyu/lib/openblas/include timer.c timer.h blas.h /home/jianyu/lib/openblas/lib/libopenblas.a test_dgemm_mkl.c fortran.h -lpthread -fopenmp
#dynamic: *.so
#gcc -o test_dgemm_openblas.x -std=c99 -O2 -L/home/jianyu/lib/openblas/lib -I/home/jianyu/lib/openblas/include timer.c timer.h blas.h test_dgemm_mkl.c -lpthread -fopenmp -lopenblas
gcc -o test_dgemm_blis.x -std=c99 -O2 -L/home/jianyu/lib/blis/lib -I/home/jianyu/lib/blis/include timer.c timer.h blas.h test_dgemm_mkl.c -lpthread -fopenmp -lblis -lm
