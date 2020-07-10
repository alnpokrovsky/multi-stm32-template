# Makefile

###### default params ##########
BUILD ?= debug
DEFS += -DHSE_VALUE=8000000

###### Create binary ###########
all: elf


########## SOURCES ##############
SRC += $(shell find src -name "*.c")
SRC += $(shell find src -name "*.cpp")

########## INCLUDES #############
# my configs
INC += src/config
# my macro utils
INC += src/utils
# my hardware abstraction level
INC += src/hardware/include


########### LIBRARIES ###############
include libs/freertos.include
# include libs/freemodbus.include
# include libs/fatfs.include
# include libs/libopencm3-usb.include
# include libs/saxml.include
# include libs/ugui.include
include libs/lvgl.include
# include libs/lua.include
include HAL/stm32f7.include

########### BUILD DEVICE ##############
# LDSCRIPT := make/stm32/f1/stm32f103x8.ld
# include make/stm32/f1.mk
# LDSCRIPT := make/stm32/f3/stm32f334.ld
# include make/stm32/f3.mk
# LDSCRIPT := make/stm32/f4/stm32f407.ld
# LDSCRIPT := make/stm32/f4/stm32f429.ld
# include make/stm32/f4.mk
# LDSCRIPT := make/stm32/f7/stm32f746.ld
# include make/stm32/f7.mk
# include make/mdr1986/ve9.mk

DEVICE = STM32F746xx
include CMSIS/stm32f7.mk
include make/rules.mk
