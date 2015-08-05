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
  int m, n, k;
  int i, j;
  DATATYPE *A, *B, *C;
  int SIZE = 5;


  char transa = 'n';
  char transb = 'n';

  if (argc <=1 ) {
	printf("exactly 1 arguments\n");
	exit(0);
  }
  SIZE = atoi(argv[1]);
  //printf("devId:%d, SIZE:%d\n", devId, SIZE);

  m = n = k = SIZE;
  A = (DATATYPE *)malloc(m*k*sizeof(DATATYPE));
  B = (DATATYPE *)malloc(k*n*sizeof(DATATYPE));
  C = (DATATYPE *)malloc(m*n*sizeof(DATATYPE));

  for (j = 0; j < k; ++j) {
	for (i = 0; i < m; ++i) {
	  A[IDX2C(i, j, m)] = (DATATYPE)IDX2C(i, j, m);
	}
  }
  for (j = 0; j < n; ++j) {
	for (i = 0; i < k; ++i) {
	  B[IDX2C(i, j, m)] = 1.0;
	}
  }

  for (j = 0; j < n; ++j) {
	for (i = 0; i < m; ++i) {
	  C[IDX2C(i, j, m)] = 0.0;
	}
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
  DATATYPE beta = 0.0;


  struct timeval time;
  double mkl_time;

  int ii;
  int nreps = 3;



  //warm up
  beginTimer(&time);
  dgemm(transa, transb, 
	  m, n, k, 
	  alpha, 
	  A, m, 
	  B, k, 
	  beta, 
	  C, m); 
  mkl_time = endTimer(&time);
  //printf("mkl_time: %g (%g GFLOPS)\n", mkl_time, (2.0*m*n*k* 1e-6) / mkl_time);
  printf("%f\t", (2.0*m*n*k* 1e-6) / mkl_time);
//  printf("%f\t", mkl_time);


  float average_time, fastest_time = 10000000, slowest_time = 0, sum = 0.0;
  for (ii = 0; ii < nreps; ++ii) {
	beginTimer(&time);
	dgemm(transa, transb, 
		m, n, k, 
		alpha, 
		A, m, 
		B, k, 
		beta, 
		C, m); 
	mkl_time = endTimer(&time);
	//printf("mkl_time: %g (%g GFLOPS)\n", mkl_time, (2.0*m*n*k* 1e-6) / mkl_time);
	sum += mkl_time;
	fastest_time = (fastest_time < mkl_time) ? fastest_time : mkl_time;
	slowest_time = (slowest_time > mkl_time) ? slowest_time : mkl_time;
  }
  average_time = sum/nreps;
  printf("%f\t", (2.0*m*n*k* 1e-6)/ fastest_time);
  printf("%f\t", (2.0*m*n*k* 1e-6)/ slowest_time);
  printf("%f\t", (2.0*m*n*k* 1e-6)/ average_time);
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


  free(A);
  free(B);
  free(C);

  return EXIT_SUCCESS;
}
