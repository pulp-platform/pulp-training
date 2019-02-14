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


#define UDMA_BASE_ADDR       0x1A102000
#define PER_ID_EXT_PER       (8+1)
#define UDMA_TRAFF_GEN_OFF  (PER_ID_EXT_PER << 7)
#define UDMA_TRAFF_GEN_ADDR (UDMA_BASE_ADDR + UDMA_TRAFF_GEN_OFF)

//Regs
#define REG_RX_SADDR              0x00
#define REG_RX_SIZE               0x04
#define REG_RX_CFG                0x08
/*
   r_rx_clr           = cfg_data_i[5];
   r_rx_en            = cfg_data_i[4];
   r_rx_datasize      = cfg_data_i[2:1];
   r_rx_continuous    = cfg_data_i[0];
*/
#define REG_RX_INTCFG             0x0C
#define REG_TX_SADDR              0x10
#define REG_TX_SIZE               0x14
#define REG_TX_CFG                0x18
#define REG_TX_INTCFG             0x1C
#define REG_EXTERNAL_PER_STATUS   0x20
#define REG_EXTERNAL_PER_SETUP    0x24
/*
    cfg_en        = cfg_setup_i[0];
    s_target_word = cfg_setup_i[15:8];
    initial_value = cfg_setup_i[31:16];
*/


#define NUM_DATA 128
int myTGdata[NUM_DATA];

void init_data(int * array, int N)
{
    int i;

    for(i=0;i<N;i++)
        array[i] = 0;
}


int main()
{

  volatile int* udma_ctrl;
  volatile int* udma_traffic_gen_status;
  volatile int* udma_traffic_gen;

  init_data(myTGdata, NUM_DATA);
  rt_freq_set(RT_FREQ_DOMAIN_PERIPH, 80*1000*1000);

  udma_ctrl           = (UDMA_BASE_ADDR);
  *(udma_ctrl)        = 1 << 8; //enable clock


  udma_traffic_gen    = (UDMA_TRAFF_GEN_ADDR + REG_RX_CFG);
  *(udma_traffic_gen) = ((1 << 4) | (2 << 1) | 0);

  udma_traffic_gen    = (UDMA_TRAFF_GEN_ADDR + REG_RX_SADDR);
  *(udma_traffic_gen) = myTGdata;

  udma_traffic_gen    = (UDMA_TRAFF_GEN_ADDR + REG_RX_SIZE);
  *(udma_traffic_gen) = NUM_DATA*4;

  udma_traffic_gen    = (UDMA_TRAFF_GEN_ADDR + REG_EXTERNAL_PER_SETUP);
  *(udma_traffic_gen) = ((NUM_DATA << 8) | 1);

  udma_traffic_gen_status = (UDMA_TRAFF_GEN_ADDR + REG_EXTERNAL_PER_STATUS);

  //for(int i = 0; i< 1000000000; i++) asm volatile("nop");

  while( !((*udma_traffic_gen_status) & 0x1) );
  while( ((*udma_traffic_gen_status) & 0x1) );

  for(int i = 0; i<NUM_DATA;i++)
    printf("myTGdata[%d] is %x\n",i,myTGdata[i]);

  return 0;
}

