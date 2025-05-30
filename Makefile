##-*- makefile -*-############################################################
#
#  System        : 
#  Module        : 
#  Object Name   : $RCSfile$
#  Revision      : $Revision$
#  Date          : $Date$
#  Author        : $Author$
#  Created By    : Robert Heller
#  Created       : Tue May 27 11:38:41 2025
#  Last Modified : <250528.1947>
#
#  Description	
#
#  Notes
#
#  History
#	
#  $Log$
#
##############################################################################
#
#    Copyright (C) 2025  Robert Heller D/B/A Deepwoods Software
#			51 Locke Hill Road
#			Wendell, MA 01379-9728
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# 
#
##############################################################################


all: Sim6502 Sim6502.pdf


Sim6502: main.o cpu.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@
	
%.o : %.cxx
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

main.o: main.cxx cpu.hxx memory.hxx

cpu.o: cpu.cxx cpu.hxx memory.hxx 

Sim6502.pdf: latex/refman.pdf
	mv latex/refman.pdf Sim6502.pdf
	

latex/refman.pdf html/index.html: cpu.hxx memory.hxx
	-rm -rf latex/ html/
	doxygen Doxyfile
	$(MAKE) -C latex

