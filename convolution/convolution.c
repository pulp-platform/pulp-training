// Copyright 2017 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the “License”); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an “AS IS” BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
#include "pulp.h"
#include "convolution.h"

void check_Conv5x5_Scalar();
void check_Conv5x5_Vector();

RT_L2_DATA Pixel Out[IMG_DIM];

int main()
{

  int errors = 0;

#ifdef PULP_EXT
  #ifdef DOTP
    check_Conv5x5_Vector();
  #else
    check_Conv5x5_Scalar();
  #endif
#else
  check_Conv5x5_Scalar();
#endif

  return 0;
}

void check_Conv5x5_Scalar() {

    unsigned int instr, cycles, ldstall, jrstall, imstall;

    printf("2D Convolution WINDOW=%d, DATA_WIDTH=%d\n",FILT_WIN,DATA_WIDTH);
    InitZero(Out, IMG_DIM);

    rt_perf_t *perf;
    perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));

    rt_perf_init(perf);
    rt_perf_conf(perf, (1<<RT_PERF_ACTIVE_CYCLES) | (1<<RT_PERF_INSTR) | (1<< RT_PERF_LD_STALL ) | (1<< RT_PERF_JR_STALL ) | (1 << RT_PERF_IMISS ) ) ;

    rt_perf_reset(perf);

    //start the monitoring
    rt_perf_start(perf);

    Conv5x5_Scalar(In_Img, Out, IMG_ROW, IMG_COL, Filter_Kern);

    //stop the HW counter used for monitoring
    rt_perf_stop(perf);

    //get the total measurement
    rt_perf_save(perf);

    instr   = rt_perf_get(perf, RT_PERF_INSTR);
    cycles  = rt_perf_get(perf, RT_PERF_ACTIVE_CYCLES);
    ldstall = rt_perf_get(perf, RT_PERF_LD_STALL);
    jrstall = rt_perf_get(perf, RT_PERF_JR_STALL);
    imstall = rt_perf_get(perf, RT_PERF_IMISS);

    printf("Perf of dot product: \n \t cycles : %d \n \t instructions : %d \n \t load stalls : %d \n \t jump stalls : %d \n \t insrtructions stalls: %d \n\n", cycles, instr, ldstall, jrstall, imstall);


    checkresult(Out, Gold_Out_Img, IMG_DIM);

}

void check_Conv5x5_Vector() {

    unsigned int instr, cycles, ldstall, jrstall, imstall;

    printf("2D Convolution WINDOW=%d, DATA_WIDTH=%d\n",FILT_WIN,DATA_WIDTH);
    InitZero(Out, IMG_DIM);

    rt_perf_t *perf;
    perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));

    rt_perf_init(perf);
    rt_perf_conf(perf, (1<<RT_PERF_ACTIVE_CYCLES) | (1<<RT_PERF_INSTR) | (1<< RT_PERF_LD_STALL ) | (1<< RT_PERF_JR_STALL ) | (1 << RT_PERF_IMISS ) ) ;

    rt_perf_reset(perf);

    //start the monitoring
    rt_perf_start(perf);

    Conv5x5_Vector(In_Img, Out, IMG_ROW, IMG_COL, Filter_Kern);

    //stop the HW counter used for monitoring
    rt_perf_stop(perf);

    //get the total measurement
    rt_perf_save(perf);

    instr   = rt_perf_get(perf, RT_PERF_INSTR);
    cycles  = rt_perf_get(perf, RT_PERF_ACTIVE_CYCLES);
    ldstall = rt_perf_get(perf, RT_PERF_LD_STALL);
    jrstall = rt_perf_get(perf, RT_PERF_JR_STALL);
    imstall = rt_perf_get(perf, RT_PERF_IMISS);

    printf("Perf of dot product: \n \t cycles : %d \n \t instructions : %d \n \t load stalls : %d \n \t jump stalls : %d \n \t insrtructions stalls: %d \n\n", cycles, instr, ldstall, jrstall, imstall);

    checkresult(Out, Gold_Out_Img, IMG_DIM);

}

// load kernel
void __attribute__ ((noinline)) InitKernel(Filtc * __restrict__ Kernel, int size)
{
  int i;
  int n = size*size;
  for (i=0; i < n; i++) {
      Kernel[i] = Filter_Kern[i];
  }
}

// load input img
void __attribute__ ((noinline)) InitData(Pixel * __restrict__ Img, int size)
{
  int i;

  for (i=0; i < size; i++)
      Img[i] = In_Img[i];

}

// load initialize out to 0
void __attribute__ ((noinline)) InitZero(Pixel * __restrict__ Img, int size)
{
  int i;

  for (i=0; i < size; i++)
      Img[i] = 0;

}

#define CONV2D_DEBUG
int  __attribute__ ((noinline)) checkresult(Pixel * __restrict__ Out, Pixel * __restrict__ OutGold, int N)
{
  int i;
  int err = 0;

  for (i = 0; i<N; i++) {
    if (Out[i]!=OutGold[i]) {
#ifdef CONV2D_DEBUG
      printf("At index %d: Actual value: %x: Expected: %x\n", i, Out[i],  OutGold[i]);
#endif
      err++;
    }
  }
  return err;
}
