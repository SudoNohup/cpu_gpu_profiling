#export BLIS_DIR=/home/ubuntu/lib/blis
export BLIS_DIR=/home/ubuntu/blis
export OPENBLAS_DIR=/home/ubuntu/lib/openblas

#static: *.a
gcc test_dgemm_openblas.c ${BLIS_DIR}/lib/libblis.a -o test_dgemm_blis.x -fopenmp -lm
#gcc test_dgemm_openblas.c ${OPENBLAS_DIR}/lib/libopenblas.a -o test_dgemm_openblas.x -fopenmp
#gcc test_sgemm_openblas.c ${OPENBLAS_DIR}/lib/libopenblas.a -o test_sgemm_openblas.x -fopenmp

#dynamic: *.so
#gcc -o test_dgemm_blis.x -std=c99 -O2 -L${BLIS_DIR}/lib -I${BLIS_DIR}/include timer.c timer.h blas.h test_dgemm_mkl.c -lpthread -fopenmp -lblis -lm
#gcc -o test_dgemm_openblas.x -std=c99 -O2 -L${OPENBLAS_DIR}/lib -I${OPENBLAS_DIR}/include timer.c timer.h blas.h test_dgemm_mkl.c -lpthread -fopenmp -lopenblas
#gcc -o test_sgemm_openblas.x -std=c99 -O2 -L${OPENBLAS_DIR}/lib -I${OPENBLAS_DIR}/include timer.c timer.h blas.h test_sgemm_mkl.c -lpthread -fopenmp -lopenblas
