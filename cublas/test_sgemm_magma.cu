#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include "cublas_v2.h"

#include <cuda.h>

//#include <helper_cuda.h>


#include "magma_gemm.cuh"

#define IDX2C(i, j, ld) (((j)*(ld)) + (i))
#define DATATYPE float
#define CUBLAS_FUNC cublasSgemm 
//#define SIZE 1000

cudaError_t checkCuda(cudaError_t result)
{
#if defined(DEBUG) || defined(_DEBUG)
  if (result != cudaSuccess) {
	fprintf(stderr, "CUDA Runtime Error: %s\n", cudaGetErrorString(result));
	assert(result == cudaSuccess);
  }
#endif
  return result;
}

static const char *cublasGetErrorString(cublasStatus_t error) {
  switch (error)
  {
	case CUBLAS_STATUS_SUCCESS:
	  return "CUBLAS_STATUS_SUCCESS";

	case CUBLAS_STATUS_NOT_INITIALIZED:
	  return "CUBLAS_STATUS_NOT_INITIALIZED";

	case CUBLAS_STATUS_ALLOC_FAILED:
	  return "CUBLAS_STATUS_ALLOC_FAILED";

	case CUBLAS_STATUS_INVALID_VALUE:
	  return "CUBLAS_STATUS_INVALID_VALUE";

	case CUBLAS_STATUS_ARCH_MISMATCH:
	  return "CUBLAS_STATUS_ARCH_MISMATCH";

	case CUBLAS_STATUS_MAPPING_ERROR:
	  return "CUBLAS_STATUS_MAPPING_ERROR";

	case CUBLAS_STATUS_EXECUTION_FAILED:
	  return "CUBLAS_STATUS_EXECUTION_FAILED";

	case CUBLAS_STATUS_INTERNAL_ERROR:
	  return "CUBLAS_STATUS_INTERNAL_ERROR";
  }
  return "<unknown>";
}


cublasStatus_t checkCublas(cublasStatus_t result)
{
#if defined(DEBUG) || defined(_DEBUG)
  if (result != CUBLAS_STATUS_SUCCESS) {
	fprintf(stderr, "CUBLAS Runtime Error: %s\n", cublasGetErrorString(result));
	assert(result == cudaSuccess);
  }
#endif
  return result;
}

