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
//  Last Modified : <250528.1624>
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

#include <csignal>
#include <iostream>
#include <cstdint>
#include "memory.hxx"

/** Class that implements a 6502 processor.
 * The basic (original) instruction set is implemented.
 * @author Robert Heller
 */
class cpu6502 {
public:
    /** Constructor -- construct a 6502 micro processs.
     * @param ram A Memory object.
     */
    cpu6502(Memory *ram);
    /** Destructor.
     */
    ~cpu6502()
    {
    }
    /** Execute one instruction.
     * @param out Ostream to display the current instruction byte.
     * @returns void.
     */
    void execute(std::ostream &out);
    /** Execute instructions continiously until interupted (^C).
     * @param out Ostream to display the registers and instruction byte.
     * @returns void.
     */
    void run(std::ostream &out);
    /** Dump the registers. 
     * @param out Ostream to display the registers to.
     * @returns void.
     */
    void dumpregisters(std::ostream &out);          
private:
    /** Program Counter register */
    uint16_t PC;
    /** Accumulator register */
    uint8_t AC; 
    /** X register */
    uint8_t X;  
    /** Y register */
    uint8_t Y;
    /** Status register */
    union {
        uint8_t byte;
        struct {
            /** Carry bit */
            unsigned C:1; 
            /** Zero bit */
            unsigned Z:1; 
            /** Interrupt bit (IRQ disable) */
            unsigned I:1; 
            /** Decimal bit (use BCD for arithmetics) */
            unsigned D:1; 
            /** Break */
            unsigned B:1; 
            /* unused */
            unsigned ignored:1;
            /** Overflow bit */
            unsigned V:1; 
            /** Negative bit */
            unsigned N:1; 
        } bits;
    } SR; 
    /** Stack pointer */
    uint8_t SP; 
    /** Memory */
    Memory *ram_; 
    /** Instruction byte layout. */
    typedef union {
        uint8_t byte;
        struct {
            unsigned c:2;
            unsigned b:3;
            unsigned a:3;
        } bits;
    } InstrutionByte; 
    /** Decode and execute an instruction.
     * @param ibyte Instruction byte.
     * @param out Ostream to display the instruction byte to.
     * @returns void.
     */
    void DecodeInstruction(uint8_t ibyte,std::ostream &out);
    /** Decode and execute an ALU instruction.
     * @param a instruction number (0..7).
     * @param b address mode number (0..7).
     * @returns void.
     */
    void ALUInstruction(uint8_t a,uint8_t b);
    /** BDC add with carry.
     * Does a BCD add to the AC.
     * @param operand operand to add.
     * @returns BDC addition result (9-bits).
     */
    uint16_t BCDAdC(uint8_t operand);
    /** BDC subtract with carry.
     * Does a BCD subtract from the AC.
     * @param operand operand to subtract.
     * @returns BDC subtraction result (9-bits).
     */
    uint16_t BCDSbC(uint8_t operand);
    /** ALU Address Mode decoder.
     * @param b address mode number (0..7).
     * @returns 16-bit address of the operand.
     */
    uint16_t ALUAddrMode(uint8_t b);
    /** Set the Z and N flags.
     * @param register to test.
     * @returns void.
     * Side effects: Z and N flags are set.
     */
    void SetFlags(uint8_t reg);
    /** Process (zpg,X) address mode.
     * @returns operand address.
     */
    inline uint16_t X_ind() {
        uint16_t low = ram_->Fetch(PC++);
        uint16_t temp  = (low + X) & 0x0FF;
        low = ram_->Fetch(temp++);
        uint16_t high = ram_->Fetch(temp);
        return low | (high << 8);
    }
    /** Process zpg address mode.
     * @returns operand address. 
     */
    inline uint16_t Zpg() {return ram_->Fetch(PC++);}
    /** Process Immediate address mode.
     * @returns operand address.
     */
    inline uint16_t Immediate() {return PC++;}
    /** Process abs address mode.
     * @returns operand address.
     */
    inline uint16_t Abs() {
        return (ram_->Fetch(PC++)) | ((ram_->Fetch(PC++)) << 8);
    }
    /** Process (zpg),Y address mode. 
     * @returns operand address.
     */
    inline uint16_t Ind_Y()
    {
        uint16_t temp = ram_->Fetch(PC++);
        uint16_t low = ram_->Fetch(temp++);
        uint16_t high = ram_->Fetch(temp);
        return (low | (high << 8)) + Y;
    }
    /** Process zpg,X address mode.
     * @returns operand address.
     */
    inline uint16_t Zpg_X()
    {
        uint16_t low = ram_->Fetch(PC++);
        return (low + X) & 0x0FF;
    }
    /** Process zpg,Y address mode.
     * @returns operand address. 
     */
    inline uint16_t Zpg_Y()
    {
        uint16_t low = ram_->Fetch(PC++);
        return (low + Y) & 0x0FF;
    }
    /** Process abs,Y address mode.
     * @returns operand address.
     */
    inline uint16_t Abs_Y()
    {
        uint16_t low = ram_->Fetch(PC++);
        uint16_t high = ram_->Fetch(PC++);
        return  (low | (high << 8)) + Y;
    }
    /** Process abs,X address mode.
     *  @returns operand address.
     */
    inline uint16_t Abs_X()
    {
        uint16_t low = ram_->Fetch(PC++);
        uint16_t high = ram_->Fetch(PC++);
        return  (low | (high << 8)) + X;
    }
    /** Process rel address mode.
     * @returns operand address.
     */
    inline uint16_t Rel()
    {
        uint16_t low = ram_->Fetch(PC++);
        if ((low & 0x80) != 0)
        {
            low = low-256;
        }
        return PC+low;
    }
    /** Process (abs) address mode. 
     * @returns operand address. 
     */
    inline uint16_t Ind()
    {
        uint16_t operand = Abs();
        return (ram_->Fetch(operand++)) | ((ram_->Fetch(operand)) << 8);
    }
    /** Perform Arithmetic Shift Left.
     * @param byte Byte to shift.  The byte is updated, as is the C flag.
     * @returns void.
     */
    inline void ASL(uint8_t &byte)
    {
        uint16_t result = (uint16_t) byte << 1;
        SR.bits.C = (result & 0x0100) ? 1 : 0;
        SetFlags(result & 0x0ff);
        byte = result & 0x0ff;
    }
    /** Perform Rotate Left.
     * @param byte Byte to shift.  The byte is updated, as is the C flag.
     * @returns void.
     */
    inline void ROL(uint8_t &byte)
    {
        uint16_t result = ((uint16_t) byte << 1) | SR.bits.C;
        SR.bits.C = (result & 0x0100) ? 1 : 0;
        SetFlags(result & 0x0ff);
        byte = result & 0x0ff;
    }
    /** Perform Logical Shift Right.
     * @param byte Byte to shift.  The byte is updated, as is the C flag.
     * @returns void.
     */
    inline void LSR(uint8_t &byte)
    {
        SR.bits.C = byte & 0x01;
        byte = (uint16_t) byte >> 1;
        SetFlags(byte);
    }
    /** Perform Rotate Right.
     * @param byte Byte to shift.  The byte is updated, as is the C flag.
     * @returns void.  
     */
    inline void ROR(uint8_t &byte)
    {
        uint8_t carry = SR.bits.C << 7;
        SR.bits.C = byte & 0x01;
        byte = ((uint16_t) byte >> 1) | carry;
        SetFlags(byte);
    }
    /** Signal status variable. */
    static volatile std::sig_atomic_t signal_;
    /** Signal handler.
     * @param signal signal number,
     * @returns void.
     * Side effects, update signal flag.
     */
    static void signal_handler(int signal)
    {
        signal_ = signal;
    }
};


#endif // __CPU_HXX

