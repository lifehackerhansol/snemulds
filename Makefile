#
#			Copyright (C) 2017  Coto
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful, but
#WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
#USA
#

#TGDS1.5 compatible Makefile

#ToolchainGenericDS specific: Use Makefiles from either TGDS, or custom
export SOURCE_MAKEFILE7 = custom
export SOURCE_MAKEFILE9 = custom

#Non FPIC Code: Use Makefiles from either TGDS, or custom
#FPIC code is always default TGDS Makefile

#Shared
include $(DEFAULT_GCC_PATH)Makefile.basenewlib

#Custom
# Project Specific
export EXECUTABLE_FNAME = SNEmulDS.nds
export EXECUTABLE_VERSION_HEADER =	0.6b
export EXECUTABLE_VERSION =	"$(EXECUTABLE_VERSION_HEADER)"

#The ndstool I use requires to have the elf section removed, so these rules create elf headerless- binaries.
export BINSTRIP_RULE_7 =	arm7.bin
export BINSTRIP_RULE_9 =	arm9.bin
export BUILD_ARM7	=	build
export BUILD_ARM9	=	build
export ELF_ARM7 = arm7.elf
export ELF_ARM9 = arm9.elf
export NONSTRIPELF_ARM7 = arm7-nonstripped.elf
export NONSTRIPELF_ARM9 = arm9-nonstripped.elf

export TARGET_LIBRARY_CRT0_FILE_7 = nds_arm_ld_crt0
export TARGET_LIBRARY_CRT0_FILE_9 = special_nds_arm_ld_crt0
export TARGET_LIBRARY_LINKER_FILE_7 = $(TARGET_LIBRARY_PATH)$(TARGET_LIBRARY_LINKER_SRC)/$(TARGET_LIBRARY_CRT0_FILE_7).S
export TARGET_LIBRARY_LINKER_FILE_9 = $(CURDIR)/$(DIR_ARM9)/$(TARGET_LIBRARY_CRT0_FILE_9).S

export TARGET_LIBRARY_NAME_7 = toolchaingen7
export TARGET_LIBRARY_NAME_9 = toolchaingen9
export TARGET_LIBRARY_FILE_7	=	lib$(TARGET_LIBRARY_NAME_7).a
export TARGET_LIBRARY_FILE_9	=	lib$(TARGET_LIBRARY_NAME_9).a

#####################################################ARM7#####################################################

export DIRS_ARM7_SRC = source/	\
			source/interrupts/	\
			../common/	\
			../common/templateCode/source/	\
			../common/templateCode/data/arm7/		
			
export DIRS_ARM7_HEADER = source/	\
			source/interrupts/	\
			include/	\
			../common/	\
			../common/templateCode/source/	\
			../common/templateCode/data/arm7/	\
			build/	\
			../$(PosIndCodeDIR_FILENAME)/$(DIR_ARM7)/include/

#####################################################ARM9#####################################################

export DIRS_ARM9_SRC = data/	\
			source/	\
			source/interrupts/	\
			source/gui/	\
			source/TGDSMemoryAllocator/	\
			source/wnifilib/	\
			../common/	\
			../common/templateCode/source/	\
			../common/templateCode/data/arm9/	
			
export DIRS_ARM9_HEADER = data/	\
			build/	\
			include/	\
			source/gui/	\
			source/TGDSMemoryAllocator/	\
			source/wnifilib/	\
			../common/	\
			../common/templateCode/source/	\
			../common/templateCode/data/arm9/	\
			../$(PosIndCodeDIR_FILENAME)/$(DIR_ARM9)/include/
			
# Build Target(s)
all: $(EXECUTABLE_FNAME)
#all:	debug

#ignore building this
.PHONY: $(ELF_ARM7)	$(ELF_ARM9)

#Make
compile	:
	-cp	-r	$(TARGET_LIBRARY_MAKEFILES_SRC7_FPIC)	$(CURDIR)/$(PosIndCodeDIR_FILENAME)/$(DIR_ARM7)
	-$(MAKE)	-R	-C	$(PosIndCodeDIR_FILENAME)/$(DIR_ARM7)/
	-cp	-r	$(TARGET_LIBRARY_MAKEFILES_SRC9_FPIC)	$(CURDIR)/$(PosIndCodeDIR_FILENAME)/$(DIR_ARM9)
	-$(MAKE)	-R	-C	$(PosIndCodeDIR_FILENAME)/$(DIR_ARM9)/
ifeq ($(SOURCE_MAKEFILE7),default)
	cp	-r	$(TARGET_LIBRARY_MAKEFILES_SRC7_NOFPIC)	$(CURDIR)/$(DIR_ARM7)
endif
	$(MAKE)	-R	-C	$(DIR_ARM7)/
ifeq ($(SOURCE_MAKEFILE9),default)
	cp	-r	$(TARGET_LIBRARY_MAKEFILES_SRC9_NOFPIC)	$(CURDIR)/$(DIR_ARM9)
endif
	$(MAKE)	-R	-C	$(DIR_ARM9)/

$(EXECUTABLE_FNAME)	:	compile
	-@echo 'ndstool begin'
	$(NDSTOOL)	-v	-c $@	-7  $(CURDIR)/arm7/$(BINSTRIP_RULE_7)	-e7  0x03800000	-9 $(CURDIR)/arm9/$(BINSTRIP_RULE_9) -e9  0x02000000 -b icon.bmp "SNEmulDS $(EXECUTABLE_VERSION_HEADER) ; SNES Emulator for DS; by archeide bubble2k gladius" 
	-@echo 'ndstool end: built: $@'
	
#---------------------------------------------------------------------------------
# Clean
each_obj = $(foreach dirres,$(dir_read_arm9_files),$(dirres).)
	
clean:
	$(MAKE)	clean	-C	$(DIR_ARM7)/
	$(MAKE) clean	-C	$(PosIndCodeDIR_FILENAME)/$(DIR_ARM7)/
ifeq ($(SOURCE_MAKEFILE7),default)
	-@rm -rf $(CURDIR)/$(DIR_ARM7)/Makefile
endif
#--------------------------------------------------------------------
	$(MAKE)	clean	-C	$(DIR_ARM9)/
	$(MAKE) clean	-C	$(PosIndCodeDIR_FILENAME)/$(DIR_ARM9)/
ifeq ($(SOURCE_MAKEFILE9),default)
	-@rm -rf $(CURDIR)/$(DIR_ARM9)/Makefile
endif
	-@rm -rf $(CURDIR)/$(PosIndCodeDIR_FILENAME)/$(DIR_ARM7)/Makefile
	-@rm -rf $(CURDIR)/$(PosIndCodeDIR_FILENAME)/$(DIR_ARM9)/Makefile
	-@rm -fr $(EXECUTABLE_FNAME)

rebase:
	git reset --hard HEAD
	git clean -f -d
	git pull
	
commitChanges:
	-@git commit -a	-m '$(COMMITMSG)'
	-@git push origin HEAD
	