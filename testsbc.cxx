// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Tue May 27 14:32:54 2025
//  Last Modified : <250527.1439>
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
/// @file testsbc.cxx
/// @author Robert Heller
/// @date Tue May 27 14:32:54 2025
/// 
///
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";


#include <cstdint>
#include <iostream>
#include <iomanip>

int main(int argc, char *argv[])
{
    uint8_t A(0x02);
    uint8_t B(0x01);
    uint16_t result;
    
    std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)A << " - ";
    std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)B << " => ";
    result = (uint16_t) A - (uint16_t) B;
    std::cout << std::setfill('0') << std::setw(4) << std::hex << result << std::endl;
    return 0;
}
