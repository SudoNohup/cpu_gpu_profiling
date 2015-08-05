#include <x86intrin.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define TIMER_TYPE CLOCK_REALTIME

extern "C" float triad_sse_asm_repeat(float *x, float *y, float *z, const int n, int repeat);
extern "C" float triad_sse_asm_repeat_unroll16(float *x, float *y, float *z, const int n, int repeat);    
extern "C" float triad_avx_asm_repeat(float *x, float *y, float *z, const int n, int repeat);
extern "C" float triad_avx_asm_repeat_unroll16(float *x, float *y, float *z, const int n, int repeat); 
extern "C" float triad_fma_asm_repeat(float *x, float *y, float *z, const int n, int repeat);
extern "C" float triad_fma_asm_repeat_unroll16(float *x, float *y, float *z, const int n, int repeat);

#if (defined(__FMA__))
float triad_fma_repeat(float *x, float *y, float *z, const int n, int repeat) {
  float k = 3.14159f;
  int r;
  for(r=0; r<repeat; r++) {
    int i;
    __m256 k4 = _mm256_set1_ps(k);
    for(i=0; i<n; i+=8) {
      _mm256_store_ps(&z[i], _mm256_fmadd_ps(k4, _mm256_load_ps(&y[i]), _mm256_load_ps(&x[i])));
    }
  }
}
#elif (defined(__AVX__))
float triad_avx_repeat(float *x, float *y, float *z, const int n, int repeat) {
  float k = 3.14159f;
  int r;
  for(r=0; r<repeat; r++) {
    int i;
    __m256 k4 = _mm256_set1_ps(k);
    for(i=0; i<n; i+=8) {
      _mm256_store_ps(&z[i], _mm256_add_ps(_mm256_load_ps(&x[i]), _mm256_mul_ps(k4, _mm256_load_ps(&y[i]))));
    }
  }
}
#else
float triad_sse_repeat(float *x, float *y, float *z, const int n, int repeat) {
  float k = 3.14159f;
  int r;
  for(r=0; r<repeat; r++) {
    int i;
    __m128 k4 = _mm_set1_ps(k);
    for(i=0; i<n; i+=4) {
      _mm_store_ps(&z[i], _mm_add_ps(_mm_load_ps(&x[i]), _mm_mul_ps(k4, _mm_load_ps(&y[i]))));
    }
  }
}
#endif

double time_diff(timespec start, timespec end)
{
  timespec temp;
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return (double)temp.tv_sec +  (double)temp.tv_nsec*1E-9;
}

int main () {
  int bytes_per_cycle = 0;
  int flops_per_cycle = 0;
  //double frequency = 2.3;  //Haswell
  double frequency = 3.4;  //IB
  //double frequency = 2.66;  //Core2
#if (defined(__FMA__))
  bytes_per_cycle = 96;
  flops_per_cycle = 32;
#elif (defined(__AVX__))
  bytes_per_cycle = 48;
  flops_per_cycle = 16;
    #else
  bytes_per_cycle = 24;
  flops_per_cycle = 8;
    #endif
  double peak  = frequency*bytes_per_cycle;
  double peakf = frequency*flops_per_cycle;

  //const int n =2048;
  //const int n =400;
  const int n =512;
  //const int n =4096;

  float* z2 = (float*)_mm_malloc(sizeof(float)*n, 64);
  char *mem = (char*)_mm_malloc(1<<18,4096);
  char *a = mem;
  char *b = a+n*sizeof(float);
  char *c = b+n*sizeof(float);

  float *x = (float*)a;
  float *y = (float*)b;
  float *z = (float*)c;

  for(int i=0; i<n; i++) {
    x[i] = 1.0f*i;
    y[i] = 1.0f*i;
    z[i] = 0;
  }
  int repeat = 1000000;
  timespec time1, time2;
#if (defined(__FMA__))
  triad_fma_repeat(x,y,z2,n,repeat);
#elif (defined(__AVX__))
  triad_avx_repeat(x,y,z2,n,repeat);
    #else
  triad_sse_repeat(x,y,z2,n,repeat);
    #endif

  while(1) {
    double dtime, rate, ratef;

    clock_gettime(TIMER_TYPE, &time1);
#if (defined(__FMA__))
    triad_fma_asm_repeat(x,y,z,n,repeat);
#elif (defined(__AVX__))
    triad_avx_asm_repeat(x,y,z,n,repeat);
        #else
    triad_sse_asm_repeat(x,y,z,n,repeat);
        #endif
    clock_gettime(TIMER_TYPE, &time2);
    dtime = time_diff(time1,time2);
    rate = 3.0*1E-9*sizeof(float)*n*repeat/dtime;
	ratef = 2.0*1E-9*n*repeat/dtime;

    printf("unroll1     rate %6.2f GB/s, efficency %6.2f%%, error %d\n", rate, 100*rate/peak, memcmp(z,z2, sizeof(float)*n));
    printf("unroll1     rate %6.2f GFLOPS, efficency %6.2f%%, error %d\n", ratef, 100*ratef/peakf, memcmp(z,z2, sizeof(float)*n));
    clock_gettime(TIMER_TYPE, &time1);
#if (defined(__FMA__))
    triad_fma_repeat(x,y,z,n,repeat);
#elif (defined(__AVX__))
    triad_avx_repeat(x,y,z,n,repeat);
        #else
    triad_sse_repeat(x,y,z,n,repeat);
        #endif
    clock_gettime(TIMER_TYPE, &time2);
    dtime = time_diff(time1,time2);
    rate = 3.0*1E-9*sizeof(float)*n*repeat/dtime;
	ratef = 2.0*1E-9*n*repeat/dtime;
    printf("intrinsic   rate %6.2f GB/s, efficency %6.2f%%, error %d\n", rate, 100*rate/peak, memcmp(z,z2, sizeof(float)*n));
    printf("intrinsic   rate %6.2f GFLOPS, efficency %6.2f%%, error %d\n", ratef, 100*ratef/peakf, memcmp(z,z2, sizeof(float)*n));
    clock_gettime(TIMER_TYPE, &time1);
#if (defined(__FMA__))
    triad_fma_asm_repeat_unroll16(x,y,z,n,repeat);
#elif (defined(__AVX__))
    triad_avx_asm_repeat_unroll16(x,y,z,n,repeat);
        #else
    triad_sse_asm_repeat_unroll16(x,y,z,n,repeat);
        #endif
    clock_gettime(TIMER_TYPE, &time2);
    dtime = time_diff(time1,time2);
    rate = 3.0*1E-9*sizeof(float)*n*repeat/dtime;
	ratef = 2.0*1E-9*n*repeat/dtime;
    printf("unroll16    rate %6.2f GB/s, efficency %6.2f%%, error %d\n", rate, 100*rate/peak, memcmp(z,z2, sizeof(float)*n));
    printf("unroll16    rate %6.2f GFLOPS, efficency %6.2f%%, error %d\n", ratef, 100*ratef/peakf, memcmp(z,z2, sizeof(float)*n));
  }
}
