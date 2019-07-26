# Makefile

PROJECT := techPult

###### Create binary ###########
BUILD := debug
all: elf


########## SOURCES ##############
SRC += $(shell find src -name "*.c")

########## INCLUDES #############
# my configs
INC += src/config
# my macro utils
INC += src/utils
# my hardware abstraction level
INC += src/hardware/include
# uGUI lib
INC += src/third-party/ugui


########### LIBRARIES ###############
include libs/freemodbus.include
include libs/fatfs.include
include libs/libopencm3-usb.include
include libs/saxml.include

########### BUILD DEVICE ##############
LDSCRIPT := make/stm32/f1/stm32f103xB.ld
include make/stm32/f1.mk
#LDSCRIPT := make/stm32/f4/stm32f407.ld
#include make/stm32/f4.mk
#include make/mdr1986/ve9.mk