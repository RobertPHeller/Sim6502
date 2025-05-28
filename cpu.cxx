// -!- C++ -!- ////////////////////////////////////////////////////////////////
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Mon May 12 20:51:05 2025
//  Last Modified : <250528.1054>
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
/// @file cpu.cxx
/// @author Robert Heller
/// @date Mon May 12 20:51:05 2025
/// 
///
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";

#include <iostream>
#include <iomanip>
#include <cstdint>
#include "cpu.hxx"
#include "memory.hxx"

cpu6502::cpu6502(Memory *ram)
      : ram_(ram)
{
    SR.bits.D = 0;
    SR.bits.I = 0;
    PC = ram_->Fetch(0xFFFC) | (ram_->Fetch(0xFFFD) << 8);
}

void cpu6502::dumpregisters(std::ostream &out)
{
    out << "PC: " << std::setfill('0') << std::setw(4) << std::hex << PC << ' ';
    out << "A: " << std::setfill('0') << std::setw(2) << std::hex << (int)AC << ' ';
    out << "X: " << std::setfill('0') << std::setw(2) << std::hex << (int)X << ' ';
    out << "Y: " << std::setfill('0') << std::setw(2) << std::hex << (int)Y << ' ';
    out << "SP: " << std::setfill('0') << std::setw(2) << std::hex << (int)SP << ' ';
    out << "Flags: C:" <<  std::setw(1) << SR.bits.C << " Z:" << SR.bits.Z
          << " I:" << SR.bits.I << " D:" << SR.bits.D << " B:" << SR.bits.B
          << " V:" << SR.bits.V << " N:" << SR.bits.N << std::endl;
}
void cpu6502::execute()
{
    uint8_t IB = ram_->Fetch(PC++);
    DecodeInstruction(IB);
}

