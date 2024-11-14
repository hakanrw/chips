#pragma once
/*#
    # mc6800.h

    Motorola 6800 CPU emulator.

    Project repo: https://github.com/floooh/chips/

    NOTE: this file is code-generated from mc6800.template.h and mc6800_gen.py
    in the 'codegen' directory.

    Do this:
    ~~~C
    #define CHIPS_IMPL
    ~~~
    before you include this file in *one* C or C++ file to create the
    implementation.

    Optionally provide the following macros with your own implementation
    ~~~C
    CHIPS_ASSERT(c)
    ~~~

    ## Emulated Pins

    ***********************************
    *           +-----------+         *
    *   IRQ --->|           |---> A0  *
    *   NMI --->|           |...      *
    *  HALT --->|           |---> A15 *
    * RESET --->|           |         *
    *    RW <---|   MC68H   |         *
    *    BA <---|           |         *
    *           |           |<--> D0  *
    *   VMA <---|           |...      *
    *           |           |<--> D7  *
    *           |           |         *
    *           +-----------+         *
    ***********************************

    ## Overview

    mc6800.h implements a cycle-stepped 6800 CPU emulator, meaning that
    the emulation state can be ticked forward in clock cycles instead
    of full instructions.

    At the end of mc6800_init(), the CPU emulation will be at the start of
    RESET state, and the first 7 ticks will execute the reset sequence
    (loading the reset vector at address 0xFFFC and continuing execution
    there. TODO correct?

    mc6800_init() will return a 64-bit pin mask which must be the input
    argument to the first call of mc6800_tick().

    To execute instructions, call mc6800_tick() in a loop. mc6800_tick()
    takes a 64-bit pin mask as input, executes one clock tick, and returns
    a modified pin mask.

    ## Functions

    ~~~C
    uint64_t mc6800_init(mc6800_t* cpu);
    ~~~
        Initializes a new mc6800_t instance,
        TODO

    ~~~C
    uint64_t mc6800_reset(mc6800_t* cpu)
    ~~~
        Resets a mc6800_t instance,
        TODO

    ~~~C
    uint64_t mc6800_tick(mc6800_t* cpu, uint64_t pins)
    ~~~
        Step the mc6800_t instance for one clock cycle.


    ## zlib/libpng license

    Copyright (c) 2024 Hakan Candar
    Copyright (c) 2018 Andre Weissflog
    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.
    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
        1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software in a
        product, an acknowledgment in the product documentation would be
        appreciated but is not required.
        2. Altered source versions must be plainly marked as such, and must not
        be misrepresented as being the original software.
        3. This notice may not be removed or altered from any source
        distribution.
*/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// address bus pins
#define MC6800_PIN_A0    (0)
#define MC6800_PIN_A1    (1)
#define MC6800_PIN_A2    (2)
#define MC6800_PIN_A3    (3)
#define MC6800_PIN_A4    (4)
#define MC6800_PIN_A5    (5)
#define MC6800_PIN_A6    (6)
#define MC6800_PIN_A7    (7)
#define MC6800_PIN_A8    (8)
#define MC6800_PIN_A9    (9)
#define MC6800_PIN_A10   (10)
#define MC6800_PIN_A11   (11)
#define MC6800_PIN_A12   (12)
#define MC6800_PIN_A13   (13)
#define MC6800_PIN_A14   (14)
#define MC6800_PIN_A15   (15)

// data bus pins
#define MC6800_PIN_D0    (16)
#define MC6800_PIN_D1    (17)
#define MC6800_PIN_D2    (18)
#define MC6800_PIN_D3    (19)
#define MC6800_PIN_D4    (20)
#define MC6800_PIN_D5    (21)
#define MC6800_PIN_D6    (22)
#define MC6800_PIN_D7    (23)

// control pins
#define MC6800_PIN_RW    (24)	  // out: memory read or write access
#define MC6800_PIN_BA    (25)	  // out: TODO not implemented
#define MC6800_PIN_IRQ   (26)	  // in: maskable interrupt requested
#define MC6800_PIN_NMI   (27)	  // in: non-maskable interrupt requested
#define MC6800_PIN_HALT  (28)	  // in: freeze execution at next read cycle
#define MC6800_PIN_RESET (29)	  // request RESET
#define MC6800_PIN_VMA   (30)     // out: TODO VMA set, ready to read/write memory

// pin bit masks
#define MC6800_A0    (1ULL<<MC6800_PIN_A0)
#define MC6800_A1    (1ULL<<MC6800_PIN_A1)
#define MC6800_A2    (1ULL<<MC6800_PIN_A2)
#define MC6800_A3    (1ULL<<MC6800_PIN_A3)
#define MC6800_A4    (1ULL<<MC6800_PIN_A4)
#define MC6800_A5    (1ULL<<MC6800_PIN_A5)
#define MC6800_A6    (1ULL<<MC6800_PIN_A6)
#define MC6800_A7    (1ULL<<MC6800_PIN_A7)
#define MC6800_A8    (1ULL<<MC6800_PIN_A8)
#define MC6800_A9    (1ULL<<MC6800_PIN_A9)
#define MC6800_A10   (1ULL<<MC6800_PIN_A10)
#define MC6800_A11   (1ULL<<MC6800_PIN_A11)
#define MC6800_A12   (1ULL<<MC6800_PIN_A12)
#define MC6800_A13   (1ULL<<MC6800_PIN_A13)
#define MC6800_A14   (1ULL<<MC6800_PIN_A14)
#define MC6800_A15   (1ULL<<MC6800_PIN_A15)
#define MC6800_D0    (1ULL<<MC6800_PIN_D0)
#define MC6800_D1    (1ULL<<MC6800_PIN_D1)
#define MC6800_D2    (1ULL<<MC6800_PIN_D2)
#define MC6800_D3    (1ULL<<MC6800_PIN_D3)
#define MC6800_D4    (1ULL<<MC6800_PIN_D4)
#define MC6800_D5    (1ULL<<MC6800_PIN_D5)
#define MC6800_D6    (1ULL<<MC6800_PIN_D6)
#define MC6800_D7    (1ULL<<MC6800_PIN_D7)
#define MC6800_RW    (1ULL<<MC6800_PIN_RW)
#define MC6800_BA    (1ULL<<MC6800_PIN_BA)
#define MC6800_IRQ   (1ULL<<MC6800_PIN_IRQ)
#define MC6800_NMI   (1ULL<<MC6800_PIN_NMI)
#define MC6800_HALT  (1ULL<<MC6800_PIN_HALT)
#define MC6800_RESET (1ULL<<MC6800_PIN_RESET)
#define MC6800_VMA   (1ULL<<MC6800_PIN_VMA)

/* bit mask for all CPU pins (up to bit pos 40) */
#define MC6800_PIN_MASK ((1ULL<<40)-1)

/* status indicator flags */
#define MC6800_CF    (1<<0)  /* carry */
#define MC6800_VF    (1<<1)  /* overflow */
#define MC6800_ZF    (1<<2)  /* zero */
#define MC6800_NF    (1<<3)  /* negative */
#define MC6800_IF    (1<<4)  /* interrupt flag */
#define MC6800_HF    (1<<5)  /* half-carry */
#define MC6800_XF    (1<<6)  /* always one */
#define MC6800_YF    (1<<7)  /* always one */

/* internal BRK state flags */
#define MC6800_BRK_IRQ   (1<<0)  /* IRQ was triggered */
#define MC6800_BRK_NMI   (1<<1)  /* NMI was triggered */
#define MC6800_BRK_RESET (1<<2)  /* RES was triggered */

/* CPU state */
typedef struct {
    uint16_t IR;        /* internal instruction register */
    uint16_t PC;        /* internal program counter register */
    uint16_t AD;        /* ADL/ADH internal register */
    uint16_t IX;        /* index register */
    uint16_t SP;        /* stack pointer */
    uint8_t A,B,P;      /* regular registers */
    uint64_t PINS;      /* last stored pin state (do NOT modify) */
    uint16_t irq_pip;
    uint16_t nmi_pip;
    uint8_t brk_flags;  /* MC6800_BRK_* */
    uint8_t bcd_enabled;
    uint8_t next_instr;
} mc6800_t;

/* initialize a new mc6800 instance and return initial pin mask */
uint64_t mc6800_init(mc6800_t* cpu);
/* execute one tick */
uint64_t mc6800_tick(mc6800_t* cpu, uint64_t pins);

/* extract 16-bit address bus from 64-bit pins */
#define MC6800_GET_ADDR(p) ((uint16_t)((p)&0xFFFFULL))
/* merge 16-bit address bus value into 64-bit pins */
#define MC6800_SET_ADDR(p,a) {p=(((p)&~0xFFFFULL)|((a)&0xFFFFULL));}
/* extract 8-bit data bus from 64-bit pins */
#define MC6800_GET_DATA(p) ((uint8_t)(((p)&0xFF0000ULL)>>16))
/* merge 8-bit data bus value into 64-bit pins */
#define MC6800_SET_DATA(p,d) {p=(((p)&~0xFF0000ULL)|(((d)<<16)&0xFF0000ULL));}
/* copy data bus value from other pin mask */
#define MC6800_COPY_DATA(p0,p1) (((p0)&~0xFF0000ULL)|((p1)&0xFF0000ULL))
/* return a pin mask with control-pins, address and data bus */
#define MC6800_MAKE_PINS(ctrl, addr, data) ((ctrl)|(((data)<<16)&0xFF0000ULL)|((addr)&0xFFFFULL))

#ifdef __cplusplus
} /* extern "C" */
#endif

/*-- IMPLEMENTATION ----------------------------------------------------------*/
#ifdef CHIPS_IMPL
#include <string.h>
#ifndef CHIPS_ASSERT
    #include <assert.h>
    #define CHIPS_ASSERT(c) assert(c)
#endif

/* helper macros and functions for code-generated instruction decoder */
#define _MC6800_NZ(p,v) ((p&~(MC6800_NF|MC6800_ZF))|((v&0xFF)?((v)>>4&MC6800_NF):MC6800_ZF))
#define _MC6800_NZ16(p,v) ((p&~(MC6800_NF|MC6800_ZF))|((v&0xFFFF)?((v)>>12&MC6800_NF):MC6800_ZF))
#define _MC6800_VF(p,v) ((p&~MC6800_VF)|(v?MC6800_VF:0))

static inline uint8_t _mc6800_asl(mc6800_t* cpu, uint8_t v) {
    cpu->P = (_MC6800_NZ(cpu->P, v<<1) & ~MC6800_CF) | ((v & 0x80) ? MC6800_CF:0);
    return v<<1;
}

static inline uint8_t _mc6800_lsr(mc6800_t* cpu, uint8_t v) {
    cpu->P = (_MC6800_NZ(cpu->P, v>>1) & ~MC6800_CF) | ((v & 0x01) ? MC6800_CF:0);
    return v>>1;
}

static inline uint8_t _mc6800_rol(mc6800_t* cpu, uint8_t v) {
    bool carry = v & 0x80;
    cpu->P &= ~(MC6800_NF|MC6800_ZF|MC6800_CF);
    if (carry) {
        cpu->P |= MC6800_CF;
    }
    v <<= 1;
    if (carry) {
        v |= 1;
    }
    cpu->P = _MC6800_NZ(cpu->P, v);
    
    if (((cpu->P & MC6800_NF) && !(cpu->P & MC6800_CF)) || 
        (!(cpu->P & MC6800_NF) && (cpu->P & MC6800_CF))) {
        cpu->P = _MC6800_VF(cpu->P, true);
    } else {
        cpu->P = _MC6800_VF(cpu->P, false);
    }
        
    return v;
}

static inline uint8_t _mc6800_ror(mc6800_t* cpu, uint8_t v) {
    bool carry = v & 0x01;
    cpu->P &= ~(MC6800_NF|MC6800_ZF|MC6800_CF);
    if (carry) {
        cpu->P |= MC6800_CF;
    }
    v >>= 1;
    if (carry) {
        v |= 0x80;
    }
    cpu->P = _MC6800_NZ(cpu->P, v);

    if (((cpu->P & MC6800_NF) && !(cpu->P & MC6800_CF)) || 
        (!(cpu->P & MC6800_NF) && (cpu->P & MC6800_CF))) {
        cpu->P = _MC6800_VF(cpu->P, true);
    } else {
        cpu->P = _MC6800_VF(cpu->P, false);
    }
        
    return v;
}

static inline void _mc6800_add(mc6800_t* cpu, uint8_t val, bool a, bool carry_mode) {
    uint16_t sum = (a?cpu->A:cpu->B) + val;
    if (carry_mode) {
        sum += cpu->P & MC6800_CF ? 1 : 0;
    }
    cpu->P &= ~(MC6800_VF|MC6800_CF);
    cpu->P = _MC6800_NZ(cpu->P, sum);
    if (~((a?cpu->A:cpu->B)^val) & ((a?cpu->A:cpu->B)^sum) & 0x80) {
        cpu->P |= MC6800_VF;
    }
    if (sum & 0xFF00) {
        cpu->P |= MC6800_CF;
    }
    if (a) {
        cpu->A = sum & 0xFF;
    }
    else {
        cpu->B = sum & 0xFF;
    }
}

static inline void _mc6800_sub(mc6800_t* cpu, uint8_t val, bool a, bool carry_mode) {
    uint16_t diff = (a?cpu->A:cpu->B) - val;
    if (carry_mode) {
        diff -= cpu->P & MC6800_CF ? 1 : 0;
    }
    cpu->P &= ~(MC6800_VF|MC6800_CF);
    cpu->P = _MC6800_NZ(cpu->P, (uint8_t)diff);
    if (((a?cpu->A:cpu->B)^val) & ((a?cpu->A:cpu->B)^diff) & 0x80) {
        cpu->P |= MC6800_VF;
    }
    if (diff & 0xFF00) {
        cpu->P |= MC6800_CF;
    }
    if (a) {
        cpu->A = diff & 0xFF;
    }
    else {
        cpu->B = diff & 0xFF;
    }
}

static inline void _mc6800_cmp(mc6800_t* cpu, uint8_t val, bool a) {
    uint16_t diff = (a?cpu->A:cpu->B) - val;
    cpu->P &= ~(MC6800_VF|MC6800_CF);
    cpu->P = _MC6800_NZ(cpu->P, (uint8_t)diff);
    if (((a?cpu->A:cpu->B)^val) & ((a?cpu->A:cpu->B)^diff) & 0x80) {
        cpu->P |= MC6800_VF;
    }
    if (diff & 0xFF00) {
        cpu->P |= MC6800_CF;
    }
}

static inline void _mc6800_cpx(mc6800_t* cpu, uint16_t val) {
    uint16_t diff = cpu->IX - val;
    cpu->P &= ~(MC6800_VF|MC6800_CF);
    cpu->P = _MC6800_NZ16(cpu->P, diff);
    if ((cpu->IX^val) & (cpu->IX^diff) & 0x80) {
        cpu->P |= MC6800_VF;
    }
}

#undef _MC6800_NZ

uint64_t mc6800_init(mc6800_t* c) {
    CHIPS_ASSERT(c);
    memset(c, 0, sizeof(*c));
    c->P = MC6800_ZF;
    c->PINS = MC6800_RW | MC6800_RESET | MC6800_VMA;
    c->next_instr = true;
    return c->PINS;
}

/* set 16-bit address in 64-bit pin mask */
#define _SA(addr) pins=(pins&~0xFFFF)|((addr)&0xFFFFULL)
/* extract 16-bit addess from pin mask */
#define _GA() ((uint16_t)(pins&0xFFFFULL))
/* set 16-bit address and 8-bit data in 64-bit pin mask */
#define _SAD(addr,data) pins=(pins&~0xFFFFFF)|((((data)&0xFF)<<16)&0xFF0000ULL)|((addr)&0xFFFFULL)
/* fetch next opcode byte */
#define _FETCH() _SA(c->PC);c->next_instr=true;
/* set 8-bit data in 64-bit pin mask */
#define _SD(data) pins=((pins&~0xFF0000ULL)|(((data&0xFF)<<16)&0xFF0000ULL))
/* extract 8-bit data from 64-bit pin mask */
#define _GD() ((uint8_t)((pins&0xFF0000ULL)>>16))
/* enable control pins */
#define _ON(m) pins|=(m)
/* disable control pins */
#define _OFF(m) pins&=~(m)
/* a memory read tick */
#define _RD() _ON(MC6800_RW);
/* a memory write tick */
#define _WR() _OFF(MC6800_RW);
/* set N and Z flags depending on value */
#define _NZ(v) c->P=((c->P&~(MC6800_NF|MC6800_ZF))|((v&0xFF)?((v)>>4&MC6800_NF):MC6800_ZF))
#define _NZ16(v) c->P=((c->P&~(MC6800_NF|MC6800_ZF))|((v&0xFFFF)?((v)>>12&MC6800_NF):MC6800_ZF))
/* set V flag if true */
#define _VF(v) c->P=((c->P&~MC6800_VF)|(v?MC6800_VF:0))
/* set CF flag if true */
#define _CF(v) c->P=((c->P&~MC6800_CF)|(v?MC6800_CF:0))
/* set VMA (address bus unstable) */
#define _VMA() _OFF(MC6800_VMA);
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4244)   /* conversion from 'uint16_t' to 'uint8_t', possible loss of data */
#endif

