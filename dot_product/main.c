// Copyright 2018 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the “License”); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an “AS IS” BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include <stdio.h>
#include "pulp.h"

//#define USE_LOOPUNROLLING
//#define USE_SIMD

#define NELEM 1000

unsigned char vecA[NELEM];
unsigned char vecB[NELEM];

extern unsigned int dotproduct(unsigned int acc, unsigned char* vA, unsigned char* vB, unsigned int N);
extern unsigned int dotproduct_loopunroll(unsigned int acc, unsigned char* vA, unsigned char* vB, unsigned int N);
extern unsigned int dotproduct_loopunroll_simd(unsigned int acc, unsigned char* vA, unsigned char* vB, unsigned int N);



unsigned int GOLDEN_VALUE = 20743432;

void init_vec(unsigned char* V, int n, unsigned char off)
{
    int i;
    for(i=0;i<n;i++)
        V[i] = (unsigned char)i + off;
}

int main()
{

  unsigned int acc = 0;

  if(get_core_id() == 0) {
    init_vec(vecA,NELEM,0);
    init_vec(vecB,NELEM,1);
    #ifdef USE_SIMD
    acc += dotproduct_loopunroll_simd(acc, vecA, vecB, NELEM);
    #else
    #ifdef USE_LOOPUNROLLING
    acc += dotproduct_loopunroll(acc, vecA, vecB, NELEM);
    #else
    acc += dotproduct(acc, vecA, vecB, NELEM);
    #endif
    #endif
    if(acc != GOLDEN_VALUE)
      printf("dot product is is %d instead of %d\n",acc, GOLDEN_VALUE);
    else
      printf("Nice! Well done! 0 errors\n");
  }

  return acc != GOLDEN_VALUE;
}

