# Makefile

PROJECT := techPult

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

# freemodbus 1.5 lib
INC += src/third-party/modbus/include src/third-party/modbus/port src/third-party/modbus/rtu


LDSCRIPT := make/stm32/f1/stm32f103xB.ld
include libs/libopencm3.include
include make/stm32/f1.mk