void cpu6502::DecodeInstruction(uint8_t ibyte)
{
    InstrutionByte ib;
    ib.byte = ibyte;
    std::cout << "IB: " << std::setfill('0') << std::setw(2) << std::hex
          << (int)ibyte << std::endl;
    //std::cerr << "*** DecodeInstruction(): IB.bits.c = " << std::dec << ib.bits.c << std::endl;
    switch (ib.bits.c)
    {
    case 0:
        if (ib.bits.b == 4)
        {
            /* branch, cond selected by ib.bits.a */
            uint16_t target = Rel();
            switch (ib.bits.a) {
            case 0: /* BPL */
                if (!SR.bits.N) PC = target;
                break;
            case 1: /* BMI */
                if (SR.bits.N) PC = target;
                break;
            case 2: /* BVC */
                if (!SR.bits.V) PC = target;
                break;
            case 3: /* BVS */
                if (SR.bits.V) PC = target;
                break;
            case 4: /* BCC */
                if (!SR.bits.C) PC = target;
                break;
            case 5: /* BCS */
                if (SR.bits.C) PC = target;
                break;
            case 6: /* BNE */
                if (!SR.bits.Z) PC = target;
                break;
            case 7: /* BEQ */
                if (SR.bits.Z) PC = target;
                break;
            }
            break;
        }
        else
        {
            switch (ib.bits.a)
            {
            case 0: 
                switch (ib.bits.b)
                {
                case 0: /* BRK */
                    PC++;
                    ram_->Fetch(0x0100 | SP--) = PC >> 8;
                    ram_->Fetch(0x0100 | SP--) = PC & 0x0FF;
                    ram_->Fetch(0x0100 | SP--) = SR.byte | 0x30;
                    PC = ram_->Fetch(0xFFFE) | (ram_->Fetch(0xFFFF) << 8);
                    break;
                case 2: /* PHP */
                    ram_->Fetch(0x0100 | SP--) = SR.byte | 0x30;
                    break;
                case 6: /* CLC */
                    SR.bits.C = 0;
                    break;
                }
                break;
            case 1:
                switch (ib.bits.b)
                {
                    case 0: /* JSR */
                    {
                        uint16_t target = Abs();
                        ram_->Fetch(0x0100 | SP--) = PC >> 8;
                        ram_->Fetch(0x0100 | SP--) = PC & 0x0FF;
                        PC = target;
                    }
                    break;
                case 1: /* BIT zpg */
                    {
                        uint16_t target = Zpg();
                        uint8_t m = ram_->Fetch(target);
                        SetFlags(AC & m);
                        SR.bits.N = (m & 0x80) ? 1 : 0;
                        SR.bits.V = (m & 0x40) ? 1 : 0;
                    }
                    break;
                case 2: /* PLP */
                    SR.byte = (ram_->Fetch(0x0100 | ++SP)) & 0xCF;
                    break;
                case 3: /* BIT abs */
                    {
                        uint16_t target = Abs();
                        uint8_t m = ram_->Fetch(target);
                        SetFlags(AC & m);
                        SR.bits.N = (m & 0x80) ? 1 : 0;
                        SR.bits.V = (m & 0x40) ? 1 : 0;
                    }
                    break;
                case 6: /* SEC */
                    SR.bits.C = 1;
                    break;
                }
                break;
            case 2:
                switch (ib.bits.b)
                {
                case 0: /* RTI */
                    SR.byte = (ram_->Fetch(0x0100 | ++SP)) & 0xCF;
                    PC = ram_->Fetch(0x0100 | ++SP);
                    PC |= (ram_->Fetch(0x0100 | ++SP) << 8);
                    break;
                case 2: /* PHA */
                    ram_->Fetch(0x0100 | SP--) = AC;
                    break;
                case 3: /* JMP abs */
                    PC = Abs();
                    break;
                case 6: /* CLI */
                    SR.bits.I = 0;
                    break;
                }
                break;
            case 3:
                switch (ib.bits.b)
                {
                case 0: /* RTS */
                    PC = ram_->Fetch(0x0100 | ++SP); 
                    PC |= (ram_->Fetch(0x0100 | ++SP) << 8);
                    break;
                case 2: /* PLA */
                    AC =  ram_->Fetch(0x0100 | ++SP);
                    break;
                case 3: /* JMP ind */
                    PC = Ind();
                    break;
                case 6: /* SEI */
                    SR.bits.I = 1;
                    break;
                }
                break;
            case 4:
                switch (ib.bits.b)
                {
                case 1: /* STY zpg*/
                    ram_->Fetch(Zpg()) = Y;
                    break;
                case 2: /* DEY */
                    SetFlags(--Y);
                    break;
                case 3: /* STY abs */
                    ram_->Fetch(Abs()) = Y;
                    break;
                case 5: /* STY zpg,X */
                    ram_->Fetch(Zpg_X()) = Y;
                    break;
                case 6: /* TYA */
                    AC = Y;
                    break;
                }
                break;
            case 5:
                switch (ib.bits.b)
                {
                case 0: /* LDY # */
                    Y = ram_->Fetch(Immediate());
                    break;
                case 1: /* LDY zpg */
                    Y = ram_->Fetch(Zpg());
                    break;
                case 2: /* TAY */
                    Y = AC;
                    break;
                case 3: /* LDY abs */
                    Y = ram_->Fetch(Abs());
                    break;
                case 5: /* LDY zpg,X */
                    Y = ram_->Fetch(Zpg_X());
                    break;
                case 6: /* CLV */
                    SR.bits.V = 0;
                    break;
                case 7: /* LDY abs,x */
                    Y = ram_->Fetch(Abs_X());
                    break;
                }
                break;
            case 6:
                switch (ib.bits.b)
                {
                case 0: /* CPY # */
                    {
                        uint16_t temp = (uint16_t) Y - (uint16_t) ram_->Fetch(Immediate());
                        if (temp > 255) SR.bits.C = 1;
                        SetFlags(temp & 0x0FF);
                    }
                    break;
                case 1: /* CPY zpg */
                    {
                        uint16_t temp = (uint16_t) Y - (uint16_t) ram_->Fetch(Zpg());
                        if (temp > 255) SR.bits.C = 1;
                        SetFlags(temp & 0x0FF);
                    }
                    break;
                    
                case 2: /* INY */
                    SetFlags(++Y);
                    break;
                case 3: /* CPY abs */
                    {
                        uint16_t temp = (uint16_t) Y - (uint16_t) ram_->Fetch(Abs());
                        if (temp > 255) SR.bits.C = 1;
                        SetFlags(temp & 0x0FF);
                    }
                    break;
                case 6: /* CLD */
                    SR.bits.D = 0;
                    break;
                }
                break;
            case 7:
                switch (ib.bits.b)
                {
                case 0: /* CPX # */
                    {
                        uint16_t temp = (uint16_t) X - (uint16_t) ram_->Fetch(Immediate());
                        if (temp > 255) SR.bits.C = 1;
                        SetFlags(temp & 0x0FF);
                    }
                    break;
                case 1: /* CPX xpg */
                    {
                        uint16_t temp = (uint16_t) X - (uint16_t) ram_->Fetch(Zpg());
                        if (temp > 255) SR.bits.C = 1;
                        SetFlags(temp & 0x0FF);
                    }
                    break;
                case 2: /* INX */
                    SetFlags(++X);
                    break;
                case 3: /* CPX abs */
                    {
                        uint16_t temp = (uint16_t) X - (uint16_t) ram_->Fetch(Abs());
                        if (temp > 255) SR.bits.C = 1;
                        SetFlags(temp & 0x0FF);
                    }
                    break;
                case 6: /* CED */
                    SR.bits.D = 0;
                    break;
                }
                break;
            }
        }
        break;
    case 1:
        ALUInstruction(ib.bits.a,ib.bits.b);
        break;
    case 2:
        //std::cerr << "*** DecodeInstruction(): IB.bits.a = " << std::dec << ib.bits.a << std::endl;
        switch (ib.bits.a)
        {
        case 0:
            switch (ib.bits.b)
            {
            case 1: /* ASL zpg */
                ASL(ram_->Fetch(Zpg()));
                break;
            case 2: /* ASL A */
                ASL(AC);
                break;
            case 3: /* ASL abs */
                ASL(ram_->Fetch(Abs()));
                break;
            case 5: /* ASL zpg,X */
                ASL(ram_->Fetch(Zpg_X()));
                break;
            case 7: /* ASL abs,X */
                ASL(ram_->Fetch(Abs_X()));
                break;
            }
            break;
        case 1:
            switch (ib.bits.b)
            {
            case 1: /* ROL zpg */
                ROL(ram_->Fetch(Zpg()));
                break;
            case 2: /* ROL A */
                ROL(AC);
                break;
            case 3: /* ROL abs */
                ROL(ram_->Fetch(Abs()));
                break;                   
            case 5: /* ROL zpg,X */
                ROL(ram_->Fetch(Zpg_X()));
                break;
            case 7: /* ROL abs,X */
                ROL(ram_->Fetch(Abs_X()));
                break;
            }
            break;
        case 2:
            switch (ib.bits.b)
            {
            case 1: /* LSR zpg */
                LSR(ram_->Fetch(Zpg()));
                break;
            case 2: /* LSR A */
                LSR(AC);
                break;
            case 3: /* LSR abs */
                LSR(ram_->Fetch(Abs()));
                break;
            case 5: /* LSR zpg,X */
                LSR(ram_->Fetch(Zpg_X()));
                break;
            case 7: /* LSR abs,X */
                LSR(ram_->Fetch(Abs_X()));
                break;
            }
            break;
        case 3:
            switch (ib.bits.b)
            {
            case 1: /* ROR zpg */
                ROR(ram_->Fetch(Zpg()));
                break;
            case 2: /* ROR A */
                ROR(AC);
                break;
            case 3: /* ROR abs */
                ROR(ram_->Fetch(Abs()));
                break;
            case 5: /* ROR zpg,X */
                ROR(ram_->Fetch(Zpg_X()));
                break;
            case 7: /* ROR abs,X */
                ROR(ram_->Fetch(Abs_X()));
                break;
            }
            break;
        case 4:
            switch (ib.bits.b)
            {
            case 1: /* STX zpg */
                ram_->Fetch(Zpg()) = X;
                break;
            case 2: /* TXA */
                AC = X;
                SetFlags(AC);
                break;
            case 3: /* STX abs */
                ram_->Fetch(Abs()) = X;
                break;
            case 5: /* STX zpg,Y */
                ram_->Fetch(Zpg_Y()) = X;
                break;
            case 6: /* TXS */
                SP = X;
                break;
            }
            break;
        case 5:
            //std::cerr << "*** DecodeInstruction(): IB.bits.b = " << std::dec << ib.bits.b << std::endl;
            switch (ib.bits.b)
            {
            case 0: /* LDX # */
                //std::cerr << "*** DecodeInstruction() [LDX #]: PC = " << std::setfill('0') << std::setw(4) << std::hex << PC << std::endl;
                X = ram_->Fetch(Immediate());
                break;
            case 1: /* LDX zpg */
                X = ram_->Fetch(Zpg());
                break;
            case 2: /* TAX */
                X = AC;
                break;
            case 3: /* LDX abs */
                X = ram_->Fetch(Abs());
                break;
            case 5: /* LDX xpg,Y */
                X = ram_->Fetch(Zpg_Y());
                break;
            case 6: /* TXS */
                X = SP;
                break;
            case 7: /* LDX abs,Y */
                X = ram_->Fetch(Abs_Y());
                break;
            }
            break;
        case 6:
            switch (ib.bits.b)
            {
            case 1: /* DEC zpg */
                SetFlags(--ram_->Fetch(Zpg()));
                break;
            case 2: /* DEX */
                SetFlags(--X);
                break;
            case 3: /* DEC abs */
                SetFlags(--ram_->Fetch(Abs()));
                break;
            case 5: /* DEC zpg,X */
                SetFlags(--ram_->Fetch(Zpg_X()));
                break;
            case 7: /* DEC abs,X */
                SetFlags(--ram_->Fetch(Abs_X()));
                break;
            }
            break;
        case 7:
            switch (ib.bits.b)
            {
            case 1: /* INC zpg */
                SetFlags(++ram_->Fetch(Zpg()));
                break;
            case 2: /* NOP */
                break;
            case 3: /* INC abs */
                SetFlags(++ram_->Fetch(Abs()));
                break;
            case 5: /* INC zpg,X */
                SetFlags(++ram_->Fetch(Zpg_X()));
                break;
            case 7: /* INC abs,X */
                SetFlags(++ram_->Fetch(Abs_X()));
                break;
            }
            break;
        }
        break;
    case 3:
        break;
    }
}

