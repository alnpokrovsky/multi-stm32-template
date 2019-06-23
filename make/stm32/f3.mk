LIBNAME		= opencm3_stm32f3
DEFS		+= -DSTM32F3

FP_FLAGS	?= -mfloat-abi=hard -mfpu=fpv4-sp-d16
ARCH_FLAGS	= -mthumb -mcpu=cortex-m4 $(FP_FLAGS)

################################################################################
# OpenOCD specific variables

OOCD		?= openocd
OOCD_INTERFACE	?= stlink-v2
OOCD_TARGET	?= stm32f3x

################################################################################
# Black Magic Probe specific variables
# Set the BMP_PORT to a serial port and then BMP is used for flashing
BMP_PORT	?=

################################################################################
# texane/stlink specific variables
#STLINK_PORT	?= :4242


include core/libopencm3.include
include rules.mk
