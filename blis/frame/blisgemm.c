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
 * blisgemm.c
 *
 *
 * Purpose:
 * this is the main file of blis gemm.
 *
 * Todo:
 *
 *
 * Modification:
 *
 * 
 * */


#include <stdio.h>
#include <omp.h>
#include <blisgemm.h>
#define min( i, j ) ( (i)<(j) ? (i): (j) )

#include <blisgemm_config.h>
#include <blisgemm_kernel.h>



inline void packA_kcxmc_d(
    int    m,
    int    k,
    double *XA,
    int    ldXA,
    int    *amap,
    double *packA
    )
{
  int    i, p;
  double *a_pntr[ DGEMM_MR ];

  for ( i = 0; i < m; i ++ ) {
    a_pntr[ i ] = XA + ldXA * amap[ i ];
  }

  for ( i = m; i < DGEMM_MR; i ++ ) {
    a_pntr[ i ] = XA + ldXA * amap[ 0 ];
  }

  for ( p = 0; p < k; p ++ ) {
    for ( i = 0; i < DGEMM_MR; i ++ ) {
      *packA ++ = *a_pntr[ i ] ++;
    }
  }
}
/*
 * --------------------------------------------------------------------------
 */



inline void packB_kcxnc_d(
    int    n,
    int    k,
    double *XB,
    int    ldXB, // ldXB is the original k
    int    *bmap,
    double *packB
    )
{
  int    j, p; 
  double *b_pntr[ DGEMM_NR ];

  for ( j = 0; j < n; j ++ ) {
    b_pntr[ j ] = XB + ldXB * bmap[ j ];
  }

  for ( j = n; j < DGEMM_NR; j ++ ) {
    b_pntr[ j ] = XB + ldXB * bmap[ 0 ];
  }

  for ( p = 0; p < k; p ++ ) {
    for ( j = 0; j < DGEMM_NR; j ++ ) {
      *packB ++ = *b_pntr[ j ] ++;
    }
  }
}
/*
 * --------------------------------------------------------------------------
 */

void dsq2nrm_macro_kernel(
    int    m,
    int    n,
    int    k,
    double *packA,
    double *packB,
    double *C,
    int    ldc,
    int    pc,
    int    lastiter
    )
{
  int    i, ii, j;
  aux_t  aux;

  aux.pc     = pc;
  aux.b_next = packB;

  //printf( "here, pc = %d, last = %d, ldc = %d, m = %d, n = %d, k %d\n", 
  //    pc, lastiter, ldc, m, n , k );

  for ( j = 0; j < n; j += DGEMM_NR ) {
    aux.n  = min( n - j, DGEMM_NR );
    for ( i = 0; i < m; i += DGEMM_MR ) {
      aux.m = min( m - i, DGEMM_MR );
      if ( i + DGEMM_MR >= m ) {
        aux.b_next += DGEMM_NR * k;
      }

      //sq2nrm_asm_d8x4(
      ( *sq2nrm ) (
          k,
          &packA[ i * k ],
          &packB[ j * k ],
          &C[ j * ldc + i ],
          (unsigned long long) ldc,
          (unsigned long long) lastiter,
          &aux
          );
    }
  }
}




// C must be aligned
void dgssq2nrm(
    int    m,
    int    n,
    int    k,
    double *XA,
    int    *amap,
    double *XB,
    int    *bmap,
    double *C,        // must be aligned
    int    ldc        // ldc must also be aligned
)
{
  int    i, j, p, blisgemm_ic_nt;
  int    ic, ib, jc, jb, pc, pb;
  int    ir, jr;
  double *packA, *packB;
  char   *str;


  // Early return if possible
  if ( m == 0 || n == 0 || k == 0 ) {
    printf( "dblisgemm(): early return\n" );
    return;
  }


  // sequential is the default situation
  blisgemm_ic_nt = 1;


  // check the environment variable
  str = getenv( "BLISGEMM_IC_NT" );
  if ( str != NULL ) {
    blisgemm_ic_nt = (int)strtol( str, NULL, 10 );
  }


  // Allocate packing buffers
  packA  = blisgemm_malloc_aligned( DGEMM_KC, ( DGEMM_MC + 1 ) * blisgemm_ic_nt, sizeof(double) );
  packB  = blisgemm_malloc_aligned( DGEMM_KC, ( DGEMM_NC + 1 )            , sizeof(double) );


  for ( jc = 0; jc < n; jc += DGEMM_NC ) {                  // 6-th loop
    jb = min( n - jc, DGEMM_NC );
    for ( pc = 0; pc < k; pc += DGEMM_KC ) {                // 5-th loop
      pb = min( k - pc, DGEMM_KC );


      #pragma omp parallel for num_threads( blisgemm_ic_nt ) private( jr )
      for ( j = 0; j < jb; j += DGEMM_NR ) {

        packB_kcxnc_d(
            min( jb - j, DGEMM_NR ),
            pb,
            &XB[ pc ],
            k, // should be ldXB instead
            &bmap[ jc + j ],
            &packB[ j * pb ]
            );
      }

      #pragma omp parallel for num_threads( blisgemm_ic_nt ) private( ic, ib, i, ir )
      for ( ic = 0; ic < m; ic += DGEMM_MC ) {              // 4-th loop
        int     tid = omp_get_thread_num();

        ib = min( m - ic, DGEMM_MC );
        for ( i = 0; i < ib; i += DGEMM_MR ) {

          packA_kcxmc_d(
              min( ib - i, DGEMM_MR ),
              pb,
              &XA[ pc ],
              k,
              &amap[ ic + i ],
              &packA[ tid * DGEMM_MC * pb + i * pb ]
              );
        }


        dsq2nrm_macro_kernel(
            ib,
            jb,
            pb,
            packA  + tid * DGEMM_MC * pb,
            packB,
            &C[ jc * ldc + ic ], 
            ldc,
            pc,
            ( pc + DGEMM_KC >= k )
            );

      }                                                    // End 4.th loop
    }                                                      // End 5.th loop
  }                                                        // End 6.th loop

  free( packA );
  free( packB );
}


void dblisgemm_var3(
    int    m,
    int    n,
    int    k,
    double *XA,
    int    *amap,
    double *XB,
    int    *bmap,
    double *XC,
    int    ldc
    )
{
  int    i, j, ldr;
  //int    ldc = ( ( m - 1 ) / DGEMM_MR + 1 ) * DGEMM_MR;
  double beg, time_sq2nrm;
  //double *C = blisgemm_malloc_aligned( ldc, n + 4, sizeof(double) );

  beg = omp_get_wtime();
  dgssq2nrm(
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
  time_sq2nrm = omp_get_wtime() - beg;



  if ( n <= 32 && m <= 32) {
    printf("myC:\n");
    blisgemm_printmatrix( XC, ldc, m, n );
  }


  //free( C );
}


void dblisgemm(
    int    m,
    int    n,
    int    k,
    double *XA,
    int    *amap,
    double *XB,
    int    *bmap,
    double *XC,
    int    ldc
    )
{
    dblisgemm_var3(
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
