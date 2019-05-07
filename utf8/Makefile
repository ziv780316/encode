#=================================================
# Makefile settings
#=================================================
.PHONY: default create_dir clean # specify phony target, do not generate file as target name

.DEFAULT_GOAL=default # specify default target if there is no target specified

VPATH=asm # specify directory where to check target file update to date or not

SHELL=/bin/bash

#=================================================
# Compiler settings
#=================================================
CC         = gcc
CPP        = g++
LINKER     = $(CC)
CPPLINKER  = $(CPP)
DEFINES    = 
BUILD_TYPE = -O0
CFLAGS     = $(BUILD_TYPE) -Wno-unused-parameter -Wfatal-errors -std=gnu99 -fPIC 
CPPFLAGS   = $(BUILD_TYPE) -Wno-unused-parameter -Wfatal-errors -std=c++98 -fPIC 
LIBS       = -lm 
INCLUDE    = 
MOVE       = mv -f

#=================================================
# Build target
#=================================================
BIN_DIR       	  := bin
LIB_DIR       	  := library
TARGET            := symbol

#=================================================
# Compile implicit rules
#=================================================
%.o:%.c
	@echo "build $(basename $@) ..."
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)
	@$(MOVE) $@ $(LIB_DIR) # @ means do not echo command

#=================================================
# Target rules
#=================================================
symbol: symbol.o
	$(LINKER) -o $@ $(LIB_DIR)/$(notdir $<) $(LIBS);\
	$(MOVE) $@ $(BIN_DIR)

#=================================================
# Phony rules
#=================================================
default: create_dir $(TARGET)

create_dir:
	@echo "create dir ..."
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(BIN_DIR)

clean:
	rm -rf $(LIB_DIR)/* $(BIN_DIR)/*


