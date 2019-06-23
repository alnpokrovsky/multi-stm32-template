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


########### LIBRARIES ###############



########### BUILD DEVICE ##############
LDSCRIPT := make/stm32/f1/stm32f103xB.ld
include make/stm32/f1.mk
#include make/mdr1986/ve9.mk