void cpu6502::ALUInstruction(uint8_t a,uint8_t b)
{    
    uint16_t temp;
    uint16_t target = ALUAddrMode(b);
    switch (a)
    {
    case 0:
        AC |= ram_->Fetch(target);
        SetFlags(AC);
        break;
    case 1:
        AC &= ram_->Fetch(target); 
        SetFlags(AC);
        break;
    case 2:
        AC ^= ram_->Fetch(target);
        SetFlags(AC);
        break;
    case 3:
        if (SR.bits.D)
        {
            temp = BCDAdC(ram_->Fetch(target));
        }
        else
        {
            temp = (uint16_t) AC + (uint16_t) ram_->Fetch(target) + SR.bits.C;
        }
        if (temp > 255) 
        {
            SR.bits.C = 1;
            SR.bits.V = 1;
        }
        AC = temp & 0x0FF;
        SetFlags(AC);
        break;
    case 4:
        ram_->Fetch(target) = AC;
        break;
    case 5:
        AC = ram_->Fetch(target);
        SetFlags(AC);
        break;
    case 6:
        temp = (uint16_t) AC - (uint16_t) ram_->Fetch(target) - SR.bits.C;
        if (temp > 255) SR.bits.C = 1;
        SetFlags(temp & 0x0FF);
        break;
    case 7:
        if (SR.bits.D)
        {
            temp = BCDSbC(ram_->Fetch(target));
        }
        else
        {
            temp = (uint16_t) AC - (uint16_t) ram_->Fetch(target) - SR.bits.C;
        }
        if (temp > 255) 
        {
            SR.bits.C = 1;
            SR.bits.V = 1;
        }
        AC = temp & 0x0FF;
        SetFlags(AC);
        break;
    }
}

