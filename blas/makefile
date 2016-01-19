#CC        = gcc
CC        = icc
OPTS      = -std=c99 -O3 

all: test_dgemm_mkl.x test_dsyrk_mkl.x test_dtrsm_mkl.x test_dpotrf_mkl.x test_daxpy_mkl.x test_saxpy_mkl.x

BASEHD = timer.h blas.h
BASESRC = timer.c

#OPENBLAS
#OPENBLAS_DIR = /home/jianyu/lib/openblas
#LIB       = -lopenblas -lpthread -fopenmp
#LIBDIR   += -L${OPENBLAS_DIR}/lib -I${OPENBLAS_DIR}/include
#BASEOBJ = $(BASESRC:.c=.o) $(BASEHD:.c=.o) #/home/jianyu/lib/openblas/lib/libopenblas.a

#BLIS
#LIBDIR   += -L/home/jianyu/Work/Github/blis/lib/haswell 
#LIB       = -lblis -lpthread -openmp
#BASEOBJ = $(BASESRC:.c=.o) $(BASEHD:.c=.o) /home/jianyu/lib/blis/lib/libblis.a

#MKL
MKL_SERIAL   = -lmkl_sequential -lmkl_core -lmkl_intel_lp64
MKL_PARALLEL = -lmkl_intel_thread -lmkl_core -lmkl_intel_lp64
LIB       = ${MKL_SERIAL} -lpthread -lm
#LIB       = ${MKL_PARALLEL} -lpthread -openmp
LIBDIR   += -L${MKLROOT}/lib/intel64 -I${MKLROOT}/include
BASEOBJ = $(BASESRC:.c=.o) $(BASEHD:.c=.o)

%.x: %.o ${BASEOBJ}
	$(CC) -o $@  $(OPTS) $(LIBDIR) $(BASEOBJ) $< $(LIB)

.c.o:
	#echo $(BASEOBJ)
	$(CC) $(OPTS) $(INC) -c $< -o $@

clean:
	rm -f *.x *~ *.o 

.PHONY: clean

#.SUFFIXES: .c .cpp .cu .o
