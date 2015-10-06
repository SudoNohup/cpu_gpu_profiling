//#include "gemm_device.cuh"

#include "util.h"

#define TILE_SIZE  96
#define CHUNK_SIZE 16
#define BLOCK_SIZE 16

__global__ void kernel_magma_sgemm(const int K, const int M, const int N,
                                  const float* A, const float* B,
                                  float* C) {
                                  //long long* C) {
  int tx = threadIdx.x;
  int ty = threadIdx.y;
  int bx = blockIdx.x;
  int by = blockIdx.y;
  int tid = ty * 16 + tx;
  int tx2 = tid % 32;
  int ty2 = tid / 32;

  volatile __shared__ float as[16][96];
  volatile __shared__ float bs[16][96];

  float cr[6][6];
  float ar;
  float br[6];

  float asr[2][3];
  float bsr[2][3];

  A += ty2 * M + (by * 96 + tx2);
  B += ty2 * N + (bx * 96 + tx2);
  C += (by * 96 + ty) * N + (bx * 96 + tx);

  // Zero C reg
  #pragma unroll
  for (int i = 0; i < 6; ++i)
    #pragma unroll
    for (int j = 0; j < 6; ++j) cr[i][j] = 0.0f;

  // Load A gmem->smem
  #pragma unroll
  for (int i = 0; i < 2; ++i) {
    #pragma unroll
    for (int j = 0; j < 3; ++j) as[i * 8 + ty2][j * 32 + tx2] = A[j * 32];
    A += M * 8;
  }

  // Load B gmem->smem
  #pragma unroll
  for (int i = 0; i < 2; ++i) {
    #pragma unroll
    for (int j = 0; j < 3; ++j) bs[i * 8 + ty2][j * 32 + tx2] = B[j * 32];
    B += N * 8;
  }

  __syncthreads();

  for (int kk = 0; kk < K - 16; kk += 16) {
    // Load A gmen->reg
    #pragma unroll
    for (int i = 0; i < 2; ++i) {
      #pragma unroll
      for (int j = 0; j < 3; ++j) asr[i][j] = A[j * 32];
      A += M * 8;
    }

    // Load B gmem->reg
    #pragma unroll
    for (int i = 0; i < 2; ++i) {
      #pragma unroll
      for (int j = 0; j < 3; ++j) bsr[i][j] = B[j * 32];
      B += N * 8;
    }

    // Compute
    #pragma unroll
    for (int k = 0; k < 16; ++k) {
      // Load B smen->reg
      #pragma unroll
      for (int j = 0; j < 6; ++j) br[j] = bs[k][j * 16 + tx];

      #pragma unroll
      for (int i = 0; i < 6; ++i) {
        ar = as[k][i * 16 + ty];
        #pragma unroll
        for (int j = 0; j < 6; ++j) {
          //float d = ar - br[j];
          //cr[i][j] += d * d;
		  cr[i][j] += ar * br[j];
        }
      }
    }

    __syncthreads();

    // Load A reg->smem
    #pragma unroll
    for (int i = 0; i < 2; ++i)
      #pragma unroll
      for (int j = 0; j < 3; ++j) as[i * 8 + ty2][j * 32 + tx2] = asr[i][j];

    // Load B reg->smem
    #pragma unroll
    for (int i = 0; i < 2; ++i)
      #pragma unroll
      for (int j = 0; j < 3; ++j) bs[i * 8 + ty2][j * 32 + tx2] = bsr[i][j];

    __syncthreads();
  }

  // Compute last 16 dimensions
  #pragma unroll
  for (int k = 0; k < 16; ++k) {
    // Load B smen->reg
    #pragma unroll
    for (int j = 0; j < 6; ++j) br[j] = bs[k][j * 16 + tx];

    #pragma unroll
    for (int i = 0; i < 6; ++i) {
      ar = as[k][i * 16 + ty];
      #pragma unroll
      for (int j = 0; j < 6; ++j) {
        //float d = ar - br[j];
        //cr[i][j] += d * d;
		cr[i][j] += ar * br[j];
      }
    }
  }

  // Store C reg->gmem
  #pragma unroll
  for (int i = 0; i < 6; ++i) {
    #pragma unroll
    for (int j = 0; j < 6; ++j) {

      //long long c = (long long)__float_as_int(cr[i][j]);
      //c = (c << 32) | (bx * 96 + j * 16 + tx);
	  ////float c = cr[i][j];
      //C[j * 16] = c;
      C[j * 16] = cr[i][j];
    }
    C += N * 16;
  }
}


void magma_sgemm(const int padded_num_k,
                      const int padded_num_m,
                      const int padded_num_n,
                      const float* const d_padded_AA,
                      const float* const d_padded_BB,
                      float* const d_padded_CC) {
  assert(padded_num_k % CHUNK_SIZE == 0);
  assert(padded_num_m % TILE_SIZE == 0);
  assert(padded_num_n % TILE_SIZE == 0);

  dim3 block(BLOCK_SIZE, BLOCK_SIZE);
  dim3 grid(padded_num_n / TILE_SIZE, padded_num_m / TILE_SIZE);

  CREATE_AND_START_TIMER;

  kernel_magma_sgemm<<<grid, block>>>
      (padded_num_k, padded_num_m, padded_num_n,
       d_padded_AA, d_padded_BB, d_padded_CC);

  SYNC_AND_CHECK_ERROR;

  STOP_TIMER_AND_CALCULATE_ELAPSED;

  //if (kPrintTime)
  printf("Time: %.3f ms\n", gpu_timer.elapsed());
}

