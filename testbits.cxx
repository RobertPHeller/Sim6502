// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Mon May 12 20:28:36 2025
//  Last Modified : <250512.2041>
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
/// @file testbits.cxx
/// @author Robert Heller
/// @date Mon May 12 20:28:36 2025
/// 
///
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";

#include <cstdint>
#include <iostream>

class testbits {
public:
    testbits(uint8_t byte)
    {
        field.byte = byte;
    }
    uint16_t Bit0() const {return field.bits.bit0;}
    uint16_t Bit7() const {return field.bits.bit7;}
    uint16_t Byte() const {return field.byte;}
private:
    union {
        uint8_t byte;
        struct {
            unsigned bit0:1;
            unsigned bit1:1;
            unsigned bit2:1;
            unsigned bit3:1;
            unsigned bit4:1;
            unsigned bit5:1;
            unsigned bit6:1;
            unsigned bit7:1;
        } bits;
    } field;
};


int main(int argc, char *argv[])
{
    testbits test1(1);
    std::cout << test1.Byte() << std::endl; 
    std::cout << test1.Bit0() << std::endl;
    return 0;
}
            
