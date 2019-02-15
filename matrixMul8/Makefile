PULP_APP = test
PULP_APP_SRCS = matrixMul.c matmul_kernels.c

PULP_FC_ARCH_CFLAGS = -march=rv32imcxpulpv2 -D__riscv__
#zero-riscy:   -march=rv32imcxpulpslim -DRV_ISA_RV32=1 -D__riscv__
#riscy:        -march=rv32imcxpulpv2 -D__riscv__

PULP_CFLAGS += -DWORD=$(WORD) -O3 -g

include $(PULP_SDK_HOME)/install/rules/pulp.mk