//inline int CeilToMultiple(int x, int f) {
//  assert(x >= 0);
//  assert(f > 0);
//  return (x + f - 1) / f * f;
//}


void magma_sgemm_pad(const int num_k, const int num_m,
                         const int num_n,
                         const float* const AA, const float* const BB,
						 float* const CC) {
  assert(num_k > 0);
  assert(num_m > 0);
  assert(num_n > 0);

  assert(AA != NULL);
  assert(BB != NULL);
  assert(CC != NULL);

  // The reason that 'AA' and 'BB' are padded is kComputeDistances
  // only works for a complete 96 X 96 tile of 'CC' and processes a chunk
  // of 16 dimensions in each iteration.

  const int padded_num_m = CeilToMultiple(num_m, TILE_SIZE);
  const int padded_num_n = CeilToMultiple(num_n, TILE_SIZE);
  const int padded_num_k = CeilToMultiple(num_k, CHUNK_SIZE);

  float* h_padded_AA = new float[padded_num_k * padded_num_m];
  float* h_padded_BB = new float[padded_num_k * padded_num_n];
  float* h_padded_CC = new float[padded_num_m * padded_num_n];

  float* d_padded_AA  = NULL;
  CHECK_ERROR( cudaMalloc((void**)&d_padded_AA, sizeof(float) * padded_num_k * padded_num_m));
  float* d_padded_BB  = NULL;
  CHECK_ERROR( cudaMalloc((void**)&d_padded_BB, sizeof(float) * padded_num_k * padded_num_n));
  float* d_padded_CC = NULL;
  CHECK_ERROR( cudaMalloc((void**)&d_padded_CC, sizeof(float) * padded_num_m * padded_num_n));
//  int* d_knn_index = NULL;
//  CHECK_ERROR(cudaMalloc((void**)&d_knn_index,
//                         sizeof(int) * num_m * num_nearest_neighbor));

//  float* d_knn_distance = NULL;
//  CHECK_ERROR(cudaMalloc((void**)&d_knn_distance,
//                         sizeof(float) * num_m * num_nearest_neighbor));

  memset((void*)h_padded_AA, 0,
         sizeof(float) * padded_num_k * padded_num_m);
  for (int i = 0; i < num_k; ++i)
    memcpy(h_padded_AA + padded_num_m * i, AA + num_m * i,
           sizeof(float) * num_m);

  memset((void*)h_padded_BB, 0,
         sizeof(float) * padded_num_k * padded_num_n);
  for (int i = 0; i < num_k; ++i)
    memcpy(h_padded_BB + padded_num_n * i,
           BB + num_n * i, sizeof(float) * num_n);

  CHECK_ERROR(
      cudaMemcpy(d_padded_AA, h_padded_AA,
                 sizeof(float) * padded_num_k * padded_num_m,
                 cudaMemcpyHostToDevice));
  CHECK_ERROR(
      cudaMemcpy(d_padded_BB, h_padded_BB,
                 sizeof(float) * padded_num_k * padded_num_n,
                 cudaMemcpyHostToDevice));

  magma_sgemm(padded_num_k, padded_num_m, padded_num_n,
                   d_padded_AA, d_padded_BB, d_padded_CC);

//  SortCandidateGroups(num_m, num_n, padded_num_n,
//                      num_nearest_neighbor, d_CC);
//
//  MergeCandidateGroups(num_m, num_n, padded_num_n,
//                       num_nearest_neighbor, d_CC);
//
//  RetrieveResults(num_m, padded_num_n, num_nearest_neighbor,
//                  d_CC, d_knn_index, d_knn_distance);

//  CHECK_ERROR(cudaMemcpy(knn_index, d_knn_index,
//                         sizeof(int) * num_m * num_nearest_neighbor,
//                         cudaMemcpyDeviceToHost));
//  CHECK_ERROR(cudaMemcpy(knn_distance, d_knn_distance,
//                         sizeof(float) * num_m * num_nearest_neighbor,
//                         cudaMemcpyDeviceToHost));


  CHECK_ERROR(cudaMemcpy(h_padded_CC, d_padded_CC,
                         sizeof(float) * padded_num_m * padded_num_n,
                         cudaMemcpyDeviceToHost));

  for (int i = 0; i < num_k; ++i)
    memcpy( CC + num_n * i, h_padded_CC + padded_num_n * i,
            sizeof(float) * num_n);


  delete[] h_padded_AA;
  delete[] h_padded_BB;
  delete[] h_padded_CC;

  CHECK_ERROR(cudaFree(d_padded_AA));
  CHECK_ERROR(cudaFree(d_padded_BB));
  CHECK_ERROR(cudaFree(d_padded_CC));
}




//__global__ void kRetrieveResults(const int pnr, const int nn,
//                                 const long long* const C, int* const idx,
//                                 float* const dist) {
//  int qid = blockIdx.y;
//  int tid = blockIdx.x * blockDim.x + threadIdx.x;
//
//  if (tid >= nn) return;
//
//  long long c = C[qid * pnr + tid];
//  idx[qid * nn + tid] = c & 0xFFFFFFFF;
//  dist[qid * nn + tid] = sqrt(__int_as_float(c >> 32));
//}
