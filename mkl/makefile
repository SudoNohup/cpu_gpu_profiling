CC        = icc
OPTS      = -std=c99 -O3 

MKL_SERIAL= -lmkl_sequential -lmkl_core -lmkl_intel_lp64
MKL_PARALLEL= -lmkl_intel_thread -lmkl_core -lmkl_intel_lp64
#LIB       = ${MKL_SERIAL} -lpthread
LIB       = ${MKL_PARALLEL} -lpthread -openmp

LIBDIR   += -L${MKLROOT}/lib/intel64 -I${MKLROOT}/include

all: test_dgemm_mkl.x test_dsyrk_mkl.x test_dtrsm_mkl.x test_dpotrf_mkl.x test_daxpy_mkl.x test_saxpy_mkl.x

BASEHD = timer.h blas.h
BASESRC = timer.c

BASEOBJ = $(BASESRC:.c=.o) $(BASEHD:.c=.o)

%.x: %.o $(BASEOBJ)
	$(CC) $(OPTS) $(LIBDIR) $(BASEOBJ) $< -o $@ $(LIB)

#.SUFFIXES: .c .cpp .cu .o

%.o : %.c
	$(CC) $(OPTS) $(INC) -c $<

clean:
	rm -f *.x *~ *.o 

#.PHONY: clean