int main(int argc, char **argv) {
  int m, n, k;
  cudaError_t cudaStat;
  cublasStatus_t stat;
  int i, j;
  DATATYPE *A, *B, *C;
  int SIZE;


  int devId = 1;
  //if (argc > 1) devId = atoi(argv[1]);
  if (argc <=2 ) {
	printf("exactly 2 arguments\n");
	exit(0);
  }
  devId = atoi(argv[1]);
  SIZE = atoi(argv[2]);
  //printf("devId:%d, SIZE:%d\n", devId, SIZE);

  struct cudaDeviceProp prop;
  checkCuda( cudaGetDeviceProperties(&prop, devId));
  //printf("Device : %s\n", prop.name);
  checkCuda( cudaSetDevice(devId) );

  m = n = k = SIZE;
  /*
  A = (DATATYPE *)malloc(m*k*sizeof(DATATYPE));
  B = (DATATYPE *)malloc(k*n*sizeof(DATATYPE));
  C = (DATATYPE *)malloc(m*n*sizeof(DATATYPE));
  */

  checkCuda( cudaMallocHost((void**)&A, m*k*sizeof(DATATYPE)) ); // host pinned
  checkCuda( cudaMallocHost((void**)&B, k*n*sizeof(DATATYPE)) ); // host pinned
  checkCuda( cudaMallocHost((void**)&C, m*n*sizeof(DATATYPE)) ); // host pinned

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


  int ii;
  int nreps = 10;
  DATATYPE *d_A, *d_B, *d_C;
  //events for timing
  cudaEvent_t start, stop, stop1, stop2, stop3;
  cudaEvent_t execStart[nreps+1], execStop[nreps+1];
  checkCuda( cudaEventCreate(&start) );
  checkCuda( cudaEventCreate(&stop) );
  //checkCuda( cudaEventCreate(&stop1) );
  //checkCuda( cudaEventCreate(&stop2) );
  checkCuda( cudaEventCreate(&stop3) );


  cudaEventRecord(start,0);
  checkCuda(cudaMalloc((void**)&d_A, m*k*sizeof(*A)));
  checkCuda(cudaMalloc((void**)&d_B, m*k*sizeof(*B)));
  checkCuda(cudaMalloc((void**)&d_C, m*k*sizeof(*C)));
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);

  float malloc_time;
  cudaEventElapsedTime(&malloc_time, start, stop);
  //printf("Malloc time: %f\n", malloc_time);
  //printf("Malloc bandwidth\t: %f ms (%f GB/s)\n", malloc_time, (8*(m*k + k*n + m*n) * 1e-6)/ malloc_time);
  
  //printf("%f\t", (8*(m*k + k*n + m*n) * 1e-6)/ malloc_time);

  cublasHandle_t handle;
  checkCublas(cublasCreate(&handle));


  cudaEventRecord(start,0);
  checkCuda(cudaMemcpy(d_A, A, m*k*sizeof(*A), cudaMemcpyHostToDevice));
  //cudaEventRecord(stop1,0);
  checkCuda(cudaMemcpy(d_B, B, k*n*sizeof(*B), cudaMemcpyHostToDevice));
  //cudaEventRecord(stop2,0);
  checkCuda(cudaMemcpy(d_C, C, m*n*sizeof(*C), cudaMemcpyHostToDevice));
  cudaEventRecord(stop3,0);
  //checkCublas(cublasSetMatrix(m, k, sizeof(*A), A, m, d_A, m));//A -> d_A
  //checkCublas(cublasSetMatrix(k, n, sizeof(*B), B, k, d_B, k));//B -> d_B
  //checkCublas(cublasSetMatrix(m, n, sizeof(*C), C, m, d_C, m));//C -> d_C
  //cudaEventRecord(stop,0);
  //cudaEventSynchronize(stop1);
  //cudaEventSynchronize(stop2);
  cudaEventSynchronize(stop3);

  float memcpy_h2d_time1, memcpy_h2d_time2, memcpy_h2d_time3;
  //cudaEventElapsedTime(&memcpy_h2d_time1, start, stop1);
  //cudaEventElapsedTime(&memcpy_h2d_time2, start, stop2);
  cudaEventElapsedTime(&memcpy_h2d_time3, start, stop3);
  //printf("Memcpy_h2d_time: %f\n", memcpy_h2d_time);
  //printf(" Memcpy host to device\t: %f ms (%f GB/s)\n", memcpy_h2d_time, (8*(m*k + k*n + m*n) * 1e-6)/ memcpy_h2d_time);
  //printf("%f\t", (8*(m*k + k*n + m*n) * 1e-6)/ memcpy_h2d_time);
  //printf("%f\t", memcpy_h2d_time3);
  //printf("%f\t", memcpy_h2d_time2);
  //printf("%f\t", memcpy_h2d_time1);

  printf("Memcpy host to device\t: %f ms (%f GB/s)\n", memcpy_h2d_time3, (8*(m*k + k*n + m*n) * 1e-6)/ memcpy_h2d_time3);
  //printf"%f\t", (float)8.0 * (m*k + k*n + m*n) * 1e-6 / mem_h2d_time3


  //printf("0\t");

  DATATYPE alpha = 1.0;
  DATATYPE beta = 1.0;


  float cublas_time;


  //warm up

  cudaEventRecord(start,0);
  ////checkCublas(CUBLAS_FUNC(handle, CUBLAS_OP_N, CUBLAS_OP_N, m, n, k, &alpha, d_A, m, d_B, k, &beta, d_C, m));
  
  magma_sgemm(k, m, n, d_A, d_B, d_C);

  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&cublas_time, start, stop);
  //printf("cublas_time: %f\n", cublas_time);
  //printf(" Cublas\t: %f ms (%f GFLOPS)\n", cublas_time, (2.0*m*n*k* 1e-6)/ cublas_time);
  printf(" first-run Cublas GFLOPS\t: %f ms (%f GFLOPS)\n", cublas_time, (2.0*m*n*k* 1e-6)/ cublas_time);
  //printf("first-run GFLOPS:%f\n", (2.0*m*n*k* 1e-6)/ cublas_time);
  //printf("%f\t", cublas_time);


  float average_time, fastest_time = 10000000, slowest_time = 0, sum = 0.0;
  cudaEventRecord(start,0);
  for (ii = 0; ii < nreps; ++ii) {
	//cudaEventRecord(start,0);
	////checkCublas(CUBLAS_FUNC(handle, CUBLAS_OP_N, CUBLAS_OP_N, m, n, k, &alpha, d_A, m, d_B, k, &beta, d_C, m));

	magma_sgemm(k, m, n, d_A, d_B, d_C);

	//cudaEventRecord(stop,0);
	//cudaEventSynchronize(stop);
	//cudaEventElapsedTime(&cublas_time, start, stop);
	//printf("cublas_time: %f\n", cublas_time);
	//printf(" Cublas\t: %f ms (%f GFLOPS)\n", cublas_time, (2.0*m*n*k* 1e-6)/ cublas_time);
	//printf("%f\t", (2.0*m*n*k* 1e-6)/ cublas_time);
	
	//sum += cublas_time;
	//fastest_time = (fastest_time < cublas_time) ? fastest_time : cublas_time;
	//slowest_time = (slowest_time > cublas_time) ? slowest_time : cublas_time;
  }
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&cublas_time, start, stop);

  sum = cublas_time;

  average_time = sum/nreps;
  //printf("%f\t", (2.0*m*n*k* 1e-6)/ fastest_time);
  //printf("%f\t", (2.0*m*n*k* 1e-6)/ slowest_time);
  //printf("hot GFLOPS:%f\n", (2.0*m*n*k* 1e-6)/ average_time);
  printf(" hot-run Cublas GFLOPS\t: %f ms (%f GFLOPS)\n", average_time, (2.0*m*n*k* 1e-6)/ average_time);

  //printf("%f\t", fastest_time);
  //printf("%f\t", slowest_time);
  //printf("%f\t", average_time);


  cudaEventRecord(start,0);
  checkCuda(cudaMemcpy(C, d_C, m*n*sizeof(*C), cudaMemcpyDeviceToHost));
  //checkCublas(cublasGetMatrix(m, n, sizeof(*C), d_C, m, C, m));//d_C -> C
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  float memcpy_d2h_time;
  cudaEventElapsedTime(&memcpy_d2h_time, start, stop);
  //printf("Memcpy_d2h_time: %f\n", memcpy_d2h_time);
  //printf(" Memcpy device to host\t: %f ms (%f GB/s)\n", memcpy_d2h_time, (8*(m*n) * 1e-6)/ memcpy_d2h_time);
  //printf("%f\n", (8*(m*n) * 1e-6)/ memcpy_d2h_time);


  printf("Memcpy device to host\t: %f ms (%f GB/s)\n", memcpy_d2h_time, (8.0*(m*n) * 1e-6)/ memcpy_d2h_time);

  //printf("%f\t", (2.0*m*n*k* 1e-6)/ average_time);

  /*
  printf("C after dgemm:\n");
  for (i = 0; i < m; ++i) {
	for (j = 0; j < n; ++j) {
	  printf("%5.0lf", C[IDX2C(i, j, m)]);
	}
	printf("\n");
  }
  */


  printf("\n");

  cudaFree(d_A);
  cudaFree(d_B);
  cudaFree(d_C);

  cublasDestroy(handle);

  /*
  free(A);
  free(B);
  free(C);
  */
  cudaFreeHost(A);
  cudaFreeHost(B);
  cudaFreeHost(C);



  return EXIT_SUCCESS;
}
