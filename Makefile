# Makefile

PROJECT := techPult

########## INCLUDES #############
# my configs
INC += src/config
# my macro utils
INC += src/third-party/utils
# freemodbus 1.5 lib
INC += src/third-party/modbus/include src/third-party/modbus/port src/third-party/modbus/rtu
# freertos
INC += src/third-party/freertos/include
# freertos-addons
INC += src/third-party/freertos-addons/c/Source/include src/third-party/freertos-addons/c++/Source/include

LDSCRIPT := make/stm32/f1/stm32f103xB.ld
include make/stm32/f1.include
