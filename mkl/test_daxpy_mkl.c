#include <stdio.h>
#include <stdlib.h>
//#include <cuda_runtime.h>
//#include "mkl_v2.h"

#include <sys/time.h>
#include "timer.h"
#include "blas.h"


#define IDX2C(i, j, ld) (((j)*(ld)) + (i))
#define DATATYPE double
//#define CUBLAS_FUNC mklDgemm 
//#define SIZE 1000

int main(int argc, char **argv) {
  int k;
  int i, j;
  DATATYPE *x, *y;
  int SIZE = 5;

  if (argc <= 1 ) {
	printf("exactly 1 arguments\n");
	exit(0);
  }
  SIZE = atoi(argv[1]);
  //printf("devId:%d, SIZE:%d\n", devId, SIZE);

  k = SIZE;
  x = (DATATYPE *)malloc(k*sizeof(DATATYPE));
  y = (DATATYPE *)malloc(k*sizeof(DATATYPE));


  for (j = 0; j < k; ++j) {
	x[j] = (DATATYPE)1.0;
	y[j] = (DATATYPE)1.0;
  }


  /*
  printf("A:\n");
  for (i = 0; i < m; ++i) {
	for (j = 0; j < k; ++j) {
	  printf("%5.0lf", A[IDX2C(i, j, m)]);
	}
	printf("\n");
  }
  printf("B:\n");
  for (i = 0; i < k; ++i) {
	for (j = 0; j < n; ++j) {
	  printf("%5.0lf", B[IDX2C(i, j, m)]);
	}
	printf("\n");
  }
  printf("C:\n");
  for (i = 0; i < m; ++i) {
	for (j = 0; j < n; ++j) {
	  printf("%5.0lf", C[IDX2C(i, j, m)]);
	}
	printf("\n");
  }
  */


  DATATYPE alpha = 1.0;

  struct timeval time;
  double mkl_time;

  int ii;
  int nreps = 1000000;


  int incx = 1;
  int incy = 1;

  //warm up
  beginTimer(&time);
 
  daxpy (k, alpha, x, incx, y, incy);

  mkl_time = endTimer(&time);
  //printf("mkl_time: %g (%g GFLOPS)\n", mkl_time, (2.0*m*n*k* 1e-6) / mkl_time);
  printf("%f\t", (2.0*k* 1e-6) / mkl_time);
//  printf("%f\t", mkl_time);


  float average_time, fastest_time = 10000000, slowest_time = 0, sum = 0.0;
  for (ii = 0; ii < nreps; ++ii) {
	beginTimer(&time);
	daxpy (k, alpha, x, incx, y, incy);
	mkl_time = endTimer(&time);
	//printf("mkl_time: %g (%g GFLOPS)\n", mkl_time, (2.0*m*n*k* 1e-6) / mkl_time);
	sum += mkl_time;
	fastest_time = (fastest_time < mkl_time) ? fastest_time : mkl_time;
	slowest_time = (slowest_time > mkl_time) ? slowest_time : mkl_time;
  }
  average_time = sum/nreps;
  printf("%f\t", (2.0*k* 1e-6)/ fastest_time);
  printf("%f\t", (2.0*k* 1e-6)/ slowest_time);
  printf("%f\t", (2.0*k* 1e-6)/ average_time);
//  printf("%f\t", fastest_time);
//  printf("%f\t", slowest_time);
//  printf("%f\t", average_time);



  printf("\n");

  /*
  printf("C after dgemm:\n");
  for (i = 0; i < m; ++i) {
	for (j = 0; j < n; ++j) {
	  printf("%5.0lf", C[IDX2C(i, j, m)]);
	}
	printf("\n");
  }
  */


  free(x);
  free(y);

  return EXIT_SUCCESS;
}
