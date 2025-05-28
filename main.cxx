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
//  Last Modified : <250528.1954>
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
/// @mainpage Sim6502 -- Simple 6502 simulator.
/// 
/// @section SYNOPSIS SYNOPSIS
/// 
/// Sim6502 [-m ramsize] [-r romsize] [-d] hexfile
/// 
/// @section DESCRIPTION DESCRIPTION
/// 
/// Sim6502 is a simple basic 6502 simulator, written in C++. It compiles and
/// runs under Linux on a Raspberry Pi 5, but should compile on any Linux
/// system and will probably compile on any operating system.
/// 
/// The program will execute the program contimiously until interupted with
/// a ^C (SIGINT) at which point if the -d flag was specified, it will dump
/// first 1024 bytes of ram and exit.  
/// 
/// @section OPTIONS OPTIONS
/// 
/// @arg -m ramsize Set the size of the ram to allocate.  Default is 2048.
/// @arg -r romsize Set the size of the rom to allocate.  Default is 2048.
/// @arg -d Dump the first 1024 bytes of ram on program executation.
/// @par
/// 
/// @section PARAMETERS PARAMETERS
/// 
/// A file containing a 6502 machine program in Intel Hex record format.
/// 
/// @section AUTHOR AUTHOR
/// Robert Heller \<heller\@deepsoft.com\>
///
//////////////////////////////////////////////////////////////////////////////


static const char rcsid[] = "@(#) : $Id$";

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "cpu.hxx"
#include "memory.hxx"

static void usage(char *program)
{
    std::cerr << "Usage: " << program << " [-m ramsize] [-r romsize] [-d] hexfile" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Where:" << std::endl;
    std::cerr << "\t-m sets the size of the ram. Default 2048" << std::endl;
    std::cerr << "\t-r sets the size of the rom. Default 2048" << std::endl;
    std::cerr << "\t-d causes the first 1024 bytes of memory to be dumped." << std::endl;
    std::cerr << "\thexfile is an Intel Hex file containing the 6502 program to run." << std::endl;
}

int main(int argc, char *argv[])
{
    int optchar;
    uint16_t ramsize=2048;
    uint16_t romsize=2048;
    bool dumpram = false;
    while ((optchar = getopt(argc,argv,"m:r:d")) >= 0)
    {
        switch (optchar)
        {
        case 'm':
            ramsize = atoi(optarg);
            break;
        case 'r':
            romsize = atoi(optarg);
            break;
        case 'd':
            dumpram = true;
            break;
        default:
            usage(argv[0]);
            return 1;
        }
    }
    MixedRAMROM memory(ramsize,romsize);
    if (optind < argc)
    {
        char buffer[150];
        std::ifstream hexfile(argv[optind]);
        while (hexfile.getline(buffer,150))
        {
            memory.BurnROM(buffer);
        }
    }
    else
    {
        std::cerr << "No program supplied!" << std::endl;
        return 1;
    }
    cpu6502 Processor(&memory);
    Processor.run(std::cout);
    if (dumpram) memory.DumpMemory(0,1024,std::cout);
    return 0;
}
