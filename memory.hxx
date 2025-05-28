// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Tue May 27 11:29:22 2025
//  Last Modified : <250528.1004>
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
/// @file memory.hxx
/// @author Robert Heller
/// @date Tue May 27 11:29:22 2025
/// 
///
//////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_HXX
#define __MEMORY_HXX

#include <iostream>
#include <iomanip>
#include <cstdint>

class Memory 
{
public:
    virtual ~Memory()
    {
    }
    virtual uint8_t &Fetch(uint16_t address) = 0;
};

class AllRam : public Memory
{
public:
    AllRam(uint16_t size = 2048)
          : size_(size)
    {
        ram_ = new uint8_t[size_];
    }
    ~AllRam()
    {
        delete [] ram_;
    }
    virtual uint8_t &Fetch(uint16_t address) 
    {
        address = address % size_;
        return ram_[address];
    }
private:
    uint16_t size_;
    uint8_t *ram_;
};

class MixedRAMROM : public Memory
{
public:
    MixedRAMROM(uint16_t ramsize = 2048, uint16_t romsize = 2048)
                : ramsize_(ramsize), romsize_(romsize)
    {
        ram_ = new uint8_t[ramsize_];
        rom_ = new uint8_t[romsize_];
    }
    ~MixedRAMROM()
    {
        delete [] ram_;
        delete [] rom_;
    }
    bool BurnROM(const char *hexrecord)
    {
        const char *p = hexrecord;
        uint8_t count;
        uint16_t address;
        uint8_t checksum = 0;
        uint8_t databyte;
        uint8_t rectype;
        //std::cerr << "*** BurnROM(\"" << hexrecord << "\")" << std::endl;
        while (*p && *p != ':') p++;
        if (!*p) return false;
        p++;
        count = base10_(p); p += 2;
        //std::cerr << "*** BurnROM(): count = " << std::dec << (int)count << std::endl;
        checksum += count;
        address = base10_(p); p += 2;
        //std::cerr << "*** BurnROM(): address = " << std::setfill('0') << std::setw(4) << std::hex << address << std::endl;
        checksum += address;  count--;
        databyte = base10_(p); p += 2;
        checksum += databyte; count--;
        //std::cerr << "*** BurnROM(): databyte = " << std::setfill('0') << std::setw(2) << std::hex << (int)databyte << std::endl;
        address = address << 8 | databyte;
        //std::cerr << "*** BurnROM(): address = " << std::setfill('0') << std::setw(4) << std::hex << address << std::endl;
        rectype = base10_(p); p += 2;
        checksum += rectype;  count--;
        if (rectype == 0 && count > 0)
        {
            while (count > 0)
            {
                databyte = base10_(p); p += 2;
                checksum += databyte; count--;
                writeRom_(address++,databyte);
            }
        }
        checksum += base10_(p);
        return (checksum == 0);
    }
    virtual uint8_t &Fetch(uint16_t address)
    {
        address = address % (ramsize_+romsize_);
        if (address < ramsize_) return ram_[address];
        byte_ = rom_[address - ramsize_];
        return byte_;
    }
private:
    void writeRom_(uint16_t address,uint8_t databyte)
    {
        //std::cerr << "*** writeRom_(" << std::setfill('0') << std::setw(4) << std::hex << address << ",";
        //std::cerr << std::setfill('0') << std::setw(2) << std::hex << (int)databyte << ")" << std::endl;
        address = address % (ramsize_+romsize_);
        
        if (address < ramsize_) return;
        rom_[address - ramsize_] = databyte;
    }
    uint8_t base10_(const char *hex) const
    {
        uint8_t result = 0;
        for (int i = 0;i < 2; i++)
        {
            if (*hex >= '0' && *hex <= '9')
            {
                result = (result * 16) + *hex++ - '0';
            }
            else if (*hex >= 'A' && *hex <= 'F')
            {
                result = (result * 16) + (*hex++ - 'A') + 10;
            }
            else if (*hex >= 'a' && *hex <= 'f')
            {
                result = (result * 16) + (*hex++ - 'a') + 10;
            }
            else
            {
                return result;
            }
        }
        return result;
    }
        
    uint16_t ramsize_, romsize_;
    uint8_t *ram_, *rom_;
    uint8_t byte_;
};
              
          

#endif // __MEMORY_HXX

