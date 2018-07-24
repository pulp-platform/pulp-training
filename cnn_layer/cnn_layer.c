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
#include "cnn_layer.h"

static Pixel PULP_L2_DATA  Out[OUT_DIM];

void check_CNN_5x5_Scalar          ();
void check_CNN_5x5_Vector          ();

int main()
{
  
#ifdef DOTP
  check_CNN_5x5_Vector();
#else
  check_CNN_5x5_Scalar();
#endif
  
  return 0;
  
}

void check_CNN_5x5_Scalar() {
  
  // start benchmark
  printf("CNN WINDOW=%d, DATA_WIDTH=%d\n",FILT_WIN,DATA_WIDTH);
  InitZero(Out, OUT_DIM);
  
  reset_timer();
  start_timer();
  
  CNN_layer_Scalar(In_Img, Out, IMG_ROW, IMG_COL, Filter_Kern);
  
  stop_timer();
  
  printf("Number of cycles: %d\n ",get_time());
  
}

void check_CNN_5x5_Vector() {
  
  printf("CNN WINDOW=%d, DATA_WIDTH=%d\n",FILT_WIN,DATA_WIDTH);
  
  InitZero(Out, OUT_DIM);
  
  reset_timer();
  start_timer();
  
  CNN_layer_Vector(In_Img, Out, IMG_ROW, IMG_COL, Filter_Kern);
  
  stop_timer();
  printf("Number of cycles: %d\n ",get_time());
  
}

// load initialize out to 0
void __attribute__ ((noinline)) InitZero(Pixel * __restrict__ Img, int size)
{
  int i;
  
  for (i=0; i < size; i++)
    Img[i] = 0;

}

#define CNN_DEBUG
int  __attribute__ ((noinline)) checkresult(Pixel * __restrict__ Out, Pixel * __restrict__ OutGold, int N)
{
  int i;
  int err = 0;
  
  for (i = 0; i<N; i++) {
    if (Out[i]!=OutGold[i]) {
#ifdef CNN_DEBUG
      printf("At index %d: Actual value: %d: Expected: %d\n", i, Out[i],  OutGold[i]);
#endif
      err++;
    }
  }
  return err;
}
