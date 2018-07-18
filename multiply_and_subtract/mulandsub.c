/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Pasquale Davide Schiavone (pschiavo@iis.ee.ethz.ch)
 */

#include "myfunc.h"


int mulandsub(int *A, int* B, int acc, int N)
{

  int i;

  for(i=0;i<N;i++){
    acc-= A[i]*B[i];
  }

  return acc;
}

