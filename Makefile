# Makefile

###### default params ##########
BUILD ?= debug
DEVICE = STM32F407xx
DEFS += -DHSE_VALUE=25000000

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
# include libs/freemodbus.include
# include libs/fatfs.include
# include libs/saxml.include
# include libs/ugui.include
# include libs/lvgl.include
# include libs/lua.include
include libs/usb.include

########### BUILD DEVICE ##############
# DEVICE = STM32F746xx
# include core/CMSIS/stm32f7.mk
# include core/HAL/stm32f7.include
include core/CMSIS/stm32f4.mk
include core/HAL/stm32f4.include
# include core/CMSIS/mdr32f9.mk
# include core/emdr1986x-std-per-lib/mdr32f9.include

include make/rules.mk
