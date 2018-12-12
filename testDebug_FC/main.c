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

#define BASE_ADDRESS 0x1C000000
#define NCORES 4
#define CORE_CLKGATE_CLRBUFF  EVENT_UNIT_DEMUX_BASE_ADDR + EU_CORE_EVENT_WAIT_CLEAR
#define SW_EVENT 0

void test_init(testresult_t *result, void (*start)(), void (*stop)());
void test_finish(testresult_t *result, void (*start)(), void (*stop)());

void test_rw_gpr(testresult_t *result, void (*start)(), void (*stop)());
void test_rw_csr(testresult_t *result, void (*start)(), void (*stop)());
void test_rw_dbg_regs(testresult_t *result, void (*start)(), void (*stop)());
void test_halt_resume(testresult_t *result, void (*start)(), void (*stop)());
void test_ebreak(testresult_t *result, void (*start)(), void (*stop)());
void test_npc_ppc(testresult_t *result, void (*start)(), void (*stop)());
void test_illegal(testresult_t *result, void (*start)(), void (*stop)());
void test_single_step(testresult_t *result, void (*start)(), void (*stop)());
void test_jumps(testresult_t *result, void (*start)(), void (*stop)());
void test_jumps_after_branch(testresult_t *result, void (*start)(), void (*stop)());
void test_branch(testresult_t *result, void (*start)(), void (*stop)());
void test_wfi_sleep(testresult_t *result, void (*start)(), void (*stop)());
void test_access_while_sleep(testresult_t *result, void (*start)(), void (*stop)());

testcase_t testcases[] = {
  { .name = "init",                           .test = test_init                  },

  { .name = " 2. test_rw_gpr",                .test = test_rw_gpr                },
  { .name = " 3. test_rw_csr",                .test = test_rw_csr                },
  { .name = " 4. test_rw_dbg_regs",           .test = test_rw_dbg_regs           },
  { .name = " 5. test_halt_resume",           .test = test_halt_resume           },
  { .name = " 6. test_ebreak",                .test = test_ebreak                },
  { .name = " 7. test_npc_ppc",               .test = test_npc_ppc               },
  { .name = " 8. test_illegal",               .test = test_illegal               },
  { .name = " 9. test_single_step",           .test = test_single_step           },
  { .name = "10. test_jumps",                 .test = test_jumps                 },
  { .name = "11. test_jumps_after_branch",    .test = test_jumps_after_branch    },
  { .name = "12. test_branch",                .test = test_branch                },
  { .name = "13. test_wfi_sleep",             .test = test_wfi_sleep             },
  { .name = "14. test_access_while_sleep",    .test = test_access_while_sleep    },

  { .name = "finish",                         .test = test_finish                },
  {0, 0}
};

volatile uint32_t testcase_current;
volatile uint32_t tb_errors;

//----------------------------------------------------------------------------
// GLOBAL VARIABLE TO LET THEM BE ACCESSIBLE FROM SPI DEBUG (L2 MEMORY)
//----------------------------------------------------------------------------
volatile uint32_t changeme;
volatile uint32_t act_hw_ss;


int main() {
  if (plp_isFc())
    return run_suite(testcases);
}

void test_init(testresult_t *result, void (*start)(), void (*stop)()) {
  //----------------------------------------------------------------------------
  // 1. Stop and tell TB about our testcase_current variable
  //----------------------------------------------------------------------------
  asm volatile ("csrw 0x300, 0x0;"
                "li x15, 0x04576763;"
                "addi x1, x0, 1;"
                "ebreak;"
                //This branch will end because the debug is writing "1" to x15
                "init_branch: bne x15, x1, init_branch;" ::: "x15", "x1");

  int coreid = get_core_id();

  testcase_current = 0;
  tb_errors        = 0;

  asm volatile ("mv x14, %[current];"
                "mv x15, %[tb_errors];"
                "ebreak" :: [current] "r" (&testcase_current), [tb_errors] "r" (&tb_errors) : "x14", "x15");

  printf("Core %d finished its initialization\n",coreid );

}