uint16_t cpu6502::ALUAddrMode(uint8_t b)
{
    uint16_t target, low, high, temp;
    switch (b)
    {
    case 0:
        /* X,ind */
        target = X_ind();
        break;
    case 1:
        /* zpg */
        target = Zpg();
        break;
    case 2:
        /* # */
        target = Immediate();
        break;
    case 3:
        /* abs */
        target = Abs();
        break;
    case 4:
        /* ind,Y */
        target = Ind_Y();
        break;
    case 5:
        /* zpg,X */
        target = Zpg_X();
        break;
    case 6:
        /* abs,Y */
        target = Abs_Y();
        break;
    case 7:
        /* abs,X */
        target = Abs_X();
        break;
    }
    return target;
}
          
void cpu6502::SetFlags(uint8_t reg)
{
    SR.bits.Z = (reg == 0);
    SR.bits.N = (reg >= 0x80);
}

uint16_t cpu6502::BCDAdC(uint8_t operand)
{
    uint8_t NA = AC & 0x0F;
    uint8_t NO = operand & 0x0F;
    uint8_t NR = NA + NO + SR.bits.C;
    uint8_t C1 = 0;
    uint8_t NRU;
    if (NR > 9)
    {
        C1 = 1;
        NR = NR - 10;
    }
    NA = AC & 0xF0 >> 4;
    NO = operand & 0xF0 >> 4;
    NRU = NA + NO + C1;
    C1 = 0;
    if (NRU > 9)
    {
        C1 = 1;
        NRU = NRU - 10;
    }
    return NR | (NRU << 4) | (C1 << 8);
}

uint16_t cpu6502::BCDSbC(uint8_t operand)
{
    uint8_t NA = AC & 0x0F;
    uint8_t NO = operand & 0x0F;
    int8_t NR = (int)NA - (int)NO - SR.bits.C;
    uint8_t C1 = 0;
    int8_t NRU;
    if (NR < 0)
    {
        C1 = 1;
        NR = NR + 10;
    }
    NA = AC & 0xF0 >> 4;
    NO = operand & 0xF0 >> 4;
    NRU = (int)NA - (int)NO - C1;
    if (NRU < 0)
    {
        C1 = 1;
        NRU = NRU + 10;
    }
    return NR | (NRU << 4) | (C1 << 8);
}

