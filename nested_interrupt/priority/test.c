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

static unsigned int ninterrupts0 = 0;
static unsigned int ninterrupts1 = 0;

void __attribute__((interrupt)) __test_timer_handler1()
{

  unsigned int mepc    = 0;
  unsigned int mstatus = 0;

  ninterrupts1++;

  //disable timer and interrupt
  hal_timer_conf(
    hal_timer_fc_addr(0, 1), PLP_TIMER_ACTIVE, PLP_TIMER_RESET_ENABLED,
    PLP_TIMER_IRQ_ENABLED, PLP_TIMER_IEM_DISABLED, PLP_TIMER_CMPCLR_ENABLED,
    PLP_TIMER_ONE_SHOT_DISABLED, PLP_TIMER_REFCLK_ENABLED,
    PLP_TIMER_PRESCALER_DISABLED, 0, PLP_TIMER_MODE_64_DISABLED
  );

  //clear pending interrupt of IRQ ARCHI_FC_EVT_TIMER1
  hal_itc_status_clr (1<<ARCHI_FC_EVT_TIMER1);

  asm volatile(
              "csrrs x15, mepc, x0\n"
              "sw x15, 0x0(%[mepc_addr])\n"
              "csrrs x15, mstatus, x0\n"
              "sw x15, 0x0(%[mstatus_addr])\n"
              "csrrsi x0, mstatus, 0x8\n" //enable interrupts now
              : : [mepc_addr] "r" (&mepc), [mstatus_addr] "r" (mstatus) : "x15");

  if(ninterrupts1 < 6)
    hal_itc_wait_for_interrupt();

  asm volatile(
              "csrrci x0, mstatus, 0x8\n" //disable interrupts now
              "lw x15, 0x0(%[mepc_addr])\n"
              "csrrw x0, mepc, x15\n"
              "lw x15, 0x0(%[mstatus_addr])\n"
              "csrrs x0, mstatus, x15\n"
              : : [mepc_addr] "r" (&mepc), [mstatus_addr] "r" (mstatus) : "x15");


}


void __attribute__((interrupt)) __test_timer_handler0()
{

  unsigned int mepc    = 0;
  unsigned int mstatus = 0;

  ninterrupts0++;

  //disable timer and interrupt
  hal_timer_conf(
    hal_timer_fc_addr(0, 0), PLP_TIMER_ACTIVE, PLP_TIMER_RESET_ENABLED,
    PLP_TIMER_IRQ_ENABLED, PLP_TIMER_IEM_DISABLED, PLP_TIMER_CMPCLR_ENABLED,
    PLP_TIMER_ONE_SHOT_DISABLED, PLP_TIMER_REFCLK_ENABLED,
    PLP_TIMER_PRESCALER_DISABLED, 0, PLP_TIMER_MODE_64_DISABLED
  );

  //clear pending interrupt of IRQ ARCHI_FC_EVT_TIMER0
  hal_itc_status_clr (1<<ARCHI_FC_EVT_TIMER0);

  unsigned int old_itc = hal_itc_enable_value_get();

  //disable all the interrupts less important than me
  hal_itc_enable_clr(1<<ARCHI_FC_EVT_TIMER1);

  asm volatile(
              "csrrs x15, mepc, x0\n"
              "sw x15, 0x0(%[mepc_addr])\n"
              "csrrs x15, mstatus, x0\n"
              "sw x15, 0x0(%[mstatus_addr])\n"
              "csrrsi x0, mstatus, 0x8\n" //enable interrupts now
              : : [mepc_addr] "r" (&mepc), [mstatus_addr] "r" (mstatus) : "x15");

  //now I can only be interrupt by the TIMER0
  if(ninterrupts0 < 2)
    hal_itc_wait_for_interrupt();

  asm volatile(
              "csrrci x0, mstatus, 0x8\n" //disable interrupts now
              "lw x15, 0x0(%[mepc_addr])\n"
              "csrrw x0, mepc, x15\n"
              "lw x15, 0x0(%[mstatus_addr])\n"
              "csrrs x0, mstatus, x15\n"
              : : [mepc_addr] "r" (&mepc), [mstatus_addr] "r" (mstatus) : "x15");

  //restore ITC
  hal_itc_enable_value_set(old_itc);

}

int main()
{

  int irq = rt_irq_disable();
  unsigned int ticks, ticks_from_now;
  unsigned int us = 100;

  // First compute the corresponding number of ticks.
  // The specified time is the minimum we must, so we have to round-up
  // the number of ticks.
  ticks = us / (1000000 / ARCHI_REF_CLOCK) + 1;

  asm volatile (
                "lw x15, 0x(%[bootaddr_apb])\n"
                "csrrw x0, mtvec, x15\n"
                :  : [bootaddr_apb] "r" (0x1a104004) : "x15");


  //set value to perform comparison
  hal_timer_cmp_set(hal_timer_fc_addr(0, 1), ticks>>1);

  //set value to perform comparison
  hal_timer_cmp_set(hal_timer_fc_addr(0, 0), ticks+1);

  //disable all the interrupts in the INTC
  hal_itc_enable_value_set(0);

  //enable the interrupts of the timer1 in the INTC
  hal_itc_enable_set(1<<ARCHI_FC_EVT_TIMER1);
  hal_itc_enable_set(1<<ARCHI_FC_EVT_TIMER0);

  //Set the ISR
  rt_irq_set_handler (ARCHI_FC_EVT_TIMER1, __test_timer_handler1);

  rt_irq_set_handler (ARCHI_FC_EVT_TIMER0, __test_timer_handler0);


  printf("Going to sleep.... (ninterrupts0-1 %d %d) \n",ninterrupts0, ninterrupts1 );

  hal_itc_status_clr(1<<ARCHI_FC_EVT_TIMER1);
  hal_itc_status_clr(1<<ARCHI_FC_EVT_TIMER0);

  //enable timer and interrupt generation
  hal_timer_conf(
    hal_timer_fc_addr(0, 1), PLP_TIMER_ACTIVE, PLP_TIMER_RESET_ENABLED,
    PLP_TIMER_IRQ_ENABLED, PLP_TIMER_IEM_DISABLED, PLP_TIMER_CMPCLR_ENABLED,
    PLP_TIMER_ONE_SHOT_DISABLED, PLP_TIMER_REFCLK_ENABLED,
    PLP_TIMER_PRESCALER_DISABLED, 0, PLP_TIMER_MODE_64_DISABLED
  );

  hal_timer_conf(
    hal_timer_fc_addr(0, 0), PLP_TIMER_ACTIVE, PLP_TIMER_RESET_ENABLED,
    PLP_TIMER_IRQ_ENABLED, PLP_TIMER_IEM_DISABLED, PLP_TIMER_CMPCLR_ENABLED,
    PLP_TIMER_ONE_SHOT_DISABLED, PLP_TIMER_REFCLK_ENABLED,
    PLP_TIMER_PRESCALER_DISABLED, 0, PLP_TIMER_MODE_64_DISABLED
  );

  rt_irq_restore(irq);

  hal_itc_wait_for_interrupt();

  irq = rt_irq_disable();

  printf("Woken up.... (ninterrupts0-1 %d %d)\n", ninterrupts0, ninterrupts1);

  rt_irq_restore(irq);

  return 0;
}