//----------------------------------------------------------------------------
// 2. Fill GPRs with well known data
//----------------------------------------------------------------------------
void test_rw_gpr(testresult_t *result, void (*start)(), void (*stop)()) {
  uint32_t a, b, c;
  int coreid = get_core_id();

  testcase_current = 2;

  asm volatile ("li x5, 0x55555555;"
                "li x6, 0x12345666;"
                "li x7, 0x77;"
                "li x8, 0x88;"
                "li x15, 0x31;"
                "ebreak;"
                ::: "x5", "x6", "x7", "x8", "x15");

  // now read back some values written by the debug system
  asm volatile ("ebreak;"
                "mv %[a], x5;"
                "mv %[b], x6;"
                "mv %[c], x15;"
                : [a] "=r" (a), [b] "=r" (b), [c] "=r" (c) :: "x5", "x6", "x15");

  check_uint32(result, "x5",  a, 0x54321089);
  check_uint32(result, "x6",  b, 0x11223344);
  check_uint32(result, "x15", c, 0xFEDCBA09);
  result->errors += tb_errors;
}

void test_finish(testresult_t *result, void (*start)(), void (*stop)()) {

  int coreid = get_core_id();
  testcase_current = 0xFFFFFFFF;

  asm volatile ("ebreak;"
                "csrw 0x300, 0x8;");

  result->errors += tb_errors;

}

void test_rw_csr(testresult_t *result, void (*start)(), void (*stop)()) {
  uint32_t a, b;

  testcase_current = 3;

  asm volatile ("csrw 0x300, %[a];"
                "csrw 0x341, %[b];"
                "ebreak;" :: [a] "r" (0x0), [b] "r" (0x12345678));

  // now read back some values written by the debug system
  asm volatile ("ebreak;"
                "csrr %[a], 0x300;"
                "csrr %[b], 0x341;"
                : [a] "=r" (a), [b] "=r" (b));

  check_uint32(result, "mstatus",  a, 0x00001880);
  check_uint32(result, "mepc",     b, 0x87654321);
}

//----------------------------------------------------------------------------
// 4. Check Debug Register access
//----------------------------------------------------------------------------
void test_rw_dbg_regs(testresult_t *result, void (*start)(), void (*stop)()) {
  uint32_t a, b;

  testcase_current = 4;

  asm volatile ("ebreak");
}

//----------------------------------------------------------------------------
// 5. Halt and resume core a couple of times
//----------------------------------------------------------------------------
void test_halt_resume(testresult_t *result, void (*start)(), void (*stop)()) {
  uint32_t a, b;
  volatile uint32_t changeme = 1;

  testcase_current = 5;

  asm volatile ("mv x14, %[addr];"
                "ebreak" :: [addr] "r" (&changeme) : "x14");

  while(changeme);

  asm volatile ("ebreak");
}
//----------------------------------------------------------------------------
// 6. ebreak, ebreak, ebreak
//
// Check if PPC and NPC work correctly
//----------------------------------------------------------------------------
void test_ebreak(testresult_t *result, void (*start)(), void (*stop)()) {
  testcase_current = 6;

  asm volatile ("nop;"
                "ebreak;"
                "ebreak;"
                "ebreak;"
                "ebreak;"
                "ebreak;"
                "ebreak;"
                "ebreak;"
                "ebreak;"
                "ebreak;"
                "ebreak;");
}

//----------------------------------------------------------------------------
// 7. NPC and PPC values
//----------------------------------------------------------------------------
void test_npc_ppc(testresult_t *result, void (*start)(), void (*stop)()) {
  testcase_current = 7;

  asm volatile ("ebreak");

  asm volatile ("la x13, before;"
                "lui x14, 0x1c008;"
                "addi x14, x14, 0x088;"
                "la x15, after;"
                "        nop;"
                "before: ecall;"
                "after:  nop"
                ::: "x13", "x14", "x15");
}

