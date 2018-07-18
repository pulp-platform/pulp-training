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
#include <stdio.h>

int vectA[]     = {9, 8, 7, 6, 5, 4, 3, 2, 1};
int vectB[]     = {2,-2, 2,-2, 2,-2, 2,-2, 2};
int acc         = 18;

// 18  - 9*2 - 8*(-2) - 7*2 - 6*(-2) - 5*2 - 4*(-2) - 3*2 2*(-2) - 1*2 =
// 18 - 18 + 16 - 14 + 12 - 10 + 8 - 6 + 4 -2 = 8

int main()
{

  acc = mulandsub(vectA,vectB, acc, 9);

  printf("The result is %d, expected %d\n", acc, 8 );

  return 0;
}

