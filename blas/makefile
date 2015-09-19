CC        = gcc
OPTS      = -std=c99 -O3 

OPENBLAS_DIR = /home/jianyu/lib/openblas
MKL_SERIAL   = -lmkl_sequential -lmkl_core -lmkl_intel_lp64
MKL_PARALLEL = -lmkl_intel_thread -lmkl_core -lmkl_intel_lp64
#LIB       = ${MKL_SERIAL} -lpthread
#LIB       = ${MKL_PARALLEL} -lpthread -openmp

#LIB       = -lblis -lpthread -openmp
LIB       = -lopenblas -lpthread -fopenmp
#LIB       = -lpthread -fopenmp

#LIBDIR   += -L${MKLROOT}/lib/intel64 -I${MKLROOT}/include
#LIBDIR   += -L/home/jianyu/Work/Github/blis/lib/haswell 
LIBDIR   += -L${OPENBLAS_DIR}/lib -I${OPENBLAS_DIR}/include

all: test_dgemm_mkl.x test_dsyrk_mkl.x test_dtrsm_mkl.x test_dpotrf_mkl.x test_daxpy_mkl.x

BASEHD = timer.h blas.h
BASESRC = timer.c

#BASEOBJ = $(BASESRC:.c=.o) $(BASEHD:.c=.o) /home/jianyu/lib/blis/lib/libblis.a
BASEOBJ = $(BASESRC:.c=.o) $(BASEHD:.c=.o) #/home/jianyu/lib/openblas/lib/libopenblas.a


test_daxpy_mkl.x: $(BASEOBJ) test_daxpy_mkl.o
	$(CC) $(OPTS) $(LIBDIR) $(BASEOBJ) test_daxpy_mkl.o -o test_daxpy_mkl.x $(LIB)

test_dgemm_mkl.x: $(BASEOBJ) test_dgemm_mkl.o
	$(CC) -o test_dgemm_mkl.x $(OPTS) $(LIBDIR) $(BASEOBJ) test_dgemm_mkl.o $(LIB)

#test_sgemm_mkl.x: $(BASEOBJ) test_sgemm_mkl.o
#	$(CC) $(OPTS) $(LIBDIR) $(BASEOBJ) test_sgemm_mkl.o -o test_sgemm_mkl.x $(LIB)

test_dsyrk_mkl.x: $(BASEOBJ) test_dsyrk_mkl.o
	$(CC) $(OPTS) $(LIBDIR) $(BASEOBJ) test_dsyrk_mkl.o -o test_dsyrk_mkl.x $(LIB)

test_dtrsm_mkl.x: $(BASEOBJ) test_dtrsm_mkl.o
	$(CC) $(OPTS) $(LIBDIR) $(BASEOBJ) test_dtrsm_mkl.o -o test_dtrsm_mkl.x $(LIB)

test_dpotrf_mkl.x: $(BASEOBJ) test_dpotrf_mkl.o
	$(CC) $(OPTS) $(LIBDIR) $(BASEOBJ) test_dpotrf_mkl.o -o test_dpotrf_mkl.x $(LIB)

#.SUFFIXES: .c .cpp .cu .o

%.o : %.c
	echo $(BASEOBJ)
	$(CC) $(OPTS) $(INC) -c $<

clean:
	rm -f *.x *~ *.o 


.PHONY: clean

