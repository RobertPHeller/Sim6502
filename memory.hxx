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
//  Last Modified : <250528.1604>
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

/** Abstract class to implement memory.
 */
class Memory 
{
public:
    /** Virtual destructor (does nothing). */
    virtual ~Memory()
    {
    }
    /** Virtual function to fetch a byte of memory.  Returns a reference to
     * allow writing to this memory location.
     * @param address addresss to access.
     * @returns a reference to the memory location.
     */
    virtual uint8_t &Fetch(uint16_t address) = 0;
    /** Dump a block of memory to an output stream.
     * @param start start address.
     * @param length number of bytes to dump.
     * @param out output stream.
     * @returns void.
     */
    void DumpMemory(uint16_t start,uint16_t length,std::ostream &out)
    {
        unsigned int linecount = 0;
        uint16_t address = start;
        out << std::setfill('0') << std::setw(4) << std::hex << address << " ";
        while (length > 0)
        {
            uint8_t databyte = Fetch(address++); length--;
            out << std::setfill('0') << std::setw(2) << std::hex << (int)databyte << " ";
            linecount++;
            if (linecount >= 16)
            {
                out << std::endl;
                if (length > 0) 
                    out << std::setfill('0') << std::setw(4) << std::hex << address << " ";
                linecount = 0;
            }
        }
        if (linecount > 0) out << std::endl;
    }
};

/** Class that is just a block of ram (all location writable).
 */
class AllRam : public Memory
{
public:
    /** Constructor: allocate a block of memory.
     * @paran size The size of the block of memory.
     */
    AllRam(uint16_t size = 2048)
          : size_(size)
    {
        ram_ = new uint8_t[size_];
    }
    /** Destructor: free up allocated memory.
     */
    ~AllRam()
    {
        delete [] ram_;
    }
    /** Implementation of the Fetch function.
     * @param address addresss to access.
     * @returns a reference to the memory location.
     */
    virtual uint8_t &Fetch(uint16_t address) 
    {
        address = address % size_;
        return ram_[address];
    }
private:
    uint16_t size_;
    uint8_t *ram_;
};

/** Class that is a block of ram (writable) and a block of rom (read-only).
 */
class MixedRAMROM : public Memory
{
public:
    /** Constructor: Alloc a block of RAM and a block of ROM
     * @param ramsize how much ram (starting at 0x0000) to allocte.
     * @param romsize how much rom (starting at ramsize) to allocte.
     */
    MixedRAMROM(uint16_t ramsize = 2048, uint16_t romsize = 2048)
                : ramsize_(ramsize), romsize_(romsize)
    {
        ram_ = new uint8_t[ramsize_];
        rom_ = new uint8_t[romsize_];
    }
    /** Destructor: free up allocated memory.
     */
    ~MixedRAMROM()
    {
        delete [] ram_;
        delete [] rom_;
    }
    /** "Burn" ROM from Intel HEX records.
     * @param hexrecord one hex record.
     * @returns true if record is valid.
     */
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
    /** Implementation of the Fetch function.
     * ROM addresses are read-only.  A reference to a scratch byte is returned
     *  for ROM address space.
     * @param address addresss to access.
     * @returns a reference to the memory location.
     */
    virtual uint8_t &Fetch(uint16_t address)
    {
        address = address % (ramsize_+romsize_);
        if (address < ramsize_) return ram_[address];
        byte_ = rom_[address - ramsize_];
        return byte_;
    }
private:
    /** method to write (burn) a ROM byte.
     * @param address Address to write.
     * @param databyte Data to write.
     */
    void writeRom_(uint16_t address,uint8_t databyte)
    {
        //std::cerr << "*** writeRom_(" << std::setfill('0') << std::setw(4) << std::hex << address << ",";
        //std::cerr << std::setfill('0') << std::setw(2) << std::hex << (int)databyte << ")" << std::endl;
        address = address % (ramsize_+romsize_);
        
        if (address < ramsize_) return;
        rom_[address - ramsize_] = databyte;
    }
    /** Base 16 => base 10 decoder.
     * @param hex Hex string.
     * @returns byte value.
     */
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
    /** RAM memory size. */
    uint16_t ramsize_;
    /** ROM memory size. */
    uint16_t romsize_;
    /** RAM memory. */
    uint8_t *ram_;
    /** ROM memory. */
    uint8_t *rom_;
    /** Dummy byte. */
    uint8_t byte_;
};
              
          

#endif // __MEMORY_HXX