uint64_t mc6800_tick(mc6800_t* c, uint64_t pins) {
    if (pins & (MC6800_IRQ|MC6800_NMI|MC6800_HALT|MC6800_RESET)) {
        // TODO check for mc6800 re: m6502: interrupt detection also works in RDY phases, but only NMI is "sticky"

        // NMI is edge-triggered
        if ((pins & (pins ^ c->PINS)) & MC6800_NMI) {
            // TODO interrupt handler

            // TODO check
            c->nmi_pip |= 0x100;
        }
	// IRQ test is level triggered
        if ((pins & MC6800_IRQ) && (0 == (c->P & MC6800_IF))) {
            // TODO interrupt handler

            // TODO check
            c->irq_pip |= 0x100;
        }

	// TODO check for mc6800 re: m6502: RDY pin is only checked during read cycles
        // current status: HALT is not limited to read
        if ((pins & MC6800_HALT) == MC6800_HALT) {
            c->PINS = pins;
            c->irq_pip <<= 1;
            return pins;
        }
    }

    if (c->next_instr) {
        // load new instruction into 'instruction register' and restart tick counter
        c->IR = _GD()<<4;
        c->next_instr = false;

        // TODO check below

        // check IRQ, NMI and RESET state
        //  - IRQ is level-triggered and must be active in the full cycle
        //    before SYNC
        //  - NMI is edge-triggered, and the change must have happened in
        //    any cycle before SYNC
        //  - RES behaves slightly different than on a real 6502, we go
        //    into RES state as soon as the pin goes active, from there
        //    on, behaviour is 'standard'
        if (c->irq_pip & 0x400) {
            c->brk_flags |= MC6800_BRK_IRQ;
        }
        if (c->nmi_pip & 0xFC00) {
            c->brk_flags |= MC6800_BRK_NMI;
        }
        if (pins & MC6800_RESET) {
            c->brk_flags |= MC6800_BRK_RESET;
        }
        c->irq_pip &= 0x3FF;
        c->nmi_pip &= 0x3FF;

        // if interrupt or reset was requested, force a BRK instruction
        if (c->brk_flags) {
            // start interrupt sequence
            c->IR = 0x3B; // SWI
            pins &= ~MC6800_RESET;
        }
        else {
            c->PC++;
        }

        // TODO check above
    }


    // reads are default, writes are special
    _RD();
    // address is stable by default
    _ON(MC6800_VMA);
    switch (c->IR++) {
    /* NOP  */
        case (0x00<<4)|0: _VMA();break;
        case (0x00<<4)|1: _FETCH();break;
        case (0x00<<4)|2: assert(false);break;
        case (0x00<<4)|3: assert(false);break;
        case (0x00<<4)|4: assert(false);break;
        case (0x00<<4)|5: assert(false);break;
        case (0x00<<4)|6: assert(false);break;
        case (0x00<<4)|7: assert(false);break;
        case (0x00<<4)|8: assert(false);break;
        case (0x00<<4)|9: assert(false);break;
        case (0x00<<4)|10: assert(false);break;
        case (0x00<<4)|11: assert(false);break;
        case (0x00<<4)|12: assert(false);break;
        case (0x00<<4)|13: assert(false);break;
        case (0x00<<4)|14: assert(false);break;
        case (0x00<<4)|15: assert(false);break;
    /* NOP  */
        case (0x01<<4)|0: _VMA();break;
        case (0x01<<4)|1: _FETCH();break;
        case (0x01<<4)|2: assert(false);break;
        case (0x01<<4)|3: assert(false);break;
        case (0x01<<4)|4: assert(false);break;
        case (0x01<<4)|5: assert(false);break;
        case (0x01<<4)|6: assert(false);break;
        case (0x01<<4)|7: assert(false);break;
        case (0x01<<4)|8: assert(false);break;
        case (0x01<<4)|9: assert(false);break;
        case (0x01<<4)|10: assert(false);break;
        case (0x01<<4)|11: assert(false);break;
        case (0x01<<4)|12: assert(false);break;
        case (0x01<<4)|13: assert(false);break;
        case (0x01<<4)|14: assert(false);break;
        case (0x01<<4)|15: assert(false);break;
    /* NOP  */
        case (0x02<<4)|0: _VMA();break;
        case (0x02<<4)|1: _FETCH();break;
        case (0x02<<4)|2: assert(false);break;
        case (0x02<<4)|3: assert(false);break;
        case (0x02<<4)|4: assert(false);break;
        case (0x02<<4)|5: assert(false);break;
        case (0x02<<4)|6: assert(false);break;
        case (0x02<<4)|7: assert(false);break;
        case (0x02<<4)|8: assert(false);break;
        case (0x02<<4)|9: assert(false);break;
        case (0x02<<4)|10: assert(false);break;
        case (0x02<<4)|11: assert(false);break;
        case (0x02<<4)|12: assert(false);break;
        case (0x02<<4)|13: assert(false);break;
        case (0x02<<4)|14: assert(false);break;
        case (0x02<<4)|15: assert(false);break;
    /* NOP  */
        case (0x03<<4)|0: _VMA();break;
        case (0x03<<4)|1: _FETCH();break;
        case (0x03<<4)|2: assert(false);break;
        case (0x03<<4)|3: assert(false);break;
        case (0x03<<4)|4: assert(false);break;
        case (0x03<<4)|5: assert(false);break;
        case (0x03<<4)|6: assert(false);break;
        case (0x03<<4)|7: assert(false);break;
        case (0x03<<4)|8: assert(false);break;
        case (0x03<<4)|9: assert(false);break;
        case (0x03<<4)|10: assert(false);break;
        case (0x03<<4)|11: assert(false);break;
        case (0x03<<4)|12: assert(false);break;
        case (0x03<<4)|13: assert(false);break;
        case (0x03<<4)|14: assert(false);break;
        case (0x03<<4)|15: assert(false);break;
    /* NOP  */
        case (0x04<<4)|0: _VMA();break;
        case (0x04<<4)|1: _FETCH();break;
        case (0x04<<4)|2: assert(false);break;
        case (0x04<<4)|3: assert(false);break;
        case (0x04<<4)|4: assert(false);break;
        case (0x04<<4)|5: assert(false);break;
        case (0x04<<4)|6: assert(false);break;
        case (0x04<<4)|7: assert(false);break;
        case (0x04<<4)|8: assert(false);break;
        case (0x04<<4)|9: assert(false);break;
        case (0x04<<4)|10: assert(false);break;
        case (0x04<<4)|11: assert(false);break;
        case (0x04<<4)|12: assert(false);break;
        case (0x04<<4)|13: assert(false);break;
        case (0x04<<4)|14: assert(false);break;
        case (0x04<<4)|15: assert(false);break;
    /* NOP  */
        case (0x05<<4)|0: _VMA();break;
        case (0x05<<4)|1: _FETCH();break;
        case (0x05<<4)|2: assert(false);break;
        case (0x05<<4)|3: assert(false);break;
        case (0x05<<4)|4: assert(false);break;
        case (0x05<<4)|5: assert(false);break;
        case (0x05<<4)|6: assert(false);break;
        case (0x05<<4)|7: assert(false);break;
        case (0x05<<4)|8: assert(false);break;
        case (0x05<<4)|9: assert(false);break;
        case (0x05<<4)|10: assert(false);break;
        case (0x05<<4)|11: assert(false);break;
        case (0x05<<4)|12: assert(false);break;
        case (0x05<<4)|13: assert(false);break;
        case (0x05<<4)|14: assert(false);break;
        case (0x05<<4)|15: assert(false);break;
    /* NOP  */
        case (0x06<<4)|0: _VMA();break;
        case (0x06<<4)|1: _FETCH();break;
        case (0x06<<4)|2: assert(false);break;
        case (0x06<<4)|3: assert(false);break;
        case (0x06<<4)|4: assert(false);break;
        case (0x06<<4)|5: assert(false);break;
        case (0x06<<4)|6: assert(false);break;
        case (0x06<<4)|7: assert(false);break;
        case (0x06<<4)|8: assert(false);break;
        case (0x06<<4)|9: assert(false);break;
        case (0x06<<4)|10: assert(false);break;
        case (0x06<<4)|11: assert(false);break;
        case (0x06<<4)|12: assert(false);break;
        case (0x06<<4)|13: assert(false);break;
        case (0x06<<4)|14: assert(false);break;
        case (0x06<<4)|15: assert(false);break;
    /* NOP  */
        case (0x07<<4)|0: _VMA();break;
        case (0x07<<4)|1: _FETCH();break;
        case (0x07<<4)|2: assert(false);break;
        case (0x07<<4)|3: assert(false);break;
        case (0x07<<4)|4: assert(false);break;
        case (0x07<<4)|5: assert(false);break;
        case (0x07<<4)|6: assert(false);break;
        case (0x07<<4)|7: assert(false);break;
        case (0x07<<4)|8: assert(false);break;
        case (0x07<<4)|9: assert(false);break;
        case (0x07<<4)|10: assert(false);break;
        case (0x07<<4)|11: assert(false);break;
        case (0x07<<4)|12: assert(false);break;
        case (0x07<<4)|13: assert(false);break;
        case (0x07<<4)|14: assert(false);break;
        case (0x07<<4)|15: assert(false);break;
    /* NOP  */
        case (0x08<<4)|0: _VMA();break;
        case (0x08<<4)|1: _FETCH();break;
        case (0x08<<4)|2: assert(false);break;
        case (0x08<<4)|3: assert(false);break;
        case (0x08<<4)|4: assert(false);break;
        case (0x08<<4)|5: assert(false);break;
        case (0x08<<4)|6: assert(false);break;
        case (0x08<<4)|7: assert(false);break;
        case (0x08<<4)|8: assert(false);break;
        case (0x08<<4)|9: assert(false);break;
        case (0x08<<4)|10: assert(false);break;
        case (0x08<<4)|11: assert(false);break;
        case (0x08<<4)|12: assert(false);break;
        case (0x08<<4)|13: assert(false);break;
        case (0x08<<4)|14: assert(false);break;
        case (0x08<<4)|15: assert(false);break;
    /* NOP  */
        case (0x09<<4)|0: _VMA();break;
        case (0x09<<4)|1: _FETCH();break;
        case (0x09<<4)|2: assert(false);break;
        case (0x09<<4)|3: assert(false);break;
        case (0x09<<4)|4: assert(false);break;
        case (0x09<<4)|5: assert(false);break;
        case (0x09<<4)|6: assert(false);break;
        case (0x09<<4)|7: assert(false);break;
        case (0x09<<4)|8: assert(false);break;
        case (0x09<<4)|9: assert(false);break;
        case (0x09<<4)|10: assert(false);break;
        case (0x09<<4)|11: assert(false);break;
        case (0x09<<4)|12: assert(false);break;
        case (0x09<<4)|13: assert(false);break;
        case (0x09<<4)|14: assert(false);break;
        case (0x09<<4)|15: assert(false);break;
    /* NOP  */
        case (0x0A<<4)|0: _VMA();break;
        case (0x0A<<4)|1: _FETCH();break;
        case (0x0A<<4)|2: assert(false);break;
        case (0x0A<<4)|3: assert(false);break;
        case (0x0A<<4)|4: assert(false);break;
        case (0x0A<<4)|5: assert(false);break;
        case (0x0A<<4)|6: assert(false);break;
        case (0x0A<<4)|7: assert(false);break;
        case (0x0A<<4)|8: assert(false);break;
        case (0x0A<<4)|9: assert(false);break;
        case (0x0A<<4)|10: assert(false);break;
        case (0x0A<<4)|11: assert(false);break;
        case (0x0A<<4)|12: assert(false);break;
        case (0x0A<<4)|13: assert(false);break;
        case (0x0A<<4)|14: assert(false);break;
        case (0x0A<<4)|15: assert(false);break;
    /* NOP  */
        case (0x0B<<4)|0: _VMA();break;
        case (0x0B<<4)|1: _FETCH();break;
        case (0x0B<<4)|2: assert(false);break;
        case (0x0B<<4)|3: assert(false);break;
        case (0x0B<<4)|4: assert(false);break;
        case (0x0B<<4)|5: assert(false);break;
        case (0x0B<<4)|6: assert(false);break;
        case (0x0B<<4)|7: assert(false);break;
        case (0x0B<<4)|8: assert(false);break;
        case (0x0B<<4)|9: assert(false);break;
        case (0x0B<<4)|10: assert(false);break;
        case (0x0B<<4)|11: assert(false);break;
        case (0x0B<<4)|12: assert(false);break;
        case (0x0B<<4)|13: assert(false);break;
        case (0x0B<<4)|14: assert(false);break;
        case (0x0B<<4)|15: assert(false);break;
    /* NOP  */
        case (0x0C<<4)|0: _VMA();break;
        case (0x0C<<4)|1: _FETCH();break;
        case (0x0C<<4)|2: assert(false);break;
        case (0x0C<<4)|3: assert(false);break;
        case (0x0C<<4)|4: assert(false);break;
        case (0x0C<<4)|5: assert(false);break;
        case (0x0C<<4)|6: assert(false);break;
        case (0x0C<<4)|7: assert(false);break;
        case (0x0C<<4)|8: assert(false);break;
        case (0x0C<<4)|9: assert(false);break;
        case (0x0C<<4)|10: assert(false);break;
        case (0x0C<<4)|11: assert(false);break;
        case (0x0C<<4)|12: assert(false);break;
        case (0x0C<<4)|13: assert(false);break;
        case (0x0C<<4)|14: assert(false);break;
        case (0x0C<<4)|15: assert(false);break;
    /* NOP  */
        case (0x0D<<4)|0: _VMA();break;
        case (0x0D<<4)|1: _FETCH();break;
        case (0x0D<<4)|2: assert(false);break;
        case (0x0D<<4)|3: assert(false);break;
        case (0x0D<<4)|4: assert(false);break;
        case (0x0D<<4)|5: assert(false);break;
        case (0x0D<<4)|6: assert(false);break;
        case (0x0D<<4)|7: assert(false);break;
        case (0x0D<<4)|8: assert(false);break;
        case (0x0D<<4)|9: assert(false);break;
        case (0x0D<<4)|10: assert(false);break;
        case (0x0D<<4)|11: assert(false);break;
        case (0x0D<<4)|12: assert(false);break;
        case (0x0D<<4)|13: assert(false);break;
        case (0x0D<<4)|14: assert(false);break;
        case (0x0D<<4)|15: assert(false);break;
    /* NOP  */
        case (0x0E<<4)|0: _VMA();break;
        case (0x0E<<4)|1: _FETCH();break;
        case (0x0E<<4)|2: assert(false);break;
        case (0x0E<<4)|3: assert(false);break;
        case (0x0E<<4)|4: assert(false);break;
        case (0x0E<<4)|5: assert(false);break;
        case (0x0E<<4)|6: assert(false);break;
        case (0x0E<<4)|7: assert(false);break;
        case (0x0E<<4)|8: assert(false);break;
        case (0x0E<<4)|9: assert(false);break;
        case (0x0E<<4)|10: assert(false);break;
        case (0x0E<<4)|11: assert(false);break;
        case (0x0E<<4)|12: assert(false);break;
        case (0x0E<<4)|13: assert(false);break;
        case (0x0E<<4)|14: assert(false);break;
        case (0x0E<<4)|15: assert(false);break;
    /* NOP  */
        case (0x0F<<4)|0: _VMA();break;
        case (0x0F<<4)|1: _FETCH();break;
        case (0x0F<<4)|2: assert(false);break;
        case (0x0F<<4)|3: assert(false);break;
        case (0x0F<<4)|4: assert(false);break;
        case (0x0F<<4)|5: assert(false);break;
        case (0x0F<<4)|6: assert(false);break;
        case (0x0F<<4)|7: assert(false);break;
        case (0x0F<<4)|8: assert(false);break;
        case (0x0F<<4)|9: assert(false);break;
        case (0x0F<<4)|10: assert(false);break;
        case (0x0F<<4)|11: assert(false);break;
        case (0x0F<<4)|12: assert(false);break;
        case (0x0F<<4)|13: assert(false);break;
        case (0x0F<<4)|14: assert(false);break;
        case (0x0F<<4)|15: assert(false);break;
    /* NOP  */
        case (0x10<<4)|0: _VMA();break;
        case (0x10<<4)|1: _FETCH();break;
        case (0x10<<4)|2: assert(false);break;
        case (0x10<<4)|3: assert(false);break;
        case (0x10<<4)|4: assert(false);break;
        case (0x10<<4)|5: assert(false);break;
        case (0x10<<4)|6: assert(false);break;
        case (0x10<<4)|7: assert(false);break;
        case (0x10<<4)|8: assert(false);break;
        case (0x10<<4)|9: assert(false);break;
        case (0x10<<4)|10: assert(false);break;
        case (0x10<<4)|11: assert(false);break;
        case (0x10<<4)|12: assert(false);break;
        case (0x10<<4)|13: assert(false);break;
        case (0x10<<4)|14: assert(false);break;
        case (0x10<<4)|15: assert(false);break;
    /* NOP  */
        case (0x11<<4)|0: _VMA();break;
        case (0x11<<4)|1: _FETCH();break;
        case (0x11<<4)|2: assert(false);break;
        case (0x11<<4)|3: assert(false);break;
        case (0x11<<4)|4: assert(false);break;
        case (0x11<<4)|5: assert(false);break;
        case (0x11<<4)|6: assert(false);break;
        case (0x11<<4)|7: assert(false);break;
        case (0x11<<4)|8: assert(false);break;
        case (0x11<<4)|9: assert(false);break;
        case (0x11<<4)|10: assert(false);break;
        case (0x11<<4)|11: assert(false);break;
        case (0x11<<4)|12: assert(false);break;
        case (0x11<<4)|13: assert(false);break;
        case (0x11<<4)|14: assert(false);break;
        case (0x11<<4)|15: assert(false);break;
    /* NOP  */
        case (0x12<<4)|0: _VMA();break;
        case (0x12<<4)|1: _FETCH();break;
        case (0x12<<4)|2: assert(false);break;
        case (0x12<<4)|3: assert(false);break;
        case (0x12<<4)|4: assert(false);break;
        case (0x12<<4)|5: assert(false);break;
        case (0x12<<4)|6: assert(false);break;
        case (0x12<<4)|7: assert(false);break;
        case (0x12<<4)|8: assert(false);break;
        case (0x12<<4)|9: assert(false);break;
        case (0x12<<4)|10: assert(false);break;
        case (0x12<<4)|11: assert(false);break;
        case (0x12<<4)|12: assert(false);break;
        case (0x12<<4)|13: assert(false);break;
        case (0x12<<4)|14: assert(false);break;
        case (0x12<<4)|15: assert(false);break;
    /* NOP  */
        case (0x13<<4)|0: _VMA();break;
        case (0x13<<4)|1: _FETCH();break;
        case (0x13<<4)|2: assert(false);break;
        case (0x13<<4)|3: assert(false);break;
        case (0x13<<4)|4: assert(false);break;
        case (0x13<<4)|5: assert(false);break;
        case (0x13<<4)|6: assert(false);break;
        case (0x13<<4)|7: assert(false);break;
        case (0x13<<4)|8: assert(false);break;
        case (0x13<<4)|9: assert(false);break;
        case (0x13<<4)|10: assert(false);break;
        case (0x13<<4)|11: assert(false);break;
        case (0x13<<4)|12: assert(false);break;
        case (0x13<<4)|13: assert(false);break;
        case (0x13<<4)|14: assert(false);break;
        case (0x13<<4)|15: assert(false);break;
    /* NOP  */
        case (0x14<<4)|0: _VMA();break;
        case (0x14<<4)|1: _FETCH();break;
        case (0x14<<4)|2: assert(false);break;
        case (0x14<<4)|3: assert(false);break;
        case (0x14<<4)|4: assert(false);break;
        case (0x14<<4)|5: assert(false);break;
        case (0x14<<4)|6: assert(false);break;
        case (0x14<<4)|7: assert(false);break;
        case (0x14<<4)|8: assert(false);break;
        case (0x14<<4)|9: assert(false);break;
        case (0x14<<4)|10: assert(false);break;
        case (0x14<<4)|11: assert(false);break;
        case (0x14<<4)|12: assert(false);break;
        case (0x14<<4)|13: assert(false);break;
        case (0x14<<4)|14: assert(false);break;
        case (0x14<<4)|15: assert(false);break;
    /* NOP  */
        case (0x15<<4)|0: _VMA();break;
        case (0x15<<4)|1: _FETCH();break;
        case (0x15<<4)|2: assert(false);break;
        case (0x15<<4)|3: assert(false);break;
        case (0x15<<4)|4: assert(false);break;
        case (0x15<<4)|5: assert(false);break;
        case (0x15<<4)|6: assert(false);break;
        case (0x15<<4)|7: assert(false);break;
        case (0x15<<4)|8: assert(false);break;
        case (0x15<<4)|9: assert(false);break;
        case (0x15<<4)|10: assert(false);break;
        case (0x15<<4)|11: assert(false);break;
        case (0x15<<4)|12: assert(false);break;
        case (0x15<<4)|13: assert(false);break;
        case (0x15<<4)|14: assert(false);break;
        case (0x15<<4)|15: assert(false);break;
    /* NOP  */
        case (0x16<<4)|0: _VMA();break;
        case (0x16<<4)|1: _FETCH();break;
        case (0x16<<4)|2: assert(false);break;
        case (0x16<<4)|3: assert(false);break;
        case (0x16<<4)|4: assert(false);break;
        case (0x16<<4)|5: assert(false);break;
        case (0x16<<4)|6: assert(false);break;
        case (0x16<<4)|7: assert(false);break;
        case (0x16<<4)|8: assert(false);break;
        case (0x16<<4)|9: assert(false);break;
        case (0x16<<4)|10: assert(false);break;
        case (0x16<<4)|11: assert(false);break;
        case (0x16<<4)|12: assert(false);break;
        case (0x16<<4)|13: assert(false);break;
        case (0x16<<4)|14: assert(false);break;
        case (0x16<<4)|15: assert(false);break;
    /* NOP  */
        case (0x17<<4)|0: _VMA();break;
        case (0x17<<4)|1: _FETCH();break;
        case (0x17<<4)|2: assert(false);break;
        case (0x17<<4)|3: assert(false);break;
        case (0x17<<4)|4: assert(false);break;
        case (0x17<<4)|5: assert(false);break;
        case (0x17<<4)|6: assert(false);break;
        case (0x17<<4)|7: assert(false);break;
        case (0x17<<4)|8: assert(false);break;
        case (0x17<<4)|9: assert(false);break;
        case (0x17<<4)|10: assert(false);break;
        case (0x17<<4)|11: assert(false);break;
        case (0x17<<4)|12: assert(false);break;
        case (0x17<<4)|13: assert(false);break;
        case (0x17<<4)|14: assert(false);break;
        case (0x17<<4)|15: assert(false);break;
    /* NOP  */
        case (0x18<<4)|0: _VMA();break;
        case (0x18<<4)|1: _FETCH();break;
        case (0x18<<4)|2: assert(false);break;
        case (0x18<<4)|3: assert(false);break;
        case (0x18<<4)|4: assert(false);break;
        case (0x18<<4)|5: assert(false);break;
        case (0x18<<4)|6: assert(false);break;
        case (0x18<<4)|7: assert(false);break;
        case (0x18<<4)|8: assert(false);break;
        case (0x18<<4)|9: assert(false);break;
        case (0x18<<4)|10: assert(false);break;
        case (0x18<<4)|11: assert(false);break;
        case (0x18<<4)|12: assert(false);break;
        case (0x18<<4)|13: assert(false);break;
        case (0x18<<4)|14: assert(false);break;
        case (0x18<<4)|15: assert(false);break;
    /* NOP  */
        case (0x19<<4)|0: _VMA();break;
        case (0x19<<4)|1: _FETCH();break;
        case (0x19<<4)|2: assert(false);break;
        case (0x19<<4)|3: assert(false);break;
        case (0x19<<4)|4: assert(false);break;
        case (0x19<<4)|5: assert(false);break;
        case (0x19<<4)|6: assert(false);break;
        case (0x19<<4)|7: assert(false);break;
        case (0x19<<4)|8: assert(false);break;
        case (0x19<<4)|9: assert(false);break;
        case (0x19<<4)|10: assert(false);break;
        case (0x19<<4)|11: assert(false);break;
        case (0x19<<4)|12: assert(false);break;
        case (0x19<<4)|13: assert(false);break;
        case (0x19<<4)|14: assert(false);break;
        case (0x19<<4)|15: assert(false);break;
    /* NOP  */
        case (0x1A<<4)|0: _VMA();break;
        case (0x1A<<4)|1: _FETCH();break;
        case (0x1A<<4)|2: assert(false);break;
        case (0x1A<<4)|3: assert(false);break;
        case (0x1A<<4)|4: assert(false);break;
        case (0x1A<<4)|5: assert(false);break;
        case (0x1A<<4)|6: assert(false);break;
        case (0x1A<<4)|7: assert(false);break;
        case (0x1A<<4)|8: assert(false);break;
        case (0x1A<<4)|9: assert(false);break;
        case (0x1A<<4)|10: assert(false);break;
        case (0x1A<<4)|11: assert(false);break;
        case (0x1A<<4)|12: assert(false);break;
        case (0x1A<<4)|13: assert(false);break;
        case (0x1A<<4)|14: assert(false);break;
        case (0x1A<<4)|15: assert(false);break;
    /* NOP  */
        case (0x1B<<4)|0: _VMA();break;
        case (0x1B<<4)|1: _FETCH();break;
        case (0x1B<<4)|2: assert(false);break;
        case (0x1B<<4)|3: assert(false);break;
        case (0x1B<<4)|4: assert(false);break;
        case (0x1B<<4)|5: assert(false);break;
        case (0x1B<<4)|6: assert(false);break;
        case (0x1B<<4)|7: assert(false);break;
        case (0x1B<<4)|8: assert(false);break;
        case (0x1B<<4)|9: assert(false);break;
        case (0x1B<<4)|10: assert(false);break;
        case (0x1B<<4)|11: assert(false);break;
        case (0x1B<<4)|12: assert(false);break;
        case (0x1B<<4)|13: assert(false);break;
        case (0x1B<<4)|14: assert(false);break;
        case (0x1B<<4)|15: assert(false);break;
    /* NOP  */
        case (0x1C<<4)|0: _VMA();break;
        case (0x1C<<4)|1: _FETCH();break;
        case (0x1C<<4)|2: assert(false);break;
        case (0x1C<<4)|3: assert(false);break;
        case (0x1C<<4)|4: assert(false);break;
        case (0x1C<<4)|5: assert(false);break;
        case (0x1C<<4)|6: assert(false);break;
        case (0x1C<<4)|7: assert(false);break;
        case (0x1C<<4)|8: assert(false);break;
        case (0x1C<<4)|9: assert(false);break;
        case (0x1C<<4)|10: assert(false);break;
        case (0x1C<<4)|11: assert(false);break;
        case (0x1C<<4)|12: assert(false);break;
        case (0x1C<<4)|13: assert(false);break;
        case (0x1C<<4)|14: assert(false);break;
        case (0x1C<<4)|15: assert(false);break;
    /* NOP  */
        case (0x1D<<4)|0: _VMA();break;
        case (0x1D<<4)|1: _FETCH();break;
        case (0x1D<<4)|2: assert(false);break;
        case (0x1D<<4)|3: assert(false);break;
        case (0x1D<<4)|4: assert(false);break;
        case (0x1D<<4)|5: assert(false);break;
        case (0x1D<<4)|6: assert(false);break;
        case (0x1D<<4)|7: assert(false);break;
        case (0x1D<<4)|8: assert(false);break;
        case (0x1D<<4)|9: assert(false);break;
        case (0x1D<<4)|10: assert(false);break;
        case (0x1D<<4)|11: assert(false);break;
        case (0x1D<<4)|12: assert(false);break;
        case (0x1D<<4)|13: assert(false);break;
        case (0x1D<<4)|14: assert(false);break;
        case (0x1D<<4)|15: assert(false);break;
    /* NOP  */
        case (0x1E<<4)|0: _VMA();break;
        case (0x1E<<4)|1: _FETCH();break;
        case (0x1E<<4)|2: assert(false);break;
        case (0x1E<<4)|3: assert(false);break;
        case (0x1E<<4)|4: assert(false);break;
        case (0x1E<<4)|5: assert(false);break;
        case (0x1E<<4)|6: assert(false);break;
        case (0x1E<<4)|7: assert(false);break;
        case (0x1E<<4)|8: assert(false);break;
        case (0x1E<<4)|9: assert(false);break;
        case (0x1E<<4)|10: assert(false);break;
        case (0x1E<<4)|11: assert(false);break;
        case (0x1E<<4)|12: assert(false);break;
        case (0x1E<<4)|13: assert(false);break;
        case (0x1E<<4)|14: assert(false);break;
        case (0x1E<<4)|15: assert(false);break;
    /* NOP  */
        case (0x1F<<4)|0: _VMA();break;
        case (0x1F<<4)|1: _FETCH();break;
        case (0x1F<<4)|2: assert(false);break;
        case (0x1F<<4)|3: assert(false);break;
        case (0x1F<<4)|4: assert(false);break;
        case (0x1F<<4)|5: assert(false);break;
        case (0x1F<<4)|6: assert(false);break;
        case (0x1F<<4)|7: assert(false);break;
        case (0x1F<<4)|8: assert(false);break;
        case (0x1F<<4)|9: assert(false);break;
        case (0x1F<<4)|10: assert(false);break;
        case (0x1F<<4)|11: assert(false);break;
        case (0x1F<<4)|12: assert(false);break;
        case (0x1F<<4)|13: assert(false);break;
        case (0x1F<<4)|14: assert(false);break;
        case (0x1F<<4)|15: assert(false);break;
    /* NOP # */
        case (0x20<<4)|0: _SA(c->PC++);break;
        case (0x20<<4)|1: _VMA();break;
        case (0x20<<4)|2: _FETCH();break;
        case (0x20<<4)|3: assert(false);break;
        case (0x20<<4)|4: assert(false);break;
        case (0x20<<4)|5: assert(false);break;
        case (0x20<<4)|6: assert(false);break;
        case (0x20<<4)|7: assert(false);break;
        case (0x20<<4)|8: assert(false);break;
        case (0x20<<4)|9: assert(false);break;
        case (0x20<<4)|10: assert(false);break;
        case (0x20<<4)|11: assert(false);break;
        case (0x20<<4)|12: assert(false);break;
        case (0x20<<4)|13: assert(false);break;
        case (0x20<<4)|14: assert(false);break;
        case (0x20<<4)|15: assert(false);break;
    /* NOP # */
        case (0x21<<4)|0: _SA(c->PC++);break;
        case (0x21<<4)|1: _VMA();break;
        case (0x21<<4)|2: _FETCH();break;
        case (0x21<<4)|3: assert(false);break;
        case (0x21<<4)|4: assert(false);break;
        case (0x21<<4)|5: assert(false);break;
        case (0x21<<4)|6: assert(false);break;
        case (0x21<<4)|7: assert(false);break;
        case (0x21<<4)|8: assert(false);break;
        case (0x21<<4)|9: assert(false);break;
        case (0x21<<4)|10: assert(false);break;
        case (0x21<<4)|11: assert(false);break;
        case (0x21<<4)|12: assert(false);break;
        case (0x21<<4)|13: assert(false);break;
        case (0x21<<4)|14: assert(false);break;
        case (0x21<<4)|15: assert(false);break;
    /* NOP # */
        case (0x22<<4)|0: _SA(c->PC++);break;
        case (0x22<<4)|1: _VMA();break;
        case (0x22<<4)|2: _FETCH();break;
        case (0x22<<4)|3: assert(false);break;
        case (0x22<<4)|4: assert(false);break;
        case (0x22<<4)|5: assert(false);break;
        case (0x22<<4)|6: assert(false);break;
        case (0x22<<4)|7: assert(false);break;
        case (0x22<<4)|8: assert(false);break;
        case (0x22<<4)|9: assert(false);break;
        case (0x22<<4)|10: assert(false);break;
        case (0x22<<4)|11: assert(false);break;
        case (0x22<<4)|12: assert(false);break;
        case (0x22<<4)|13: assert(false);break;
        case (0x22<<4)|14: assert(false);break;
        case (0x22<<4)|15: assert(false);break;
    /* NOP # */
        case (0x23<<4)|0: _SA(c->PC++);break;
        case (0x23<<4)|1: _VMA();break;
        case (0x23<<4)|2: _FETCH();break;
        case (0x23<<4)|3: assert(false);break;
        case (0x23<<4)|4: assert(false);break;
        case (0x23<<4)|5: assert(false);break;
        case (0x23<<4)|6: assert(false);break;
        case (0x23<<4)|7: assert(false);break;
        case (0x23<<4)|8: assert(false);break;
        case (0x23<<4)|9: assert(false);break;
        case (0x23<<4)|10: assert(false);break;
        case (0x23<<4)|11: assert(false);break;
        case (0x23<<4)|12: assert(false);break;
        case (0x23<<4)|13: assert(false);break;
        case (0x23<<4)|14: assert(false);break;
        case (0x23<<4)|15: assert(false);break;
    /* NOP # */
        case (0x24<<4)|0: _SA(c->PC++);break;
        case (0x24<<4)|1: _VMA();break;
        case (0x24<<4)|2: _FETCH();break;
        case (0x24<<4)|3: assert(false);break;
        case (0x24<<4)|4: assert(false);break;
        case (0x24<<4)|5: assert(false);break;
        case (0x24<<4)|6: assert(false);break;
        case (0x24<<4)|7: assert(false);break;
        case (0x24<<4)|8: assert(false);break;
        case (0x24<<4)|9: assert(false);break;
        case (0x24<<4)|10: assert(false);break;
        case (0x24<<4)|11: assert(false);break;
        case (0x24<<4)|12: assert(false);break;
        case (0x24<<4)|13: assert(false);break;
        case (0x24<<4)|14: assert(false);break;
        case (0x24<<4)|15: assert(false);break;
    /* NOP # */
        case (0x25<<4)|0: _SA(c->PC++);break;
        case (0x25<<4)|1: _VMA();break;
        case (0x25<<4)|2: _FETCH();break;
        case (0x25<<4)|3: assert(false);break;
        case (0x25<<4)|4: assert(false);break;
        case (0x25<<4)|5: assert(false);break;
        case (0x25<<4)|6: assert(false);break;
        case (0x25<<4)|7: assert(false);break;
        case (0x25<<4)|8: assert(false);break;
        case (0x25<<4)|9: assert(false);break;
        case (0x25<<4)|10: assert(false);break;
        case (0x25<<4)|11: assert(false);break;
        case (0x25<<4)|12: assert(false);break;
        case (0x25<<4)|13: assert(false);break;
        case (0x25<<4)|14: assert(false);break;
        case (0x25<<4)|15: assert(false);break;
    /* NOP # */
        case (0x26<<4)|0: _SA(c->PC++);break;
        case (0x26<<4)|1: _VMA();break;
        case (0x26<<4)|2: _FETCH();break;
        case (0x26<<4)|3: assert(false);break;
        case (0x26<<4)|4: assert(false);break;
        case (0x26<<4)|5: assert(false);break;
        case (0x26<<4)|6: assert(false);break;
        case (0x26<<4)|7: assert(false);break;
        case (0x26<<4)|8: assert(false);break;
        case (0x26<<4)|9: assert(false);break;
        case (0x26<<4)|10: assert(false);break;
        case (0x26<<4)|11: assert(false);break;
        case (0x26<<4)|12: assert(false);break;
        case (0x26<<4)|13: assert(false);break;
        case (0x26<<4)|14: assert(false);break;
        case (0x26<<4)|15: assert(false);break;
    /* NOP # */
        case (0x27<<4)|0: _SA(c->PC++);break;
        case (0x27<<4)|1: _VMA();break;
        case (0x27<<4)|2: _FETCH();break;
        case (0x27<<4)|3: assert(false);break;
        case (0x27<<4)|4: assert(false);break;
        case (0x27<<4)|5: assert(false);break;
        case (0x27<<4)|6: assert(false);break;
        case (0x27<<4)|7: assert(false);break;
        case (0x27<<4)|8: assert(false);break;
        case (0x27<<4)|9: assert(false);break;
        case (0x27<<4)|10: assert(false);break;
        case (0x27<<4)|11: assert(false);break;
        case (0x27<<4)|12: assert(false);break;
        case (0x27<<4)|13: assert(false);break;
        case (0x27<<4)|14: assert(false);break;
        case (0x27<<4)|15: assert(false);break;
    /* NOP # */
        case (0x28<<4)|0: _SA(c->PC++);break;
        case (0x28<<4)|1: _VMA();break;
        case (0x28<<4)|2: _FETCH();break;
        case (0x28<<4)|3: assert(false);break;
        case (0x28<<4)|4: assert(false);break;
        case (0x28<<4)|5: assert(false);break;
        case (0x28<<4)|6: assert(false);break;
        case (0x28<<4)|7: assert(false);break;
        case (0x28<<4)|8: assert(false);break;
        case (0x28<<4)|9: assert(false);break;
        case (0x28<<4)|10: assert(false);break;
        case (0x28<<4)|11: assert(false);break;
        case (0x28<<4)|12: assert(false);break;
        case (0x28<<4)|13: assert(false);break;
        case (0x28<<4)|14: assert(false);break;
        case (0x28<<4)|15: assert(false);break;
    /* NOP # */
        case (0x29<<4)|0: _SA(c->PC++);break;
        case (0x29<<4)|1: _VMA();break;
        case (0x29<<4)|2: _FETCH();break;
        case (0x29<<4)|3: assert(false);break;
        case (0x29<<4)|4: assert(false);break;
        case (0x29<<4)|5: assert(false);break;
        case (0x29<<4)|6: assert(false);break;
        case (0x29<<4)|7: assert(false);break;
        case (0x29<<4)|8: assert(false);break;
        case (0x29<<4)|9: assert(false);break;
        case (0x29<<4)|10: assert(false);break;
        case (0x29<<4)|11: assert(false);break;
        case (0x29<<4)|12: assert(false);break;
        case (0x29<<4)|13: assert(false);break;
        case (0x29<<4)|14: assert(false);break;
        case (0x29<<4)|15: assert(false);break;
    /* NOP # */
        case (0x2A<<4)|0: _SA(c->PC++);break;
        case (0x2A<<4)|1: _VMA();break;
        case (0x2A<<4)|2: _FETCH();break;
        case (0x2A<<4)|3: assert(false);break;
        case (0x2A<<4)|4: assert(false);break;
        case (0x2A<<4)|5: assert(false);break;
        case (0x2A<<4)|6: assert(false);break;
        case (0x2A<<4)|7: assert(false);break;
        case (0x2A<<4)|8: assert(false);break;
        case (0x2A<<4)|9: assert(false);break;
        case (0x2A<<4)|10: assert(false);break;
        case (0x2A<<4)|11: assert(false);break;
        case (0x2A<<4)|12: assert(false);break;
        case (0x2A<<4)|13: assert(false);break;
        case (0x2A<<4)|14: assert(false);break;
        case (0x2A<<4)|15: assert(false);break;
    /* NOP # */
        case (0x2B<<4)|0: _SA(c->PC++);break;
        case (0x2B<<4)|1: _VMA();break;
        case (0x2B<<4)|2: _FETCH();break;
        case (0x2B<<4)|3: assert(false);break;
        case (0x2B<<4)|4: assert(false);break;
        case (0x2B<<4)|5: assert(false);break;
        case (0x2B<<4)|6: assert(false);break;
        case (0x2B<<4)|7: assert(false);break;
        case (0x2B<<4)|8: assert(false);break;
        case (0x2B<<4)|9: assert(false);break;
        case (0x2B<<4)|10: assert(false);break;
        case (0x2B<<4)|11: assert(false);break;
        case (0x2B<<4)|12: assert(false);break;
        case (0x2B<<4)|13: assert(false);break;
        case (0x2B<<4)|14: assert(false);break;
        case (0x2B<<4)|15: assert(false);break;
    /* NOP # */
        case (0x2C<<4)|0: _SA(c->PC++);break;
        case (0x2C<<4)|1: _VMA();break;
        case (0x2C<<4)|2: _FETCH();break;
        case (0x2C<<4)|3: assert(false);break;
        case (0x2C<<4)|4: assert(false);break;
        case (0x2C<<4)|5: assert(false);break;
        case (0x2C<<4)|6: assert(false);break;
        case (0x2C<<4)|7: assert(false);break;
        case (0x2C<<4)|8: assert(false);break;
        case (0x2C<<4)|9: assert(false);break;
        case (0x2C<<4)|10: assert(false);break;
        case (0x2C<<4)|11: assert(false);break;
        case (0x2C<<4)|12: assert(false);break;
        case (0x2C<<4)|13: assert(false);break;
        case (0x2C<<4)|14: assert(false);break;
        case (0x2C<<4)|15: assert(false);break;
    /* NOP # */
        case (0x2D<<4)|0: _SA(c->PC++);break;
        case (0x2D<<4)|1: _VMA();break;
        case (0x2D<<4)|2: _FETCH();break;
        case (0x2D<<4)|3: assert(false);break;
        case (0x2D<<4)|4: assert(false);break;
        case (0x2D<<4)|5: assert(false);break;
        case (0x2D<<4)|6: assert(false);break;
        case (0x2D<<4)|7: assert(false);break;
        case (0x2D<<4)|8: assert(false);break;
        case (0x2D<<4)|9: assert(false);break;
        case (0x2D<<4)|10: assert(false);break;
        case (0x2D<<4)|11: assert(false);break;
        case (0x2D<<4)|12: assert(false);break;
        case (0x2D<<4)|13: assert(false);break;
        case (0x2D<<4)|14: assert(false);break;
        case (0x2D<<4)|15: assert(false);break;
    /* NOP # */
        case (0x2E<<4)|0: _SA(c->PC++);break;
        case (0x2E<<4)|1: _VMA();break;
        case (0x2E<<4)|2: _FETCH();break;
        case (0x2E<<4)|3: assert(false);break;
        case (0x2E<<4)|4: assert(false);break;
        case (0x2E<<4)|5: assert(false);break;
        case (0x2E<<4)|6: assert(false);break;
        case (0x2E<<4)|7: assert(false);break;
        case (0x2E<<4)|8: assert(false);break;
        case (0x2E<<4)|9: assert(false);break;
        case (0x2E<<4)|10: assert(false);break;
        case (0x2E<<4)|11: assert(false);break;
        case (0x2E<<4)|12: assert(false);break;
        case (0x2E<<4)|13: assert(false);break;
        case (0x2E<<4)|14: assert(false);break;
        case (0x2E<<4)|15: assert(false);break;
    /* NOP # */
        case (0x2F<<4)|0: _SA(c->PC++);break;
        case (0x2F<<4)|1: _VMA();break;
        case (0x2F<<4)|2: _FETCH();break;
        case (0x2F<<4)|3: assert(false);break;
        case (0x2F<<4)|4: assert(false);break;
        case (0x2F<<4)|5: assert(false);break;
        case (0x2F<<4)|6: assert(false);break;
        case (0x2F<<4)|7: assert(false);break;
        case (0x2F<<4)|8: assert(false);break;
        case (0x2F<<4)|9: assert(false);break;
        case (0x2F<<4)|10: assert(false);break;
        case (0x2F<<4)|11: assert(false);break;
        case (0x2F<<4)|12: assert(false);break;
        case (0x2F<<4)|13: assert(false);break;
        case (0x2F<<4)|14: assert(false);break;
        case (0x2F<<4)|15: assert(false);break;
    /* NOP  */
        case (0x30<<4)|0: _VMA();break;
        case (0x30<<4)|1: _FETCH();break;
        case (0x30<<4)|2: assert(false);break;
        case (0x30<<4)|3: assert(false);break;
        case (0x30<<4)|4: assert(false);break;
        case (0x30<<4)|5: assert(false);break;
        case (0x30<<4)|6: assert(false);break;
        case (0x30<<4)|7: assert(false);break;
        case (0x30<<4)|8: assert(false);break;
        case (0x30<<4)|9: assert(false);break;
        case (0x30<<4)|10: assert(false);break;
        case (0x30<<4)|11: assert(false);break;
        case (0x30<<4)|12: assert(false);break;
        case (0x30<<4)|13: assert(false);break;
        case (0x30<<4)|14: assert(false);break;
        case (0x30<<4)|15: assert(false);break;
    /* NOP  */
        case (0x31<<4)|0: _VMA();break;
        case (0x31<<4)|1: _FETCH();break;
        case (0x31<<4)|2: assert(false);break;
        case (0x31<<4)|3: assert(false);break;
        case (0x31<<4)|4: assert(false);break;
        case (0x31<<4)|5: assert(false);break;
        case (0x31<<4)|6: assert(false);break;
        case (0x31<<4)|7: assert(false);break;
        case (0x31<<4)|8: assert(false);break;
        case (0x31<<4)|9: assert(false);break;
        case (0x31<<4)|10: assert(false);break;
        case (0x31<<4)|11: assert(false);break;
        case (0x31<<4)|12: assert(false);break;
        case (0x31<<4)|13: assert(false);break;
        case (0x31<<4)|14: assert(false);break;
        case (0x31<<4)|15: assert(false);break;
    /* NOP  */
        case (0x32<<4)|0: _FETCH();break;
        case (0x32<<4)|1: assert(false);break;
        case (0x32<<4)|2: assert(false);break;
        case (0x32<<4)|3: assert(false);break;
        case (0x32<<4)|4: assert(false);break;
        case (0x32<<4)|5: assert(false);break;
        case (0x32<<4)|6: assert(false);break;
        case (0x32<<4)|7: assert(false);break;
        case (0x32<<4)|8: assert(false);break;
        case (0x32<<4)|9: assert(false);break;
        case (0x32<<4)|10: assert(false);break;
        case (0x32<<4)|11: assert(false);break;
        case (0x32<<4)|12: assert(false);break;
        case (0x32<<4)|13: assert(false);break;
        case (0x32<<4)|14: assert(false);break;
        case (0x32<<4)|15: assert(false);break;
    /* NOP  */
        case (0x33<<4)|0: _FETCH();break;
        case (0x33<<4)|1: assert(false);break;
        case (0x33<<4)|2: assert(false);break;
        case (0x33<<4)|3: assert(false);break;
        case (0x33<<4)|4: assert(false);break;
        case (0x33<<4)|5: assert(false);break;
        case (0x33<<4)|6: assert(false);break;
        case (0x33<<4)|7: assert(false);break;
        case (0x33<<4)|8: assert(false);break;
        case (0x33<<4)|9: assert(false);break;
        case (0x33<<4)|10: assert(false);break;
        case (0x33<<4)|11: assert(false);break;
        case (0x33<<4)|12: assert(false);break;
        case (0x33<<4)|13: assert(false);break;
        case (0x33<<4)|14: assert(false);break;
        case (0x33<<4)|15: assert(false);break;
    /* NOP  */
        case (0x34<<4)|0: _VMA();break;
        case (0x34<<4)|1: _FETCH();break;
        case (0x34<<4)|2: assert(false);break;
        case (0x34<<4)|3: assert(false);break;
        case (0x34<<4)|4: assert(false);break;
        case (0x34<<4)|5: assert(false);break;
        case (0x34<<4)|6: assert(false);break;
        case (0x34<<4)|7: assert(false);break;
        case (0x34<<4)|8: assert(false);break;
        case (0x34<<4)|9: assert(false);break;
        case (0x34<<4)|10: assert(false);break;
        case (0x34<<4)|11: assert(false);break;
        case (0x34<<4)|12: assert(false);break;
        case (0x34<<4)|13: assert(false);break;
        case (0x34<<4)|14: assert(false);break;
        case (0x34<<4)|15: assert(false);break;
    /* NOP  */
        case (0x35<<4)|0: _VMA();break;
        case (0x35<<4)|1: _FETCH();break;
        case (0x35<<4)|2: assert(false);break;
        case (0x35<<4)|3: assert(false);break;
        case (0x35<<4)|4: assert(false);break;
        case (0x35<<4)|5: assert(false);break;
        case (0x35<<4)|6: assert(false);break;
        case (0x35<<4)|7: assert(false);break;
        case (0x35<<4)|8: assert(false);break;
        case (0x35<<4)|9: assert(false);break;
        case (0x35<<4)|10: assert(false);break;
        case (0x35<<4)|11: assert(false);break;
        case (0x35<<4)|12: assert(false);break;
        case (0x35<<4)|13: assert(false);break;
        case (0x35<<4)|14: assert(false);break;
        case (0x35<<4)|15: assert(false);break;
    /* NOP  */
        case (0x36<<4)|0: break;
        case (0x36<<4)|1: _FETCH();break;
        case (0x36<<4)|2: assert(false);break;
        case (0x36<<4)|3: assert(false);break;
        case (0x36<<4)|4: assert(false);break;
        case (0x36<<4)|5: assert(false);break;
        case (0x36<<4)|6: assert(false);break;
        case (0x36<<4)|7: assert(false);break;
        case (0x36<<4)|8: assert(false);break;
        case (0x36<<4)|9: assert(false);break;
        case (0x36<<4)|10: assert(false);break;
        case (0x36<<4)|11: assert(false);break;
        case (0x36<<4)|12: assert(false);break;
        case (0x36<<4)|13: assert(false);break;
        case (0x36<<4)|14: assert(false);break;
        case (0x36<<4)|15: assert(false);break;
    /* NOP  */
        case (0x37<<4)|0: break;
        case (0x37<<4)|1: _FETCH();break;
        case (0x37<<4)|2: assert(false);break;
        case (0x37<<4)|3: assert(false);break;
        case (0x37<<4)|4: assert(false);break;
        case (0x37<<4)|5: assert(false);break;
        case (0x37<<4)|6: assert(false);break;
        case (0x37<<4)|7: assert(false);break;
        case (0x37<<4)|8: assert(false);break;
        case (0x37<<4)|9: assert(false);break;
        case (0x37<<4)|10: assert(false);break;
        case (0x37<<4)|11: assert(false);break;
        case (0x37<<4)|12: assert(false);break;
        case (0x37<<4)|13: assert(false);break;
        case (0x37<<4)|14: assert(false);break;
        case (0x37<<4)|15: assert(false);break;
    /* NOP  */
        case (0x38<<4)|0: _VMA();break;
        case (0x38<<4)|1: _FETCH();break;
        case (0x38<<4)|2: assert(false);break;
        case (0x38<<4)|3: assert(false);break;
        case (0x38<<4)|4: assert(false);break;
        case (0x38<<4)|5: assert(false);break;
        case (0x38<<4)|6: assert(false);break;
        case (0x38<<4)|7: assert(false);break;
        case (0x38<<4)|8: assert(false);break;
        case (0x38<<4)|9: assert(false);break;
        case (0x38<<4)|10: assert(false);break;
        case (0x38<<4)|11: assert(false);break;
        case (0x38<<4)|12: assert(false);break;
        case (0x38<<4)|13: assert(false);break;
        case (0x38<<4)|14: assert(false);break;
        case (0x38<<4)|15: assert(false);break;
    /* NOP  */
        case (0x39<<4)|0: _VMA();break;
        case (0x39<<4)|1: _FETCH();break;
        case (0x39<<4)|2: assert(false);break;
        case (0x39<<4)|3: assert(false);break;
        case (0x39<<4)|4: assert(false);break;
        case (0x39<<4)|5: assert(false);break;
        case (0x39<<4)|6: assert(false);break;
        case (0x39<<4)|7: assert(false);break;
        case (0x39<<4)|8: assert(false);break;
        case (0x39<<4)|9: assert(false);break;
        case (0x39<<4)|10: assert(false);break;
        case (0x39<<4)|11: assert(false);break;
        case (0x39<<4)|12: assert(false);break;
        case (0x39<<4)|13: assert(false);break;
        case (0x39<<4)|14: assert(false);break;
        case (0x39<<4)|15: assert(false);break;
    /* NOP  */
        case (0x3A<<4)|0: _VMA();break;
        case (0x3A<<4)|1: _FETCH();break;
        case (0x3A<<4)|2: assert(false);break;
        case (0x3A<<4)|3: assert(false);break;
        case (0x3A<<4)|4: assert(false);break;
        case (0x3A<<4)|5: assert(false);break;
        case (0x3A<<4)|6: assert(false);break;
        case (0x3A<<4)|7: assert(false);break;
        case (0x3A<<4)|8: assert(false);break;
        case (0x3A<<4)|9: assert(false);break;
        case (0x3A<<4)|10: assert(false);break;
        case (0x3A<<4)|11: assert(false);break;
        case (0x3A<<4)|12: assert(false);break;
        case (0x3A<<4)|13: assert(false);break;
        case (0x3A<<4)|14: assert(false);break;
        case (0x3A<<4)|15: assert(false);break;
    /* NOP  */
        case (0x3B<<4)|0: _VMA();break;
        case (0x3B<<4)|1: _FETCH();break;
        case (0x3B<<4)|2: assert(false);break;
        case (0x3B<<4)|3: assert(false);break;
        case (0x3B<<4)|4: assert(false);break;
        case (0x3B<<4)|5: assert(false);break;
        case (0x3B<<4)|6: assert(false);break;
        case (0x3B<<4)|7: assert(false);break;
        case (0x3B<<4)|8: assert(false);break;
        case (0x3B<<4)|9: assert(false);break;
        case (0x3B<<4)|10: assert(false);break;
        case (0x3B<<4)|11: assert(false);break;
        case (0x3B<<4)|12: assert(false);break;
        case (0x3B<<4)|13: assert(false);break;
        case (0x3B<<4)|14: assert(false);break;
        case (0x3B<<4)|15: assert(false);break;
    /* NOP  */
        case (0x3C<<4)|0: _VMA();break;
        case (0x3C<<4)|1: _FETCH();break;
        case (0x3C<<4)|2: assert(false);break;
        case (0x3C<<4)|3: assert(false);break;
        case (0x3C<<4)|4: assert(false);break;
        case (0x3C<<4)|5: assert(false);break;
        case (0x3C<<4)|6: assert(false);break;
        case (0x3C<<4)|7: assert(false);break;
        case (0x3C<<4)|8: assert(false);break;
        case (0x3C<<4)|9: assert(false);break;
        case (0x3C<<4)|10: assert(false);break;
        case (0x3C<<4)|11: assert(false);break;
        case (0x3C<<4)|12: assert(false);break;
        case (0x3C<<4)|13: assert(false);break;
        case (0x3C<<4)|14: assert(false);break;
        case (0x3C<<4)|15: assert(false);break;
    /* NOP  */
        case (0x3D<<4)|0: _VMA();break;
        case (0x3D<<4)|1: _FETCH();break;
        case (0x3D<<4)|2: assert(false);break;
        case (0x3D<<4)|3: assert(false);break;
        case (0x3D<<4)|4: assert(false);break;
        case (0x3D<<4)|5: assert(false);break;
        case (0x3D<<4)|6: assert(false);break;
        case (0x3D<<4)|7: assert(false);break;
        case (0x3D<<4)|8: assert(false);break;
        case (0x3D<<4)|9: assert(false);break;
        case (0x3D<<4)|10: assert(false);break;
        case (0x3D<<4)|11: assert(false);break;
        case (0x3D<<4)|12: assert(false);break;
        case (0x3D<<4)|13: assert(false);break;
        case (0x3D<<4)|14: assert(false);break;
        case (0x3D<<4)|15: assert(false);break;
    /* NOP  */
        case (0x3E<<4)|0: _VMA();break;
        case (0x3E<<4)|1: _FETCH();break;
        case (0x3E<<4)|2: assert(false);break;
        case (0x3E<<4)|3: assert(false);break;
        case (0x3E<<4)|4: assert(false);break;
        case (0x3E<<4)|5: assert(false);break;
        case (0x3E<<4)|6: assert(false);break;
        case (0x3E<<4)|7: assert(false);break;
        case (0x3E<<4)|8: assert(false);break;
        case (0x3E<<4)|9: assert(false);break;
        case (0x3E<<4)|10: assert(false);break;
        case (0x3E<<4)|11: assert(false);break;
        case (0x3E<<4)|12: assert(false);break;
        case (0x3E<<4)|13: assert(false);break;
        case (0x3E<<4)|14: assert(false);break;
        case (0x3E<<4)|15: assert(false);break;
    /* NOP  */
        case (0x3F<<4)|0: _VMA();break;
        case (0x3F<<4)|1: _FETCH();break;
        case (0x3F<<4)|2: assert(false);break;
        case (0x3F<<4)|3: assert(false);break;
        case (0x3F<<4)|4: assert(false);break;
        case (0x3F<<4)|5: assert(false);break;
        case (0x3F<<4)|6: assert(false);break;
        case (0x3F<<4)|7: assert(false);break;
        case (0x3F<<4)|8: assert(false);break;
        case (0x3F<<4)|9: assert(false);break;
        case (0x3F<<4)|10: assert(false);break;
        case (0x3F<<4)|11: assert(false);break;
        case (0x3F<<4)|12: assert(false);break;
        case (0x3F<<4)|13: assert(false);break;
        case (0x3F<<4)|14: assert(false);break;
        case (0x3F<<4)|15: assert(false);break;
    /* NOP  */
        case (0x40<<4)|0: _VMA();break;
        case (0x40<<4)|1: _FETCH();break;
        case (0x40<<4)|2: assert(false);break;
        case (0x40<<4)|3: assert(false);break;
        case (0x40<<4)|4: assert(false);break;
        case (0x40<<4)|5: assert(false);break;
        case (0x40<<4)|6: assert(false);break;
        case (0x40<<4)|7: assert(false);break;
        case (0x40<<4)|8: assert(false);break;
        case (0x40<<4)|9: assert(false);break;
        case (0x40<<4)|10: assert(false);break;
        case (0x40<<4)|11: assert(false);break;
        case (0x40<<4)|12: assert(false);break;
        case (0x40<<4)|13: assert(false);break;
        case (0x40<<4)|14: assert(false);break;
        case (0x40<<4)|15: assert(false);break;
    /* NOP  */
        case (0x41<<4)|0: _VMA();break;
        case (0x41<<4)|1: _FETCH();break;
        case (0x41<<4)|2: assert(false);break;
        case (0x41<<4)|3: assert(false);break;
        case (0x41<<4)|4: assert(false);break;
        case (0x41<<4)|5: assert(false);break;
        case (0x41<<4)|6: assert(false);break;
        case (0x41<<4)|7: assert(false);break;
        case (0x41<<4)|8: assert(false);break;
        case (0x41<<4)|9: assert(false);break;
        case (0x41<<4)|10: assert(false);break;
        case (0x41<<4)|11: assert(false);break;
        case (0x41<<4)|12: assert(false);break;
        case (0x41<<4)|13: assert(false);break;
        case (0x41<<4)|14: assert(false);break;
        case (0x41<<4)|15: assert(false);break;
    /* NOP  */
        case (0x42<<4)|0: _VMA();break;
        case (0x42<<4)|1: _FETCH();break;
        case (0x42<<4)|2: assert(false);break;
        case (0x42<<4)|3: assert(false);break;
        case (0x42<<4)|4: assert(false);break;
        case (0x42<<4)|5: assert(false);break;
        case (0x42<<4)|6: assert(false);break;
        case (0x42<<4)|7: assert(false);break;
        case (0x42<<4)|8: assert(false);break;
        case (0x42<<4)|9: assert(false);break;
        case (0x42<<4)|10: assert(false);break;
        case (0x42<<4)|11: assert(false);break;
        case (0x42<<4)|12: assert(false);break;
        case (0x42<<4)|13: assert(false);break;
        case (0x42<<4)|14: assert(false);break;
        case (0x42<<4)|15: assert(false);break;
    /* NOP  */
        case (0x43<<4)|0: _VMA();break;
        case (0x43<<4)|1: _FETCH();break;
        case (0x43<<4)|2: assert(false);break;
        case (0x43<<4)|3: assert(false);break;
        case (0x43<<4)|4: assert(false);break;
        case (0x43<<4)|5: assert(false);break;
        case (0x43<<4)|6: assert(false);break;
        case (0x43<<4)|7: assert(false);break;
        case (0x43<<4)|8: assert(false);break;
        case (0x43<<4)|9: assert(false);break;
        case (0x43<<4)|10: assert(false);break;
        case (0x43<<4)|11: assert(false);break;
        case (0x43<<4)|12: assert(false);break;
        case (0x43<<4)|13: assert(false);break;
        case (0x43<<4)|14: assert(false);break;
        case (0x43<<4)|15: assert(false);break;
    /* LSRA  */
        case (0x44<<4)|0: c->A=_mc6800_lsr(c, c->A);_VMA();break;
        case (0x44<<4)|1: _FETCH();break;
        case (0x44<<4)|2: assert(false);break;
        case (0x44<<4)|3: assert(false);break;
        case (0x44<<4)|4: assert(false);break;
        case (0x44<<4)|5: assert(false);break;
        case (0x44<<4)|6: assert(false);break;
        case (0x44<<4)|7: assert(false);break;
        case (0x44<<4)|8: assert(false);break;
        case (0x44<<4)|9: assert(false);break;
        case (0x44<<4)|10: assert(false);break;
        case (0x44<<4)|11: assert(false);break;
        case (0x44<<4)|12: assert(false);break;
        case (0x44<<4)|13: assert(false);break;
        case (0x44<<4)|14: assert(false);break;
        case (0x44<<4)|15: assert(false);break;
    /* NOP  */
        case (0x45<<4)|0: _VMA();break;
        case (0x45<<4)|1: _FETCH();break;
        case (0x45<<4)|2: assert(false);break;
        case (0x45<<4)|3: assert(false);break;
        case (0x45<<4)|4: assert(false);break;
        case (0x45<<4)|5: assert(false);break;
        case (0x45<<4)|6: assert(false);break;
        case (0x45<<4)|7: assert(false);break;
        case (0x45<<4)|8: assert(false);break;
        case (0x45<<4)|9: assert(false);break;
        case (0x45<<4)|10: assert(false);break;
        case (0x45<<4)|11: assert(false);break;
        case (0x45<<4)|12: assert(false);break;
        case (0x45<<4)|13: assert(false);break;
        case (0x45<<4)|14: assert(false);break;
        case (0x45<<4)|15: assert(false);break;
    /* RORA  */
        case (0x46<<4)|0: c->A=_mc6800_ror(c, c->A);_VMA();break;
        case (0x46<<4)|1: _FETCH();break;
        case (0x46<<4)|2: assert(false);break;
        case (0x46<<4)|3: assert(false);break;
        case (0x46<<4)|4: assert(false);break;
        case (0x46<<4)|5: assert(false);break;
        case (0x46<<4)|6: assert(false);break;
        case (0x46<<4)|7: assert(false);break;
        case (0x46<<4)|8: assert(false);break;
        case (0x46<<4)|9: assert(false);break;
        case (0x46<<4)|10: assert(false);break;
        case (0x46<<4)|11: assert(false);break;
        case (0x46<<4)|12: assert(false);break;
        case (0x46<<4)|13: assert(false);break;
        case (0x46<<4)|14: assert(false);break;
        case (0x46<<4)|15: assert(false);break;
    /* ASRA  */
        case (0x47<<4)|0: c->A=(_GD()&0x80)|_mc6800_lsr(c, c->A);_NZ(c->A);_VMA();break;
        case (0x47<<4)|1: _FETCH();break;
        case (0x47<<4)|2: assert(false);break;
        case (0x47<<4)|3: assert(false);break;
        case (0x47<<4)|4: assert(false);break;
        case (0x47<<4)|5: assert(false);break;
        case (0x47<<4)|6: assert(false);break;
        case (0x47<<4)|7: assert(false);break;
        case (0x47<<4)|8: assert(false);break;
        case (0x47<<4)|9: assert(false);break;
        case (0x47<<4)|10: assert(false);break;
        case (0x47<<4)|11: assert(false);break;
        case (0x47<<4)|12: assert(false);break;
        case (0x47<<4)|13: assert(false);break;
        case (0x47<<4)|14: assert(false);break;
        case (0x47<<4)|15: assert(false);break;
    /* ASLA  */
        case (0x48<<4)|0: c->A=_mc6800_asl(c, c->A);_VMA();break;
        case (0x48<<4)|1: _FETCH();break;
        case (0x48<<4)|2: assert(false);break;
        case (0x48<<4)|3: assert(false);break;
        case (0x48<<4)|4: assert(false);break;
        case (0x48<<4)|5: assert(false);break;
        case (0x48<<4)|6: assert(false);break;
        case (0x48<<4)|7: assert(false);break;
        case (0x48<<4)|8: assert(false);break;
        case (0x48<<4)|9: assert(false);break;
        case (0x48<<4)|10: assert(false);break;
        case (0x48<<4)|11: assert(false);break;
        case (0x48<<4)|12: assert(false);break;
        case (0x48<<4)|13: assert(false);break;
        case (0x48<<4)|14: assert(false);break;
        case (0x48<<4)|15: assert(false);break;
    /* ROLA  */
        case (0x49<<4)|0: c->A=_mc6800_rol(c, c->A);_VMA();break;
        case (0x49<<4)|1: _FETCH();break;
        case (0x49<<4)|2: assert(false);break;
        case (0x49<<4)|3: assert(false);break;
        case (0x49<<4)|4: assert(false);break;
        case (0x49<<4)|5: assert(false);break;
        case (0x49<<4)|6: assert(false);break;
        case (0x49<<4)|7: assert(false);break;
        case (0x49<<4)|8: assert(false);break;
        case (0x49<<4)|9: assert(false);break;
        case (0x49<<4)|10: assert(false);break;
        case (0x49<<4)|11: assert(false);break;
        case (0x49<<4)|12: assert(false);break;
        case (0x49<<4)|13: assert(false);break;
        case (0x49<<4)|14: assert(false);break;
        case (0x49<<4)|15: assert(false);break;
    /* DECA  */
        case (0x4A<<4)|0: _VF(c->A==0x80);c->A--;_NZ(c->A);_VMA();break;
        case (0x4A<<4)|1: _FETCH();break;
        case (0x4A<<4)|2: assert(false);break;
        case (0x4A<<4)|3: assert(false);break;
        case (0x4A<<4)|4: assert(false);break;
        case (0x4A<<4)|5: assert(false);break;
        case (0x4A<<4)|6: assert(false);break;
        case (0x4A<<4)|7: assert(false);break;
        case (0x4A<<4)|8: assert(false);break;
        case (0x4A<<4)|9: assert(false);break;
        case (0x4A<<4)|10: assert(false);break;
        case (0x4A<<4)|11: assert(false);break;
        case (0x4A<<4)|12: assert(false);break;
        case (0x4A<<4)|13: assert(false);break;
        case (0x4A<<4)|14: assert(false);break;
        case (0x4A<<4)|15: assert(false);break;
    /* NOP  */
        case (0x4B<<4)|0: _VMA();break;
        case (0x4B<<4)|1: _FETCH();break;
        case (0x4B<<4)|2: assert(false);break;
        case (0x4B<<4)|3: assert(false);break;
        case (0x4B<<4)|4: assert(false);break;
        case (0x4B<<4)|5: assert(false);break;
        case (0x4B<<4)|6: assert(false);break;
        case (0x4B<<4)|7: assert(false);break;
        case (0x4B<<4)|8: assert(false);break;
        case (0x4B<<4)|9: assert(false);break;
        case (0x4B<<4)|10: assert(false);break;
        case (0x4B<<4)|11: assert(false);break;
        case (0x4B<<4)|12: assert(false);break;
        case (0x4B<<4)|13: assert(false);break;
        case (0x4B<<4)|14: assert(false);break;
        case (0x4B<<4)|15: assert(false);break;
    /* INCA  */
        case (0x4C<<4)|0: _VF(c->A==0x7F);c->A++;_NZ(c->A);_VMA();break;
        case (0x4C<<4)|1: _FETCH();break;
        case (0x4C<<4)|2: assert(false);break;
        case (0x4C<<4)|3: assert(false);break;
        case (0x4C<<4)|4: assert(false);break;
        case (0x4C<<4)|5: assert(false);break;
        case (0x4C<<4)|6: assert(false);break;
        case (0x4C<<4)|7: assert(false);break;
        case (0x4C<<4)|8: assert(false);break;
        case (0x4C<<4)|9: assert(false);break;
        case (0x4C<<4)|10: assert(false);break;
        case (0x4C<<4)|11: assert(false);break;
        case (0x4C<<4)|12: assert(false);break;
        case (0x4C<<4)|13: assert(false);break;
        case (0x4C<<4)|14: assert(false);break;
        case (0x4C<<4)|15: assert(false);break;
    /* TSTA  */
        case (0x4D<<4)|0: _mc6800_sub(c, 0, true, false);_VMA();break;
        case (0x4D<<4)|1: _FETCH();break;
        case (0x4D<<4)|2: assert(false);break;
        case (0x4D<<4)|3: assert(false);break;
        case (0x4D<<4)|4: assert(false);break;
        case (0x4D<<4)|5: assert(false);break;
        case (0x4D<<4)|6: assert(false);break;
        case (0x4D<<4)|7: assert(false);break;
        case (0x4D<<4)|8: assert(false);break;
        case (0x4D<<4)|9: assert(false);break;
        case (0x4D<<4)|10: assert(false);break;
        case (0x4D<<4)|11: assert(false);break;
        case (0x4D<<4)|12: assert(false);break;
        case (0x4D<<4)|13: assert(false);break;
        case (0x4D<<4)|14: assert(false);break;
        case (0x4D<<4)|15: assert(false);break;
    /* NOP  */
        case (0x4E<<4)|0: _VMA();break;
        case (0x4E<<4)|1: _FETCH();break;
        case (0x4E<<4)|2: assert(false);break;
        case (0x4E<<4)|3: assert(false);break;
        case (0x4E<<4)|4: assert(false);break;
        case (0x4E<<4)|5: assert(false);break;
        case (0x4E<<4)|6: assert(false);break;
        case (0x4E<<4)|7: assert(false);break;
        case (0x4E<<4)|8: assert(false);break;
        case (0x4E<<4)|9: assert(false);break;
        case (0x4E<<4)|10: assert(false);break;
        case (0x4E<<4)|11: assert(false);break;
        case (0x4E<<4)|12: assert(false);break;
        case (0x4E<<4)|13: assert(false);break;
        case (0x4E<<4)|14: assert(false);break;
        case (0x4E<<4)|15: assert(false);break;
    /* CLRA  */
        case (0x4F<<4)|0: _VF(false);_CF(false);c->A=0;_NZ(0);_VMA();break;
        case (0x4F<<4)|1: _FETCH();break;
        case (0x4F<<4)|2: assert(false);break;
        case (0x4F<<4)|3: assert(false);break;
        case (0x4F<<4)|4: assert(false);break;
        case (0x4F<<4)|5: assert(false);break;
        case (0x4F<<4)|6: assert(false);break;
        case (0x4F<<4)|7: assert(false);break;
        case (0x4F<<4)|8: assert(false);break;
        case (0x4F<<4)|9: assert(false);break;
        case (0x4F<<4)|10: assert(false);break;
        case (0x4F<<4)|11: assert(false);break;
        case (0x4F<<4)|12: assert(false);break;
        case (0x4F<<4)|13: assert(false);break;
        case (0x4F<<4)|14: assert(false);break;
        case (0x4F<<4)|15: assert(false);break;
    /* NOP  */
        case (0x50<<4)|0: _VMA();break;
        case (0x50<<4)|1: _FETCH();break;
        case (0x50<<4)|2: assert(false);break;
        case (0x50<<4)|3: assert(false);break;
        case (0x50<<4)|4: assert(false);break;
        case (0x50<<4)|5: assert(false);break;
        case (0x50<<4)|6: assert(false);break;
        case (0x50<<4)|7: assert(false);break;
        case (0x50<<4)|8: assert(false);break;
        case (0x50<<4)|9: assert(false);break;
        case (0x50<<4)|10: assert(false);break;
        case (0x50<<4)|11: assert(false);break;
        case (0x50<<4)|12: assert(false);break;
        case (0x50<<4)|13: assert(false);break;
        case (0x50<<4)|14: assert(false);break;
        case (0x50<<4)|15: assert(false);break;
    /* NOP  */
        case (0x51<<4)|0: _VMA();break;
        case (0x51<<4)|1: _FETCH();break;
        case (0x51<<4)|2: assert(false);break;
        case (0x51<<4)|3: assert(false);break;
        case (0x51<<4)|4: assert(false);break;
        case (0x51<<4)|5: assert(false);break;
        case (0x51<<4)|6: assert(false);break;
        case (0x51<<4)|7: assert(false);break;
        case (0x51<<4)|8: assert(false);break;
        case (0x51<<4)|9: assert(false);break;
        case (0x51<<4)|10: assert(false);break;
        case (0x51<<4)|11: assert(false);break;
        case (0x51<<4)|12: assert(false);break;
        case (0x51<<4)|13: assert(false);break;
        case (0x51<<4)|14: assert(false);break;
        case (0x51<<4)|15: assert(false);break;
    /* NOP  */
        case (0x52<<4)|0: _VMA();break;
        case (0x52<<4)|1: _FETCH();break;
        case (0x52<<4)|2: assert(false);break;
        case (0x52<<4)|3: assert(false);break;
        case (0x52<<4)|4: assert(false);break;
        case (0x52<<4)|5: assert(false);break;
        case (0x52<<4)|6: assert(false);break;
        case (0x52<<4)|7: assert(false);break;
        case (0x52<<4)|8: assert(false);break;
        case (0x52<<4)|9: assert(false);break;
        case (0x52<<4)|10: assert(false);break;
        case (0x52<<4)|11: assert(false);break;
        case (0x52<<4)|12: assert(false);break;
        case (0x52<<4)|13: assert(false);break;
        case (0x52<<4)|14: assert(false);break;
        case (0x52<<4)|15: assert(false);break;
    /* NOP  */
        case (0x53<<4)|0: _VMA();break;
        case (0x53<<4)|1: _FETCH();break;
        case (0x53<<4)|2: assert(false);break;
        case (0x53<<4)|3: assert(false);break;
        case (0x53<<4)|4: assert(false);break;
        case (0x53<<4)|5: assert(false);break;
        case (0x53<<4)|6: assert(false);break;
        case (0x53<<4)|7: assert(false);break;
        case (0x53<<4)|8: assert(false);break;
        case (0x53<<4)|9: assert(false);break;
        case (0x53<<4)|10: assert(false);break;
        case (0x53<<4)|11: assert(false);break;
        case (0x53<<4)|12: assert(false);break;
        case (0x53<<4)|13: assert(false);break;
        case (0x53<<4)|14: assert(false);break;
        case (0x53<<4)|15: assert(false);break;
    /* LSRB  */
        case (0x54<<4)|0: c->B=_mc6800_lsr(c, c->B);_VMA();break;
        case (0x54<<4)|1: _FETCH();break;
        case (0x54<<4)|2: assert(false);break;
        case (0x54<<4)|3: assert(false);break;
        case (0x54<<4)|4: assert(false);break;
        case (0x54<<4)|5: assert(false);break;
        case (0x54<<4)|6: assert(false);break;
        case (0x54<<4)|7: assert(false);break;
        case (0x54<<4)|8: assert(false);break;
        case (0x54<<4)|9: assert(false);break;
        case (0x54<<4)|10: assert(false);break;
        case (0x54<<4)|11: assert(false);break;
        case (0x54<<4)|12: assert(false);break;
        case (0x54<<4)|13: assert(false);break;
        case (0x54<<4)|14: assert(false);break;
        case (0x54<<4)|15: assert(false);break;
    /* NOP  */
        case (0x55<<4)|0: _VMA();break;
        case (0x55<<4)|1: _FETCH();break;
        case (0x55<<4)|2: assert(false);break;
        case (0x55<<4)|3: assert(false);break;
        case (0x55<<4)|4: assert(false);break;
        case (0x55<<4)|5: assert(false);break;
        case (0x55<<4)|6: assert(false);break;
        case (0x55<<4)|7: assert(false);break;
        case (0x55<<4)|8: assert(false);break;
        case (0x55<<4)|9: assert(false);break;
        case (0x55<<4)|10: assert(false);break;
        case (0x55<<4)|11: assert(false);break;
        case (0x55<<4)|12: assert(false);break;
        case (0x55<<4)|13: assert(false);break;
        case (0x55<<4)|14: assert(false);break;
        case (0x55<<4)|15: assert(false);break;
    /* RORB  */
        case (0x56<<4)|0: c->B=_mc6800_ror(c, c->B);_VMA();break;
        case (0x56<<4)|1: _FETCH();break;
        case (0x56<<4)|2: assert(false);break;
        case (0x56<<4)|3: assert(false);break;
        case (0x56<<4)|4: assert(false);break;
        case (0x56<<4)|5: assert(false);break;
        case (0x56<<4)|6: assert(false);break;
        case (0x56<<4)|7: assert(false);break;
        case (0x56<<4)|8: assert(false);break;
        case (0x56<<4)|9: assert(false);break;
        case (0x56<<4)|10: assert(false);break;
        case (0x56<<4)|11: assert(false);break;
        case (0x56<<4)|12: assert(false);break;
        case (0x56<<4)|13: assert(false);break;
        case (0x56<<4)|14: assert(false);break;
        case (0x56<<4)|15: assert(false);break;
    /* ASRB  */
        case (0x57<<4)|0: c->B=(_GD()&0x80)|_mc6800_lsr(c, c->B);_NZ(c->B);_VMA();break;
        case (0x57<<4)|1: _FETCH();break;
        case (0x57<<4)|2: assert(false);break;
        case (0x57<<4)|3: assert(false);break;
        case (0x57<<4)|4: assert(false);break;
        case (0x57<<4)|5: assert(false);break;
        case (0x57<<4)|6: assert(false);break;
        case (0x57<<4)|7: assert(false);break;
        case (0x57<<4)|8: assert(false);break;
        case (0x57<<4)|9: assert(false);break;
        case (0x57<<4)|10: assert(false);break;
        case (0x57<<4)|11: assert(false);break;
        case (0x57<<4)|12: assert(false);break;
        case (0x57<<4)|13: assert(false);break;
        case (0x57<<4)|14: assert(false);break;
        case (0x57<<4)|15: assert(false);break;
    /* ASLB  */
        case (0x58<<4)|0: c->B=_mc6800_asl(c, c->B);_VMA();break;
        case (0x58<<4)|1: _FETCH();break;
        case (0x58<<4)|2: assert(false);break;
        case (0x58<<4)|3: assert(false);break;
        case (0x58<<4)|4: assert(false);break;
        case (0x58<<4)|5: assert(false);break;
        case (0x58<<4)|6: assert(false);break;
        case (0x58<<4)|7: assert(false);break;
        case (0x58<<4)|8: assert(false);break;
        case (0x58<<4)|9: assert(false);break;
        case (0x58<<4)|10: assert(false);break;
        case (0x58<<4)|11: assert(false);break;
        case (0x58<<4)|12: assert(false);break;
        case (0x58<<4)|13: assert(false);break;
        case (0x58<<4)|14: assert(false);break;
        case (0x58<<4)|15: assert(false);break;
    /* ROLB  */
        case (0x59<<4)|0: c->B=_mc6800_rol(c, c->B);_VMA();break;
        case (0x59<<4)|1: _FETCH();break;
        case (0x59<<4)|2: assert(false);break;
        case (0x59<<4)|3: assert(false);break;
        case (0x59<<4)|4: assert(false);break;
        case (0x59<<4)|5: assert(false);break;
        case (0x59<<4)|6: assert(false);break;
        case (0x59<<4)|7: assert(false);break;
        case (0x59<<4)|8: assert(false);break;
        case (0x59<<4)|9: assert(false);break;
        case (0x59<<4)|10: assert(false);break;
        case (0x59<<4)|11: assert(false);break;
        case (0x59<<4)|12: assert(false);break;
        case (0x59<<4)|13: assert(false);break;
        case (0x59<<4)|14: assert(false);break;
        case (0x59<<4)|15: assert(false);break;
    /* DECB  */
        case (0x5A<<4)|0: _VF(c->B==0x80);c->B--;_NZ(c->B);_VMA();break;
        case (0x5A<<4)|1: _FETCH();break;
        case (0x5A<<4)|2: assert(false);break;
        case (0x5A<<4)|3: assert(false);break;
        case (0x5A<<4)|4: assert(false);break;
        case (0x5A<<4)|5: assert(false);break;
        case (0x5A<<4)|6: assert(false);break;
        case (0x5A<<4)|7: assert(false);break;
        case (0x5A<<4)|8: assert(false);break;
        case (0x5A<<4)|9: assert(false);break;
        case (0x5A<<4)|10: assert(false);break;
        case (0x5A<<4)|11: assert(false);break;
        case (0x5A<<4)|12: assert(false);break;
        case (0x5A<<4)|13: assert(false);break;
        case (0x5A<<4)|14: assert(false);break;
        case (0x5A<<4)|15: assert(false);break;
    /* NOP  */
        case (0x5B<<4)|0: _VMA();break;
        case (0x5B<<4)|1: _FETCH();break;
        case (0x5B<<4)|2: assert(false);break;
        case (0x5B<<4)|3: assert(false);break;
        case (0x5B<<4)|4: assert(false);break;
        case (0x5B<<4)|5: assert(false);break;
        case (0x5B<<4)|6: assert(false);break;
        case (0x5B<<4)|7: assert(false);break;
        case (0x5B<<4)|8: assert(false);break;
        case (0x5B<<4)|9: assert(false);break;
        case (0x5B<<4)|10: assert(false);break;
        case (0x5B<<4)|11: assert(false);break;
        case (0x5B<<4)|12: assert(false);break;
        case (0x5B<<4)|13: assert(false);break;
        case (0x5B<<4)|14: assert(false);break;
        case (0x5B<<4)|15: assert(false);break;
    /* INCB  */
        case (0x5C<<4)|0: _VF(c->B==0x7F);c->B++;_NZ(c->B);_VMA();break;
        case (0x5C<<4)|1: _FETCH();break;
        case (0x5C<<4)|2: assert(false);break;
        case (0x5C<<4)|3: assert(false);break;
        case (0x5C<<4)|4: assert(false);break;
        case (0x5C<<4)|5: assert(false);break;
        case (0x5C<<4)|6: assert(false);break;
        case (0x5C<<4)|7: assert(false);break;
        case (0x5C<<4)|8: assert(false);break;
        case (0x5C<<4)|9: assert(false);break;
        case (0x5C<<4)|10: assert(false);break;
        case (0x5C<<4)|11: assert(false);break;
        case (0x5C<<4)|12: assert(false);break;
        case (0x5C<<4)|13: assert(false);break;
        case (0x5C<<4)|14: assert(false);break;
        case (0x5C<<4)|15: assert(false);break;
    /* TSTB  */
        case (0x5D<<4)|0: _mc6800_sub(c, 0, false, false);_VMA();break;
        case (0x5D<<4)|1: _FETCH();break;
        case (0x5D<<4)|2: assert(false);break;
        case (0x5D<<4)|3: assert(false);break;
        case (0x5D<<4)|4: assert(false);break;
        case (0x5D<<4)|5: assert(false);break;
        case (0x5D<<4)|6: assert(false);break;
        case (0x5D<<4)|7: assert(false);break;
        case (0x5D<<4)|8: assert(false);break;
        case (0x5D<<4)|9: assert(false);break;
        case (0x5D<<4)|10: assert(false);break;
        case (0x5D<<4)|11: assert(false);break;
        case (0x5D<<4)|12: assert(false);break;
        case (0x5D<<4)|13: assert(false);break;
        case (0x5D<<4)|14: assert(false);break;
        case (0x5D<<4)|15: assert(false);break;
    /* NOP  */
        case (0x5E<<4)|0: _VMA();break;
        case (0x5E<<4)|1: _FETCH();break;
        case (0x5E<<4)|2: assert(false);break;
        case (0x5E<<4)|3: assert(false);break;
        case (0x5E<<4)|4: assert(false);break;
        case (0x5E<<4)|5: assert(false);break;
        case (0x5E<<4)|6: assert(false);break;
        case (0x5E<<4)|7: assert(false);break;
        case (0x5E<<4)|8: assert(false);break;
        case (0x5E<<4)|9: assert(false);break;
        case (0x5E<<4)|10: assert(false);break;
        case (0x5E<<4)|11: assert(false);break;
        case (0x5E<<4)|12: assert(false);break;
        case (0x5E<<4)|13: assert(false);break;
        case (0x5E<<4)|14: assert(false);break;
        case (0x5E<<4)|15: assert(false);break;
    /* CLRB  */
        case (0x5F<<4)|0: _VF(false);_CF(false);c->B=0;_NZ(0);_VMA();break;
        case (0x5F<<4)|1: _FETCH();break;
        case (0x5F<<4)|2: assert(false);break;
        case (0x5F<<4)|3: assert(false);break;
        case (0x5F<<4)|4: assert(false);break;
        case (0x5F<<4)|5: assert(false);break;
        case (0x5F<<4)|6: assert(false);break;
        case (0x5F<<4)|7: assert(false);break;
        case (0x5F<<4)|8: assert(false);break;
        case (0x5F<<4)|9: assert(false);break;
        case (0x5F<<4)|10: assert(false);break;
        case (0x5F<<4)|11: assert(false);break;
        case (0x5F<<4)|12: assert(false);break;
        case (0x5F<<4)|13: assert(false);break;
        case (0x5F<<4)|14: assert(false);break;
        case (0x5F<<4)|15: assert(false);break;
    /* NOP (zp,X) */
        case (0x60<<4)|0: _SA(c->PC++);break;
        case (0x60<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x60<<4)|2: _SA(c->AD);_VMA();break;
        case (0x60<<4)|3: break;
        case (0x60<<4)|4: _FETCH();break;
        case (0x60<<4)|5: assert(false);break;
        case (0x60<<4)|6: assert(false);break;
        case (0x60<<4)|7: assert(false);break;
        case (0x60<<4)|8: assert(false);break;
        case (0x60<<4)|9: assert(false);break;
        case (0x60<<4)|10: assert(false);break;
        case (0x60<<4)|11: assert(false);break;
        case (0x60<<4)|12: assert(false);break;
        case (0x60<<4)|13: assert(false);break;
        case (0x60<<4)|14: assert(false);break;
        case (0x60<<4)|15: assert(false);break;
    /* NOP  */
        case (0x61<<4)|0: _VMA();break;
        case (0x61<<4)|1: _FETCH();break;
        case (0x61<<4)|2: assert(false);break;
        case (0x61<<4)|3: assert(false);break;
        case (0x61<<4)|4: assert(false);break;
        case (0x61<<4)|5: assert(false);break;
        case (0x61<<4)|6: assert(false);break;
        case (0x61<<4)|7: assert(false);break;
        case (0x61<<4)|8: assert(false);break;
        case (0x61<<4)|9: assert(false);break;
        case (0x61<<4)|10: assert(false);break;
        case (0x61<<4)|11: assert(false);break;
        case (0x61<<4)|12: assert(false);break;
        case (0x61<<4)|13: assert(false);break;
        case (0x61<<4)|14: assert(false);break;
        case (0x61<<4)|15: assert(false);break;
    /* NOP  */
        case (0x62<<4)|0: _VMA();break;
        case (0x62<<4)|1: _FETCH();break;
        case (0x62<<4)|2: assert(false);break;
        case (0x62<<4)|3: assert(false);break;
        case (0x62<<4)|4: assert(false);break;
        case (0x62<<4)|5: assert(false);break;
        case (0x62<<4)|6: assert(false);break;
        case (0x62<<4)|7: assert(false);break;
        case (0x62<<4)|8: assert(false);break;
        case (0x62<<4)|9: assert(false);break;
        case (0x62<<4)|10: assert(false);break;
        case (0x62<<4)|11: assert(false);break;
        case (0x62<<4)|12: assert(false);break;
        case (0x62<<4)|13: assert(false);break;
        case (0x62<<4)|14: assert(false);break;
        case (0x62<<4)|15: assert(false);break;
    /* NOP (zp,X) */
        case (0x63<<4)|0: _SA(c->PC++);break;
        case (0x63<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x63<<4)|2: _SA(c->AD);_VMA();break;
        case (0x63<<4)|3: break;
        case (0x63<<4)|4: _FETCH();break;
        case (0x63<<4)|5: assert(false);break;
        case (0x63<<4)|6: assert(false);break;
        case (0x63<<4)|7: assert(false);break;
        case (0x63<<4)|8: assert(false);break;
        case (0x63<<4)|9: assert(false);break;
        case (0x63<<4)|10: assert(false);break;
        case (0x63<<4)|11: assert(false);break;
        case (0x63<<4)|12: assert(false);break;
        case (0x63<<4)|13: assert(false);break;
        case (0x63<<4)|14: assert(false);break;
        case (0x63<<4)|15: assert(false);break;
    /* LSR (zp,X) */
        case (0x64<<4)|0: _SA(c->PC++);break;
        case (0x64<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x64<<4)|2: _SA(c->AD);_VMA();break;
        case (0x64<<4)|3: _VMA();break;
        case (0x64<<4)|4: break;
        case (0x64<<4)|5: _SD(_mc6800_lsr(c, _GD()));_WR();break;
        case (0x64<<4)|6: _FETCH();break;
        case (0x64<<4)|7: assert(false);break;
        case (0x64<<4)|8: assert(false);break;
        case (0x64<<4)|9: assert(false);break;
        case (0x64<<4)|10: assert(false);break;
        case (0x64<<4)|11: assert(false);break;
        case (0x64<<4)|12: assert(false);break;
        case (0x64<<4)|13: assert(false);break;
        case (0x64<<4)|14: assert(false);break;
        case (0x64<<4)|15: assert(false);break;
    /* NOP  */
        case (0x65<<4)|0: _VMA();break;
        case (0x65<<4)|1: _FETCH();break;
        case (0x65<<4)|2: assert(false);break;
        case (0x65<<4)|3: assert(false);break;
        case (0x65<<4)|4: assert(false);break;
        case (0x65<<4)|5: assert(false);break;
        case (0x65<<4)|6: assert(false);break;
        case (0x65<<4)|7: assert(false);break;
        case (0x65<<4)|8: assert(false);break;
        case (0x65<<4)|9: assert(false);break;
        case (0x65<<4)|10: assert(false);break;
        case (0x65<<4)|11: assert(false);break;
        case (0x65<<4)|12: assert(false);break;
        case (0x65<<4)|13: assert(false);break;
        case (0x65<<4)|14: assert(false);break;
        case (0x65<<4)|15: assert(false);break;
    /* ROR (zp,X) */
        case (0x66<<4)|0: _SA(c->PC++);break;
        case (0x66<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x66<<4)|2: _SA(c->AD);_VMA();break;
        case (0x66<<4)|3: _VMA();break;
        case (0x66<<4)|4: break;
        case (0x66<<4)|5: _SD(_mc6800_ror(c, _GD()));_WR();break;
        case (0x66<<4)|6: _FETCH();break;
        case (0x66<<4)|7: assert(false);break;
        case (0x66<<4)|8: assert(false);break;
        case (0x66<<4)|9: assert(false);break;
        case (0x66<<4)|10: assert(false);break;
        case (0x66<<4)|11: assert(false);break;
        case (0x66<<4)|12: assert(false);break;
        case (0x66<<4)|13: assert(false);break;
        case (0x66<<4)|14: assert(false);break;
        case (0x66<<4)|15: assert(false);break;
    /* ASR (zp,X) */
        case (0x67<<4)|0: _SA(c->PC++);break;
        case (0x67<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x67<<4)|2: _SA(c->AD);_VMA();break;
        case (0x67<<4)|3: _VMA();break;
        case (0x67<<4)|4: break;
        case (0x67<<4)|5: _SD((_GD()&0x80)|_mc6800_lsr(c, _GD()));_NZ(_GA());_WR();break;
        case (0x67<<4)|6: _FETCH();break;
        case (0x67<<4)|7: assert(false);break;
        case (0x67<<4)|8: assert(false);break;
        case (0x67<<4)|9: assert(false);break;
        case (0x67<<4)|10: assert(false);break;
        case (0x67<<4)|11: assert(false);break;
        case (0x67<<4)|12: assert(false);break;
        case (0x67<<4)|13: assert(false);break;
        case (0x67<<4)|14: assert(false);break;
        case (0x67<<4)|15: assert(false);break;
    /* ASL (zp,X) */
        case (0x68<<4)|0: _SA(c->PC++);break;
        case (0x68<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x68<<4)|2: _SA(c->AD);_VMA();break;
        case (0x68<<4)|3: _VMA();break;
        case (0x68<<4)|4: break;
        case (0x68<<4)|5: _SD(_mc6800_asl(c, _GD()));_WR();break;
        case (0x68<<4)|6: _FETCH();break;
        case (0x68<<4)|7: assert(false);break;
        case (0x68<<4)|8: assert(false);break;
        case (0x68<<4)|9: assert(false);break;
        case (0x68<<4)|10: assert(false);break;
        case (0x68<<4)|11: assert(false);break;
        case (0x68<<4)|12: assert(false);break;
        case (0x68<<4)|13: assert(false);break;
        case (0x68<<4)|14: assert(false);break;
        case (0x68<<4)|15: assert(false);break;
    /* ROL (zp,X) */
        case (0x69<<4)|0: _SA(c->PC++);break;
        case (0x69<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x69<<4)|2: _SA(c->AD);_VMA();break;
        case (0x69<<4)|3: _VMA();break;
        case (0x69<<4)|4: break;
        case (0x69<<4)|5: _SD(_mc6800_rol(c, _GD()));_WR();break;
        case (0x69<<4)|6: _FETCH();break;
        case (0x69<<4)|7: assert(false);break;
        case (0x69<<4)|8: assert(false);break;
        case (0x69<<4)|9: assert(false);break;
        case (0x69<<4)|10: assert(false);break;
        case (0x69<<4)|11: assert(false);break;
        case (0x69<<4)|12: assert(false);break;
        case (0x69<<4)|13: assert(false);break;
        case (0x69<<4)|14: assert(false);break;
        case (0x69<<4)|15: assert(false);break;
    /* DEC (zp,X) */
        case (0x6A<<4)|0: _SA(c->PC++);break;
        case (0x6A<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x6A<<4)|2: _SA(c->AD);_VMA();break;
        case (0x6A<<4)|3: _VMA();break;
        case (0x6A<<4)|4: break;
        case (0x6A<<4)|5: _VF(_GD()==0x80);c->AD=_GD()-1;_NZ(c->AD);_SD(c->AD);_WR();break;
        case (0x6A<<4)|6: _FETCH();break;
        case (0x6A<<4)|7: assert(false);break;
        case (0x6A<<4)|8: assert(false);break;
        case (0x6A<<4)|9: assert(false);break;
        case (0x6A<<4)|10: assert(false);break;
        case (0x6A<<4)|11: assert(false);break;
        case (0x6A<<4)|12: assert(false);break;
        case (0x6A<<4)|13: assert(false);break;
        case (0x6A<<4)|14: assert(false);break;
        case (0x6A<<4)|15: assert(false);break;
    /* NOP  */
        case (0x6B<<4)|0: _VMA();break;
        case (0x6B<<4)|1: _FETCH();break;
        case (0x6B<<4)|2: assert(false);break;
        case (0x6B<<4)|3: assert(false);break;
        case (0x6B<<4)|4: assert(false);break;
        case (0x6B<<4)|5: assert(false);break;
        case (0x6B<<4)|6: assert(false);break;
        case (0x6B<<4)|7: assert(false);break;
        case (0x6B<<4)|8: assert(false);break;
        case (0x6B<<4)|9: assert(false);break;
        case (0x6B<<4)|10: assert(false);break;
        case (0x6B<<4)|11: assert(false);break;
        case (0x6B<<4)|12: assert(false);break;
        case (0x6B<<4)|13: assert(false);break;
        case (0x6B<<4)|14: assert(false);break;
        case (0x6B<<4)|15: assert(false);break;
    /* INC (zp,X) */
        case (0x6C<<4)|0: _SA(c->PC++);break;
        case (0x6C<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x6C<<4)|2: _SA(c->AD);_VMA();break;
        case (0x6C<<4)|3: _VMA();break;
        case (0x6C<<4)|4: break;
        case (0x6C<<4)|5: _VF(_GD()==0x7F);c->AD=_GD()+1;_NZ(c->AD);_SD(c->AD);_WR();break;
        case (0x6C<<4)|6: _FETCH();break;
        case (0x6C<<4)|7: assert(false);break;
        case (0x6C<<4)|8: assert(false);break;
        case (0x6C<<4)|9: assert(false);break;
        case (0x6C<<4)|10: assert(false);break;
        case (0x6C<<4)|11: assert(false);break;
        case (0x6C<<4)|12: assert(false);break;
        case (0x6C<<4)|13: assert(false);break;
        case (0x6C<<4)|14: assert(false);break;
        case (0x6C<<4)|15: assert(false);break;
    /* TST (zp,X) */
        case (0x6D<<4)|0: _SA(c->PC++);break;
        case (0x6D<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x6D<<4)|2: _SA(c->AD);_VMA();break;
        case (0x6D<<4)|3: break;
        case (0x6D<<4)|4: _VF(false);_CF(false);_NZ(_GD());_FETCH();break;
        case (0x6D<<4)|5: assert(false);break;
        case (0x6D<<4)|6: assert(false);break;
        case (0x6D<<4)|7: assert(false);break;
        case (0x6D<<4)|8: assert(false);break;
        case (0x6D<<4)|9: assert(false);break;
        case (0x6D<<4)|10: assert(false);break;
        case (0x6D<<4)|11: assert(false);break;
        case (0x6D<<4)|12: assert(false);break;
        case (0x6D<<4)|13: assert(false);break;
        case (0x6D<<4)|14: assert(false);break;
        case (0x6D<<4)|15: assert(false);break;
    /* JMP # */
        case (0x6E<<4)|0: _SA(c->PC++);break;
        case (0x6E<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x6E<<4)|2: c->PC=c->AD;_VMA();break;
        case (0x6E<<4)|3: _FETCH();break;
        case (0x6E<<4)|4: assert(false);break;
        case (0x6E<<4)|5: assert(false);break;
        case (0x6E<<4)|6: assert(false);break;
        case (0x6E<<4)|7: assert(false);break;
        case (0x6E<<4)|8: assert(false);break;
        case (0x6E<<4)|9: assert(false);break;
        case (0x6E<<4)|10: assert(false);break;
        case (0x6E<<4)|11: assert(false);break;
        case (0x6E<<4)|12: assert(false);break;
        case (0x6E<<4)|13: assert(false);break;
        case (0x6E<<4)|14: assert(false);break;
        case (0x6E<<4)|15: assert(false);break;
    /* CLR (zp,X) */
        case (0x6F<<4)|0: _SA(c->PC++);break;
        case (0x6F<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0x6F<<4)|2: _SA(c->AD);_VMA();break;
        case (0x6F<<4)|3: _VMA();break;
        case (0x6F<<4)|4: _VF(false);_CF(false);_NZ(0);_SD(0);_WR();break;
        case (0x6F<<4)|5: _FETCH();break;
        case (0x6F<<4)|6: assert(false);break;
        case (0x6F<<4)|7: assert(false);break;
        case (0x6F<<4)|8: assert(false);break;
        case (0x6F<<4)|9: assert(false);break;
        case (0x6F<<4)|10: assert(false);break;
        case (0x6F<<4)|11: assert(false);break;
        case (0x6F<<4)|12: assert(false);break;
        case (0x6F<<4)|13: assert(false);break;
        case (0x6F<<4)|14: assert(false);break;
        case (0x6F<<4)|15: assert(false);break;
    /* NOP abs */
        case (0x70<<4)|0: _SA(c->PC++);break;
        case (0x70<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x70<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0x70<<4)|3: _FETCH();break;
        case (0x70<<4)|4: assert(false);break;
        case (0x70<<4)|5: assert(false);break;
        case (0x70<<4)|6: assert(false);break;
        case (0x70<<4)|7: assert(false);break;
        case (0x70<<4)|8: assert(false);break;
        case (0x70<<4)|9: assert(false);break;
        case (0x70<<4)|10: assert(false);break;
        case (0x70<<4)|11: assert(false);break;
        case (0x70<<4)|12: assert(false);break;
        case (0x70<<4)|13: assert(false);break;
        case (0x70<<4)|14: assert(false);break;
        case (0x70<<4)|15: assert(false);break;
    /* NOP  */
        case (0x71<<4)|0: _VMA();break;
        case (0x71<<4)|1: _FETCH();break;
        case (0x71<<4)|2: assert(false);break;
        case (0x71<<4)|3: assert(false);break;
        case (0x71<<4)|4: assert(false);break;
        case (0x71<<4)|5: assert(false);break;
        case (0x71<<4)|6: assert(false);break;
        case (0x71<<4)|7: assert(false);break;
        case (0x71<<4)|8: assert(false);break;
        case (0x71<<4)|9: assert(false);break;
        case (0x71<<4)|10: assert(false);break;
        case (0x71<<4)|11: assert(false);break;
        case (0x71<<4)|12: assert(false);break;
        case (0x71<<4)|13: assert(false);break;
        case (0x71<<4)|14: assert(false);break;
        case (0x71<<4)|15: assert(false);break;
    /* NOP  */
        case (0x72<<4)|0: _VMA();break;
        case (0x72<<4)|1: _FETCH();break;
        case (0x72<<4)|2: assert(false);break;
        case (0x72<<4)|3: assert(false);break;
        case (0x72<<4)|4: assert(false);break;
        case (0x72<<4)|5: assert(false);break;
        case (0x72<<4)|6: assert(false);break;
        case (0x72<<4)|7: assert(false);break;
        case (0x72<<4)|8: assert(false);break;
        case (0x72<<4)|9: assert(false);break;
        case (0x72<<4)|10: assert(false);break;
        case (0x72<<4)|11: assert(false);break;
        case (0x72<<4)|12: assert(false);break;
        case (0x72<<4)|13: assert(false);break;
        case (0x72<<4)|14: assert(false);break;
        case (0x72<<4)|15: assert(false);break;
    /* NOP abs */
        case (0x73<<4)|0: _SA(c->PC++);break;
        case (0x73<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x73<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0x73<<4)|3: _FETCH();break;
        case (0x73<<4)|4: assert(false);break;
        case (0x73<<4)|5: assert(false);break;
        case (0x73<<4)|6: assert(false);break;
        case (0x73<<4)|7: assert(false);break;
        case (0x73<<4)|8: assert(false);break;
        case (0x73<<4)|9: assert(false);break;
        case (0x73<<4)|10: assert(false);break;
        case (0x73<<4)|11: assert(false);break;
        case (0x73<<4)|12: assert(false);break;
        case (0x73<<4)|13: assert(false);break;
        case (0x73<<4)|14: assert(false);break;
        case (0x73<<4)|15: assert(false);break;
    /* LSR abs */
        case (0x74<<4)|0: _SA(c->PC++);break;
        case (0x74<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x74<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0x74<<4)|3: break;
        case (0x74<<4)|4: _SD(_mc6800_lsr(c, _GD()));_WR();break;
        case (0x74<<4)|5: _FETCH();break;
        case (0x74<<4)|6: assert(false);break;
        case (0x74<<4)|7: assert(false);break;
        case (0x74<<4)|8: assert(false);break;
        case (0x74<<4)|9: assert(false);break;
        case (0x74<<4)|10: assert(false);break;
        case (0x74<<4)|11: assert(false);break;
        case (0x74<<4)|12: assert(false);break;
        case (0x74<<4)|13: assert(false);break;
        case (0x74<<4)|14: assert(false);break;
        case (0x74<<4)|15: assert(false);break;
    /* NOP  */
        case (0x75<<4)|0: _VMA();break;
        case (0x75<<4)|1: _FETCH();break;
        case (0x75<<4)|2: assert(false);break;
        case (0x75<<4)|3: assert(false);break;
        case (0x75<<4)|4: assert(false);break;
        case (0x75<<4)|5: assert(false);break;
        case (0x75<<4)|6: assert(false);break;
        case (0x75<<4)|7: assert(false);break;
        case (0x75<<4)|8: assert(false);break;
        case (0x75<<4)|9: assert(false);break;
        case (0x75<<4)|10: assert(false);break;
        case (0x75<<4)|11: assert(false);break;
        case (0x75<<4)|12: assert(false);break;
        case (0x75<<4)|13: assert(false);break;
        case (0x75<<4)|14: assert(false);break;
        case (0x75<<4)|15: assert(false);break;
    /* ROR abs */
        case (0x76<<4)|0: _SA(c->PC++);break;
        case (0x76<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x76<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0x76<<4)|3: break;
        case (0x76<<4)|4: _SD(_mc6800_ror(c, _GD()));_WR();break;
        case (0x76<<4)|5: _FETCH();break;
        case (0x76<<4)|6: assert(false);break;
        case (0x76<<4)|7: assert(false);break;
        case (0x76<<4)|8: assert(false);break;
        case (0x76<<4)|9: assert(false);break;
        case (0x76<<4)|10: assert(false);break;
        case (0x76<<4)|11: assert(false);break;
        case (0x76<<4)|12: assert(false);break;
        case (0x76<<4)|13: assert(false);break;
        case (0x76<<4)|14: assert(false);break;
        case (0x76<<4)|15: assert(false);break;
    /* ASR abs */
        case (0x77<<4)|0: _SA(c->PC++);break;
        case (0x77<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x77<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0x77<<4)|3: break;
        case (0x77<<4)|4: _SD((_GD()&0x80)|_mc6800_lsr(c, _GD()));_NZ(_GA());_WR();break;
        case (0x77<<4)|5: _FETCH();break;
        case (0x77<<4)|6: assert(false);break;
        case (0x77<<4)|7: assert(false);break;
        case (0x77<<4)|8: assert(false);break;
        case (0x77<<4)|9: assert(false);break;
        case (0x77<<4)|10: assert(false);break;
        case (0x77<<4)|11: assert(false);break;
        case (0x77<<4)|12: assert(false);break;
        case (0x77<<4)|13: assert(false);break;
        case (0x77<<4)|14: assert(false);break;
        case (0x77<<4)|15: assert(false);break;
    /* ASL abs */
        case (0x78<<4)|0: _SA(c->PC++);break;
        case (0x78<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x78<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0x78<<4)|3: break;
        case (0x78<<4)|4: _SD(_mc6800_asl(c, _GD()));_WR();break;
        case (0x78<<4)|5: _FETCH();break;
        case (0x78<<4)|6: assert(false);break;
        case (0x78<<4)|7: assert(false);break;
        case (0x78<<4)|8: assert(false);break;
        case (0x78<<4)|9: assert(false);break;
        case (0x78<<4)|10: assert(false);break;
        case (0x78<<4)|11: assert(false);break;
        case (0x78<<4)|12: assert(false);break;
        case (0x78<<4)|13: assert(false);break;
        case (0x78<<4)|14: assert(false);break;
        case (0x78<<4)|15: assert(false);break;
    /* ROL abs */
        case (0x79<<4)|0: _SA(c->PC++);break;
        case (0x79<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x79<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0x79<<4)|3: break;
        case (0x79<<4)|4: _SD(_mc6800_rol(c, _GD()));_WR();break;
        case (0x79<<4)|5: _FETCH();break;
        case (0x79<<4)|6: assert(false);break;
        case (0x79<<4)|7: assert(false);break;
        case (0x79<<4)|8: assert(false);break;
        case (0x79<<4)|9: assert(false);break;
        case (0x79<<4)|10: assert(false);break;
        case (0x79<<4)|11: assert(false);break;
        case (0x79<<4)|12: assert(false);break;
        case (0x79<<4)|13: assert(false);break;
        case (0x79<<4)|14: assert(false);break;
        case (0x79<<4)|15: assert(false);break;
    /* DEC abs */
        case (0x7A<<4)|0: _SA(c->PC++);break;
        case (0x7A<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x7A<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0x7A<<4)|3: break;
        case (0x7A<<4)|4: _VF(_GD()==0x80);c->AD=_GD()-1;_NZ(c->AD);_SD(c->AD);_WR();break;
        case (0x7A<<4)|5: _FETCH();break;
        case (0x7A<<4)|6: assert(false);break;
        case (0x7A<<4)|7: assert(false);break;
        case (0x7A<<4)|8: assert(false);break;
        case (0x7A<<4)|9: assert(false);break;
        case (0x7A<<4)|10: assert(false);break;
        case (0x7A<<4)|11: assert(false);break;
        case (0x7A<<4)|12: assert(false);break;
        case (0x7A<<4)|13: assert(false);break;
        case (0x7A<<4)|14: assert(false);break;
        case (0x7A<<4)|15: assert(false);break;
    /* NOP  */
        case (0x7B<<4)|0: _VMA();break;
        case (0x7B<<4)|1: _FETCH();break;
        case (0x7B<<4)|2: assert(false);break;
        case (0x7B<<4)|3: assert(false);break;
        case (0x7B<<4)|4: assert(false);break;
        case (0x7B<<4)|5: assert(false);break;
        case (0x7B<<4)|6: assert(false);break;
        case (0x7B<<4)|7: assert(false);break;
        case (0x7B<<4)|8: assert(false);break;
        case (0x7B<<4)|9: assert(false);break;
        case (0x7B<<4)|10: assert(false);break;
        case (0x7B<<4)|11: assert(false);break;
        case (0x7B<<4)|12: assert(false);break;
        case (0x7B<<4)|13: assert(false);break;
        case (0x7B<<4)|14: assert(false);break;
        case (0x7B<<4)|15: assert(false);break;
    /* INC abs */
        case (0x7C<<4)|0: _SA(c->PC++);break;
        case (0x7C<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x7C<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0x7C<<4)|3: break;
        case (0x7C<<4)|4: _VF(_GD()==0x7F);c->AD=_GD()+1;_NZ(c->AD);_SD(c->AD);_WR();break;
        case (0x7C<<4)|5: _FETCH();break;
        case (0x7C<<4)|6: assert(false);break;
        case (0x7C<<4)|7: assert(false);break;
        case (0x7C<<4)|8: assert(false);break;
        case (0x7C<<4)|9: assert(false);break;
        case (0x7C<<4)|10: assert(false);break;
        case (0x7C<<4)|11: assert(false);break;
        case (0x7C<<4)|12: assert(false);break;
        case (0x7C<<4)|13: assert(false);break;
        case (0x7C<<4)|14: assert(false);break;
        case (0x7C<<4)|15: assert(false);break;
    /* TST abs */
        case (0x7D<<4)|0: _SA(c->PC++);break;
        case (0x7D<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x7D<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0x7D<<4)|3: _VF(false);_CF(false);_NZ(_GD());_FETCH();break;
        case (0x7D<<4)|4: assert(false);break;
        case (0x7D<<4)|5: assert(false);break;
        case (0x7D<<4)|6: assert(false);break;
        case (0x7D<<4)|7: assert(false);break;
        case (0x7D<<4)|8: assert(false);break;
        case (0x7D<<4)|9: assert(false);break;
        case (0x7D<<4)|10: assert(false);break;
        case (0x7D<<4)|11: assert(false);break;
        case (0x7D<<4)|12: assert(false);break;
        case (0x7D<<4)|13: assert(false);break;
        case (0x7D<<4)|14: assert(false);break;
        case (0x7D<<4)|15: assert(false);break;
    /* JMP #16 */
        case (0x7E<<4)|0: _SA(c->PC++);c->PC++;break;
        case (0x7E<<4)|1: c->AD=_GD();c->AD<<=8;_SA(_GA()+1);break;
        case (0x7E<<4)|2: c->PC=c->AD|_GD();_FETCH();break;
        case (0x7E<<4)|3: assert(false);break;
        case (0x7E<<4)|4: assert(false);break;
        case (0x7E<<4)|5: assert(false);break;
        case (0x7E<<4)|6: assert(false);break;
        case (0x7E<<4)|7: assert(false);break;
        case (0x7E<<4)|8: assert(false);break;
        case (0x7E<<4)|9: assert(false);break;
        case (0x7E<<4)|10: assert(false);break;
        case (0x7E<<4)|11: assert(false);break;
        case (0x7E<<4)|12: assert(false);break;
        case (0x7E<<4)|13: assert(false);break;
        case (0x7E<<4)|14: assert(false);break;
        case (0x7E<<4)|15: assert(false);break;
    /* CLR abs */
        case (0x7F<<4)|0: _SA(c->PC++);break;
        case (0x7F<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0x7F<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0x7F<<4)|3: _VF(false);_CF(false);_NZ(0);_SD(0);_WR();break;
        case (0x7F<<4)|4: _FETCH();break;
        case (0x7F<<4)|5: assert(false);break;
        case (0x7F<<4)|6: assert(false);break;
        case (0x7F<<4)|7: assert(false);break;
        case (0x7F<<4)|8: assert(false);break;
        case (0x7F<<4)|9: assert(false);break;
        case (0x7F<<4)|10: assert(false);break;
        case (0x7F<<4)|11: assert(false);break;
        case (0x7F<<4)|12: assert(false);break;
        case (0x7F<<4)|13: assert(false);break;
        case (0x7F<<4)|14: assert(false);break;
        case (0x7F<<4)|15: assert(false);break;
    /* SUBA # */
        case (0x80<<4)|0: _SA(c->PC++);break;
        case (0x80<<4)|1: _mc6800_sub(c, _GD(), true, false);_FETCH();break;
        case (0x80<<4)|2: assert(false);break;
        case (0x80<<4)|3: assert(false);break;
        case (0x80<<4)|4: assert(false);break;
        case (0x80<<4)|5: assert(false);break;
        case (0x80<<4)|6: assert(false);break;
        case (0x80<<4)|7: assert(false);break;
        case (0x80<<4)|8: assert(false);break;
        case (0x80<<4)|9: assert(false);break;
        case (0x80<<4)|10: assert(false);break;
        case (0x80<<4)|11: assert(false);break;
        case (0x80<<4)|12: assert(false);break;
        case (0x80<<4)|13: assert(false);break;
        case (0x80<<4)|14: assert(false);break;
        case (0x80<<4)|15: assert(false);break;
    /* CMPA # */
        case (0x81<<4)|0: _SA(c->PC++);break;
        case (0x81<<4)|1: _mc6800_cmp(c, _GD(), true);_FETCH();break;
        case (0x81<<4)|2: assert(false);break;
        case (0x81<<4)|3: assert(false);break;
        case (0x81<<4)|4: assert(false);break;
        case (0x81<<4)|5: assert(false);break;
        case (0x81<<4)|6: assert(false);break;
        case (0x81<<4)|7: assert(false);break;
        case (0x81<<4)|8: assert(false);break;
        case (0x81<<4)|9: assert(false);break;
        case (0x81<<4)|10: assert(false);break;
        case (0x81<<4)|11: assert(false);break;
        case (0x81<<4)|12: assert(false);break;
        case (0x81<<4)|13: assert(false);break;
        case (0x81<<4)|14: assert(false);break;
        case (0x81<<4)|15: assert(false);break;
    /* SBCA # */
        case (0x82<<4)|0: _SA(c->PC++);break;
        case (0x82<<4)|1: _mc6800_sub(c, _GD(), true, true);_FETCH();break;
        case (0x82<<4)|2: assert(false);break;
        case (0x82<<4)|3: assert(false);break;
        case (0x82<<4)|4: assert(false);break;
        case (0x82<<4)|5: assert(false);break;
        case (0x82<<4)|6: assert(false);break;
        case (0x82<<4)|7: assert(false);break;
        case (0x82<<4)|8: assert(false);break;
        case (0x82<<4)|9: assert(false);break;
        case (0x82<<4)|10: assert(false);break;
        case (0x82<<4)|11: assert(false);break;
        case (0x82<<4)|12: assert(false);break;
        case (0x82<<4)|13: assert(false);break;
        case (0x82<<4)|14: assert(false);break;
        case (0x82<<4)|15: assert(false);break;
    /* NOP  */
        case (0x83<<4)|0: _VMA();break;
        case (0x83<<4)|1: _FETCH();break;
        case (0x83<<4)|2: assert(false);break;
        case (0x83<<4)|3: assert(false);break;
        case (0x83<<4)|4: assert(false);break;
        case (0x83<<4)|5: assert(false);break;
        case (0x83<<4)|6: assert(false);break;
        case (0x83<<4)|7: assert(false);break;
        case (0x83<<4)|8: assert(false);break;
        case (0x83<<4)|9: assert(false);break;
        case (0x83<<4)|10: assert(false);break;
        case (0x83<<4)|11: assert(false);break;
        case (0x83<<4)|12: assert(false);break;
        case (0x83<<4)|13: assert(false);break;
        case (0x83<<4)|14: assert(false);break;
        case (0x83<<4)|15: assert(false);break;
    /* ANDA # */
        case (0x84<<4)|0: _SA(c->PC++);break;
        case (0x84<<4)|1: _VF(false);c->A&=_GD();_NZ(c->A);_FETCH();break;
        case (0x84<<4)|2: assert(false);break;
        case (0x84<<4)|3: assert(false);break;
        case (0x84<<4)|4: assert(false);break;
        case (0x84<<4)|5: assert(false);break;
        case (0x84<<4)|6: assert(false);break;
        case (0x84<<4)|7: assert(false);break;
        case (0x84<<4)|8: assert(false);break;
        case (0x84<<4)|9: assert(false);break;
        case (0x84<<4)|10: assert(false);break;
        case (0x84<<4)|11: assert(false);break;
        case (0x84<<4)|12: assert(false);break;
        case (0x84<<4)|13: assert(false);break;
        case (0x84<<4)|14: assert(false);break;
        case (0x84<<4)|15: assert(false);break;
    /* BITA # */
        case (0x85<<4)|0: _SA(c->PC++);break;
        case (0x85<<4)|1: _VF(false);_NZ(c->A&_GD());_FETCH();break;
        case (0x85<<4)|2: assert(false);break;
        case (0x85<<4)|3: assert(false);break;
        case (0x85<<4)|4: assert(false);break;
        case (0x85<<4)|5: assert(false);break;
        case (0x85<<4)|6: assert(false);break;
        case (0x85<<4)|7: assert(false);break;
        case (0x85<<4)|8: assert(false);break;
        case (0x85<<4)|9: assert(false);break;
        case (0x85<<4)|10: assert(false);break;
        case (0x85<<4)|11: assert(false);break;
        case (0x85<<4)|12: assert(false);break;
        case (0x85<<4)|13: assert(false);break;
        case (0x85<<4)|14: assert(false);break;
        case (0x85<<4)|15: assert(false);break;
    /* LDAA # */
        case (0x86<<4)|0: _SA(c->PC++);break;
        case (0x86<<4)|1: _VF(false);c->A=_GD();_NZ(c->A);_FETCH();break;
        case (0x86<<4)|2: assert(false);break;
        case (0x86<<4)|3: assert(false);break;
        case (0x86<<4)|4: assert(false);break;
        case (0x86<<4)|5: assert(false);break;
        case (0x86<<4)|6: assert(false);break;
        case (0x86<<4)|7: assert(false);break;
        case (0x86<<4)|8: assert(false);break;
        case (0x86<<4)|9: assert(false);break;
        case (0x86<<4)|10: assert(false);break;
        case (0x86<<4)|11: assert(false);break;
        case (0x86<<4)|12: assert(false);break;
        case (0x86<<4)|13: assert(false);break;
        case (0x86<<4)|14: assert(false);break;
        case (0x86<<4)|15: assert(false);break;
    /* NOP  */
        case (0x87<<4)|0: _VMA();break;
        case (0x87<<4)|1: _FETCH();break;
        case (0x87<<4)|2: assert(false);break;
        case (0x87<<4)|3: assert(false);break;
        case (0x87<<4)|4: assert(false);break;
        case (0x87<<4)|5: assert(false);break;
        case (0x87<<4)|6: assert(false);break;
        case (0x87<<4)|7: assert(false);break;
        case (0x87<<4)|8: assert(false);break;
        case (0x87<<4)|9: assert(false);break;
        case (0x87<<4)|10: assert(false);break;
        case (0x87<<4)|11: assert(false);break;
        case (0x87<<4)|12: assert(false);break;
        case (0x87<<4)|13: assert(false);break;
        case (0x87<<4)|14: assert(false);break;
        case (0x87<<4)|15: assert(false);break;
    /* EORA # */
        case (0x88<<4)|0: _SA(c->PC++);break;
        case (0x88<<4)|1: _VF(false);c->A^=_GD();_NZ(c->A);_FETCH();break;
        case (0x88<<4)|2: assert(false);break;
        case (0x88<<4)|3: assert(false);break;
        case (0x88<<4)|4: assert(false);break;
        case (0x88<<4)|5: assert(false);break;
        case (0x88<<4)|6: assert(false);break;
        case (0x88<<4)|7: assert(false);break;
        case (0x88<<4)|8: assert(false);break;
        case (0x88<<4)|9: assert(false);break;
        case (0x88<<4)|10: assert(false);break;
        case (0x88<<4)|11: assert(false);break;
        case (0x88<<4)|12: assert(false);break;
        case (0x88<<4)|13: assert(false);break;
        case (0x88<<4)|14: assert(false);break;
        case (0x88<<4)|15: assert(false);break;
    /* ADCA # */
        case (0x89<<4)|0: _SA(c->PC++);break;
        case (0x89<<4)|1: _mc6800_add(c, _GD(), true, true);_FETCH();break;
        case (0x89<<4)|2: assert(false);break;
        case (0x89<<4)|3: assert(false);break;
        case (0x89<<4)|4: assert(false);break;
        case (0x89<<4)|5: assert(false);break;
        case (0x89<<4)|6: assert(false);break;
        case (0x89<<4)|7: assert(false);break;
        case (0x89<<4)|8: assert(false);break;
        case (0x89<<4)|9: assert(false);break;
        case (0x89<<4)|10: assert(false);break;
        case (0x89<<4)|11: assert(false);break;
        case (0x89<<4)|12: assert(false);break;
        case (0x89<<4)|13: assert(false);break;
        case (0x89<<4)|14: assert(false);break;
        case (0x89<<4)|15: assert(false);break;
    /* ORAA # */
        case (0x8A<<4)|0: _SA(c->PC++);break;
        case (0x8A<<4)|1: _VF(false);c->A|=_GD();_NZ(c->A);_FETCH();break;
        case (0x8A<<4)|2: assert(false);break;
        case (0x8A<<4)|3: assert(false);break;
        case (0x8A<<4)|4: assert(false);break;
        case (0x8A<<4)|5: assert(false);break;
        case (0x8A<<4)|6: assert(false);break;
        case (0x8A<<4)|7: assert(false);break;
        case (0x8A<<4)|8: assert(false);break;
        case (0x8A<<4)|9: assert(false);break;
        case (0x8A<<4)|10: assert(false);break;
        case (0x8A<<4)|11: assert(false);break;
        case (0x8A<<4)|12: assert(false);break;
        case (0x8A<<4)|13: assert(false);break;
        case (0x8A<<4)|14: assert(false);break;
        case (0x8A<<4)|15: assert(false);break;
    /* ADDA # */
        case (0x8B<<4)|0: _SA(c->PC++);break;
        case (0x8B<<4)|1: _mc6800_add(c, _GD(), true, false);_FETCH();break;
        case (0x8B<<4)|2: assert(false);break;
        case (0x8B<<4)|3: assert(false);break;
        case (0x8B<<4)|4: assert(false);break;
        case (0x8B<<4)|5: assert(false);break;
        case (0x8B<<4)|6: assert(false);break;
        case (0x8B<<4)|7: assert(false);break;
        case (0x8B<<4)|8: assert(false);break;
        case (0x8B<<4)|9: assert(false);break;
        case (0x8B<<4)|10: assert(false);break;
        case (0x8B<<4)|11: assert(false);break;
        case (0x8B<<4)|12: assert(false);break;
        case (0x8B<<4)|13: assert(false);break;
        case (0x8B<<4)|14: assert(false);break;
        case (0x8B<<4)|15: assert(false);break;
    /* CPX # */
        case (0x8C<<4)|0: _SA(c->PC++);break;
        case (0x8C<<4)|1: c->AD=_GD();c->AD<<=8;_SA(_GA()+1);break;
        case (0x8C<<4)|2: c->AD=c->AD|_GD();_mc6800_cpx(c, c->AD);_FETCH();break;
        case (0x8C<<4)|3: assert(false);break;
        case (0x8C<<4)|4: assert(false);break;
        case (0x8C<<4)|5: assert(false);break;
        case (0x8C<<4)|6: assert(false);break;
        case (0x8C<<4)|7: assert(false);break;
        case (0x8C<<4)|8: assert(false);break;
        case (0x8C<<4)|9: assert(false);break;
        case (0x8C<<4)|10: assert(false);break;
        case (0x8C<<4)|11: assert(false);break;
        case (0x8C<<4)|12: assert(false);break;
        case (0x8C<<4)|13: assert(false);break;
        case (0x8C<<4)|14: assert(false);break;
        case (0x8C<<4)|15: assert(false);break;
    /* NOP # */
        case (0x8D<<4)|0: _SA(c->PC++);break;
        case (0x8D<<4)|1: _FETCH();break;
        case (0x8D<<4)|2: assert(false);break;
        case (0x8D<<4)|3: assert(false);break;
        case (0x8D<<4)|4: assert(false);break;
        case (0x8D<<4)|5: assert(false);break;
        case (0x8D<<4)|6: assert(false);break;
        case (0x8D<<4)|7: assert(false);break;
        case (0x8D<<4)|8: assert(false);break;
        case (0x8D<<4)|9: assert(false);break;
        case (0x8D<<4)|10: assert(false);break;
        case (0x8D<<4)|11: assert(false);break;
        case (0x8D<<4)|12: assert(false);break;
        case (0x8D<<4)|13: assert(false);break;
        case (0x8D<<4)|14: assert(false);break;
        case (0x8D<<4)|15: assert(false);break;
    /* LDS #16 */
        case (0x8E<<4)|0: _SA(c->PC++);c->PC++;break;
        case (0x8E<<4)|1: c->SP=_GD();c->SP<<=8;_SA(_GA()+1);break;
        case (0x8E<<4)|2: _VF(false);c->SP|=_GD();_NZ16(c->SP);_FETCH();break;
        case (0x8E<<4)|3: assert(false);break;
        case (0x8E<<4)|4: assert(false);break;
        case (0x8E<<4)|5: assert(false);break;
        case (0x8E<<4)|6: assert(false);break;
        case (0x8E<<4)|7: assert(false);break;
        case (0x8E<<4)|8: assert(false);break;
        case (0x8E<<4)|9: assert(false);break;
        case (0x8E<<4)|10: assert(false);break;
        case (0x8E<<4)|11: assert(false);break;
        case (0x8E<<4)|12: assert(false);break;
        case (0x8E<<4)|13: assert(false);break;
        case (0x8E<<4)|14: assert(false);break;
        case (0x8E<<4)|15: assert(false);break;
    /* NOP  */
        case (0x8F<<4)|0: _VMA();break;
        case (0x8F<<4)|1: _FETCH();break;
        case (0x8F<<4)|2: assert(false);break;
        case (0x8F<<4)|3: assert(false);break;
        case (0x8F<<4)|4: assert(false);break;
        case (0x8F<<4)|5: assert(false);break;
        case (0x8F<<4)|6: assert(false);break;
        case (0x8F<<4)|7: assert(false);break;
        case (0x8F<<4)|8: assert(false);break;
        case (0x8F<<4)|9: assert(false);break;
        case (0x8F<<4)|10: assert(false);break;
        case (0x8F<<4)|11: assert(false);break;
        case (0x8F<<4)|12: assert(false);break;
        case (0x8F<<4)|13: assert(false);break;
        case (0x8F<<4)|14: assert(false);break;
        case (0x8F<<4)|15: assert(false);break;
    /* SUBA zp */
        case (0x90<<4)|0: _SA(c->PC++);break;
        case (0x90<<4)|1: _SA(_GD());break;
        case (0x90<<4)|2: _mc6800_sub(c, _GD(), true, false);_FETCH();break;
        case (0x90<<4)|3: assert(false);break;
        case (0x90<<4)|4: assert(false);break;
        case (0x90<<4)|5: assert(false);break;
        case (0x90<<4)|6: assert(false);break;
        case (0x90<<4)|7: assert(false);break;
        case (0x90<<4)|8: assert(false);break;
        case (0x90<<4)|9: assert(false);break;
        case (0x90<<4)|10: assert(false);break;
        case (0x90<<4)|11: assert(false);break;
        case (0x90<<4)|12: assert(false);break;
        case (0x90<<4)|13: assert(false);break;
        case (0x90<<4)|14: assert(false);break;
        case (0x90<<4)|15: assert(false);break;
    /* CMPA zp */
        case (0x91<<4)|0: _SA(c->PC++);break;
        case (0x91<<4)|1: _SA(_GD());break;
        case (0x91<<4)|2: _mc6800_cmp(c, _GD(), true);_FETCH();break;
        case (0x91<<4)|3: assert(false);break;
        case (0x91<<4)|4: assert(false);break;
        case (0x91<<4)|5: assert(false);break;
        case (0x91<<4)|6: assert(false);break;
        case (0x91<<4)|7: assert(false);break;
        case (0x91<<4)|8: assert(false);break;
        case (0x91<<4)|9: assert(false);break;
        case (0x91<<4)|10: assert(false);break;
        case (0x91<<4)|11: assert(false);break;
        case (0x91<<4)|12: assert(false);break;
        case (0x91<<4)|13: assert(false);break;
        case (0x91<<4)|14: assert(false);break;
        case (0x91<<4)|15: assert(false);break;
    /* SBCA zp */
        case (0x92<<4)|0: _SA(c->PC++);break;
        case (0x92<<4)|1: _SA(_GD());break;
        case (0x92<<4)|2: _mc6800_sub(c, _GD(), true, true);_FETCH();break;
        case (0x92<<4)|3: assert(false);break;
        case (0x92<<4)|4: assert(false);break;
        case (0x92<<4)|5: assert(false);break;
        case (0x92<<4)|6: assert(false);break;
        case (0x92<<4)|7: assert(false);break;
        case (0x92<<4)|8: assert(false);break;
        case (0x92<<4)|9: assert(false);break;
        case (0x92<<4)|10: assert(false);break;
        case (0x92<<4)|11: assert(false);break;
        case (0x92<<4)|12: assert(false);break;
        case (0x92<<4)|13: assert(false);break;
        case (0x92<<4)|14: assert(false);break;
        case (0x92<<4)|15: assert(false);break;
    /* NOP  */
        case (0x93<<4)|0: _VMA();break;
        case (0x93<<4)|1: _FETCH();break;
        case (0x93<<4)|2: assert(false);break;
        case (0x93<<4)|3: assert(false);break;
        case (0x93<<4)|4: assert(false);break;
        case (0x93<<4)|5: assert(false);break;
        case (0x93<<4)|6: assert(false);break;
        case (0x93<<4)|7: assert(false);break;
        case (0x93<<4)|8: assert(false);break;
        case (0x93<<4)|9: assert(false);break;
        case (0x93<<4)|10: assert(false);break;
        case (0x93<<4)|11: assert(false);break;
        case (0x93<<4)|12: assert(false);break;
        case (0x93<<4)|13: assert(false);break;
        case (0x93<<4)|14: assert(false);break;
        case (0x93<<4)|15: assert(false);break;
    /* ANDA zp */
        case (0x94<<4)|0: _SA(c->PC++);break;
        case (0x94<<4)|1: _SA(_GD());break;
        case (0x94<<4)|2: _VF(false);c->A&=_GD();_NZ(c->A);_FETCH();break;
        case (0x94<<4)|3: assert(false);break;
        case (0x94<<4)|4: assert(false);break;
        case (0x94<<4)|5: assert(false);break;
        case (0x94<<4)|6: assert(false);break;
        case (0x94<<4)|7: assert(false);break;
        case (0x94<<4)|8: assert(false);break;
        case (0x94<<4)|9: assert(false);break;
        case (0x94<<4)|10: assert(false);break;
        case (0x94<<4)|11: assert(false);break;
        case (0x94<<4)|12: assert(false);break;
        case (0x94<<4)|13: assert(false);break;
        case (0x94<<4)|14: assert(false);break;
        case (0x94<<4)|15: assert(false);break;
    /* BITA zp */
        case (0x95<<4)|0: _SA(c->PC++);break;
        case (0x95<<4)|1: _SA(_GD());break;
        case (0x95<<4)|2: _VF(false);_NZ(c->A&_GD());_FETCH();break;
        case (0x95<<4)|3: assert(false);break;
        case (0x95<<4)|4: assert(false);break;
        case (0x95<<4)|5: assert(false);break;
        case (0x95<<4)|6: assert(false);break;
        case (0x95<<4)|7: assert(false);break;
        case (0x95<<4)|8: assert(false);break;
        case (0x95<<4)|9: assert(false);break;
        case (0x95<<4)|10: assert(false);break;
        case (0x95<<4)|11: assert(false);break;
        case (0x95<<4)|12: assert(false);break;
        case (0x95<<4)|13: assert(false);break;
        case (0x95<<4)|14: assert(false);break;
        case (0x95<<4)|15: assert(false);break;
    /* LDAA zp */
        case (0x96<<4)|0: _SA(c->PC++);break;
        case (0x96<<4)|1: _SA(_GD());break;
        case (0x96<<4)|2: _VF(false);c->A=_GD();_NZ(c->A);_FETCH();break;
        case (0x96<<4)|3: assert(false);break;
        case (0x96<<4)|4: assert(false);break;
        case (0x96<<4)|5: assert(false);break;
        case (0x96<<4)|6: assert(false);break;
        case (0x96<<4)|7: assert(false);break;
        case (0x96<<4)|8: assert(false);break;
        case (0x96<<4)|9: assert(false);break;
        case (0x96<<4)|10: assert(false);break;
        case (0x96<<4)|11: assert(false);break;
        case (0x96<<4)|12: assert(false);break;
        case (0x96<<4)|13: assert(false);break;
        case (0x96<<4)|14: assert(false);break;
        case (0x96<<4)|15: assert(false);break;
    /* STAA zp */
        case (0x97<<4)|0: _SA(c->PC++);break;
        case (0x97<<4)|1: _SA(_GD());_VMA();break;
        case (0x97<<4)|2: _VF(false);_SD(c->A);_NZ(c->A);_WR();break;
        case (0x97<<4)|3: _FETCH();break;
        case (0x97<<4)|4: assert(false);break;
        case (0x97<<4)|5: assert(false);break;
        case (0x97<<4)|6: assert(false);break;
        case (0x97<<4)|7: assert(false);break;
        case (0x97<<4)|8: assert(false);break;
        case (0x97<<4)|9: assert(false);break;
        case (0x97<<4)|10: assert(false);break;
        case (0x97<<4)|11: assert(false);break;
        case (0x97<<4)|12: assert(false);break;
        case (0x97<<4)|13: assert(false);break;
        case (0x97<<4)|14: assert(false);break;
        case (0x97<<4)|15: assert(false);break;
    /* EORA zp */
        case (0x98<<4)|0: _SA(c->PC++);break;
        case (0x98<<4)|1: _SA(_GD());break;
        case (0x98<<4)|2: _VF(false);c->A^=_GD();_NZ(c->A);_FETCH();break;
        case (0x98<<4)|3: assert(false);break;
        case (0x98<<4)|4: assert(false);break;
        case (0x98<<4)|5: assert(false);break;
        case (0x98<<4)|6: assert(false);break;
        case (0x98<<4)|7: assert(false);break;
        case (0x98<<4)|8: assert(false);break;
        case (0x98<<4)|9: assert(false);break;
        case (0x98<<4)|10: assert(false);break;
        case (0x98<<4)|11: assert(false);break;
        case (0x98<<4)|12: assert(false);break;
        case (0x98<<4)|13: assert(false);break;
        case (0x98<<4)|14: assert(false);break;
        case (0x98<<4)|15: assert(false);break;
    /* ADCA zp */
        case (0x99<<4)|0: _SA(c->PC++);break;
        case (0x99<<4)|1: _SA(_GD());break;
        case (0x99<<4)|2: _mc6800_add(c, _GD(), true, true);_FETCH();break;
        case (0x99<<4)|3: assert(false);break;
        case (0x99<<4)|4: assert(false);break;
        case (0x99<<4)|5: assert(false);break;
        case (0x99<<4)|6: assert(false);break;
        case (0x99<<4)|7: assert(false);break;
        case (0x99<<4)|8: assert(false);break;
        case (0x99<<4)|9: assert(false);break;
        case (0x99<<4)|10: assert(false);break;
        case (0x99<<4)|11: assert(false);break;
        case (0x99<<4)|12: assert(false);break;
        case (0x99<<4)|13: assert(false);break;
        case (0x99<<4)|14: assert(false);break;
        case (0x99<<4)|15: assert(false);break;
    /* ORAA zp */
        case (0x9A<<4)|0: _SA(c->PC++);break;
        case (0x9A<<4)|1: _SA(_GD());break;
        case (0x9A<<4)|2: _VF(false);c->A|=_GD();_NZ(c->A);_FETCH();break;
        case (0x9A<<4)|3: assert(false);break;
        case (0x9A<<4)|4: assert(false);break;
        case (0x9A<<4)|5: assert(false);break;
        case (0x9A<<4)|6: assert(false);break;
        case (0x9A<<4)|7: assert(false);break;
        case (0x9A<<4)|8: assert(false);break;
        case (0x9A<<4)|9: assert(false);break;
        case (0x9A<<4)|10: assert(false);break;
        case (0x9A<<4)|11: assert(false);break;
        case (0x9A<<4)|12: assert(false);break;
        case (0x9A<<4)|13: assert(false);break;
        case (0x9A<<4)|14: assert(false);break;
        case (0x9A<<4)|15: assert(false);break;
    /* ADDA zp */
        case (0x9B<<4)|0: _SA(c->PC++);break;
        case (0x9B<<4)|1: _SA(_GD());break;
        case (0x9B<<4)|2: _mc6800_add(c, _GD(), true, false);_FETCH();break;
        case (0x9B<<4)|3: assert(false);break;
        case (0x9B<<4)|4: assert(false);break;
        case (0x9B<<4)|5: assert(false);break;
        case (0x9B<<4)|6: assert(false);break;
        case (0x9B<<4)|7: assert(false);break;
        case (0x9B<<4)|8: assert(false);break;
        case (0x9B<<4)|9: assert(false);break;
        case (0x9B<<4)|10: assert(false);break;
        case (0x9B<<4)|11: assert(false);break;
        case (0x9B<<4)|12: assert(false);break;
        case (0x9B<<4)|13: assert(false);break;
        case (0x9B<<4)|14: assert(false);break;
        case (0x9B<<4)|15: assert(false);break;
    /* CPX zp */
        case (0x9C<<4)|0: _SA(c->PC++);break;
        case (0x9C<<4)|1: _SA(_GD());break;
        case (0x9C<<4)|2: c->AD=_GD();c->AD<<=8;_SA(_GA()+1);break;
        case (0x9C<<4)|3: c->AD=c->AD|_GD();_mc6800_cpx(c, c->AD);_FETCH();break;
        case (0x9C<<4)|4: assert(false);break;
        case (0x9C<<4)|5: assert(false);break;
        case (0x9C<<4)|6: assert(false);break;
        case (0x9C<<4)|7: assert(false);break;
        case (0x9C<<4)|8: assert(false);break;
        case (0x9C<<4)|9: assert(false);break;
        case (0x9C<<4)|10: assert(false);break;
        case (0x9C<<4)|11: assert(false);break;
        case (0x9C<<4)|12: assert(false);break;
        case (0x9C<<4)|13: assert(false);break;
        case (0x9C<<4)|14: assert(false);break;
        case (0x9C<<4)|15: assert(false);break;
    /* NOP zp */
        case (0x9D<<4)|0: _SA(c->PC++);break;
        case (0x9D<<4)|1: _SA(_GD());break;
        case (0x9D<<4)|2: _FETCH();break;
        case (0x9D<<4)|3: assert(false);break;
        case (0x9D<<4)|4: assert(false);break;
        case (0x9D<<4)|5: assert(false);break;
        case (0x9D<<4)|6: assert(false);break;
        case (0x9D<<4)|7: assert(false);break;
        case (0x9D<<4)|8: assert(false);break;
        case (0x9D<<4)|9: assert(false);break;
        case (0x9D<<4)|10: assert(false);break;
        case (0x9D<<4)|11: assert(false);break;
        case (0x9D<<4)|12: assert(false);break;
        case (0x9D<<4)|13: assert(false);break;
        case (0x9D<<4)|14: assert(false);break;
        case (0x9D<<4)|15: assert(false);break;
    /* LDS zp */
        case (0x9E<<4)|0: _SA(c->PC++);break;
        case (0x9E<<4)|1: _SA(_GD());break;
        case (0x9E<<4)|2: c->SP=_GD();c->SP<<=8;_SA(_GA()+1);break;
        case (0x9E<<4)|3: _VF(false);c->SP|=_GD();_NZ16(c->SP);_FETCH();break;
        case (0x9E<<4)|4: assert(false);break;
        case (0x9E<<4)|5: assert(false);break;
        case (0x9E<<4)|6: assert(false);break;
        case (0x9E<<4)|7: assert(false);break;
        case (0x9E<<4)|8: assert(false);break;
        case (0x9E<<4)|9: assert(false);break;
        case (0x9E<<4)|10: assert(false);break;
        case (0x9E<<4)|11: assert(false);break;
        case (0x9E<<4)|12: assert(false);break;
        case (0x9E<<4)|13: assert(false);break;
        case (0x9E<<4)|14: assert(false);break;
        case (0x9E<<4)|15: assert(false);break;
    /* STS zp */
        case (0x9F<<4)|0: _SA(c->PC++);break;
        case (0x9F<<4)|1: _SA(_GD());_VMA();break;
        case (0x9F<<4)|2: _NZ16(c->SP);_SD(c->SP>>8);_WR();break;
        case (0x9F<<4)|3: _VF(false);_SA(_GA()+1);_SD(c->SP&0xFF);_WR();break;
        case (0x9F<<4)|4: _FETCH();break;
        case (0x9F<<4)|5: assert(false);break;
        case (0x9F<<4)|6: assert(false);break;
        case (0x9F<<4)|7: assert(false);break;
        case (0x9F<<4)|8: assert(false);break;
        case (0x9F<<4)|9: assert(false);break;
        case (0x9F<<4)|10: assert(false);break;
        case (0x9F<<4)|11: assert(false);break;
        case (0x9F<<4)|12: assert(false);break;
        case (0x9F<<4)|13: assert(false);break;
        case (0x9F<<4)|14: assert(false);break;
        case (0x9F<<4)|15: assert(false);break;
    /* SUBA (zp,X) */
        case (0xA0<<4)|0: _SA(c->PC++);break;
        case (0xA0<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xA0<<4)|2: _SA(c->AD);_VMA();break;
        case (0xA0<<4)|3: break;
        case (0xA0<<4)|4: _mc6800_sub(c, _GD(), true, false);_FETCH();break;
        case (0xA0<<4)|5: assert(false);break;
        case (0xA0<<4)|6: assert(false);break;
        case (0xA0<<4)|7: assert(false);break;
        case (0xA0<<4)|8: assert(false);break;
        case (0xA0<<4)|9: assert(false);break;
        case (0xA0<<4)|10: assert(false);break;
        case (0xA0<<4)|11: assert(false);break;
        case (0xA0<<4)|12: assert(false);break;
        case (0xA0<<4)|13: assert(false);break;
        case (0xA0<<4)|14: assert(false);break;
        case (0xA0<<4)|15: assert(false);break;
    /* CMPA (zp,X) */
        case (0xA1<<4)|0: _SA(c->PC++);break;
        case (0xA1<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xA1<<4)|2: _SA(c->AD);_VMA();break;
        case (0xA1<<4)|3: break;
        case (0xA1<<4)|4: _mc6800_cmp(c, _GD(), true);_FETCH();break;
        case (0xA1<<4)|5: assert(false);break;
        case (0xA1<<4)|6: assert(false);break;
        case (0xA1<<4)|7: assert(false);break;
        case (0xA1<<4)|8: assert(false);break;
        case (0xA1<<4)|9: assert(false);break;
        case (0xA1<<4)|10: assert(false);break;
        case (0xA1<<4)|11: assert(false);break;
        case (0xA1<<4)|12: assert(false);break;
        case (0xA1<<4)|13: assert(false);break;
        case (0xA1<<4)|14: assert(false);break;
        case (0xA1<<4)|15: assert(false);break;
    /* SBCA (zp,X) */
        case (0xA2<<4)|0: _SA(c->PC++);break;
        case (0xA2<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xA2<<4)|2: _SA(c->AD);_VMA();break;
        case (0xA2<<4)|3: break;
        case (0xA2<<4)|4: _mc6800_sub(c, _GD(), true, true);_FETCH();break;
        case (0xA2<<4)|5: assert(false);break;
        case (0xA2<<4)|6: assert(false);break;
        case (0xA2<<4)|7: assert(false);break;
        case (0xA2<<4)|8: assert(false);break;
        case (0xA2<<4)|9: assert(false);break;
        case (0xA2<<4)|10: assert(false);break;
        case (0xA2<<4)|11: assert(false);break;
        case (0xA2<<4)|12: assert(false);break;
        case (0xA2<<4)|13: assert(false);break;
        case (0xA2<<4)|14: assert(false);break;
        case (0xA2<<4)|15: assert(false);break;
    /* NOP  */
        case (0xA3<<4)|0: _VMA();break;
        case (0xA3<<4)|1: _FETCH();break;
        case (0xA3<<4)|2: assert(false);break;
        case (0xA3<<4)|3: assert(false);break;
        case (0xA3<<4)|4: assert(false);break;
        case (0xA3<<4)|5: assert(false);break;
        case (0xA3<<4)|6: assert(false);break;
        case (0xA3<<4)|7: assert(false);break;
        case (0xA3<<4)|8: assert(false);break;
        case (0xA3<<4)|9: assert(false);break;
        case (0xA3<<4)|10: assert(false);break;
        case (0xA3<<4)|11: assert(false);break;
        case (0xA3<<4)|12: assert(false);break;
        case (0xA3<<4)|13: assert(false);break;
        case (0xA3<<4)|14: assert(false);break;
        case (0xA3<<4)|15: assert(false);break;
    /* ANDA (zp,X) */
        case (0xA4<<4)|0: _SA(c->PC++);break;
        case (0xA4<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xA4<<4)|2: _SA(c->AD);_VMA();break;
        case (0xA4<<4)|3: break;
        case (0xA4<<4)|4: _VF(false);c->A&=_GD();_NZ(c->A);_FETCH();break;
        case (0xA4<<4)|5: assert(false);break;
        case (0xA4<<4)|6: assert(false);break;
        case (0xA4<<4)|7: assert(false);break;
        case (0xA4<<4)|8: assert(false);break;
        case (0xA4<<4)|9: assert(false);break;
        case (0xA4<<4)|10: assert(false);break;
        case (0xA4<<4)|11: assert(false);break;
        case (0xA4<<4)|12: assert(false);break;
        case (0xA4<<4)|13: assert(false);break;
        case (0xA4<<4)|14: assert(false);break;
        case (0xA4<<4)|15: assert(false);break;
    /* BITA (zp,X) */
        case (0xA5<<4)|0: _SA(c->PC++);break;
        case (0xA5<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xA5<<4)|2: _SA(c->AD);_VMA();break;
        case (0xA5<<4)|3: break;
        case (0xA5<<4)|4: _VF(false);_NZ(c->A&_GD());_FETCH();break;
        case (0xA5<<4)|5: assert(false);break;
        case (0xA5<<4)|6: assert(false);break;
        case (0xA5<<4)|7: assert(false);break;
        case (0xA5<<4)|8: assert(false);break;
        case (0xA5<<4)|9: assert(false);break;
        case (0xA5<<4)|10: assert(false);break;
        case (0xA5<<4)|11: assert(false);break;
        case (0xA5<<4)|12: assert(false);break;
        case (0xA5<<4)|13: assert(false);break;
        case (0xA5<<4)|14: assert(false);break;
        case (0xA5<<4)|15: assert(false);break;
    /* LDAA (zp,X) */
        case (0xA6<<4)|0: _SA(c->PC++);break;
        case (0xA6<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xA6<<4)|2: _SA(c->AD);_VMA();break;
        case (0xA6<<4)|3: break;
        case (0xA6<<4)|4: _VF(false);c->A=_GD();_NZ(c->A);_FETCH();break;
        case (0xA6<<4)|5: assert(false);break;
        case (0xA6<<4)|6: assert(false);break;
        case (0xA6<<4)|7: assert(false);break;
        case (0xA6<<4)|8: assert(false);break;
        case (0xA6<<4)|9: assert(false);break;
        case (0xA6<<4)|10: assert(false);break;
        case (0xA6<<4)|11: assert(false);break;
        case (0xA6<<4)|12: assert(false);break;
        case (0xA6<<4)|13: assert(false);break;
        case (0xA6<<4)|14: assert(false);break;
        case (0xA6<<4)|15: assert(false);break;
    /* STAA (zp,X) */
        case (0xA7<<4)|0: _SA(c->PC++);break;
        case (0xA7<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xA7<<4)|2: _SA(c->AD);_VMA();break;
        case (0xA7<<4)|3: _VMA();break;
        case (0xA7<<4)|4: _VF(false);_SD(c->A);_NZ(c->A);_WR();break;
        case (0xA7<<4)|5: _FETCH();break;
        case (0xA7<<4)|6: assert(false);break;
        case (0xA7<<4)|7: assert(false);break;
        case (0xA7<<4)|8: assert(false);break;
        case (0xA7<<4)|9: assert(false);break;
        case (0xA7<<4)|10: assert(false);break;
        case (0xA7<<4)|11: assert(false);break;
        case (0xA7<<4)|12: assert(false);break;
        case (0xA7<<4)|13: assert(false);break;
        case (0xA7<<4)|14: assert(false);break;
        case (0xA7<<4)|15: assert(false);break;
    /* EORA (zp,X) */
        case (0xA8<<4)|0: _SA(c->PC++);break;
        case (0xA8<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xA8<<4)|2: _SA(c->AD);_VMA();break;
        case (0xA8<<4)|3: break;
        case (0xA8<<4)|4: _VF(false);c->A^=_GD();_NZ(c->A);_FETCH();break;
        case (0xA8<<4)|5: assert(false);break;
        case (0xA8<<4)|6: assert(false);break;
        case (0xA8<<4)|7: assert(false);break;
        case (0xA8<<4)|8: assert(false);break;
        case (0xA8<<4)|9: assert(false);break;
        case (0xA8<<4)|10: assert(false);break;
        case (0xA8<<4)|11: assert(false);break;
        case (0xA8<<4)|12: assert(false);break;
        case (0xA8<<4)|13: assert(false);break;
        case (0xA8<<4)|14: assert(false);break;
        case (0xA8<<4)|15: assert(false);break;
    /* ADCA (zp,X) */
        case (0xA9<<4)|0: _SA(c->PC++);break;
        case (0xA9<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xA9<<4)|2: _SA(c->AD);_VMA();break;
        case (0xA9<<4)|3: break;
        case (0xA9<<4)|4: _mc6800_add(c, _GD(), true, true);_FETCH();break;
        case (0xA9<<4)|5: assert(false);break;
        case (0xA9<<4)|6: assert(false);break;
        case (0xA9<<4)|7: assert(false);break;
        case (0xA9<<4)|8: assert(false);break;
        case (0xA9<<4)|9: assert(false);break;
        case (0xA9<<4)|10: assert(false);break;
        case (0xA9<<4)|11: assert(false);break;
        case (0xA9<<4)|12: assert(false);break;
        case (0xA9<<4)|13: assert(false);break;
        case (0xA9<<4)|14: assert(false);break;
        case (0xA9<<4)|15: assert(false);break;
    /* ORAA (zp,X) */
        case (0xAA<<4)|0: _SA(c->PC++);break;
        case (0xAA<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xAA<<4)|2: _SA(c->AD);_VMA();break;
        case (0xAA<<4)|3: break;
        case (0xAA<<4)|4: _VF(false);c->A|=_GD();_NZ(c->A);_FETCH();break;
        case (0xAA<<4)|5: assert(false);break;
        case (0xAA<<4)|6: assert(false);break;
        case (0xAA<<4)|7: assert(false);break;
        case (0xAA<<4)|8: assert(false);break;
        case (0xAA<<4)|9: assert(false);break;
        case (0xAA<<4)|10: assert(false);break;
        case (0xAA<<4)|11: assert(false);break;
        case (0xAA<<4)|12: assert(false);break;
        case (0xAA<<4)|13: assert(false);break;
        case (0xAA<<4)|14: assert(false);break;
        case (0xAA<<4)|15: assert(false);break;
    /* ADDA (zp,X) */
        case (0xAB<<4)|0: _SA(c->PC++);break;
        case (0xAB<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xAB<<4)|2: _SA(c->AD);_VMA();break;
        case (0xAB<<4)|3: break;
        case (0xAB<<4)|4: _mc6800_add(c, _GD(), true, false);_FETCH();break;
        case (0xAB<<4)|5: assert(false);break;
        case (0xAB<<4)|6: assert(false);break;
        case (0xAB<<4)|7: assert(false);break;
        case (0xAB<<4)|8: assert(false);break;
        case (0xAB<<4)|9: assert(false);break;
        case (0xAB<<4)|10: assert(false);break;
        case (0xAB<<4)|11: assert(false);break;
        case (0xAB<<4)|12: assert(false);break;
        case (0xAB<<4)|13: assert(false);break;
        case (0xAB<<4)|14: assert(false);break;
        case (0xAB<<4)|15: assert(false);break;
    /* CPX (zp,X) */
        case (0xAC<<4)|0: _SA(c->PC++);break;
        case (0xAC<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xAC<<4)|2: _SA(c->AD);_VMA();break;
        case (0xAC<<4)|3: break;
        case (0xAC<<4)|4: c->AD=_GD();c->AD<<=8;_SA(_GA()+1);break;
        case (0xAC<<4)|5: c->AD=c->AD|_GD();_mc6800_cpx(c, c->AD);_FETCH();break;
        case (0xAC<<4)|6: assert(false);break;
        case (0xAC<<4)|7: assert(false);break;
        case (0xAC<<4)|8: assert(false);break;
        case (0xAC<<4)|9: assert(false);break;
        case (0xAC<<4)|10: assert(false);break;
        case (0xAC<<4)|11: assert(false);break;
        case (0xAC<<4)|12: assert(false);break;
        case (0xAC<<4)|13: assert(false);break;
        case (0xAC<<4)|14: assert(false);break;
        case (0xAC<<4)|15: assert(false);break;
    /* NOP (zp,X) */
        case (0xAD<<4)|0: _SA(c->PC++);break;
        case (0xAD<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xAD<<4)|2: _SA(c->AD);_VMA();break;
        case (0xAD<<4)|3: break;
        case (0xAD<<4)|4: _FETCH();break;
        case (0xAD<<4)|5: assert(false);break;
        case (0xAD<<4)|6: assert(false);break;
        case (0xAD<<4)|7: assert(false);break;
        case (0xAD<<4)|8: assert(false);break;
        case (0xAD<<4)|9: assert(false);break;
        case (0xAD<<4)|10: assert(false);break;
        case (0xAD<<4)|11: assert(false);break;
        case (0xAD<<4)|12: assert(false);break;
        case (0xAD<<4)|13: assert(false);break;
        case (0xAD<<4)|14: assert(false);break;
        case (0xAD<<4)|15: assert(false);break;
    /* LDS (zp,X) */
        case (0xAE<<4)|0: _SA(c->PC++);break;
        case (0xAE<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xAE<<4)|2: _SA(c->AD);_VMA();break;
        case (0xAE<<4)|3: break;
        case (0xAE<<4)|4: c->SP=_GD();c->SP<<=8;_SA(_GA()+1);break;
        case (0xAE<<4)|5: _VF(false);c->SP|=_GD();_NZ16(c->SP);_FETCH();break;
        case (0xAE<<4)|6: assert(false);break;
        case (0xAE<<4)|7: assert(false);break;
        case (0xAE<<4)|8: assert(false);break;
        case (0xAE<<4)|9: assert(false);break;
        case (0xAE<<4)|10: assert(false);break;
        case (0xAE<<4)|11: assert(false);break;
        case (0xAE<<4)|12: assert(false);break;
        case (0xAE<<4)|13: assert(false);break;
        case (0xAE<<4)|14: assert(false);break;
        case (0xAE<<4)|15: assert(false);break;
    /* STS (zp,X) */
        case (0xAF<<4)|0: _SA(c->PC++);break;
        case (0xAF<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xAF<<4)|2: _SA(c->AD);_VMA();break;
        case (0xAF<<4)|3: _VMA();break;
        case (0xAF<<4)|4: _NZ16(c->SP);_SD(c->SP>>8);_WR();break;
        case (0xAF<<4)|5: _VF(false);_SA(_GA()+1);_SD(c->SP&0xFF);_WR();break;
        case (0xAF<<4)|6: _FETCH();break;
        case (0xAF<<4)|7: assert(false);break;
        case (0xAF<<4)|8: assert(false);break;
        case (0xAF<<4)|9: assert(false);break;
        case (0xAF<<4)|10: assert(false);break;
        case (0xAF<<4)|11: assert(false);break;
        case (0xAF<<4)|12: assert(false);break;
        case (0xAF<<4)|13: assert(false);break;
        case (0xAF<<4)|14: assert(false);break;
        case (0xAF<<4)|15: assert(false);break;
    /* SUBA abs */
        case (0xB0<<4)|0: _SA(c->PC++);break;
        case (0xB0<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xB0<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xB0<<4)|3: _mc6800_sub(c, _GD(), true, false);_FETCH();break;
        case (0xB0<<4)|4: assert(false);break;
        case (0xB0<<4)|5: assert(false);break;
        case (0xB0<<4)|6: assert(false);break;
        case (0xB0<<4)|7: assert(false);break;
        case (0xB0<<4)|8: assert(false);break;
        case (0xB0<<4)|9: assert(false);break;
        case (0xB0<<4)|10: assert(false);break;
        case (0xB0<<4)|11: assert(false);break;
        case (0xB0<<4)|12: assert(false);break;
        case (0xB0<<4)|13: assert(false);break;
        case (0xB0<<4)|14: assert(false);break;
        case (0xB0<<4)|15: assert(false);break;
    /* CMPA abs */
        case (0xB1<<4)|0: _SA(c->PC++);break;
        case (0xB1<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xB1<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xB1<<4)|3: _mc6800_cmp(c, _GD(), true);_FETCH();break;
        case (0xB1<<4)|4: assert(false);break;
        case (0xB1<<4)|5: assert(false);break;
        case (0xB1<<4)|6: assert(false);break;
        case (0xB1<<4)|7: assert(false);break;
        case (0xB1<<4)|8: assert(false);break;
        case (0xB1<<4)|9: assert(false);break;
        case (0xB1<<4)|10: assert(false);break;
        case (0xB1<<4)|11: assert(false);break;
        case (0xB1<<4)|12: assert(false);break;
        case (0xB1<<4)|13: assert(false);break;
        case (0xB1<<4)|14: assert(false);break;
        case (0xB1<<4)|15: assert(false);break;
    /* SBCA abs */
        case (0xB2<<4)|0: _SA(c->PC++);break;
        case (0xB2<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xB2<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xB2<<4)|3: _mc6800_sub(c, _GD(), true, true);_FETCH();break;
        case (0xB2<<4)|4: assert(false);break;
        case (0xB2<<4)|5: assert(false);break;
        case (0xB2<<4)|6: assert(false);break;
        case (0xB2<<4)|7: assert(false);break;
        case (0xB2<<4)|8: assert(false);break;
        case (0xB2<<4)|9: assert(false);break;
        case (0xB2<<4)|10: assert(false);break;
        case (0xB2<<4)|11: assert(false);break;
        case (0xB2<<4)|12: assert(false);break;
        case (0xB2<<4)|13: assert(false);break;
        case (0xB2<<4)|14: assert(false);break;
        case (0xB2<<4)|15: assert(false);break;
    /* NOP  */
        case (0xB3<<4)|0: _VMA();break;
        case (0xB3<<4)|1: _FETCH();break;
        case (0xB3<<4)|2: assert(false);break;
        case (0xB3<<4)|3: assert(false);break;
        case (0xB3<<4)|4: assert(false);break;
        case (0xB3<<4)|5: assert(false);break;
        case (0xB3<<4)|6: assert(false);break;
        case (0xB3<<4)|7: assert(false);break;
        case (0xB3<<4)|8: assert(false);break;
        case (0xB3<<4)|9: assert(false);break;
        case (0xB3<<4)|10: assert(false);break;
        case (0xB3<<4)|11: assert(false);break;
        case (0xB3<<4)|12: assert(false);break;
        case (0xB3<<4)|13: assert(false);break;
        case (0xB3<<4)|14: assert(false);break;
        case (0xB3<<4)|15: assert(false);break;
    /* ANDA abs */
        case (0xB4<<4)|0: _SA(c->PC++);break;
        case (0xB4<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xB4<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xB4<<4)|3: _VF(false);c->A&=_GD();_NZ(c->A);_FETCH();break;
        case (0xB4<<4)|4: assert(false);break;
        case (0xB4<<4)|5: assert(false);break;
        case (0xB4<<4)|6: assert(false);break;
        case (0xB4<<4)|7: assert(false);break;
        case (0xB4<<4)|8: assert(false);break;
        case (0xB4<<4)|9: assert(false);break;
        case (0xB4<<4)|10: assert(false);break;
        case (0xB4<<4)|11: assert(false);break;
        case (0xB4<<4)|12: assert(false);break;
        case (0xB4<<4)|13: assert(false);break;
        case (0xB4<<4)|14: assert(false);break;
        case (0xB4<<4)|15: assert(false);break;
    /* BITA abs */
        case (0xB5<<4)|0: _SA(c->PC++);break;
        case (0xB5<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xB5<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xB5<<4)|3: _VF(false);_NZ(c->A&_GD());_FETCH();break;
        case (0xB5<<4)|4: assert(false);break;
        case (0xB5<<4)|5: assert(false);break;
        case (0xB5<<4)|6: assert(false);break;
        case (0xB5<<4)|7: assert(false);break;
        case (0xB5<<4)|8: assert(false);break;
        case (0xB5<<4)|9: assert(false);break;
        case (0xB5<<4)|10: assert(false);break;
        case (0xB5<<4)|11: assert(false);break;
        case (0xB5<<4)|12: assert(false);break;
        case (0xB5<<4)|13: assert(false);break;
        case (0xB5<<4)|14: assert(false);break;
        case (0xB5<<4)|15: assert(false);break;
    /* LDAA abs */
        case (0xB6<<4)|0: _SA(c->PC++);break;
        case (0xB6<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xB6<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xB6<<4)|3: _VF(false);c->A=_GD();_NZ(c->A);_FETCH();break;
        case (0xB6<<4)|4: assert(false);break;
        case (0xB6<<4)|5: assert(false);break;
        case (0xB6<<4)|6: assert(false);break;
        case (0xB6<<4)|7: assert(false);break;
        case (0xB6<<4)|8: assert(false);break;
        case (0xB6<<4)|9: assert(false);break;
        case (0xB6<<4)|10: assert(false);break;
        case (0xB6<<4)|11: assert(false);break;
        case (0xB6<<4)|12: assert(false);break;
        case (0xB6<<4)|13: assert(false);break;
        case (0xB6<<4)|14: assert(false);break;
        case (0xB6<<4)|15: assert(false);break;
    /* STAA abs */
        case (0xB7<<4)|0: _SA(c->PC++);break;
        case (0xB7<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xB7<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0xB7<<4)|3: _VF(false);_SD(c->A);_NZ(c->A);_WR();break;
        case (0xB7<<4)|4: _FETCH();break;
        case (0xB7<<4)|5: assert(false);break;
        case (0xB7<<4)|6: assert(false);break;
        case (0xB7<<4)|7: assert(false);break;
        case (0xB7<<4)|8: assert(false);break;
        case (0xB7<<4)|9: assert(false);break;
        case (0xB7<<4)|10: assert(false);break;
        case (0xB7<<4)|11: assert(false);break;
        case (0xB7<<4)|12: assert(false);break;
        case (0xB7<<4)|13: assert(false);break;
        case (0xB7<<4)|14: assert(false);break;
        case (0xB7<<4)|15: assert(false);break;
    /* EORA abs */
        case (0xB8<<4)|0: _SA(c->PC++);break;
        case (0xB8<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xB8<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xB8<<4)|3: _VF(false);c->A^=_GD();_NZ(c->A);_FETCH();break;
        case (0xB8<<4)|4: assert(false);break;
        case (0xB8<<4)|5: assert(false);break;
        case (0xB8<<4)|6: assert(false);break;
        case (0xB8<<4)|7: assert(false);break;
        case (0xB8<<4)|8: assert(false);break;
        case (0xB8<<4)|9: assert(false);break;
        case (0xB8<<4)|10: assert(false);break;
        case (0xB8<<4)|11: assert(false);break;
        case (0xB8<<4)|12: assert(false);break;
        case (0xB8<<4)|13: assert(false);break;
        case (0xB8<<4)|14: assert(false);break;
        case (0xB8<<4)|15: assert(false);break;
    /* ADCA abs */
        case (0xB9<<4)|0: _SA(c->PC++);break;
        case (0xB9<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xB9<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xB9<<4)|3: _mc6800_add(c, _GD(), true, true);_FETCH();break;
        case (0xB9<<4)|4: assert(false);break;
        case (0xB9<<4)|5: assert(false);break;
        case (0xB9<<4)|6: assert(false);break;
        case (0xB9<<4)|7: assert(false);break;
        case (0xB9<<4)|8: assert(false);break;
        case (0xB9<<4)|9: assert(false);break;
        case (0xB9<<4)|10: assert(false);break;
        case (0xB9<<4)|11: assert(false);break;
        case (0xB9<<4)|12: assert(false);break;
        case (0xB9<<4)|13: assert(false);break;
        case (0xB9<<4)|14: assert(false);break;
        case (0xB9<<4)|15: assert(false);break;
    /* ORAA abs */
        case (0xBA<<4)|0: _SA(c->PC++);break;
        case (0xBA<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xBA<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xBA<<4)|3: _VF(false);c->A|=_GD();_NZ(c->A);_FETCH();break;
        case (0xBA<<4)|4: assert(false);break;
        case (0xBA<<4)|5: assert(false);break;
        case (0xBA<<4)|6: assert(false);break;
        case (0xBA<<4)|7: assert(false);break;
        case (0xBA<<4)|8: assert(false);break;
        case (0xBA<<4)|9: assert(false);break;
        case (0xBA<<4)|10: assert(false);break;
        case (0xBA<<4)|11: assert(false);break;
        case (0xBA<<4)|12: assert(false);break;
        case (0xBA<<4)|13: assert(false);break;
        case (0xBA<<4)|14: assert(false);break;
        case (0xBA<<4)|15: assert(false);break;
    /* ADDA abs */
        case (0xBB<<4)|0: _SA(c->PC++);break;
        case (0xBB<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xBB<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xBB<<4)|3: _mc6800_add(c, _GD(), true, false);_FETCH();break;
        case (0xBB<<4)|4: assert(false);break;
        case (0xBB<<4)|5: assert(false);break;
        case (0xBB<<4)|6: assert(false);break;
        case (0xBB<<4)|7: assert(false);break;
        case (0xBB<<4)|8: assert(false);break;
        case (0xBB<<4)|9: assert(false);break;
        case (0xBB<<4)|10: assert(false);break;
        case (0xBB<<4)|11: assert(false);break;
        case (0xBB<<4)|12: assert(false);break;
        case (0xBB<<4)|13: assert(false);break;
        case (0xBB<<4)|14: assert(false);break;
        case (0xBB<<4)|15: assert(false);break;
    /* CPX abs */
        case (0xBC<<4)|0: _SA(c->PC++);break;
        case (0xBC<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xBC<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xBC<<4)|3: c->AD=_GD();c->AD<<=8;_SA(_GA()+1);break;
        case (0xBC<<4)|4: c->AD=c->AD|_GD();_mc6800_cpx(c, c->AD);_FETCH();break;
        case (0xBC<<4)|5: assert(false);break;
        case (0xBC<<4)|6: assert(false);break;
        case (0xBC<<4)|7: assert(false);break;
        case (0xBC<<4)|8: assert(false);break;
        case (0xBC<<4)|9: assert(false);break;
        case (0xBC<<4)|10: assert(false);break;
        case (0xBC<<4)|11: assert(false);break;
        case (0xBC<<4)|12: assert(false);break;
        case (0xBC<<4)|13: assert(false);break;
        case (0xBC<<4)|14: assert(false);break;
        case (0xBC<<4)|15: assert(false);break;
    /* NOP abs */
        case (0xBD<<4)|0: _SA(c->PC++);break;
        case (0xBD<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xBD<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xBD<<4)|3: _FETCH();break;
        case (0xBD<<4)|4: assert(false);break;
        case (0xBD<<4)|5: assert(false);break;
        case (0xBD<<4)|6: assert(false);break;
        case (0xBD<<4)|7: assert(false);break;
        case (0xBD<<4)|8: assert(false);break;
        case (0xBD<<4)|9: assert(false);break;
        case (0xBD<<4)|10: assert(false);break;
        case (0xBD<<4)|11: assert(false);break;
        case (0xBD<<4)|12: assert(false);break;
        case (0xBD<<4)|13: assert(false);break;
        case (0xBD<<4)|14: assert(false);break;
        case (0xBD<<4)|15: assert(false);break;
    /* LDS abs */
        case (0xBE<<4)|0: _SA(c->PC++);break;
        case (0xBE<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xBE<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xBE<<4)|3: c->SP=_GD();c->SP<<=8;_SA(_GA()+1);break;
        case (0xBE<<4)|4: _VF(false);c->SP|=_GD();_NZ16(c->SP);_FETCH();break;
        case (0xBE<<4)|5: assert(false);break;
        case (0xBE<<4)|6: assert(false);break;
        case (0xBE<<4)|7: assert(false);break;
        case (0xBE<<4)|8: assert(false);break;
        case (0xBE<<4)|9: assert(false);break;
        case (0xBE<<4)|10: assert(false);break;
        case (0xBE<<4)|11: assert(false);break;
        case (0xBE<<4)|12: assert(false);break;
        case (0xBE<<4)|13: assert(false);break;
        case (0xBE<<4)|14: assert(false);break;
        case (0xBE<<4)|15: assert(false);break;
    /* STS abs */
        case (0xBF<<4)|0: _SA(c->PC++);break;
        case (0xBF<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xBF<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0xBF<<4)|3: _NZ16(c->SP);_SD(c->SP>>8);_WR();break;
        case (0xBF<<4)|4: _VF(false);_SA(_GA()+1);_SD(c->SP&0xFF);_WR();break;
        case (0xBF<<4)|5: _FETCH();break;
        case (0xBF<<4)|6: assert(false);break;
        case (0xBF<<4)|7: assert(false);break;
        case (0xBF<<4)|8: assert(false);break;
        case (0xBF<<4)|9: assert(false);break;
        case (0xBF<<4)|10: assert(false);break;
        case (0xBF<<4)|11: assert(false);break;
        case (0xBF<<4)|12: assert(false);break;
        case (0xBF<<4)|13: assert(false);break;
        case (0xBF<<4)|14: assert(false);break;
        case (0xBF<<4)|15: assert(false);break;
    /* SUBB # */
        case (0xC0<<4)|0: _SA(c->PC++);break;
        case (0xC0<<4)|1: _mc6800_sub(c, _GD(), false, false);_FETCH();break;
        case (0xC0<<4)|2: assert(false);break;
        case (0xC0<<4)|3: assert(false);break;
        case (0xC0<<4)|4: assert(false);break;
        case (0xC0<<4)|5: assert(false);break;
        case (0xC0<<4)|6: assert(false);break;
        case (0xC0<<4)|7: assert(false);break;
        case (0xC0<<4)|8: assert(false);break;
        case (0xC0<<4)|9: assert(false);break;
        case (0xC0<<4)|10: assert(false);break;
        case (0xC0<<4)|11: assert(false);break;
        case (0xC0<<4)|12: assert(false);break;
        case (0xC0<<4)|13: assert(false);break;
        case (0xC0<<4)|14: assert(false);break;
        case (0xC0<<4)|15: assert(false);break;
    /* CMPB # */
        case (0xC1<<4)|0: _SA(c->PC++);break;
        case (0xC1<<4)|1: _mc6800_cmp(c, _GD(), false);_FETCH();break;
        case (0xC1<<4)|2: assert(false);break;
        case (0xC1<<4)|3: assert(false);break;
        case (0xC1<<4)|4: assert(false);break;
        case (0xC1<<4)|5: assert(false);break;
        case (0xC1<<4)|6: assert(false);break;
        case (0xC1<<4)|7: assert(false);break;
        case (0xC1<<4)|8: assert(false);break;
        case (0xC1<<4)|9: assert(false);break;
        case (0xC1<<4)|10: assert(false);break;
        case (0xC1<<4)|11: assert(false);break;
        case (0xC1<<4)|12: assert(false);break;
        case (0xC1<<4)|13: assert(false);break;
        case (0xC1<<4)|14: assert(false);break;
        case (0xC1<<4)|15: assert(false);break;
    /* SBCB # */
        case (0xC2<<4)|0: _SA(c->PC++);break;
        case (0xC2<<4)|1: _mc6800_sub(c, _GD(), false, true);_FETCH();break;
        case (0xC2<<4)|2: assert(false);break;
        case (0xC2<<4)|3: assert(false);break;
        case (0xC2<<4)|4: assert(false);break;
        case (0xC2<<4)|5: assert(false);break;
        case (0xC2<<4)|6: assert(false);break;
        case (0xC2<<4)|7: assert(false);break;
        case (0xC2<<4)|8: assert(false);break;
        case (0xC2<<4)|9: assert(false);break;
        case (0xC2<<4)|10: assert(false);break;
        case (0xC2<<4)|11: assert(false);break;
        case (0xC2<<4)|12: assert(false);break;
        case (0xC2<<4)|13: assert(false);break;
        case (0xC2<<4)|14: assert(false);break;
        case (0xC2<<4)|15: assert(false);break;
    /* NOP  */
        case (0xC3<<4)|0: _VMA();break;
        case (0xC3<<4)|1: _FETCH();break;
        case (0xC3<<4)|2: assert(false);break;
        case (0xC3<<4)|3: assert(false);break;
        case (0xC3<<4)|4: assert(false);break;
        case (0xC3<<4)|5: assert(false);break;
        case (0xC3<<4)|6: assert(false);break;
        case (0xC3<<4)|7: assert(false);break;
        case (0xC3<<4)|8: assert(false);break;
        case (0xC3<<4)|9: assert(false);break;
        case (0xC3<<4)|10: assert(false);break;
        case (0xC3<<4)|11: assert(false);break;
        case (0xC3<<4)|12: assert(false);break;
        case (0xC3<<4)|13: assert(false);break;
        case (0xC3<<4)|14: assert(false);break;
        case (0xC3<<4)|15: assert(false);break;
    /* ANDB # */
        case (0xC4<<4)|0: _SA(c->PC++);break;
        case (0xC4<<4)|1: _VF(false);c->B&=_GD();_NZ(c->B);_FETCH();break;
        case (0xC4<<4)|2: assert(false);break;
        case (0xC4<<4)|3: assert(false);break;
        case (0xC4<<4)|4: assert(false);break;
        case (0xC4<<4)|5: assert(false);break;
        case (0xC4<<4)|6: assert(false);break;
        case (0xC4<<4)|7: assert(false);break;
        case (0xC4<<4)|8: assert(false);break;
        case (0xC4<<4)|9: assert(false);break;
        case (0xC4<<4)|10: assert(false);break;
        case (0xC4<<4)|11: assert(false);break;
        case (0xC4<<4)|12: assert(false);break;
        case (0xC4<<4)|13: assert(false);break;
        case (0xC4<<4)|14: assert(false);break;
        case (0xC4<<4)|15: assert(false);break;
    /* BITB # */
        case (0xC5<<4)|0: _SA(c->PC++);break;
        case (0xC5<<4)|1: _VF(false);_NZ(c->B&_GD());_FETCH();break;
        case (0xC5<<4)|2: assert(false);break;
        case (0xC5<<4)|3: assert(false);break;
        case (0xC5<<4)|4: assert(false);break;
        case (0xC5<<4)|5: assert(false);break;
        case (0xC5<<4)|6: assert(false);break;
        case (0xC5<<4)|7: assert(false);break;
        case (0xC5<<4)|8: assert(false);break;
        case (0xC5<<4)|9: assert(false);break;
        case (0xC5<<4)|10: assert(false);break;
        case (0xC5<<4)|11: assert(false);break;
        case (0xC5<<4)|12: assert(false);break;
        case (0xC5<<4)|13: assert(false);break;
        case (0xC5<<4)|14: assert(false);break;
        case (0xC5<<4)|15: assert(false);break;
    /* LDAB # */
        case (0xC6<<4)|0: _SA(c->PC++);break;
        case (0xC6<<4)|1: _VF(false);c->B=_GD();_NZ(c->B);_FETCH();break;
        case (0xC6<<4)|2: assert(false);break;
        case (0xC6<<4)|3: assert(false);break;
        case (0xC6<<4)|4: assert(false);break;
        case (0xC6<<4)|5: assert(false);break;
        case (0xC6<<4)|6: assert(false);break;
        case (0xC6<<4)|7: assert(false);break;
        case (0xC6<<4)|8: assert(false);break;
        case (0xC6<<4)|9: assert(false);break;
        case (0xC6<<4)|10: assert(false);break;
        case (0xC6<<4)|11: assert(false);break;
        case (0xC6<<4)|12: assert(false);break;
        case (0xC6<<4)|13: assert(false);break;
        case (0xC6<<4)|14: assert(false);break;
        case (0xC6<<4)|15: assert(false);break;
    /* NOP  */
        case (0xC7<<4)|0: _VMA();break;
        case (0xC7<<4)|1: _FETCH();break;
        case (0xC7<<4)|2: assert(false);break;
        case (0xC7<<4)|3: assert(false);break;
        case (0xC7<<4)|4: assert(false);break;
        case (0xC7<<4)|5: assert(false);break;
        case (0xC7<<4)|6: assert(false);break;
        case (0xC7<<4)|7: assert(false);break;
        case (0xC7<<4)|8: assert(false);break;
        case (0xC7<<4)|9: assert(false);break;
        case (0xC7<<4)|10: assert(false);break;
        case (0xC7<<4)|11: assert(false);break;
        case (0xC7<<4)|12: assert(false);break;
        case (0xC7<<4)|13: assert(false);break;
        case (0xC7<<4)|14: assert(false);break;
        case (0xC7<<4)|15: assert(false);break;
    /* EORB # */
        case (0xC8<<4)|0: _SA(c->PC++);break;
        case (0xC8<<4)|1: _VF(false);c->B^=_GD();_NZ(c->B);_FETCH();break;
        case (0xC8<<4)|2: assert(false);break;
        case (0xC8<<4)|3: assert(false);break;
        case (0xC8<<4)|4: assert(false);break;
        case (0xC8<<4)|5: assert(false);break;
        case (0xC8<<4)|6: assert(false);break;
        case (0xC8<<4)|7: assert(false);break;
        case (0xC8<<4)|8: assert(false);break;
        case (0xC8<<4)|9: assert(false);break;
        case (0xC8<<4)|10: assert(false);break;
        case (0xC8<<4)|11: assert(false);break;
        case (0xC8<<4)|12: assert(false);break;
        case (0xC8<<4)|13: assert(false);break;
        case (0xC8<<4)|14: assert(false);break;
        case (0xC8<<4)|15: assert(false);break;
    /* ADCB # */
        case (0xC9<<4)|0: _SA(c->PC++);break;
        case (0xC9<<4)|1: _mc6800_add(c, _GD(), false, true);_FETCH();break;
        case (0xC9<<4)|2: assert(false);break;
        case (0xC9<<4)|3: assert(false);break;
        case (0xC9<<4)|4: assert(false);break;
        case (0xC9<<4)|5: assert(false);break;
        case (0xC9<<4)|6: assert(false);break;
        case (0xC9<<4)|7: assert(false);break;
        case (0xC9<<4)|8: assert(false);break;
        case (0xC9<<4)|9: assert(false);break;
        case (0xC9<<4)|10: assert(false);break;
        case (0xC9<<4)|11: assert(false);break;
        case (0xC9<<4)|12: assert(false);break;
        case (0xC9<<4)|13: assert(false);break;
        case (0xC9<<4)|14: assert(false);break;
        case (0xC9<<4)|15: assert(false);break;
    /* ORAB # */
        case (0xCA<<4)|0: _SA(c->PC++);break;
        case (0xCA<<4)|1: _VF(false);c->B|=_GD();_NZ(c->B);_FETCH();break;
        case (0xCA<<4)|2: assert(false);break;
        case (0xCA<<4)|3: assert(false);break;
        case (0xCA<<4)|4: assert(false);break;
        case (0xCA<<4)|5: assert(false);break;
        case (0xCA<<4)|6: assert(false);break;
        case (0xCA<<4)|7: assert(false);break;
        case (0xCA<<4)|8: assert(false);break;
        case (0xCA<<4)|9: assert(false);break;
        case (0xCA<<4)|10: assert(false);break;
        case (0xCA<<4)|11: assert(false);break;
        case (0xCA<<4)|12: assert(false);break;
        case (0xCA<<4)|13: assert(false);break;
        case (0xCA<<4)|14: assert(false);break;
        case (0xCA<<4)|15: assert(false);break;
    /* ADDB # */
        case (0xCB<<4)|0: _SA(c->PC++);break;
        case (0xCB<<4)|1: _mc6800_add(c, _GD(), false, false);_FETCH();break;
        case (0xCB<<4)|2: assert(false);break;
        case (0xCB<<4)|3: assert(false);break;
        case (0xCB<<4)|4: assert(false);break;
        case (0xCB<<4)|5: assert(false);break;
        case (0xCB<<4)|6: assert(false);break;
        case (0xCB<<4)|7: assert(false);break;
        case (0xCB<<4)|8: assert(false);break;
        case (0xCB<<4)|9: assert(false);break;
        case (0xCB<<4)|10: assert(false);break;
        case (0xCB<<4)|11: assert(false);break;
        case (0xCB<<4)|12: assert(false);break;
        case (0xCB<<4)|13: assert(false);break;
        case (0xCB<<4)|14: assert(false);break;
        case (0xCB<<4)|15: assert(false);break;
    /* CPX  */
        case (0xCC<<4)|0: c->AD=_GD();c->AD<<=8;_SA(_GA()+1);break;
        case (0xCC<<4)|1: c->AD=c->AD|_GD();_mc6800_cpx(c, c->AD);_VMA();break;
        case (0xCC<<4)|2: _FETCH();break;
        case (0xCC<<4)|3: assert(false);break;
        case (0xCC<<4)|4: assert(false);break;
        case (0xCC<<4)|5: assert(false);break;
        case (0xCC<<4)|6: assert(false);break;
        case (0xCC<<4)|7: assert(false);break;
        case (0xCC<<4)|8: assert(false);break;
        case (0xCC<<4)|9: assert(false);break;
        case (0xCC<<4)|10: assert(false);break;
        case (0xCC<<4)|11: assert(false);break;
        case (0xCC<<4)|12: assert(false);break;
        case (0xCC<<4)|13: assert(false);break;
        case (0xCC<<4)|14: assert(false);break;
        case (0xCC<<4)|15: assert(false);break;
    /* NOP  */
        case (0xCD<<4)|0: _VMA();break;
        case (0xCD<<4)|1: _FETCH();break;
        case (0xCD<<4)|2: assert(false);break;
        case (0xCD<<4)|3: assert(false);break;
        case (0xCD<<4)|4: assert(false);break;
        case (0xCD<<4)|5: assert(false);break;
        case (0xCD<<4)|6: assert(false);break;
        case (0xCD<<4)|7: assert(false);break;
        case (0xCD<<4)|8: assert(false);break;
        case (0xCD<<4)|9: assert(false);break;
        case (0xCD<<4)|10: assert(false);break;
        case (0xCD<<4)|11: assert(false);break;
        case (0xCD<<4)|12: assert(false);break;
        case (0xCD<<4)|13: assert(false);break;
        case (0xCD<<4)|14: assert(false);break;
        case (0xCD<<4)|15: assert(false);break;
    /* LDX #16 */
        case (0xCE<<4)|0: _SA(c->PC++);c->PC++;break;
        case (0xCE<<4)|1: c->IX=_GD();c->IX<<=8;_SA(_GA()+1);break;
        case (0xCE<<4)|2: _VF(false);c->IX|=_GD();_NZ16(c->IX);_FETCH();break;
        case (0xCE<<4)|3: assert(false);break;
        case (0xCE<<4)|4: assert(false);break;
        case (0xCE<<4)|5: assert(false);break;
        case (0xCE<<4)|6: assert(false);break;
        case (0xCE<<4)|7: assert(false);break;
        case (0xCE<<4)|8: assert(false);break;
        case (0xCE<<4)|9: assert(false);break;
        case (0xCE<<4)|10: assert(false);break;
        case (0xCE<<4)|11: assert(false);break;
        case (0xCE<<4)|12: assert(false);break;
        case (0xCE<<4)|13: assert(false);break;
        case (0xCE<<4)|14: assert(false);break;
        case (0xCE<<4)|15: assert(false);break;
    /* NOP  */
        case (0xCF<<4)|0: _VMA();break;
        case (0xCF<<4)|1: _FETCH();break;
        case (0xCF<<4)|2: assert(false);break;
        case (0xCF<<4)|3: assert(false);break;
        case (0xCF<<4)|4: assert(false);break;
        case (0xCF<<4)|5: assert(false);break;
        case (0xCF<<4)|6: assert(false);break;
        case (0xCF<<4)|7: assert(false);break;
        case (0xCF<<4)|8: assert(false);break;
        case (0xCF<<4)|9: assert(false);break;
        case (0xCF<<4)|10: assert(false);break;
        case (0xCF<<4)|11: assert(false);break;
        case (0xCF<<4)|12: assert(false);break;
        case (0xCF<<4)|13: assert(false);break;
        case (0xCF<<4)|14: assert(false);break;
        case (0xCF<<4)|15: assert(false);break;
    /* SUBB zp */
        case (0xD0<<4)|0: _SA(c->PC++);break;
        case (0xD0<<4)|1: _SA(_GD());break;
        case (0xD0<<4)|2: _mc6800_sub(c, _GD(), false, false);_FETCH();break;
        case (0xD0<<4)|3: assert(false);break;
        case (0xD0<<4)|4: assert(false);break;
        case (0xD0<<4)|5: assert(false);break;
        case (0xD0<<4)|6: assert(false);break;
        case (0xD0<<4)|7: assert(false);break;
        case (0xD0<<4)|8: assert(false);break;
        case (0xD0<<4)|9: assert(false);break;
        case (0xD0<<4)|10: assert(false);break;
        case (0xD0<<4)|11: assert(false);break;
        case (0xD0<<4)|12: assert(false);break;
        case (0xD0<<4)|13: assert(false);break;
        case (0xD0<<4)|14: assert(false);break;
        case (0xD0<<4)|15: assert(false);break;
    /* CMPB zp */
        case (0xD1<<4)|0: _SA(c->PC++);break;
        case (0xD1<<4)|1: _SA(_GD());break;
        case (0xD1<<4)|2: _mc6800_cmp(c, _GD(), false);_FETCH();break;
        case (0xD1<<4)|3: assert(false);break;
        case (0xD1<<4)|4: assert(false);break;
        case (0xD1<<4)|5: assert(false);break;
        case (0xD1<<4)|6: assert(false);break;
        case (0xD1<<4)|7: assert(false);break;
        case (0xD1<<4)|8: assert(false);break;
        case (0xD1<<4)|9: assert(false);break;
        case (0xD1<<4)|10: assert(false);break;
        case (0xD1<<4)|11: assert(false);break;
        case (0xD1<<4)|12: assert(false);break;
        case (0xD1<<4)|13: assert(false);break;
        case (0xD1<<4)|14: assert(false);break;
        case (0xD1<<4)|15: assert(false);break;
    /* SBCB zp */
        case (0xD2<<4)|0: _SA(c->PC++);break;
        case (0xD2<<4)|1: _SA(_GD());break;
        case (0xD2<<4)|2: _mc6800_sub(c, _GD(), false, true);_FETCH();break;
        case (0xD2<<4)|3: assert(false);break;
        case (0xD2<<4)|4: assert(false);break;
        case (0xD2<<4)|5: assert(false);break;
        case (0xD2<<4)|6: assert(false);break;
        case (0xD2<<4)|7: assert(false);break;
        case (0xD2<<4)|8: assert(false);break;
        case (0xD2<<4)|9: assert(false);break;
        case (0xD2<<4)|10: assert(false);break;
        case (0xD2<<4)|11: assert(false);break;
        case (0xD2<<4)|12: assert(false);break;
        case (0xD2<<4)|13: assert(false);break;
        case (0xD2<<4)|14: assert(false);break;
        case (0xD2<<4)|15: assert(false);break;
    /* NOP  */
        case (0xD3<<4)|0: _VMA();break;
        case (0xD3<<4)|1: _FETCH();break;
        case (0xD3<<4)|2: assert(false);break;
        case (0xD3<<4)|3: assert(false);break;
        case (0xD3<<4)|4: assert(false);break;
        case (0xD3<<4)|5: assert(false);break;
        case (0xD3<<4)|6: assert(false);break;
        case (0xD3<<4)|7: assert(false);break;
        case (0xD3<<4)|8: assert(false);break;
        case (0xD3<<4)|9: assert(false);break;
        case (0xD3<<4)|10: assert(false);break;
        case (0xD3<<4)|11: assert(false);break;
        case (0xD3<<4)|12: assert(false);break;
        case (0xD3<<4)|13: assert(false);break;
        case (0xD3<<4)|14: assert(false);break;
        case (0xD3<<4)|15: assert(false);break;
    /* ANDB zp */
        case (0xD4<<4)|0: _SA(c->PC++);break;
        case (0xD4<<4)|1: _SA(_GD());break;
        case (0xD4<<4)|2: _VF(false);c->B&=_GD();_NZ(c->B);_FETCH();break;
        case (0xD4<<4)|3: assert(false);break;
        case (0xD4<<4)|4: assert(false);break;
        case (0xD4<<4)|5: assert(false);break;
        case (0xD4<<4)|6: assert(false);break;
        case (0xD4<<4)|7: assert(false);break;
        case (0xD4<<4)|8: assert(false);break;
        case (0xD4<<4)|9: assert(false);break;
        case (0xD4<<4)|10: assert(false);break;
        case (0xD4<<4)|11: assert(false);break;
        case (0xD4<<4)|12: assert(false);break;
        case (0xD4<<4)|13: assert(false);break;
        case (0xD4<<4)|14: assert(false);break;
        case (0xD4<<4)|15: assert(false);break;
    /* BITB zp */
        case (0xD5<<4)|0: _SA(c->PC++);break;
        case (0xD5<<4)|1: _SA(_GD());break;
        case (0xD5<<4)|2: _VF(false);_NZ(c->B&_GD());_FETCH();break;
        case (0xD5<<4)|3: assert(false);break;
        case (0xD5<<4)|4: assert(false);break;
        case (0xD5<<4)|5: assert(false);break;
        case (0xD5<<4)|6: assert(false);break;
        case (0xD5<<4)|7: assert(false);break;
        case (0xD5<<4)|8: assert(false);break;
        case (0xD5<<4)|9: assert(false);break;
        case (0xD5<<4)|10: assert(false);break;
        case (0xD5<<4)|11: assert(false);break;
        case (0xD5<<4)|12: assert(false);break;
        case (0xD5<<4)|13: assert(false);break;
        case (0xD5<<4)|14: assert(false);break;
        case (0xD5<<4)|15: assert(false);break;
    /* LDAB zp */
        case (0xD6<<4)|0: _SA(c->PC++);break;
        case (0xD6<<4)|1: _SA(_GD());break;
        case (0xD6<<4)|2: _VF(false);c->B=_GD();_NZ(c->B);_FETCH();break;
        case (0xD6<<4)|3: assert(false);break;
        case (0xD6<<4)|4: assert(false);break;
        case (0xD6<<4)|5: assert(false);break;
        case (0xD6<<4)|6: assert(false);break;
        case (0xD6<<4)|7: assert(false);break;
        case (0xD6<<4)|8: assert(false);break;
        case (0xD6<<4)|9: assert(false);break;
        case (0xD6<<4)|10: assert(false);break;
        case (0xD6<<4)|11: assert(false);break;
        case (0xD6<<4)|12: assert(false);break;
        case (0xD6<<4)|13: assert(false);break;
        case (0xD6<<4)|14: assert(false);break;
        case (0xD6<<4)|15: assert(false);break;
    /* STAB zp */
        case (0xD7<<4)|0: _SA(c->PC++);break;
        case (0xD7<<4)|1: _SA(_GD());_VMA();break;
        case (0xD7<<4)|2: _VF(false);_SD(c->B);_NZ(c->B);_WR();break;
        case (0xD7<<4)|3: _FETCH();break;
        case (0xD7<<4)|4: assert(false);break;
        case (0xD7<<4)|5: assert(false);break;
        case (0xD7<<4)|6: assert(false);break;
        case (0xD7<<4)|7: assert(false);break;
        case (0xD7<<4)|8: assert(false);break;
        case (0xD7<<4)|9: assert(false);break;
        case (0xD7<<4)|10: assert(false);break;
        case (0xD7<<4)|11: assert(false);break;
        case (0xD7<<4)|12: assert(false);break;
        case (0xD7<<4)|13: assert(false);break;
        case (0xD7<<4)|14: assert(false);break;
        case (0xD7<<4)|15: assert(false);break;
    /* EORB zp */
        case (0xD8<<4)|0: _SA(c->PC++);break;
        case (0xD8<<4)|1: _SA(_GD());break;
        case (0xD8<<4)|2: _VF(false);c->B^=_GD();_NZ(c->B);_FETCH();break;
        case (0xD8<<4)|3: assert(false);break;
        case (0xD8<<4)|4: assert(false);break;
        case (0xD8<<4)|5: assert(false);break;
        case (0xD8<<4)|6: assert(false);break;
        case (0xD8<<4)|7: assert(false);break;
        case (0xD8<<4)|8: assert(false);break;
        case (0xD8<<4)|9: assert(false);break;
        case (0xD8<<4)|10: assert(false);break;
        case (0xD8<<4)|11: assert(false);break;
        case (0xD8<<4)|12: assert(false);break;
        case (0xD8<<4)|13: assert(false);break;
        case (0xD8<<4)|14: assert(false);break;
        case (0xD8<<4)|15: assert(false);break;
    /* ADCB zp */
        case (0xD9<<4)|0: _SA(c->PC++);break;
        case (0xD9<<4)|1: _SA(_GD());break;
        case (0xD9<<4)|2: _mc6800_add(c, _GD(), false, true);_FETCH();break;
        case (0xD9<<4)|3: assert(false);break;
        case (0xD9<<4)|4: assert(false);break;
        case (0xD9<<4)|5: assert(false);break;
        case (0xD9<<4)|6: assert(false);break;
        case (0xD9<<4)|7: assert(false);break;
        case (0xD9<<4)|8: assert(false);break;
        case (0xD9<<4)|9: assert(false);break;
        case (0xD9<<4)|10: assert(false);break;
        case (0xD9<<4)|11: assert(false);break;
        case (0xD9<<4)|12: assert(false);break;
        case (0xD9<<4)|13: assert(false);break;
        case (0xD9<<4)|14: assert(false);break;
        case (0xD9<<4)|15: assert(false);break;
    /* ORAB zp */
        case (0xDA<<4)|0: _SA(c->PC++);break;
        case (0xDA<<4)|1: _SA(_GD());break;
        case (0xDA<<4)|2: _VF(false);c->B|=_GD();_NZ(c->B);_FETCH();break;
        case (0xDA<<4)|3: assert(false);break;
        case (0xDA<<4)|4: assert(false);break;
        case (0xDA<<4)|5: assert(false);break;
        case (0xDA<<4)|6: assert(false);break;
        case (0xDA<<4)|7: assert(false);break;
        case (0xDA<<4)|8: assert(false);break;
        case (0xDA<<4)|9: assert(false);break;
        case (0xDA<<4)|10: assert(false);break;
        case (0xDA<<4)|11: assert(false);break;
        case (0xDA<<4)|12: assert(false);break;
        case (0xDA<<4)|13: assert(false);break;
        case (0xDA<<4)|14: assert(false);break;
        case (0xDA<<4)|15: assert(false);break;
    /* ADDB zp */
        case (0xDB<<4)|0: _SA(c->PC++);break;
        case (0xDB<<4)|1: _SA(_GD());break;
        case (0xDB<<4)|2: _mc6800_add(c, _GD(), false, false);_FETCH();break;
        case (0xDB<<4)|3: assert(false);break;
        case (0xDB<<4)|4: assert(false);break;
        case (0xDB<<4)|5: assert(false);break;
        case (0xDB<<4)|6: assert(false);break;
        case (0xDB<<4)|7: assert(false);break;
        case (0xDB<<4)|8: assert(false);break;
        case (0xDB<<4)|9: assert(false);break;
        case (0xDB<<4)|10: assert(false);break;
        case (0xDB<<4)|11: assert(false);break;
        case (0xDB<<4)|12: assert(false);break;
        case (0xDB<<4)|13: assert(false);break;
        case (0xDB<<4)|14: assert(false);break;
        case (0xDB<<4)|15: assert(false);break;
    /* CPX  */
        case (0xDC<<4)|0: c->AD=_GD();c->AD<<=8;_SA(_GA()+1);break;
        case (0xDC<<4)|1: c->AD=c->AD|_GD();_mc6800_cpx(c, c->AD);_VMA();break;
        case (0xDC<<4)|2: _FETCH();break;
        case (0xDC<<4)|3: assert(false);break;
        case (0xDC<<4)|4: assert(false);break;
        case (0xDC<<4)|5: assert(false);break;
        case (0xDC<<4)|6: assert(false);break;
        case (0xDC<<4)|7: assert(false);break;
        case (0xDC<<4)|8: assert(false);break;
        case (0xDC<<4)|9: assert(false);break;
        case (0xDC<<4)|10: assert(false);break;
        case (0xDC<<4)|11: assert(false);break;
        case (0xDC<<4)|12: assert(false);break;
        case (0xDC<<4)|13: assert(false);break;
        case (0xDC<<4)|14: assert(false);break;
        case (0xDC<<4)|15: assert(false);break;
    /* NOP  */
        case (0xDD<<4)|0: _VMA();break;
        case (0xDD<<4)|1: _FETCH();break;
        case (0xDD<<4)|2: assert(false);break;
        case (0xDD<<4)|3: assert(false);break;
        case (0xDD<<4)|4: assert(false);break;
        case (0xDD<<4)|5: assert(false);break;
        case (0xDD<<4)|6: assert(false);break;
        case (0xDD<<4)|7: assert(false);break;
        case (0xDD<<4)|8: assert(false);break;
        case (0xDD<<4)|9: assert(false);break;
        case (0xDD<<4)|10: assert(false);break;
        case (0xDD<<4)|11: assert(false);break;
        case (0xDD<<4)|12: assert(false);break;
        case (0xDD<<4)|13: assert(false);break;
        case (0xDD<<4)|14: assert(false);break;
        case (0xDD<<4)|15: assert(false);break;
    /* LDX zp */
        case (0xDE<<4)|0: _SA(c->PC++);break;
        case (0xDE<<4)|1: _SA(_GD());break;
        case (0xDE<<4)|2: c->IX=_GD();c->IX<<=8;_SA(_GA()+1);break;
        case (0xDE<<4)|3: _VF(false);c->IX|=_GD();_NZ16(c->IX);_FETCH();break;
        case (0xDE<<4)|4: assert(false);break;
        case (0xDE<<4)|5: assert(false);break;
        case (0xDE<<4)|6: assert(false);break;
        case (0xDE<<4)|7: assert(false);break;
        case (0xDE<<4)|8: assert(false);break;
        case (0xDE<<4)|9: assert(false);break;
        case (0xDE<<4)|10: assert(false);break;
        case (0xDE<<4)|11: assert(false);break;
        case (0xDE<<4)|12: assert(false);break;
        case (0xDE<<4)|13: assert(false);break;
        case (0xDE<<4)|14: assert(false);break;
        case (0xDE<<4)|15: assert(false);break;
    /* STX zp */
        case (0xDF<<4)|0: _SA(c->PC++);break;
        case (0xDF<<4)|1: _SA(_GD());_VMA();break;
        case (0xDF<<4)|2: _NZ16(c->IX);_SD(c->IX>>8);_WR();break;
        case (0xDF<<4)|3: _VF(false);_SA(_GA()+1);_SD(c->IX&0xFF);_WR();break;
        case (0xDF<<4)|4: _FETCH();break;
        case (0xDF<<4)|5: assert(false);break;
        case (0xDF<<4)|6: assert(false);break;
        case (0xDF<<4)|7: assert(false);break;
        case (0xDF<<4)|8: assert(false);break;
        case (0xDF<<4)|9: assert(false);break;
        case (0xDF<<4)|10: assert(false);break;
        case (0xDF<<4)|11: assert(false);break;
        case (0xDF<<4)|12: assert(false);break;
        case (0xDF<<4)|13: assert(false);break;
        case (0xDF<<4)|14: assert(false);break;
        case (0xDF<<4)|15: assert(false);break;
    /* SUBB (zp,X) */
        case (0xE0<<4)|0: _SA(c->PC++);break;
        case (0xE0<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xE0<<4)|2: _SA(c->AD);_VMA();break;
        case (0xE0<<4)|3: break;
        case (0xE0<<4)|4: _mc6800_sub(c, _GD(), false, false);_FETCH();break;
        case (0xE0<<4)|5: assert(false);break;
        case (0xE0<<4)|6: assert(false);break;
        case (0xE0<<4)|7: assert(false);break;
        case (0xE0<<4)|8: assert(false);break;
        case (0xE0<<4)|9: assert(false);break;
        case (0xE0<<4)|10: assert(false);break;
        case (0xE0<<4)|11: assert(false);break;
        case (0xE0<<4)|12: assert(false);break;
        case (0xE0<<4)|13: assert(false);break;
        case (0xE0<<4)|14: assert(false);break;
        case (0xE0<<4)|15: assert(false);break;
    /* CMPB (zp,X) */
        case (0xE1<<4)|0: _SA(c->PC++);break;
        case (0xE1<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xE1<<4)|2: _SA(c->AD);_VMA();break;
        case (0xE1<<4)|3: break;
        case (0xE1<<4)|4: _mc6800_cmp(c, _GD(), false);_FETCH();break;
        case (0xE1<<4)|5: assert(false);break;
        case (0xE1<<4)|6: assert(false);break;
        case (0xE1<<4)|7: assert(false);break;
        case (0xE1<<4)|8: assert(false);break;
        case (0xE1<<4)|9: assert(false);break;
        case (0xE1<<4)|10: assert(false);break;
        case (0xE1<<4)|11: assert(false);break;
        case (0xE1<<4)|12: assert(false);break;
        case (0xE1<<4)|13: assert(false);break;
        case (0xE1<<4)|14: assert(false);break;
        case (0xE1<<4)|15: assert(false);break;
    /* SBCB (zp,X) */
        case (0xE2<<4)|0: _SA(c->PC++);break;
        case (0xE2<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xE2<<4)|2: _SA(c->AD);_VMA();break;
        case (0xE2<<4)|3: break;
        case (0xE2<<4)|4: _mc6800_sub(c, _GD(), false, true);_FETCH();break;
        case (0xE2<<4)|5: assert(false);break;
        case (0xE2<<4)|6: assert(false);break;
        case (0xE2<<4)|7: assert(false);break;
        case (0xE2<<4)|8: assert(false);break;
        case (0xE2<<4)|9: assert(false);break;
        case (0xE2<<4)|10: assert(false);break;
        case (0xE2<<4)|11: assert(false);break;
        case (0xE2<<4)|12: assert(false);break;
        case (0xE2<<4)|13: assert(false);break;
        case (0xE2<<4)|14: assert(false);break;
        case (0xE2<<4)|15: assert(false);break;
    /* NOP  */
        case (0xE3<<4)|0: _VMA();break;
        case (0xE3<<4)|1: _FETCH();break;
        case (0xE3<<4)|2: assert(false);break;
        case (0xE3<<4)|3: assert(false);break;
        case (0xE3<<4)|4: assert(false);break;
        case (0xE3<<4)|5: assert(false);break;
        case (0xE3<<4)|6: assert(false);break;
        case (0xE3<<4)|7: assert(false);break;
        case (0xE3<<4)|8: assert(false);break;
        case (0xE3<<4)|9: assert(false);break;
        case (0xE3<<4)|10: assert(false);break;
        case (0xE3<<4)|11: assert(false);break;
        case (0xE3<<4)|12: assert(false);break;
        case (0xE3<<4)|13: assert(false);break;
        case (0xE3<<4)|14: assert(false);break;
        case (0xE3<<4)|15: assert(false);break;
    /* ANDB (zp,X) */
        case (0xE4<<4)|0: _SA(c->PC++);break;
        case (0xE4<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xE4<<4)|2: _SA(c->AD);_VMA();break;
        case (0xE4<<4)|3: break;
        case (0xE4<<4)|4: _VF(false);c->B&=_GD();_NZ(c->B);_FETCH();break;
        case (0xE4<<4)|5: assert(false);break;
        case (0xE4<<4)|6: assert(false);break;
        case (0xE4<<4)|7: assert(false);break;
        case (0xE4<<4)|8: assert(false);break;
        case (0xE4<<4)|9: assert(false);break;
        case (0xE4<<4)|10: assert(false);break;
        case (0xE4<<4)|11: assert(false);break;
        case (0xE4<<4)|12: assert(false);break;
        case (0xE4<<4)|13: assert(false);break;
        case (0xE4<<4)|14: assert(false);break;
        case (0xE4<<4)|15: assert(false);break;
    /* BITB (zp,X) */
        case (0xE5<<4)|0: _SA(c->PC++);break;
        case (0xE5<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xE5<<4)|2: _SA(c->AD);_VMA();break;
        case (0xE5<<4)|3: break;
        case (0xE5<<4)|4: _VF(false);_NZ(c->B&_GD());_FETCH();break;
        case (0xE5<<4)|5: assert(false);break;
        case (0xE5<<4)|6: assert(false);break;
        case (0xE5<<4)|7: assert(false);break;
        case (0xE5<<4)|8: assert(false);break;
        case (0xE5<<4)|9: assert(false);break;
        case (0xE5<<4)|10: assert(false);break;
        case (0xE5<<4)|11: assert(false);break;
        case (0xE5<<4)|12: assert(false);break;
        case (0xE5<<4)|13: assert(false);break;
        case (0xE5<<4)|14: assert(false);break;
        case (0xE5<<4)|15: assert(false);break;
    /* LDAB (zp,X) */
        case (0xE6<<4)|0: _SA(c->PC++);break;
        case (0xE6<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xE6<<4)|2: _SA(c->AD);_VMA();break;
        case (0xE6<<4)|3: break;
        case (0xE6<<4)|4: _VF(false);c->B=_GD();_NZ(c->B);_FETCH();break;
        case (0xE6<<4)|5: assert(false);break;
        case (0xE6<<4)|6: assert(false);break;
        case (0xE6<<4)|7: assert(false);break;
        case (0xE6<<4)|8: assert(false);break;
        case (0xE6<<4)|9: assert(false);break;
        case (0xE6<<4)|10: assert(false);break;
        case (0xE6<<4)|11: assert(false);break;
        case (0xE6<<4)|12: assert(false);break;
        case (0xE6<<4)|13: assert(false);break;
        case (0xE6<<4)|14: assert(false);break;
        case (0xE6<<4)|15: assert(false);break;
    /* STAB (zp,X) */
        case (0xE7<<4)|0: _SA(c->PC++);break;
        case (0xE7<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xE7<<4)|2: _SA(c->AD);_VMA();break;
        case (0xE7<<4)|3: _VMA();break;
        case (0xE7<<4)|4: _VF(false);_SD(c->B);_NZ(c->B);_WR();break;
        case (0xE7<<4)|5: _FETCH();break;
        case (0xE7<<4)|6: assert(false);break;
        case (0xE7<<4)|7: assert(false);break;
        case (0xE7<<4)|8: assert(false);break;
        case (0xE7<<4)|9: assert(false);break;
        case (0xE7<<4)|10: assert(false);break;
        case (0xE7<<4)|11: assert(false);break;
        case (0xE7<<4)|12: assert(false);break;
        case (0xE7<<4)|13: assert(false);break;
        case (0xE7<<4)|14: assert(false);break;
        case (0xE7<<4)|15: assert(false);break;
    /* EORB (zp,X) */
        case (0xE8<<4)|0: _SA(c->PC++);break;
        case (0xE8<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xE8<<4)|2: _SA(c->AD);_VMA();break;
        case (0xE8<<4)|3: break;
        case (0xE8<<4)|4: _VF(false);c->B^=_GD();_NZ(c->B);_FETCH();break;
        case (0xE8<<4)|5: assert(false);break;
        case (0xE8<<4)|6: assert(false);break;
        case (0xE8<<4)|7: assert(false);break;
        case (0xE8<<4)|8: assert(false);break;
        case (0xE8<<4)|9: assert(false);break;
        case (0xE8<<4)|10: assert(false);break;
        case (0xE8<<4)|11: assert(false);break;
        case (0xE8<<4)|12: assert(false);break;
        case (0xE8<<4)|13: assert(false);break;
        case (0xE8<<4)|14: assert(false);break;
        case (0xE8<<4)|15: assert(false);break;
    /* ADCB (zp,X) */
        case (0xE9<<4)|0: _SA(c->PC++);break;
        case (0xE9<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xE9<<4)|2: _SA(c->AD);_VMA();break;
        case (0xE9<<4)|3: break;
        case (0xE9<<4)|4: _mc6800_add(c, _GD(), false, true);_FETCH();break;
        case (0xE9<<4)|5: assert(false);break;
        case (0xE9<<4)|6: assert(false);break;
        case (0xE9<<4)|7: assert(false);break;
        case (0xE9<<4)|8: assert(false);break;
        case (0xE9<<4)|9: assert(false);break;
        case (0xE9<<4)|10: assert(false);break;
        case (0xE9<<4)|11: assert(false);break;
        case (0xE9<<4)|12: assert(false);break;
        case (0xE9<<4)|13: assert(false);break;
        case (0xE9<<4)|14: assert(false);break;
        case (0xE9<<4)|15: assert(false);break;
    /* ORAB (zp,X) */
        case (0xEA<<4)|0: _SA(c->PC++);break;
        case (0xEA<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xEA<<4)|2: _SA(c->AD);_VMA();break;
        case (0xEA<<4)|3: break;
        case (0xEA<<4)|4: _VF(false);c->B|=_GD();_NZ(c->B);_FETCH();break;
        case (0xEA<<4)|5: assert(false);break;
        case (0xEA<<4)|6: assert(false);break;
        case (0xEA<<4)|7: assert(false);break;
        case (0xEA<<4)|8: assert(false);break;
        case (0xEA<<4)|9: assert(false);break;
        case (0xEA<<4)|10: assert(false);break;
        case (0xEA<<4)|11: assert(false);break;
        case (0xEA<<4)|12: assert(false);break;
        case (0xEA<<4)|13: assert(false);break;
        case (0xEA<<4)|14: assert(false);break;
        case (0xEA<<4)|15: assert(false);break;
    /* ADDB (zp,X) */
        case (0xEB<<4)|0: _SA(c->PC++);break;
        case (0xEB<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xEB<<4)|2: _SA(c->AD);_VMA();break;
        case (0xEB<<4)|3: break;
        case (0xEB<<4)|4: _mc6800_add(c, _GD(), false, false);_FETCH();break;
        case (0xEB<<4)|5: assert(false);break;
        case (0xEB<<4)|6: assert(false);break;
        case (0xEB<<4)|7: assert(false);break;
        case (0xEB<<4)|8: assert(false);break;
        case (0xEB<<4)|9: assert(false);break;
        case (0xEB<<4)|10: assert(false);break;
        case (0xEB<<4)|11: assert(false);break;
        case (0xEB<<4)|12: assert(false);break;
        case (0xEB<<4)|13: assert(false);break;
        case (0xEB<<4)|14: assert(false);break;
        case (0xEB<<4)|15: assert(false);break;
    /* CPX  */
        case (0xEC<<4)|0: c->AD=_GD();c->AD<<=8;_SA(_GA()+1);break;
        case (0xEC<<4)|1: c->AD=c->AD|_GD();_mc6800_cpx(c, c->AD);_VMA();break;
        case (0xEC<<4)|2: _FETCH();break;
        case (0xEC<<4)|3: assert(false);break;
        case (0xEC<<4)|4: assert(false);break;
        case (0xEC<<4)|5: assert(false);break;
        case (0xEC<<4)|6: assert(false);break;
        case (0xEC<<4)|7: assert(false);break;
        case (0xEC<<4)|8: assert(false);break;
        case (0xEC<<4)|9: assert(false);break;
        case (0xEC<<4)|10: assert(false);break;
        case (0xEC<<4)|11: assert(false);break;
        case (0xEC<<4)|12: assert(false);break;
        case (0xEC<<4)|13: assert(false);break;
        case (0xEC<<4)|14: assert(false);break;
        case (0xEC<<4)|15: assert(false);break;
    /* NOP  */
        case (0xED<<4)|0: _VMA();break;
        case (0xED<<4)|1: _FETCH();break;
        case (0xED<<4)|2: assert(false);break;
        case (0xED<<4)|3: assert(false);break;
        case (0xED<<4)|4: assert(false);break;
        case (0xED<<4)|5: assert(false);break;
        case (0xED<<4)|6: assert(false);break;
        case (0xED<<4)|7: assert(false);break;
        case (0xED<<4)|8: assert(false);break;
        case (0xED<<4)|9: assert(false);break;
        case (0xED<<4)|10: assert(false);break;
        case (0xED<<4)|11: assert(false);break;
        case (0xED<<4)|12: assert(false);break;
        case (0xED<<4)|13: assert(false);break;
        case (0xED<<4)|14: assert(false);break;
        case (0xED<<4)|15: assert(false);break;
    /* LDX (zp,X) */
        case (0xEE<<4)|0: _SA(c->PC++);break;
        case (0xEE<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xEE<<4)|2: _SA(c->AD);_VMA();break;
        case (0xEE<<4)|3: break;
        case (0xEE<<4)|4: c->IX=_GD();c->IX<<=8;_SA(_GA()+1);break;
        case (0xEE<<4)|5: _VF(false);c->IX|=_GD();_NZ16(c->IX);_FETCH();break;
        case (0xEE<<4)|6: assert(false);break;
        case (0xEE<<4)|7: assert(false);break;
        case (0xEE<<4)|8: assert(false);break;
        case (0xEE<<4)|9: assert(false);break;
        case (0xEE<<4)|10: assert(false);break;
        case (0xEE<<4)|11: assert(false);break;
        case (0xEE<<4)|12: assert(false);break;
        case (0xEE<<4)|13: assert(false);break;
        case (0xEE<<4)|14: assert(false);break;
        case (0xEE<<4)|15: assert(false);break;
    /* STX (zp,X) */
        case (0xEF<<4)|0: _SA(c->PC++);break;
        case (0xEF<<4)|1: c->AD=_GD()+c->IX;_VMA();break;
        case (0xEF<<4)|2: _SA(c->AD);_VMA();break;
        case (0xEF<<4)|3: _VMA();break;
        case (0xEF<<4)|4: _NZ16(c->IX);_SD(c->IX>>8);_WR();break;
        case (0xEF<<4)|5: _VF(false);_SA(_GA()+1);_SD(c->IX&0xFF);_WR();break;
        case (0xEF<<4)|6: _FETCH();break;
        case (0xEF<<4)|7: assert(false);break;
        case (0xEF<<4)|8: assert(false);break;
        case (0xEF<<4)|9: assert(false);break;
        case (0xEF<<4)|10: assert(false);break;
        case (0xEF<<4)|11: assert(false);break;
        case (0xEF<<4)|12: assert(false);break;
        case (0xEF<<4)|13: assert(false);break;
        case (0xEF<<4)|14: assert(false);break;
        case (0xEF<<4)|15: assert(false);break;
    /* SUBB abs */
        case (0xF0<<4)|0: _SA(c->PC++);break;
        case (0xF0<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xF0<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xF0<<4)|3: _mc6800_sub(c, _GD(), false, false);_FETCH();break;
        case (0xF0<<4)|4: assert(false);break;
        case (0xF0<<4)|5: assert(false);break;
        case (0xF0<<4)|6: assert(false);break;
        case (0xF0<<4)|7: assert(false);break;
        case (0xF0<<4)|8: assert(false);break;
        case (0xF0<<4)|9: assert(false);break;
        case (0xF0<<4)|10: assert(false);break;
        case (0xF0<<4)|11: assert(false);break;
        case (0xF0<<4)|12: assert(false);break;
        case (0xF0<<4)|13: assert(false);break;
        case (0xF0<<4)|14: assert(false);break;
        case (0xF0<<4)|15: assert(false);break;
    /* CMPB abs */
        case (0xF1<<4)|0: _SA(c->PC++);break;
        case (0xF1<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xF1<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xF1<<4)|3: _mc6800_cmp(c, _GD(), false);_FETCH();break;
        case (0xF1<<4)|4: assert(false);break;
        case (0xF1<<4)|5: assert(false);break;
        case (0xF1<<4)|6: assert(false);break;
        case (0xF1<<4)|7: assert(false);break;
        case (0xF1<<4)|8: assert(false);break;
        case (0xF1<<4)|9: assert(false);break;
        case (0xF1<<4)|10: assert(false);break;
        case (0xF1<<4)|11: assert(false);break;
        case (0xF1<<4)|12: assert(false);break;
        case (0xF1<<4)|13: assert(false);break;
        case (0xF1<<4)|14: assert(false);break;
        case (0xF1<<4)|15: assert(false);break;
    /* SBCB abs */
        case (0xF2<<4)|0: _SA(c->PC++);break;
        case (0xF2<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xF2<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xF2<<4)|3: _mc6800_sub(c, _GD(), false, true);_FETCH();break;
        case (0xF2<<4)|4: assert(false);break;
        case (0xF2<<4)|5: assert(false);break;
        case (0xF2<<4)|6: assert(false);break;
        case (0xF2<<4)|7: assert(false);break;
        case (0xF2<<4)|8: assert(false);break;
        case (0xF2<<4)|9: assert(false);break;
        case (0xF2<<4)|10: assert(false);break;
        case (0xF2<<4)|11: assert(false);break;
        case (0xF2<<4)|12: assert(false);break;
        case (0xF2<<4)|13: assert(false);break;
        case (0xF2<<4)|14: assert(false);break;
        case (0xF2<<4)|15: assert(false);break;
    /* NOP  */
        case (0xF3<<4)|0: _VMA();break;
        case (0xF3<<4)|1: _FETCH();break;
        case (0xF3<<4)|2: assert(false);break;
        case (0xF3<<4)|3: assert(false);break;
        case (0xF3<<4)|4: assert(false);break;
        case (0xF3<<4)|5: assert(false);break;
        case (0xF3<<4)|6: assert(false);break;
        case (0xF3<<4)|7: assert(false);break;
        case (0xF3<<4)|8: assert(false);break;
        case (0xF3<<4)|9: assert(false);break;
        case (0xF3<<4)|10: assert(false);break;
        case (0xF3<<4)|11: assert(false);break;
        case (0xF3<<4)|12: assert(false);break;
        case (0xF3<<4)|13: assert(false);break;
        case (0xF3<<4)|14: assert(false);break;
        case (0xF3<<4)|15: assert(false);break;
    /* ANDB abs */
        case (0xF4<<4)|0: _SA(c->PC++);break;
        case (0xF4<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xF4<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xF4<<4)|3: _VF(false);c->B&=_GD();_NZ(c->B);_FETCH();break;
        case (0xF4<<4)|4: assert(false);break;
        case (0xF4<<4)|5: assert(false);break;
        case (0xF4<<4)|6: assert(false);break;
        case (0xF4<<4)|7: assert(false);break;
        case (0xF4<<4)|8: assert(false);break;
        case (0xF4<<4)|9: assert(false);break;
        case (0xF4<<4)|10: assert(false);break;
        case (0xF4<<4)|11: assert(false);break;
        case (0xF4<<4)|12: assert(false);break;
        case (0xF4<<4)|13: assert(false);break;
        case (0xF4<<4)|14: assert(false);break;
        case (0xF4<<4)|15: assert(false);break;
    /* BITB abs */
        case (0xF5<<4)|0: _SA(c->PC++);break;
        case (0xF5<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xF5<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xF5<<4)|3: _VF(false);_NZ(c->B&_GD());_FETCH();break;
        case (0xF5<<4)|4: assert(false);break;
        case (0xF5<<4)|5: assert(false);break;
        case (0xF5<<4)|6: assert(false);break;
        case (0xF5<<4)|7: assert(false);break;
        case (0xF5<<4)|8: assert(false);break;
        case (0xF5<<4)|9: assert(false);break;
        case (0xF5<<4)|10: assert(false);break;
        case (0xF5<<4)|11: assert(false);break;
        case (0xF5<<4)|12: assert(false);break;
        case (0xF5<<4)|13: assert(false);break;
        case (0xF5<<4)|14: assert(false);break;
        case (0xF5<<4)|15: assert(false);break;
    /* LDAB abs */
        case (0xF6<<4)|0: _SA(c->PC++);break;
        case (0xF6<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xF6<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xF6<<4)|3: _VF(false);c->B=_GD();_NZ(c->B);_FETCH();break;
        case (0xF6<<4)|4: assert(false);break;
        case (0xF6<<4)|5: assert(false);break;
        case (0xF6<<4)|6: assert(false);break;
        case (0xF6<<4)|7: assert(false);break;
        case (0xF6<<4)|8: assert(false);break;
        case (0xF6<<4)|9: assert(false);break;
        case (0xF6<<4)|10: assert(false);break;
        case (0xF6<<4)|11: assert(false);break;
        case (0xF6<<4)|12: assert(false);break;
        case (0xF6<<4)|13: assert(false);break;
        case (0xF6<<4)|14: assert(false);break;
        case (0xF6<<4)|15: assert(false);break;
    /* STAB abs */
        case (0xF7<<4)|0: _SA(c->PC++);break;
        case (0xF7<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xF7<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0xF7<<4)|3: _VF(false);_SD(c->B);_NZ(c->B);_WR();break;
        case (0xF7<<4)|4: _FETCH();break;
        case (0xF7<<4)|5: assert(false);break;
        case (0xF7<<4)|6: assert(false);break;
        case (0xF7<<4)|7: assert(false);break;
        case (0xF7<<4)|8: assert(false);break;
        case (0xF7<<4)|9: assert(false);break;
        case (0xF7<<4)|10: assert(false);break;
        case (0xF7<<4)|11: assert(false);break;
        case (0xF7<<4)|12: assert(false);break;
        case (0xF7<<4)|13: assert(false);break;
        case (0xF7<<4)|14: assert(false);break;
        case (0xF7<<4)|15: assert(false);break;
    /* EORB abs */
        case (0xF8<<4)|0: _SA(c->PC++);break;
        case (0xF8<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xF8<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xF8<<4)|3: _VF(false);c->B^=_GD();_NZ(c->B);_FETCH();break;
        case (0xF8<<4)|4: assert(false);break;
        case (0xF8<<4)|5: assert(false);break;
        case (0xF8<<4)|6: assert(false);break;
        case (0xF8<<4)|7: assert(false);break;
        case (0xF8<<4)|8: assert(false);break;
        case (0xF8<<4)|9: assert(false);break;
        case (0xF8<<4)|10: assert(false);break;
        case (0xF8<<4)|11: assert(false);break;
        case (0xF8<<4)|12: assert(false);break;
        case (0xF8<<4)|13: assert(false);break;
        case (0xF8<<4)|14: assert(false);break;
        case (0xF8<<4)|15: assert(false);break;
    /* ADCB abs */
        case (0xF9<<4)|0: _SA(c->PC++);break;
        case (0xF9<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xF9<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xF9<<4)|3: _mc6800_add(c, _GD(), false, true);_FETCH();break;
        case (0xF9<<4)|4: assert(false);break;
        case (0xF9<<4)|5: assert(false);break;
        case (0xF9<<4)|6: assert(false);break;
        case (0xF9<<4)|7: assert(false);break;
        case (0xF9<<4)|8: assert(false);break;
        case (0xF9<<4)|9: assert(false);break;
        case (0xF9<<4)|10: assert(false);break;
        case (0xF9<<4)|11: assert(false);break;
        case (0xF9<<4)|12: assert(false);break;
        case (0xF9<<4)|13: assert(false);break;
        case (0xF9<<4)|14: assert(false);break;
        case (0xF9<<4)|15: assert(false);break;
    /* ORAB abs */
        case (0xFA<<4)|0: _SA(c->PC++);break;
        case (0xFA<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xFA<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xFA<<4)|3: _VF(false);c->B|=_GD();_NZ(c->B);_FETCH();break;
        case (0xFA<<4)|4: assert(false);break;
        case (0xFA<<4)|5: assert(false);break;
        case (0xFA<<4)|6: assert(false);break;
        case (0xFA<<4)|7: assert(false);break;
        case (0xFA<<4)|8: assert(false);break;
        case (0xFA<<4)|9: assert(false);break;
        case (0xFA<<4)|10: assert(false);break;
        case (0xFA<<4)|11: assert(false);break;
        case (0xFA<<4)|12: assert(false);break;
        case (0xFA<<4)|13: assert(false);break;
        case (0xFA<<4)|14: assert(false);break;
        case (0xFA<<4)|15: assert(false);break;
    /* ADDB abs */
        case (0xFB<<4)|0: _SA(c->PC++);break;
        case (0xFB<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xFB<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xFB<<4)|3: _mc6800_add(c, _GD(), false, false);_FETCH();break;
        case (0xFB<<4)|4: assert(false);break;
        case (0xFB<<4)|5: assert(false);break;
        case (0xFB<<4)|6: assert(false);break;
        case (0xFB<<4)|7: assert(false);break;
        case (0xFB<<4)|8: assert(false);break;
        case (0xFB<<4)|9: assert(false);break;
        case (0xFB<<4)|10: assert(false);break;
        case (0xFB<<4)|11: assert(false);break;
        case (0xFB<<4)|12: assert(false);break;
        case (0xFB<<4)|13: assert(false);break;
        case (0xFB<<4)|14: assert(false);break;
        case (0xFB<<4)|15: assert(false);break;
    /* CPX  */
        case (0xFC<<4)|0: c->AD=_GD();c->AD<<=8;_SA(_GA()+1);break;
        case (0xFC<<4)|1: c->AD=c->AD|_GD();_mc6800_cpx(c, c->AD);_VMA();break;
        case (0xFC<<4)|2: _FETCH();break;
        case (0xFC<<4)|3: assert(false);break;
        case (0xFC<<4)|4: assert(false);break;
        case (0xFC<<4)|5: assert(false);break;
        case (0xFC<<4)|6: assert(false);break;
        case (0xFC<<4)|7: assert(false);break;
        case (0xFC<<4)|8: assert(false);break;
        case (0xFC<<4)|9: assert(false);break;
        case (0xFC<<4)|10: assert(false);break;
        case (0xFC<<4)|11: assert(false);break;
        case (0xFC<<4)|12: assert(false);break;
        case (0xFC<<4)|13: assert(false);break;
        case (0xFC<<4)|14: assert(false);break;
        case (0xFC<<4)|15: assert(false);break;
    /* NOP  */
        case (0xFD<<4)|0: _VMA();break;
        case (0xFD<<4)|1: _FETCH();break;
        case (0xFD<<4)|2: assert(false);break;
        case (0xFD<<4)|3: assert(false);break;
        case (0xFD<<4)|4: assert(false);break;
        case (0xFD<<4)|5: assert(false);break;
        case (0xFD<<4)|6: assert(false);break;
        case (0xFD<<4)|7: assert(false);break;
        case (0xFD<<4)|8: assert(false);break;
        case (0xFD<<4)|9: assert(false);break;
        case (0xFD<<4)|10: assert(false);break;
        case (0xFD<<4)|11: assert(false);break;
        case (0xFD<<4)|12: assert(false);break;
        case (0xFD<<4)|13: assert(false);break;
        case (0xFD<<4)|14: assert(false);break;
        case (0xFD<<4)|15: assert(false);break;
    /* LDX abs */
        case (0xFE<<4)|0: _SA(c->PC++);break;
        case (0xFE<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xFE<<4)|2: _SA(_GD()|(c->AD<<8));break;
        case (0xFE<<4)|3: c->IX=_GD();c->IX<<=8;_SA(_GA()+1);break;
        case (0xFE<<4)|4: _VF(false);c->IX|=_GD();_NZ16(c->IX);_FETCH();break;
        case (0xFE<<4)|5: assert(false);break;
        case (0xFE<<4)|6: assert(false);break;
        case (0xFE<<4)|7: assert(false);break;
        case (0xFE<<4)|8: assert(false);break;
        case (0xFE<<4)|9: assert(false);break;
        case (0xFE<<4)|10: assert(false);break;
        case (0xFE<<4)|11: assert(false);break;
        case (0xFE<<4)|12: assert(false);break;
        case (0xFE<<4)|13: assert(false);break;
        case (0xFE<<4)|14: assert(false);break;
        case (0xFE<<4)|15: assert(false);break;
    /* STX abs */
        case (0xFF<<4)|0: _SA(c->PC++);break;
        case (0xFF<<4)|1: _SA(c->PC++);c->AD=_GD();break;
        case (0xFF<<4)|2: _SA(_GD()|(c->AD<<8));_VMA();break;
        case (0xFF<<4)|3: _NZ16(c->IX);_SD(c->IX>>8);_WR();break;
        case (0xFF<<4)|4: _VF(false);_SA(_GA()+1);_SD(c->IX&0xFF);_WR();break;
        case (0xFF<<4)|5: _FETCH();break;
        case (0xFF<<4)|6: assert(false);break;
        case (0xFF<<4)|7: assert(false);break;
        case (0xFF<<4)|8: assert(false);break;
        case (0xFF<<4)|9: assert(false);break;
        case (0xFF<<4)|10: assert(false);break;
        case (0xFF<<4)|11: assert(false);break;
        case (0xFF<<4)|12: assert(false);break;
        case (0xFF<<4)|13: assert(false);break;
        case (0xFF<<4)|14: assert(false);break;
        case (0xFF<<4)|15: assert(false);break;

    }
    c->PINS = pins;
    c->irq_pip <<= 1;
    c->nmi_pip <<= 1;
    return pins;
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#undef _SA
#undef _SAD
#undef _FETCH
#undef _SD
#undef _GD
#undef _ON
#undef _OFF
#undef _RD
#undef _WR
#undef _NZ
#undef _VF
#undef _VMA
#endif /* CHIPS_IMPL */