//----------------------------------------------------------------------------
// 8. Illegal Instruction Exception
//----------------------------------------------------------------------------
void test_illegal(testresult_t *result, void (*start)(), void (*stop)()) {
  testcase_current = 8;

  asm volatile ("ebreak");

  asm volatile ("la x13,     before_ill;"
                "lui x14, 0x1c008;"
                "addi x14, x14, 0x084;"
                "la x15,     after_ill;"
                "            nop;"
                "before_ill: .word 0xF0F0F0F;"
                "after_ill:  nop"
                ::: "x13", "x14", "x15");
}

//----------------------------------------------------------------------------
// 9. Single Step
//----------------------------------------------------------------------------
void test_single_step(testresult_t *result, void (*start)(), void (*stop)()) {
  volatile uint32_t changeme = 1;
  volatile uint32_t act = 0;
  testcase_current = 9;

  // check single-step with addi [and mul]
  asm volatile ("ebreak;"
                "addi %[a], %[a], 1;"
                "addi %[a], %[a], 2;"
                "addi %[a], %[a], 4;"
                "addi %[a], %[a], 8;"
                "p.mul  %[a], %[a], %[a];"
                : [a] "+r" (act));

  check_uint32(result, "addi single step", act, (1 + 2 + 4 + 8)*(1 + 2 + 4 + 8));

  //----------------------------------------------------------------------------
  // check tight branch loop with load
  //----------------------------------------------------------------------------
  asm volatile ("ebreak;"
                "mv x14, %[addr];"
                "ss_start: lw x15, 0(x14);"
                "          bne x0, x15, ss_start;"
                :: [addr] "r" (&changeme) : "x14", "x15");

  //----------------------------------------------------------------------------
  // check tight branch loop with only bf, so we have to jump out
  //----------------------------------------------------------------------------
  asm volatile ("ebreak;"
                "la x14, tb_jump;"
                "tb_start: beq x0, x0, tb_start;"
                "tb_jump:  nop;" : : : "x14");

  //----------------------------------------------------------------------------
  // check tight loop with no exit, so we have to jump out
  //----------------------------------------------------------------------------
  asm volatile ("ebreak;"
                "la x15, tl_jump;"
                "tl_start: j tl_start;"
                "tl_jump:  nop;" : : : "x15");

  //----------------------------------------------------------------------------
  // check tight loop with no exit, so we have to jump out
  //----------------------------------------------------------------------------

  asm volatile (
                "la x13, tc_jump;"
                "la x14, tc_ecall;"
                "lui x15, 0x1c008;"
                "addi x15, x15, 0x088;"
                "ebreak;"
                "tc_ecall: ecall;"
                "tc_jump:  nop;" : : : "x13", "x14", "x15");

}

//----------------------------------------------------------------------------
// 10. Jumps with NPC setting
//----------------------------------------------------------------------------
void test_jumps(testresult_t *result, void (*start)(), void (*stop)()) {
  uint32_t act = 0;
  testcase_current = 10;

  // check jumps with addi
  asm volatile ("la x15, jmp1;"
                "ebreak;"
                "      addi %[a], %[a], 1;"
                "      addi %[a], %[a], 2;"
                "jmp1: addi %[a], %[a], 4;"
                "      addi %[a], %[a], 8;"
                : [a] "+r" (act)
                :: "x15");

  check_uint32(result, "jmp1", act, 4 + 8);

  // check jumps to ebreak
  asm volatile ("la x15, jmpe;"
                "ebreak;"
                "      j jmpd;"
                "      nop;"
                "jmpe: ebreak;"
                "      nop;"
                "jmpd: nop;"
                ::: "x15");
}

