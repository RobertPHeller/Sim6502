// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Tue May 27 15:11:48 2025
//  Last Modified : <250527.1531>
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
/// @file testBCDsbc.cxx
/// @author Robert Heller
/// @date Tue May 27 15:11:48 2025
/// 
///
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";

#include <cstdint>
#include <iostream>
#include <iomanip>

uint16_t BCDSbC(uint8_t AC,uint8_t operand,uint8_t C)
{
    uint8_t NA = AC & 0x0F;
    uint8_t NO = operand & 0x0F;
    int8_t  NR = (int8_t)NA - (int8_t)NO - (int8_t)C;
    uint8_t C1 = 0;
    int8_t NRU;
    std::cerr << "*** BCDSbC():" << (int)NA << " - " << (int)NO << " - " << (int)C << " => " << (int)NR << std::endl;
    if (NR < 0)
    {
        C1 = 1;
        NR = 10 + NR;
    }
    NA = AC & 0xF0 >> 4;
    NO = operand & 0xF0 >> 4;
    NRU = (int)NA - (int)NO - (int)C1;
    std::cerr << "*** BCDSbC():" << (int)NA << " - " << (int)NO << " - " << (int)C1 << " => " << (int)NRU << std::endl;
    if (NRU < 0)
    {
        C1 = 1;
        NRU = 10 + NRU;
    }
    return NR | (NRU << 4) | (C1 << 8);
}

void test1(uint8_t A,uint8_t B,uint8_t C)
{
    uint16_t result = BCDSbC(A,B,C);
    std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)A << " - ";
    std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)B << " - ";
    std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)C << " => ";
    std::cout << std::setfill('0') << std::setw(4) << std::hex << result << std::endl;
}

int main(int argc, char *argv[])
{
    test1(0x99,0x88,0x0);
    test1(0x88,0x99,0x0);
    test1(0x00,0x01,0x01);
}
