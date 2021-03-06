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
 * blisgemm_ref_impl.h
 *
 *
 * Purpose:
 * This file implement the common code segments shared by double precision 
 * blisgemm_ref.c and blisgemm_ref_stl.cpp.
 *
 * Todo:
 *
 *
 * Modification:
 *
 * 
 * */


  // Collect As from XA and XB.
//  beg = omp_get_wtime();
//  #pragma omp parallel for private( p )
//  for ( i = 0; i < m; i ++ ) {
//    for ( p = 0; p < k; p ++ ) {
//      As[ i * k + p ] = XA[ alpha[ i ] * k + p ];
//    }
//  }
//  #pragma omp parallel for private( p )
//  for ( j = 0; j < n; j ++ ) {
//    for ( p = 0; p < k; p ++ ) {
//      Bs[ j * k + p ] = XB[ beta[ j ] * k + p ];
//    }
//  }
//
//
//  time_collect = omp_get_wtime() - beg;


  fneg2 = 1.0;

  // Compute the inner-product term.
  beg = omp_get_wtime();
//#ifdef USE_BLAS


  int    len2   = k * n;
  double alpha2 = 1.0;
  int    incx   = 1;
  int    incy   = 1;


  dcopy( &len2, Bs, &incx, Gs, &incy );
  daxpy( &len2, &alpha2, Es, &incx, Gs, &incy );
  



//  int len = k*m;
//  double alpha2 = 1.0;
//  int incx = 1;
//  int incy = 1;
//  daxpy( &len, &alpha2, As, &incx, As, &incy );



  dgemm( "T", "N", &m, &n, &k, &fneg2,
        //As, &k, Bs, &k, &fzero, Cs, &m );
        As, &k, Gs, &k, &fzero, Cs, &m );

//#else
//  #pragma omp parallel for private( i, p )
//  for ( j = 0; j < n; j ++ ) {
//    for ( i = 0; i < m; i ++ ) {
//      Cs[ j * m + i ] = 0.0;
//      for ( p = 0; p < k; p ++ ) {
//        Cs[ j * m + i ] += As[ i * k + p ] * Bs[ j * k + p ];
//      }
//    }
//  }
//#endif
  time_dgemm = omp_get_wtime() - beg;


  //printf("time_dgemm: %lf\n", time_dgemm);

  //printf("%lf GFLOPS\n", 2.0*m*n*k/time_dgemm/1000.0/1000.0/1000.0);

  /*
  // 1-norm
  #pragma omp parallel for private( i, p )
  for ( j = 0; j < n; j ++ ) {
    for ( i = 0; i < m; i ++ ) {
      Cs[ j * m + i ] = 0.0;
      for ( p = 0; p < k; p ++ ) {
        Cs[ j * m + i ] += fabs( As[ i * k + p ] - Bs[ j * k + p ] );
      }
    }
  }
  */

// time_dgemm = omp_get_wtime() - beg;

//  beg = omp_get_wtime();
//  #pragma omp parallel for private( i )
//  for ( j = 0; j < n; j ++ ) {
//    for ( i = 0; i < m; i ++ ) {
//#ifdef USE_BLAS
//#else
//      Cs[ j * m + i ] *= -2.0;
//#endif
//      Cs[ j * m + i ] += XA2[ alpha[ i ] ];
//      Cs[ j * m + i ] += XB2[ beta[ j ] ];
//    }
//  }
//  time_square = omp_get_wtime() - beg;
