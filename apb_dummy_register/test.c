/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Pasquale Davide Schiavone (pschiavo@iis.ee.ethz.ch)
 */

#include <rt/rt_api.h>


#define APB_DUMMY_ADDR      0X1A120000
#define OFFSET_SIGNATURE    0x00 //CONTAINS A READ-ONLY Signature
#define OFFSET_SCRATCH      0x04 //R/W REGISTER AS SCRATCH

int main()
{

  volatile int* apb_dummy_signature;
  volatile int* apb_dummy_scratch;


  apb_dummy_signature = (APB_DUMMY_ADDR + OFFSET_SIGNATURE);
  apb_dummy_scratch   = (APB_DUMMY_ADDR + OFFSET_SCRATCH);

  *(apb_dummy_signature) = 10;

  printf("Signature is %x\n", *(apb_dummy_signature));

  *(apb_dummy_scratch) = 0x64;

  printf("Scratch is %x\n", *(apb_dummy_scratch));

  return 0;
}

