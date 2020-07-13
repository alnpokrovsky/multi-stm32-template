# CMSIS

######## core #######
include core/CMSIS/cmsis.include

####### specific #######

# compile flags
DEFS += -DUSE_MDR1986VE9x
DEFS += -DCMSIS_device_header="<MDR32Fx.h>"
DEFS += -D__STARTUP_CLEAR_BSS -D__START=main
FP_FLAGS	?= -msoft-float
ARCH_FLAGS	:= -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd

################################################################################
# OpenOCD specific variables

OOCD			?= openocd
OOCD_INTERFACE	?= stlink-v2
OOCD_TARGET		?= mdr32f9q2i

###########################################################
# Sources

# base src and inc
DEVICE_FOLDER := core/CMSIS/Device/Milandr/MDR32F9Qx
INC += $(DEVICE_FOLDER)/Include
SRC += $(DEVICE_FOLDER)/Source/system_MDR32F9Qx.c


####### concrete ######

DEVICE ?= MDR32F9Qx
DEFS += -D$(DEVICE)
SRC += $(DEVICE_FOLDER)/Source/gcc/startup_$(DEVICE).s
LDSCRIPT = $(DEVICE_FOLDER)/Link/$(DEVICE).ld
