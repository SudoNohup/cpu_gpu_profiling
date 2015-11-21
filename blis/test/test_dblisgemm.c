#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <limits.h>

#include <blisgemm.h>
#include <blisgemm_ref.h>

#include <blisgemm_config.h>

#define NUM_POINTS 10240
#define USE_SET_DIFF 1
#define TOLERANCE 1E-12

void computeError(
    int    ldc,
    int    ldc_ref,
    int    m,
    int    n,
    double *C,
    double *C_ref
    )
{
  int    i, j;
  for ( i = 0; i < m; i ++ ) {
    for ( j = 0; j < n; j ++ ) {
      if ( fabs( C[ j * ldc + i ] - C_ref[ j * ldc_ref + i ] ) > TOLERANCE ) {
        printf( "C[ %d ][ %d ] != C_gold, %E, %E\n", i, j, C[ j * ldc + i ], C_ref[ j * ldc_ref + i ] );
        break;
      }
    }
  }

}


void test_dblisgemm(
    int m,
    int n,
    int k
    ) 
{
  int    i, j, p, nx;
  int    *amap, *bmap;
  double *XA, *XB, *XC, *XC_ref;
  double tmp, error, flops;
  double ref_beg, ref_time, dblisgemm_beg, dblisgemm_time;
  int    nrepeats;
  int    ldc;
  double ref_rectime, dblisgemm_rectime;

  //nx    = NUM_POINTS;

  amap  = (int*)malloc( sizeof(int) * m );
  bmap  = (int*)malloc( sizeof(int) * n );

  XA    = (double*)malloc( sizeof(double) * k * m );
  XB    = (double*)malloc( sizeof(double) * k * n );


  ldc = ( ( m - 1 ) / DGEMM_MR + 1 ) * DGEMM_MR;
  XC     = blisgemm_malloc_aligned( ldc, n + 4, sizeof(double) );
  XC_ref = (double*)malloc( sizeof(double) * m * n );
  
  nrepeats = 3;

  // Assign reference indecies.
  for ( i = 0; i < m; i ++ ) {
    amap[ i ] = i;
  }

  // Assign query indecies.
  for ( j = 0; j < n; j ++ ) {
    bmap[ j ] = j;
  }

  // Randonly generate points in [ 0, 1 ].
  for ( i = 0; i < m; i ++ ) {
    for ( p = 0; p < k; p ++ ) {
      XA[ i * k + p ] = (double)( rand() % 1000000 ) / 1000000.0;	
    }
  }
  for ( i = 0; i < n; i ++ ) {
    for ( p = 0; p < k; p ++ ) {
      XB[ i * k + p ] = (double)( rand() % 1000000 ) / 1000000.0;	
    }
  }


  // Use the same coordinate table
  //XB  = XA;


  for ( i = 0; i < nrepeats; i ++ ) {
    dblisgemm_beg = omp_get_wtime();
    {
      dblisgemm(
          m,
          n,
          k,
          XA,
          amap,
          XB,
          bmap,
          XC,
          ldc
          );
    }
    dblisgemm_time = omp_get_wtime() - dblisgemm_beg;

    if ( i == 0 ) {
      dblisgemm_rectime = dblisgemm_time;
    } else {
      //dblisgemm_rectime = dblisgemm_time < dblisgemm_rectime ? dblisgemm_time : dblisgemm_rectime;
      if ( dblisgemm_time < dblisgemm_rectime ) {
        dblisgemm_rectime = dblisgemm_time;
      }
    }
  }
  printf("dblisgemm_rectime: %lf\n", dblisgemm_rectime);


  for ( i = 0; i < nrepeats; i ++ ) {
    ref_beg = omp_get_wtime();
    {
      dblisgemm_ref(
          m,
          n,
          k,
          XA,
          amap,
          XB,
          bmap,
          XC_ref,
          m
          );
    }
    ref_time = omp_get_wtime() - ref_beg;

    if ( i == 0 ) {
      ref_rectime = ref_time;
    } else {
      //ref_rectime = ref_time < ref_rectime ? ref_time : ref_rectime;
      if ( ref_time < ref_rectime ) {
        ref_rectime = ref_time;
      }
    }
  }

  printf("ref_rectime: %lf\n", ref_rectime);

  //XC_ref[2] = 0.0;

  computeError(
      ldc,
      m,
      m,
      n,
      XC,
      XC_ref
      );

  // Compute overall floating point operations.
  flops = ( m * n / ( 1000.0 * 1000.0 * 1000.0 ) ) * ( 2 * k );

  printf( "%5d, %5d, %5d, %5.2lf GFLOPS, %5.2lf GFLOPS;\n", 
      m, n, k, flops / dblisgemm_rectime, flops / ref_rectime );


  free( XA     );
  free( XC     );
  free( XC_ref );
}


int main( int argc, char *argv[] )
{
  int    m, n, k, r; 

  if ( argc != 5 ) {
    printf( "Error: require 4 arguments, but only %d provided.\n", argc - 1 );
    exit( 0 );
  }

  sscanf( argv[ 1 ], "%d", &m );
  sscanf( argv[ 2 ], "%d", &n );
  sscanf( argv[ 3 ], "%d", &k );


  test_dblisgemm( m, n, k );


  return 0;
}
