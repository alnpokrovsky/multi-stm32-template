##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2010 Piotr Esden-Tempski <piotr@esden.net>
## Copyright (C) 2013 Frantisek Burian <BuFran@seznam.cz>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##


all: bin

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q			:= @
NULL		:= 2>/dev/null
endif

# 'make BUILD=debug' will compile with debugging symbols 
# By default builds debug version with optimization 
ifeq ($(BUILD),debug)
OPT	  := -O0
DEBUG := -ggdb3
else
OPT	  := -Os
DEBUG := -g0
endif

###############################################################################
# Optional

PROJECT		?= main
INC 		+= src
BUILD_DIR 	?= build
CSTD		?= -std=c99

###############################################################################
# Executables

PREFIX	:= arm-none-eabi
CC		:= $(PREFIX)-gcc
CXX		:= $(PREFIX)-g++
LD		:= $(PREFIX)-gcc
AR		:= $(PREFIX)-ar
AS		:= $(PREFIX)-as
OBJCOPY	:= $(PREFIX)-objcopy
OBJDUMP	:= $(PREFIX)-objdump
GDB		:= $(PREFIX)-gdb
STFLASH	:= $(shell which st-flash)


###############################################################################
# C flags

TGT_CFLAGS	+= $(OPT) $(CSTD) $(DEBUG)
TGT_CFLAGS	+= $(ARCH_FLAGS)
TGT_CFLAGS	+= -Wextra -Wshadow -Wimplicit-function-declaration -Werror
TGT_CFLAGS	+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
TGT_CFLAGS	+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C++ flags

TGT_CXXFLAGS	+= $(OPT) $(CXXSTD) $(DEBUG)
TGT_CXXFLAGS	+= $(ARCH_FLAGS)
TGT_CXXFLAGS	+= -Wextra -Wshadow -Wredundant-decls  -Weffc++
TGT_CXXFLAGS	+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# Assembler flags

TGT_ASFLAGS += $(OPT) $(ARCH_FLAGS) -ggdb3

###############################################################################
# C & C++ preprocessor common flags

TGT_CPPFLAGS	+= -MD
TGT_CPPFLAGS	+= -Wall -Wundef
TGT_CPPFLAGS	+= $(DEFS)
TGT_CPPFLAGS	+= $(addprefix -I, $(INC))

###############################################################################
# Linker flags

TGT_LDFLAGS		+= --static -nostartfiles
TGT_LDFLAGS		+= -T$(LDSCRIPT)
TGT_LDFLAGS		+= $(ARCH_FLAGS) $(DEBUG)
TGT_LDFLAGS 	+= -specs=nano.specs
TGT_LDFLAGS 	+= -specs=nosys.specs
TGT_LDFLAGS 	+= -Wl,--print-memory-usage
TGT_LDFLAGS		+= -Wl,-Map=$(*).map -Wl,--cref
TGT_LDFLAGS		+= -Wl,--gc-sections
ifeq ($(V),99)
TGT_LDFLAGS		+= -Wl,--print-gc-sections
endif

###############################################################################
# Used libraries

LDLIBS		+= -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

###############################################################################
###############################################################################
###############################################################################

# Look objectives from source files
SRC_AS := $(filter %.S, $(SRC))
SRC_C := $(filter %.c, $(SRC))
SRC_CXX := $(filter %.cpp, $(SRC))
OBJS_AS := $(SRC_AS:%.S=$(BUILD_DIR)/%.o)
OBJS_C := $(SRC_C:%.c=$(BUILD_DIR)/%.o)
OBJS_CXX := $(SRC_CXX:%.cpp=$(BUILD_DIR)/%.o)
OBJS := $(OBJS_AS) $(OBJS_C) $(OBJS_CXX)


.SUFFIXES: .elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:

elf: $(PROJECT).elf
bin: $(PROJECT).bin
hex: $(PROJECT).hex
srec: $(PROJECT).srec
list: $(PROJECT).list
images: $(PROJECT).images
flash: $(PROJECT).flash


%.elf %.map: $(OBJS) $(LDSCRIPT) $(LIBS_A)
	@printf "  LD \t$(*).elf\n"
	$(Q)$(LD) $(TGT_LDFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(*).elf

%.images: %.bin %.hex %.srec %.list %.map
	@printf "*** $* images generated ***\n"

%.bin: %.elf
	@printf "  OBJCOPY $(*).bin\n"
	$(Q)$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex: %.elf
	@printf "  OBJCOPY $(*).hex\n"
	$(Q)$(OBJCOPY) -Oihex $(*).elf $(*).hex

%.srec: %.elf
	@printf "  OBJCOPY $(*).srec\n"
	$(Q)$(OBJCOPY) -Osrec $(*).elf $(*).srec

%.list: %.elf
	@printf "  OBJDUMP $(*).list\n"
	$(Q)$(OBJDUMP) -S $(*).elf > $(*).list

# Define a helper macro for debugging make errors online
# you can type "make print-SRC" and it will show you
# how that ended up being resolved by all of the included
# makefiles.
print-%:
	@echo $*=$($*)

# assembler rule
$(BUILD_DIR)/%.o: %.S
	@printf "  AS \t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(TGT_ASFLAGS) $(ASFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

# c rule
$(BUILD_DIR)/%.o: %.c
	@printf "  CC \t$(*).c\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

# cxx rule
$(BUILD_DIR)/%.o: %.cpp
	@printf "  CXX \t$(*).cpp\n"
	@mkdir -p $(dir $@)
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

clean:
	@printf "  CLEAN\n"
	$(Q)$(RM) $(PROJECT).elf $(PROJECT).bin $(PROJECT).hex $(PROJECT).srec $(PROJECT).list $(PROJECT).map generated.* $(OBJS) $(OBJS:%.o=%.d)
	$(Q)$(RM) -r $(BUILD_DIR)


%.stlink-flash: %.bin
	@printf "  FLASH  $<\n"
	$(STFLASH) write $(*).bin 0x8000000

%.flash: %.elf
	@printf "  FLASH\t$<\n"
ifeq (,$(OOCD_FILE))
	$(Q)(echo "halt; program $(realpath $(*).elf) verify reset" | nc -4 localhost 4444 2>/dev/null) || \
		$(OOCD) -f interface/$(OOCD_INTERFACE).cfg \
		-f target/$(OOCD_TARGET).cfg \
		-c "program $(realpath $(*).elf) verify reset exit" \
		$(NULL)
else
	$(Q)(echo "halt; program $(realpath $(*).elf) verify reset" | nc -4 localhost 4444 2>/dev/null) || \
		$(OOCD) -f $(OOCD_FILE) \
		-c "program $(realpath $(*).elf) verify reset exit" \
		$(NULL)
endif

.PHONY: images clean elf bin hex srec list

-include $(OBJS:.o=.d)
