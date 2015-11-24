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
 * blisgemm_ref.c
 *
 *
 * Purpose:
 * implement reference mkl using GEMM (optional) in C.
 *
 * Todo:
 *
 *
 * Modification:
 *
 * 
 * */

#include <omp.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

#include <blisgemm.h>
#include <blisgemm_ref.h>

#ifdef USE_BLAS
/* 
 * dgemm and sgemm prototypes
 *
 */ 
void dgemm(char*, char*, int*, int*, int*, double*, double*, 
    int*, double*, int*, double*, double*, int*);
void sgemm(char*, char*, int*, int*, int*, float*, float*, 
    int*, float*, int*, float*, float*, int*);
void daxpy(int*, double*, double*, int*, double*, int*);
void dcopy(int*, double*, int*, double*, int*);
#endif


void dblisaddmul_ref(
    int    m,
    int    n,
    int    k,
    double *XA,
    int    *alpha,
    double *XB,
    int    *beta,
    double *XE,
    double *XC,
    int    ldc
    )
{
  // Local variables.
  int    i, j, p;
  double beg, time_collect, time_dgemm, time_square;
  double *As, *Bs, *Cs, *Es, *Gs;
  double fneg2 = -2.0, fzero = 0.0;


  // Sanity check for early return.
  if ( m == 0 || n == 0 || k == 0 ) return;


  // Allocate buffers.
//  As = (double*)malloc( sizeof(double) * m * k );
//  Bs = (double*)malloc( sizeof(double) * n * k );
  As = XA;
  Bs = XB;
  Es = XE;
  Cs = XC;

  Gs = (double*)malloc( sizeof(double) * n * k );

  if ( n <= 32 && m <= 32 ) {
    printf("As:\n");
    blisgemm_printmatrix( As, k, k, m );
  }

  if ( n <= 32 && m <= 32 ) {
    printf("Bs:\n");
    blisgemm_printmatrix( Bs, k, k, n );
  }

  #include "dblisgemm_ref_impl.h"

  if ( n <= 32 && m <= 32 ) {
    printf("refC:\n");
    blisgemm_printmatrix( Cs, m, m, n );
  }

  // Free  buffers
//  free( As );
//  free( Bs );
  free( Gs );

}
