DEFS		+= -DUSE_MDR1986VE9x
LDSCRIPT := std-per-lib/CMSIS/CM3/DeviceSupport/MDR32F9Qx/startup/gcc/MDR32F9Qx.ld

FP_FLAGS	?= -msoft-float
ARCH_FLAGS	:= -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd

include core/emdr1986x-std-per-lib.include

SRC += SRC_MDR_VE9

INC += INC_MDR_VE9

LDSCRIPT := LD_MDR_VE9

include rules.mk
