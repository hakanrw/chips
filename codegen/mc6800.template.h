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
$decode_block
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
