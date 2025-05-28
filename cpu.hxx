// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Mon May 12 19:53:50 2025
//  Last Modified : <250528.0751>
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
/// @file cpu.hxx
/// @author Robert Heller
/// @date Mon May 12 19:53:50 2025
/// 
///
//////////////////////////////////////////////////////////////////////////////

#ifndef __CPU_HXX
#define __CPU_HXX

#include <iostream>
#include <cstdint>
#include "memory.hxx"

class cpu6502 {
public:
    cpu6502(Memory *ram);
    ~cpu6502()
    {
    }
    void execute();
    void dumpregisters(std::ostream &out);          
private:
    uint16_t PC; /* Program Counter register */
    uint8_t AC; /* Accumulator register */
    uint8_t X;  /* X register */
    uint8_t Y;  /* Y register */
    union {
        uint8_t byte;
        struct {
            unsigned C:1; /* Carry bit */
            unsigned Z:1; /* Zero bit */
            unsigned I:1; /* Interrupt bit (IRQ disable) */
            unsigned D:1; /* Decimal bit (use BCD for arithmetics) */
            unsigned B:1; /* Break */
            unsigned ignored:1; /* unused */
            unsigned V:1; /* Overflow bit */
            unsigned N:1; /* Negative bit */
        } bits;
    } SR; /* Status register */
    uint8_t SP; /* Stack pointer */
    Memory *ram_;
    typedef union {
        uint8_t byte;
        struct {
            unsigned c:2;
            unsigned b:3;
            unsigned a:3;
        } bits;
    } InstrutionByte;
    void DecodeInstruction(uint8_t ibyte);
    void ALUInstruction(uint8_t a,uint8_t b);
    uint16_t BCDAdC(uint8_t operand);
    uint16_t BCDSbC(uint8_t operand);
    uint16_t ALUAddrMode(uint8_t b);
    void SetFlags(uint8_t reg);
    inline uint16_t X_ind() {
        uint16_t low = ram_->Fetch(PC++);
        uint16_t temp  = (low + X) & 0x0FF;
        low = ram_->Fetch(temp++);
        uint16_t high = ram_->Fetch(temp);
        return low | (high << 8);
    }
    inline uint16_t Zpg() {return ram_->Fetch(PC++);}
    inline uint16_t Immediate() {return PC++;}
    inline uint16_t Abs() {
        return (ram_->Fetch(PC++)) | ((ram_->Fetch(PC++)) << 8);
    }
    inline uint16_t Ind_Y()
    {
        uint16_t temp = ram_->Fetch(PC++);
        uint16_t low = ram_->Fetch(temp++);
        uint16_t high = ram_->Fetch(temp);
        return (low | (high << 8)) + Y;
    }
    inline uint16_t Zpg_X()
    {
        uint16_t low = ram_->Fetch(PC++);
        return (low + X) & 0x0FF;
    }
    inline uint16_t Zpg_Y()
    {
        uint16_t low = ram_->Fetch(PC++);
        return (low + Y) & 0x0FF;
    }
    inline uint16_t Abs_Y()
    {
        uint16_t low = ram_->Fetch(PC++);
        uint16_t high = ram_->Fetch(PC++);
        return  (low | (high << 8)) + Y;
    }
    inline uint16_t Abs_X()
    {
        uint16_t low = ram_->Fetch(PC++);
        uint16_t high = ram_->Fetch(PC++);
        return  (low | (high << 8)) + X;
    }
    inline uint16_t Rel()
    {
        uint16_t low = ram_->Fetch(PC++);
        if ((low & 0x80) != 0)
        {
            low = low-256;
        }
        return PC+low;
    }
    inline uint16_t Ind()
    {
        uint16_t operand = Abs();
        return (ram_->Fetch(operand++)) | ((ram_->Fetch(operand)) << 8);
    }
    inline void ASL(uint8_t &byte)
    {
        uint16_t result = (uint16_t) byte << 1;
        SR.bits.C = (result & 0x0100) ? 1 : 0;
        SetFlags(result & 0x0ff);
        byte = result & 0x0ff;
    }
    inline void ROL(uint8_t &byte)
    {
        uint16_t result = ((uint16_t) byte << 1) | SR.bits.C;
        SR.bits.C = (result & 0x0100) ? 1 : 0;
        SetFlags(result & 0x0ff);
        byte = result & 0x0ff;
    }
    inline void LSR(uint8_t &byte)
    {
        SR.bits.C = byte & 0x01;
        byte = (uint16_t) byte >> 1;
        SetFlags(byte);
    }
    inline void ROR(uint8_t &byte)
    {
        uint8_t carry = SR.bits.C << 7;
        SR.bits.C = byte & 0x01;
        byte = ((uint16_t) byte >> 1) | carry;
        SetFlags(byte);
    }
};


#endif // __CPU_HXX

