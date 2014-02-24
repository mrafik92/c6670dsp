#*******************************************************************************
#* FILE PURPOSE: Top level makefile for Creating Platform Libraries
#*******************************************************************************
#* FILE NAME: makefile
#*
#* DESCRIPTION: Defines Compiler tools paths, libraries , Build Options 
#*
#*
#*******************************************************************************
#*
# (Mandatory) Specify where various tools are installed.

# Output for prebuilt generated libraries
export LIBDIR ?= ./lib

# Library directory macros
export ARDBGDIR = debug

# ROOT Directory
export ROOTDIR := ../../../..

# INCLUDE Directory
export INCDIR := ../../../..;$(C6X_GEN_INSTALL_PATH)/include;$(PDK_INSTALL_PATH)/ti/platform/simc6670/platform_lib/include/;$(PDK_INSTALL_PATH)/.;$(PDK_INSTALL_PATH)/ti/csl/.;$(PDK_INSTALL_PATH)/ti/platform/.;$(ROOTDIR)

# Common Macros used in make

ifndef RM
export RM = rm -f
endif

ifndef CP
export CP = cp -p
endif

export MKDIR = mkdir -p

ifndef RMDIR
export RMDIR = rm -rf
endif

ifndef SED
export SED = sed
endif

ifndef MAKE
export MAKE = make
endif

# PHONY Targets
.PHONY: all clean cleanall 

# FORCE Targets
FORCE: 

# all rule
all: .executables
.executables: .libraries
.libraries:

# Clean Rule
clean:: clean_package
# Clean Top Level Object Directory 
clean_package :
	$(RMDIR) $(LIBDIR)/*/
	$(RMDIR) package/cfg

# Make rule to create $(LIBDIR)/$(ARDBGDIR)/ti.platform.simc6670.ae66 library
.libraries: $(LIBDIR)/$(ARDBGDIR)/ti.platform.simc6670.ae66
$(LIBDIR)/$(ARDBGDIR)/ti.platform.simc6670.ae66: FORCE
	$(MAKE) -f ./lib/ti.platform.simc6670.ae66.mk $@
# Rule to clean $(LIBDIR)/$(ARDBGDIR)/ti.platform.simc6670.ae66 library
clean ::
	$(RM) $(LIBDIR)/$(ARDBGDIR)/ti.platform.simc6670.ae66

# Make rule to create $(LIBDIR)/$(ARDBGDIR)/ti.platform.simc6670.ae66e library
.libraries: $(LIBDIR)/$(ARDBGDIR)/ti.platform.simc6670.ae66e
$(LIBDIR)/$(ARDBGDIR)/ti.platform.simc6670.ae66e: FORCE
	$(MAKE) -f ./lib/ti.platform.simc6670.ae66e.mk $@
# Rule to clean $(LIBDIR)/$(ARDBGDIR)/ti.platform.simc6670.ae66e library
clean ::
	$(RM) $(LIBDIR)/$(ARDBGDIR)/ti.platform.simc6670.ae66e	

