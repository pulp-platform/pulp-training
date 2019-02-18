// Copyright 2017 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the “License”); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an “AS IS” BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

typedef signed char charV __attribute__((vector_size (4)));
#define SumDotp(a, b, c) __builtin_pulp_sdotsp4(a, b, c)
#define Dotp(a, b)       __builtin_pulp_dotsp4(a, b)


signed int matMul8(signed char* mA, signed char* mB, signed int* mC, int Row, int Col){}
signed int matMul8_loopunroll(signed char* mA, signed char* mB, signed int* mC, int Row, int Col){}
signed int matMul8_loopunroll_simd(signed char* mA, signed char* mB, signed int* mC, int Row, int Col){}