//----------------------------------------------------------------------------
// 11. Jumps after Branch, in single-stepping mode
//----------------------------------------------------------------------------
void test_jumps_after_branch(testresult_t *result, void (*start)(), void (*stop)()) {
  uint32_t act = 0;
  testcase_current = 11;

  // check jumps after branch taken
  asm volatile ("        la  x12, bt_11;"
                "        la  x13, jmp_11;"
                "        la  x14, pc0_11;"
                "        la  x15, pc1_11;"
                "        ebreak;"
                "pc0_11: addi %[a], x0, 4;"
                "pc1_11: beq x0, x0, bt_11;"
                "        j jmp_11;"
                "        nop;"
                "bt_11:  addi %[a], x0, 2;"
                "jmp_11: nop;"
                : [a] "=r" (act)
                ::  "x12", "x13", "x14", "x15");

  check_uint32(result, "branch_aft_jmp_t", act, 2);

  // check jumps after branch not taken
  asm volatile ("          la  x12, pc2_11_2;"
                "          la  x13, jmp_11_2;"
                "          la  x14, pc0_11_2;"
                "          la  x15, pc1_11_2;"
                "          ebreak;"
                "pc0_11_2: addi %[a], x0, 4;"
                "pc1_11_2: beq x0, x12, bt_11_2;"
                "pc2_11_2: j jmp_11_2;"
                "bt_11_2:  addi %[a], x0, 2;"
                "jmp_11_2: nop;"
                : [a] "=r" (act)
                ::  "x12", "x13", "x14", "x15");

  check_uint32(result, "branch_aft_jmp_nt", act, 4);
}

//----------------------------------------------------------------------------
// 12. Branch Taken and not taken, in single-stepping mode
//----------------------------------------------------------------------------
void test_branch(testresult_t *result, void (*start)(), void (*stop)()) {
  uint32_t act = 0;
  testcase_current = 12;

  // check branch taken toward
  asm volatile ("         la  x12, pc_btt_0;"
                "         la  x13, pc_btt_1;"
                "         la  x14, pc_btt_2;"
                "         la  x15, pc_btt_3;"
                "         ebreak;"
                "pc_btt_0: addi %[a], %[a], 4;"
                "pc_btt_1: beq x0, x0, pc_btt_2;"
                "         j pc_btt_0;"
                "pc_btt_2: addi %[a], %[a], 2;"
                "pc_btt_3: nop;"
                : [a] "+r" (act)
                ::  "x12", "x13", "x14", "x15");

  check_uint32(result, "branch_taken_toward", act, 4+2);

  act = 0;
  // check branch not taken toward
  asm volatile ("         la  x12, pc_bnt_0;"
                "         la  x13, pc_bnt_1;"
                "         la  x14, pc_bnt_2;"
                "         la  x15, pc_bnt_3;"
                "         addi %[a], %[a], 4;"
                "         ebreak;"
                "pc_bnt_0: bne x0, x0, pc_bnt_2;"
                "pc_bnt_1: j pc_bnt_3;"
                "pc_bnt_2: addi %[a], %[a], 2;"
                "pc_bnt_3: nop;"
                : [a] "+r" (act)
                ::  "x12", "x13", "x14", "x15");

  check_uint32(result, "branch_not_taken_toward", act, 4);

  act = 0;
  // check branch taken backward
  asm volatile ("         la  x12, pc_btb_0;"
                "         la  x13, pc_btb_1;"
                "         la  x14, pc_btb_2;"
                "         la  x15, pc_btb_3;"
                "         ebreak;"
                "pc_btb_0: addi %[a], %[a], 1;"
                "pc_btb_1: bne %[k], %[a], pc_btb_0;"
                "pc_btb_2: j pc_btb_3;"
                "         addi %[a], %[a], 2;"
                "pc_btb_3: nop;"
                : [a] "+r" (act)
                : [k] "r"  (10)
                :  "x12", "x13", "x14", "x15");

  check_uint32(result, "branch_taken_backward", act, 10);

  act = 0;
  // check branch not taken backward
  asm volatile ("         la  x12, pc_bnb_0;"
                "         la  x13, pc_bnb_1;"
                "         la  x14, pc_bnb_2;"
                "         la  x15, pc_bnb_3;"
                "         ebreak;"
                "pc_bnb_0: addi %[a], %[a], 1;"
                "pc_bnb_1: beq %[k], %[a], pc_bnb_0;"
                "pc_bnb_2: j pc_bnb_3;"
                "pc_bnb_3: addi %[a], %[a], 2;"
                : [a] "+r" (act)
                : [k] "r"  (10)
                :  "x12", "x13", "x14", "x15");

  check_uint32(result, "branch_not_taken_backward", act, 1+2);
}

