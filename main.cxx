// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Tue May 27 11:18:12 2025
//  Last Modified : <250528.1419>
//
//  Description	
//
//  Notes
//
//  History
//	
/////////////////////////////////////////////////////////////////////////////
/// @copyright
///    Copyright (C) 2025  Robert Heller D/B/A Deepwoods Software
///			51 Locke Hill Road
///			Wendell, MA 01379-9728
///
///    This program is free software; you can redistribute it and/or modify
///    it under the terms of the GNU General Public License as published by
///    the Free Software Foundation; either version 2 of the License, or
///    (at your option) any later version.
///
///    This program is distributed in the hope that it will be useful,
///    but WITHOUT ANY WARRANTY; without even the implied warranty of
///    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///    GNU General Public License for more details.
///
///    You should have received a copy of the GNU General Public License
///    along with this program; if not, write to the Free Software
///    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/// @file main.cxx
/// @author Robert Heller
/// @date Tue May 27 11:18:12 2025
/// 
///
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";

#include <iostream>
#include <fstream>
#include <string>
#include "cpu.hxx"
#include "memory.hxx"

int main(int argc, char *argv[])
{
    MixedRAMROM memory;
    if (argc > 1)
    {
        char buffer[150];
        std::ifstream hexfile(argv[1]);
        while (hexfile.getline(buffer,150))
        {
            memory.BurnROM(buffer);
        }
    }
    memory.DumpMemory(0xFF00,256,std::cout);
    cpu6502 Processor(&memory);
    Processor.run(std::cout);
    memory.DumpMemory(0,1024,std::cout);
    return 0;
}
