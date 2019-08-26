/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include "pulp.h"

#define REG_CORESTATUS 0x10000000
  
int main()
{
  
  volatile int wait;
  
  printf("Starting Power Analysis\n");
  
  *(volatile int*)(REG_CORESTATUS) = 0xABBAABBA;
  
  for (int i = 0; i<100; i++)
    wait++;
  
  *(volatile int*)(REG_CORESTATUS) = 0xDEADCACA;
      
  printf("Done!\n");
  
  return 0;
  
}