//----------------------------------------------------------------------------
// 13. WFI and sleep
//----------------------------------------------------------------------------
void test_wfi_sleep(testresult_t *result, void (*start)(), void (*stop)()) {
  uint32_t volatile act = 0;
  testcase_current = 13;

  uint32_t int_eu_mask = hal_itc_enable_value_get();
  hal_itc_enable_value_set(0);

  //----------------------------------------------------------------------------
  // single step mode
  //----------------------------------------------------------------------------
  asm volatile ("la x13, WFI_PPC_1;"
                "la x14, WFI_NPC_1;"
                "la x15, WFI_NPC_2;"
                "ebreak;"
                "WFI_PPC_1: wfi;"
                "WFI_NPC_1: addi %[act], %[act], 1;"
                "WFI_NPC_2: addi %[act], %[act], 2;"
                : [act] "+r" (act)
                :
                : "x13", "x14", "x15");

  check_uint32(result, "wfi", act, 1+2);

  //----------------------------------------------------------------------------
  // single step mode
  //----------------------------------------------------------------------------
  act = 0;
  asm volatile ("la x11, SLEEP_SS_NPC_1;"
                "la x12, SLEEP_SS_NPC_2;"
                "la x13, SLEEP_SS_NPC_3;"
                "la x14, SLEEP_SS_NPC_4;"
                "ebreak;"
                "SLEEP_SS_NPC_1: wfi;"
                "SLEEP_SS_NPC_2: addi %[act], %[act], 10;"   //this instruction will be skipted by the TB
                "SLEEP_SS_NPC_3: addi %[act], %[act], 10;"
                "SLEEP_SS_NPC_4: nop;"
                : [act]  "+r" (act)
                : : "x11", "x12", "x13", "x14");

  check_uint32(result, "sleep_ss", act, 10);


  hal_itc_enable_value_set(int_eu_mask);

}

//----------------------------------------------------------------------------
// 14. Access while sleep
//----------------------------------------------------------------------------
void test_access_while_sleep(testresult_t *result, void (*start)(), void (*stop)()) {
  uint32_t volatile gpr = 0;
  uint32_t volatile csr = 0xB15B00B5;
  testcase_current = 14;

  uint32_t int_eu_mask = hal_itc_enable_value_get();
  hal_itc_enable_value_set(0);

  // check if we can access internal registers while the core is sleeping
  // This includes GPR, CSR and Debug Registers
  asm volatile ("         ebreak;"
                "         li x12, 0x16161616;"
                "         li x13, 0x17171717;"
                "         la x14, npc_wfi;"
                "         la x15, ppc_wfi;"
                "         csrw 0x780, %[csr];"
                "ppc_wfi: wfi;"
                "npc_wfi: mv   %[gpr], x13;"
                "         csrr %[csr], 0x780;"
                : [gpr] "+r"       ( gpr        ),
                  [csr] "+r"       ( csr        )
                : : "x12", "x13", "x14", "x15");

  check_uint32(result, "gpr written", gpr, 0xFEDCBA00);
  check_uint32(result, "csr written", csr, 0xC0DE1234);

  // Identical to the one before, but this time we exit the sleep mode by setting the NPC register
  asm volatile ("         ebreak;"
                "         li x13, 0x16161616;"
                "         li x14, 0x17171717;"
                "         la x15, npc_wfi2;"
                "         csrw 0x780, %[csr];"
                "         wfi;"
                "npc_wfi2: mv   %[gpr], x13;"
                "         csrr %[csr], 0x780;"
                : [gpr] "+r"       ( gpr        ),
                  [csr] "+r"       ( csr        )
                : : "x13", "x14", "x15");

  check_uint32(result, "gpr written", gpr, 0xFEDCBA00);
  check_uint32(result, "csr written", csr, 0xC0DE1234);

  hal_itc_enable_value_set(int_eu_mask);

}
