#*******************************************************************************
#* FILE PURPOSE: Lower level makefile for Creating Platform Debug Libraries
#*******************************************************************************
#* FILE NAME: ./lib/ti.platform.evm6670l.dbg.ae66.mk
#*
#* DESCRIPTION: Defines Source Files, Compilers flags and build rules
#*
#*
#*******************************************************************************
#

#
# Macro definitions referenced below
#
empty =
space =$(empty) $(empty)
CC = $(C6X_GEN_INSTALL_PATH)/bin/cl6x -c -mo -g -q -k -eo.o -mv6600 --abi=elfabi
AC = $(C6X_GEN_INSTALL_PATH)/bin/cl6x -c -qq -mv6600 --abi=elfabi
ARIN = $(C6X_GEN_INSTALL_PATH)/bin/ar6x rq
LD = $(C6X_GEN_INSTALL_PATH)/bin/lnk6x --abi=elfabi
RTSLIB = -l $(C6X_GEN_INSTALL_PATH)/lib/rts6600_elf.lib
INCS = -I. -I$(strip $(subst ;, -I,$(subst $(space),\$(space),$(INCDIR))))
OBJEXT = dbg.oe66
AOBJEXT = dbg.se66
INTERNALDEFS = -Dti_targets_elf_C66  -DMAKEFILE_BUILD -eo.$(OBJEXT) -ea.$(AOBJEXT) -fr=$(@D) -fs=$(@D) -ppa -ppd=$@.dep
INTERNALLINKDEFS = -o $@ -m $@.map
OBJDIR = $(LIBDIR)

#List the COMMONSRC Files
COMMONSRCC= \
    src/evm66x_i2c.c \
    src/evm66x_nand.c \
    src/evmc6670.c \
    src/evmc6670_phy.c \
    src/evmc66x_ecc.c \
    src/evmc66x_fpga.c \
    src/evmc66x_gpio.c \
    src/evmc66x_i2c_eeprom.c \
    src/evmc66x_nor.c \
    src/evmc66x_spi.c \
    src/evmc66x_uart.c \
    src/platform.c 

# FLAGS for the COMMONSRC Files
COMMONSRCCFLAGS =   -i./src -i. -i./test 

# Make Rule for the COMMONSRC Files
COMMONSRCCOBJS = $(patsubst %.c, $(OBJDIR)/%.$(OBJEXT), $(COMMONSRCC))

$(COMMONSRCCOBJS): $(OBJDIR)/%.$(OBJEXT): %.c
	-@echo cle66 $< ...
	if [ ! -d $(@D) ]; then $(MKDIR) $(@D) ; fi;
	$(RM) $@.dep
	$(CC) $(COMMONSRCCFLAGS) $(INTERNALDEFS) $(INCS) -fc $< 
	-@$(CP) $@.dep $@.pp; \
         $(SED) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
             -e '/^$$/ d' -e 's/$$/ :/' < $@.pp >> $@.dep; \
         $(RM) $@.pp 

#Create Empty rule for dependency
$(COMMONSRCCOBJS):.\lib\ti.platform.evm6670l.dbg.ae66.mk
.\lib\ti.platform.evm6670l.dbg.ae66.mk:

#Include Depedency for COMMONSRC Files
ifneq (clean,$(MAKECMDGOALS))
 -include $(COMMONSRCCOBJS:%.$(OBJEXT)=%.$(OBJEXT).dep)
endif


$(LIBDIR)/$(ARDBGDIR)/ti.platform.evm6670l.ae66 : $(COMMONSRCCOBJS)
	@echo archiving $? into $@ ...
	if [ ! -d $(LIBDIR)/$(ARDBGDIR) ]; then $(MKDIR) $(LIBDIR)/$(ARDBGDIR) ; fi;
	$(ARIN) $@ $?

