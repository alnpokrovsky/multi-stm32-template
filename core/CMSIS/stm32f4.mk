# CMSIS

######## core #######
include core/CMSIS/cmsis.include

####### specific #######

# compile flags
DEFS += -DSTM32F4
DEFS += -DCMSIS_device_header="<stm32f4xx.h>"
FP_FLAGS	?= -mfloat-abi=hard -mfpu=fpv4-sp-d16
ARCH_FLAGS	= -mthumb -mcpu=cortex-m7 $(FP_FLAGS)

# base src and inc
DEVICE_FOLDER := core/CMSIS/Device/ST/STM32F4xx
INC += $(DEVICE_FOLDER)/Include
SRC += $(DEVICE_FOLDER)/Source/system_stm32f4xx.c

# OpenOCD specific variables
OOCD			?= openocd
OOCD_INTERFACE	?= stlink-v2
OOCD_TARGET		?= stm32f4x

####### concrete ######

DEVICE ?= STM32F407xx
DEVICE_LC = $(shell echo $(DEVICE) | tr A-Z a-z)
DEFS += -D$(DEVICE)
SRC += $(DEVICE_FOLDER)/Source/gcc/startup_$(DEVICE_LC).s
LDSCRIPT = $(DEVICE_FOLDER)/Link/$(DEVICE_LC).ld
