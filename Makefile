# Makefile

PROJECT = techPult
SOURCE_DIR = src

########## INCLUDES #############
# my configs
INCLUDES += $(SOURCE_DIR)/config
# my macro utils
INCLUDES += $(SOURCE_DIR)/third-party/utils
# freemodbus 1.5 lib
INCLUDES += $(SOURCE_DIR)/third-party/modbus/include $(SOURCE_DIR)/third-party/modbus/port $(SOURCE_DIR)/third-party/modbus/rtu
# freertos
INCLUDES += $(SOURCE_DIR)/third-party/freertos/include
# freertos-addons
INCLUDES += $(SOURCE_DIR)/third-party/freertos-addons/c/Source/include $(SOURCE_DIR)/third-party/freertos-addons/c++/Source/include

LDSCRIPT = make/stm32/f1/stm32f103xB.ld
include make/stm32/f1.include
