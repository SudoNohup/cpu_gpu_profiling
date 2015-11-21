/*
 * --------------------------------------------------------------------------
 * BLISGEMM 
 * --------------------------------------------------------------------------
 * Copyright (C) 2015, The University of Texas at Austin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 * blisgemm_util.c
 *
 *
 *
 * Purpose:
 * Aligned memory allocation.
 *
 * Todo:
 *
 *
 * Modification:
 *
 * 
 * */


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include <blisgemm.h>
#include <blisgemm_config.h>


/*
 *
 *
 */ 
inline void swap_float( float *x, int i, int j ) {
  float  tmp = x[ i ];
  x[ i ] = x[ j ];
  x[ j ] = tmp;
}

inline void swap_double( double *x, int i, int j ) {
  double tmp = x[ i ];
  x[ i ] = x[ j ];
  x[ j ] = tmp;
}

inline void swap_int( int *x, int i, int j ) {
  int    tmp = x[ i ];
  x[ i ] = x[ j ];
  x[ j ] = tmp;
}



/*
 *
 *
 */ 
void bubbleSort_s(
    int    n,
    float  *D,
    int    *I
    )
{
  int    i, j;

  for ( i = 0; i < n - 1; i ++ ) {
    for ( j = 0; j < n - 1 - i; j ++ ) {
      if ( D[ j ] > D[ j + 1 ] ) {
        swap_float( D, j, j + 1 );
        swap_int( I, j, j + 1 );
      }
    }
  }
}

void bubbleSort_d(
    int    n,
    double *D,
    int    *I
    )
{
  int    i, j;

  for ( i = 0; i < n - 1; i ++ ) {
    for ( j = 0; j < n - 1 - i; j ++ ) {
      if ( D[ j ] > D[ j + 1 ] ) {
        swap_double( D, j, j + 1 );
        swap_int( I, j, j + 1 );
      }
    }
  }
}



/*
 *
 *
 */ 
double *blisgemm_malloc_aligned(
    int    m,
    int    n,
    int    size
    )
{
  double *ptr;
  int    err;

  err = posix_memalign( (void**)&ptr, (size_t)GEMM_SIMD_ALIGN_SIZE, size * m * n );

  if ( err ) {
    printf( "blisgemm_malloc_aligned(): posix_memalign() failures" );
    exit( 1 );    
  }

  return ptr;
}



/*
 *
 *
 */
void blisgemm_printmatrix(
    double *A,
    int    lda,
    int    m,
    int    n
    )
{
  int    i, j;
  for ( i = 0; i < m; i ++ ) {
    for ( j = 0; j < n; j ++ ) {
      printf("%lf\t", A[j * lda + i]);
    }
    printf("\n");
  }
}


