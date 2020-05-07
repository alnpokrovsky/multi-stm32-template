DEFS		+= -DUSE_MDR1986VE9x
LDSCRIPT := std-per-lib/CMSIS/CM3/DeviceSupport/MDR32F9Qx/startup/gcc/MDR32F9Qx.ld

FP_FLAGS	?= -msoft-float
ARCH_FLAGS	:= -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd

################################################################################
# OpenOCD specific variables

OOCD			?= openocd
OOCD_INTERFACE	?= stlink
OOCD_TARGET		?= mdr32f9q2i

###########################################################
# Sources

include core/emdr1986x-std-per-lib.include

SRC += $(SRC_MDR_VE9)

INC += $(INC_MDR_VE9)

LDSCRIPT := $(LD_MDR_VE9)

include make/rules.mk
