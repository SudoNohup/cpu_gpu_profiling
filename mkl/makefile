OPTS      = -std=c99 -O3 

MKL_SERIAL= -lmkl_sequential -lmkl_core -lmkl_intel_lp64
MKL_PARALLEL= -lmkl_intel_thread -lmkl_core -lmkl_intel_lp64
#LIB       = ${MKL_SERIAL} -lgfortran -lpthread
LIB       = ${MKL_PARALLEL} -lgfortran -lpthread -openmp

LIBDIR   += -L${MKLROOT}/lib/intel64 -I${MKLROOT}/include


all: test_dgemm_mkl.x test_dsyrk_mkl.x test_dtrsm_mkl.x test_dpotrf_mkl.x

BASEHD = timer.h blas.h
BASESRC = timer.c

BASEOBJ = $(BASESRC:.c=.o)

test_dgemm_mkl.x: $(BASEOBJ) test_dgemm_mkl.o
	icc $(OPTS) $(LIBDIR) $(LIB) $(BASEOBJ) test_dgemm_mkl.o -o test_dgemm_mkl.x

#test_sgemm_mkl.x: $(BASEOBJ) test_sgemm_mkl.o
#	icc $(OPTS) $(LIBDIR) $(LIB) $(BASEOBJ) test_sgemm_mkl.o -o test_sgemm_mkl.x

test_dsyrk_mkl.x: $(BASEOBJ) test_dsyrk_mkl.o
	icc $(OPTS) $(LIBDIR) $(LIB) $(BASEOBJ) test_dsyrk_mkl.o -o test_dsyrk_mkl.x

test_dtrsm_mkl.x: $(BASEOBJ) test_dtrsm_mkl.o
	icc $(OPTS) $(LIBDIR) $(LIB) $(BASEOBJ) test_dtrsm_mkl.o -o test_dtrsm_mkl.x

test_dpotrf_mkl.x: $(BASEOBJ) test_dpotrf_mkl.o
	icc $(OPTS) $(LIBDIR) $(LIB) $(BASEOBJ) test_dpotrf_mkl.o -o test_dpotrf_mkl.x

#.SUFFIXES: .c .cpp .cu .o

%.o : %.c
	echo $(BASEOBJ)
	icc $(OPTS) $(INC) -c $<

clean:
	rm -f *.x *~ *.o 


.PHONY: clean

