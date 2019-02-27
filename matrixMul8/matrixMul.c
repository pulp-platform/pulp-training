// Copyright 2017 ETH Zurich and University of Bologna.
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

#include "dotMatrixMul8_stimuli.h"

int check_matrix_mul          ();

extern signed int matMul8(signed char* mA, signed char* mB, signed int* mC, int Row, int Col);
extern signed int matMul8_loopunroll(signed char* mA, signed char* mB, signed int* mC, int Row, int Col);
extern signed int matMul8_loopunroll_simd(signed char* mA, signed char* mB, signed int* mC, int Row, int Col);

void __attribute__ ((noinline)) matrix_init(signed char * A, signed char * B, signed int * C);
unsigned int __attribute__ ((noinline)) matrix_check(signed int * C);



int main() {
    if(get_core_id() == 0) {
      return check_matrix_mul();
    }
}


int check_matrix_mul() {

  int N = SIZE;
  int M = SIZE;

  signed char matA[N*M];
  signed char matB[N*M];
  signed int  matC[N*M];

  matrix_init(matA,matB,matC);

    #ifdef USE_SIMD
    matMul8_loopunroll_simd(matA, matB, matC, N, M);
    #else
    #ifdef USE_LOOPUNROLLING
    matMul8_loopunroll(matA, matB, matC, N, M);
    #else
    matMul8(matA, matB, matC, N, M);
    #endif
    #endif



  return matrix_check(matC);
}


// helper functions
void __attribute__ ((noinline)) matrix_init(signed char * __restrict__ A, signed char * __restrict__ B, signed int * __restrict__ C) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      A[i*SIZE+j] = m_a[i * SIZE + j];
      B[i*SIZE+j] = m_b[i * SIZE + j];
      C[i*SIZE+j] = 0;
    }
  }
}

unsigned int __attribute__ ((noinline)) matrix_check(signed int * __restrict__ C) {
  unsigned int errors = 0;
  // check
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (C[i*SIZE+j] != m_exp[i * SIZE + j]) {
        printf("At index %d, %d\n", i, j);
        errors++;
      }
    }
  }
  return errors;
}