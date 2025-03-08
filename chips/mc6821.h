#pragma once
/*#
    # mc6821.h

    Motorola 6821 Peripheral Interface Adapter (PIA)

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

    ************************************
    *           +-----------+          *
    *    CS --->|           |<--- CA1  *
    *    RW --->|           |<--> CA2  *
    *   RES --->|           |          *
    *   IRQ <---|           |<--> PA0  *
    *           |           |...       *
    *           |           |<--> PA7  *
    *   RS0 --->|  MC 6821  |          *
    *   RS1 --->|           |<--- CB1  *
    *           |           |<--> CB2  *
    *           |           |          *
    *    D0 <-->|           |<--> PB0  *
    *        ...|           |...       *
    *    D7 <-->|           |<--> PB7  *
    *           |           |          *
    *           +-----------+          *
    ************************************

    ## NOT IMPLEMENTED:

    - TODO

    ## LINKS:

    - TODO

    TODO: Documentation


    This file was written by Hakan Candar in 2025. However, it takes significant
    inspiration from the structure and design of source files written by Andre
    Weissflog for this project.

    ## zlib/libpng license

    Copyright (c) 2025 Hakan Candar
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
#*/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// register select same as lower 2 shared address bus bits
#define MC6821_PIN_RS0   (0)
#define MC6821_PIN_RS1   (1)

// data bus pins shared with CPU
#define MC6821_PIN_D0    (16)
#define MC6821_PIN_D1    (17)
#define MC6821_PIN_D2    (18)
#define MC6821_PIN_D3    (19)
#define MC6821_PIN_D4    (20)
#define MC6821_PIN_D5    (21)
#define MC6821_PIN_D6    (22)
#define MC6821_PIN_D7    (23)

// control pins shared with CPU
#define MC6821_PIN_RW    (24)     // same as MC6800_RW
#define MC6821_PIN_IRQ   (26)     // same as MC6800_IRQ

// chip-specific control pins
#define MC6821_PIN_CS    (40)

// chip-specific register control
#define MC6821_PIN_CA1   (42)
#define MC6821_PIN_CA2   (43)
#define MC6821_PIN_CB1   (44)
#define MC6821_PIN_CB2   (45)

// chip-specific interrupt lines
#define MC6821_PIN_IRQA  (46)
#define MC6821_PIN_IRQB  (47)

// port I/O pins
#define MC6821_PIN_PA0   (48)
#define MC6821_PIN_PA1   (49)
#define MC6821_PIN_PA2   (50)
#define MC6821_PIN_PA3   (51)
#define MC6821_PIN_PA4   (52)
#define MC6821_PIN_PA5   (53)
#define MC6821_PIN_PA6   (54)
#define MC6821_PIN_PA7   (55)

#define MC6821_PIN_PB0   (56)
#define MC6821_PIN_PB1   (57)
#define MC6821_PIN_PB2   (58)
#define MC6821_PIN_PB3   (59)
#define MC6821_PIN_PB4   (60)
#define MC6821_PIN_PB5   (61)
#define MC6821_PIN_PB6   (62)
#define MC6821_PIN_PB7   (63)

// pin bit masks
#define MC6821_RS0   (1ULL<<MC6821_PIN_RS0)
#define MC6821_RS1   (1ULL<<MC6821_PIN_RS1)

#define MC6821_RS_PINS (MC6821_RS1|MC6821_RS0)
#define MC6821_D0    (1ULL<<MC6821_PIN_D0)
#define MC6821_D1    (1ULL<<MC6821_PIN_D1)
#define MC6821_D2    (1ULL<<MC6821_PIN_D2)
#define MC6821_D3    (1ULL<<MC6821_PIN_D3)
#define MC6821_D4    (1ULL<<MC6821_PIN_D4)
#define MC6821_D5    (1ULL<<MC6821_PIN_D5)
#define MC6821_D6    (1ULL<<MC6821_PIN_D6)
#define MC6821_D7    (1ULL<<MC6821_PIN_D7)
#define MC6821_DB_PINS (0xFF0000ULL)
#define MC6821_RW    (1ULL<<MC6821_PIN_RW)
#define MC6821_IRQ   (1ULL<<MC6821_PIN_IRQ)
#define MC6821_CS    (1ULL<<MC6821_PIN_CS)

#define MC6821_IRQA  (1ULL<<MC6821_PIN_IRQA)
#define MC6821_IRQB  (1ULL<<MC6821_PIN_IRQB)
#define MC6821_CA1   (1ULL<<MC6821_PIN_CA1)
#define MC6821_CA2   (1ULL<<MC6821_PIN_CA2)
#define MC6821_CB1   (1ULL<<MC6821_PIN_CB1)
#define MC6821_CB2   (1ULL<<MC6821_PIN_CB2)

#define MC6821_PA0   (1ULL<<MC6821_PIN_PA0)
#define MC6821_PA1   (1ULL<<MC6821_PIN_PA1)
#define MC6821_PA2   (1ULL<<MC6821_PIN_PA2)
#define MC6821_PA3   (1ULL<<MC6821_PIN_PA3)
#define MC6821_PA4   (1ULL<<MC6821_PIN_PA4)
#define MC6821_PA5   (1ULL<<MC6821_PIN_PA5)
#define MC6821_PA6   (1ULL<<MC6821_PIN_PA6)
#define MC6821_PA7   (1ULL<<MC6821_PIN_PA7)
#define MC6821_PA_PINS   (MC6821_PA0|MC6821_PA1|MC6821_PA2|MC6821_PA3|MC6821_PA4|MC6821_PA5|MC6821_PA6|MC6821_PA7)

#define MC6821_PB0   (1ULL<<MC6821_PIN_PB0)
#define MC6821_PB1   (1ULL<<MC6821_PIN_PB1)
#define MC6821_PB2   (1ULL<<MC6821_PIN_PB2)
#define MC6821_PB3   (1ULL<<MC6821_PIN_PB3)
#define MC6821_PB4   (1ULL<<MC6821_PIN_PB4)
#define MC6821_PB5   (1ULL<<MC6821_PIN_PB5)
#define MC6821_PB6   (1ULL<<MC6821_PIN_PB6)
#define MC6821_PB7   (1ULL<<MC6821_PIN_PB7)
#define MC6821_PB_PINS   (MC6821_PB0|MC6821_PB1|MC6821_PB2|MC6821_PB3|MC6821_PB4|MC6821_PB5|MC6821_PB6|MC6821_PB7)

// register indices
#define MC6821_REG_PRA      (0)    // peripheral data reg A
#define MC6821_REG_CRA      (1)    // control reg A
#define MC6821_REG_PRB      (2)    // peripheral data reg B
#define MC6821_REG_CRB      (3)    // control reg B

// control word bits
#define MC6821_CR_C1_IRQ      (1<<0)
#define MC6821_CR_C1_LTOH     (1<<1)
#define MC6821_CR_PR          (1<<2)
#define MC6821_CR_C2_IRQ      (1<<3)
#define MC6821_CR_C2_LTOH     (1<<4)
#define MC6821_CR_C2_OUT      (1<<5)
#define MC6821_CR_IRQ2        (1<<6)
#define MC6821_CR_IRQ1        (1<<7)

// aliases
#define MC6821_CR_C2_B3       MC6821_CR_C2_IRQ     // for output mode
#define MC6821_CR_C2_SET      MC6821_CR_C2_LTOH    // for output mode

// I/O port state
typedef struct {
//    uint8_t inpr;
    uint8_t outr;
    uint8_t ddr;
    uint8_t pins;
    uint8_t ctrl;
} mc6821_port_t;

// mc6821 state
typedef struct {
    mc6821_port_t pa;
    mc6821_port_t pb;
    uint64_t pins;
} mc6821_t;

// extract 8-bit data bus from 64-bit pins
#define MC6821_GET_DATA(p) ((uint8_t)((p)>>16))
// merge 8-bit data bus value into 64-bit pins
#define MC6821_SET_DATA(p,d) {p=(((p)&~0xFF0000ULL)|(((d)<<16)&0xFF0000ULL));}
// extract port A pins
#define MC6821_GET_PA(p) ((uint8_t)((p)>>48))
// extract port B pins
#define MC6821_GET_PB(p) ((uint8_t)((p)>>56))
// merge port A pins into pin mask
#define MC6821_SET_PA(p,a) {p=((p)&0xFF00FFFFFFFFFFFFULL)|(((a)&0xFFULL)<<48);}
// merge port B pins into pin mask
#define MC6821_SET_PB(p,b) {p=((p)&0x00FFFFFFFFFFFFFFULL)|(((b)&0xFFULL)<<56);}
// merge port A and B pins into pin mask
#define MC6821_SET_PAB(p,a,b) {p=((p)&0x0000FFFFFFFFFFFFULL)|(((a)&0xFFULL)<<48)|(((b)&0xFFULL)<<56);}

// initialize a new mc6821_t instance
void mc6821_init(mc6821_t* c);
// reset an existing mc6821_t instance
void mc6821_reset(mc6821_t* c);
// tick the mc6821_t instance
uint64_t mc6821_tick(mc6821_t* c, uint64_t pins);

#ifdef __cplusplus
} // extern "C"
#endif

/*-- IMPLEMENTATION ----------------------------------------------------------*/
#ifdef CHIPS_IMPL
#include <string.h>
#ifndef CHIPS_ASSERT
    #include <assert.h>
    #define CHIPS_ASSERT(c) assert(c)
#endif

static void _mc6821_init_port(mc6821_port_t* p) {
    p->outr = 0;
    p->ddr = 0;
    p->pins = 0;
    p->ctrl = 0;
}

void mc6821_init(mc6821_t* c) {
    CHIPS_ASSERT(c);
    memset(c, 0, sizeof(*c));
    _mc6821_init_port(&c->pa);
    _mc6821_init_port(&c->pb);
}

void mc6821_reset(mc6821_t* c) {
    CHIPS_ASSERT(c);
    memset(c, 0, sizeof(*c));
    _mc6821_init_port(&c->pa);
    _mc6821_init_port(&c->pb);
}

/*--- port implementation ---*/
static inline void _mc6821_read_port_pins(mc6821_t* c, uint64_t pins) {
    /* check CA1/CA2/CB1/CB2 triggered */
    bool new_ca1 = 0 != (pins & MC6821_CA1);
    bool new_ca2 = 0 != (pins & MC6821_CA2);
    bool new_cb1 = 0 != (pins & MC6821_CB1);
    bool new_cb2 = 0 != (pins & MC6821_CB2);

    bool old_ca1 = 0 != (c->pins & MC6821_CA1);
    bool old_ca2 = 0 != (c->pins & MC6821_CA2);
    bool old_cb1 = 0 != (c->pins & MC6821_CB1);
    bool old_cb2 = 0 != (c->pins & MC6821_CB2);

    if (new_ca1 != old_ca1) {
        if (new_ca1 && (c->pa.ctrl & MC6821_CR_C1_LTOH)) // set on low to high transition
            c->pa.ctrl |= MC6821_CR_IRQ1;
        if (!new_ca1 && !(c->pa.ctrl & MC6821_CR_C1_LTOH)) // set on high to low transition
            c->pa.ctrl |= MC6821_CR_IRQ1;
    }

    if (new_cb1 != old_cb1) {
        if (new_cb1 && (c->pb.ctrl & MC6821_CR_C1_LTOH)) // set on low to high transition
            c->pb.ctrl |= MC6821_CR_IRQ1;
        if (!new_cb1 && !(c->pb.ctrl & MC6821_CR_C1_LTOH)) // set on high to low transition
            c->pb.ctrl |= MC6821_CR_IRQ1;
    }

    if (c->pa.ctrl & MC6821_CR_C2_OUT) {
        c->pa.ctrl &= ~MC6821_CR_IRQ2; // irq2 is set to 0 on output mode
    } else {
        if (new_ca2 != old_ca2) {
            if (new_ca2 && (c->pa.ctrl & MC6821_CR_C2_LTOH)) // set on low to high transition
                c->pa.ctrl |= MC6821_CR_IRQ2;
            if (!new_ca2 && !(c->pa.ctrl & MC6821_CR_C2_LTOH)) // set on high to low transition
                c->pa.ctrl |= MC6821_CR_IRQ2;
        }
    }

    if (c->pb.ctrl & MC6821_CR_C2_OUT) {
        c->pb.ctrl &= ~MC6821_CR_IRQ2; // irq2 is set to 0 on output mode
    } else {
        if (new_cb2 != old_cb2 && !(c->pb.ctrl & MC6821_CR_C2_OUT)) {
            if (new_cb2 && (c->pb.ctrl & MC6821_CR_C2_LTOH)) // set on low to high transition
                c->pb.ctrl |= MC6821_CR_IRQ2;
            if (!new_cb2 && !(c->pb.ctrl & MC6821_CR_C2_LTOH)) // set on high to low transition
                c->pb.ctrl |= MC6821_CR_IRQ2;
        }
    }

    // TODO CA2 CB2

    c->pa.pins = MC6821_GET_PA(pins) & ~c->pa.ddr;
    c->pb.pins = MC6821_GET_PB(pins) & ~c->pb.ddr;
}

static inline uint64_t _mc6821_write_port_pins(mc6821_t* c, uint64_t pins) {
    c->pa.pins |= c->pa.outr & c->pa.ddr;
    c->pb.pins |= c->pb.outr & c->pb.ddr;
    MC6821_SET_PAB(pins, c->pa.pins, c->pb.pins);

    if ((c->pa.ctrl & (MC6821_CR_C2_OUT|MC6821_CR_C2_SET)) == (MC6821_CR_C2_OUT|MC6821_CR_C2_SET)) // C2 is out, set mode
        if (c->pa.ctrl & MC6821_CR_C2_B3) // on
            pins |= MC6821_CA2;
        else
            pins &= ~MC6821_CA2;

    if ((c->pb.ctrl & (MC6821_CR_C2_OUT|MC6821_CR_C2_SET)) == (MC6821_CR_C2_OUT|MC6821_CR_C2_SET)) // C2 is out, set mode
        if (c->pb.ctrl & MC6821_CR_C2_B3) // on
            pins |= MC6821_CB2;
        else
            pins &= ~MC6821_CB2;

    return pins;
}

static uint64_t _mc6821_update_irq(mc6821_t* c, uint64_t pins) {
    pins &= ~(MC6821_IRQA|MC6821_IRQB|MC6821_IRQ);

    if ((c->pa.ctrl & MC6821_CR_IRQ1) && (c->pa.ctrl & MC6821_CR_C1_IRQ))
        pins |= MC6821_IRQA;

    if ((c->pb.ctrl & MC6821_CR_IRQ1) && (c->pb.ctrl & MC6821_CR_C1_IRQ))
        pins |= MC6821_IRQB;

    if ((c->pa.ctrl & MC6821_CR_IRQ2) && (c->pa.ctrl & MC6821_CR_C2_IRQ) && !(c->pa.ctrl & MC6821_CR_C2_OUT))
        pins |= MC6821_IRQA;

    if ((c->pb.ctrl & MC6821_CR_IRQ2) && (c->pb.ctrl & MC6821_CR_C2_IRQ) && !(c->pb.ctrl & MC6821_CR_C2_OUT))
        pins |= MC6821_IRQB;

    if (pins & (MC6821_IRQA|MC6821_IRQB))
        pins |= MC6821_IRQ;

    return pins;
}

/* perform a tick */
static uint64_t _mc6821_tick(mc6821_t* c, uint64_t pins) {
    _mc6821_read_port_pins(c, pins);
    //_m6522_update_cab(c); // TODO
    pins = _mc6821_update_irq(c, pins);
    pins = _mc6821_write_port_pins(c, pins);
    return pins;
}

/* read a register */
static uint8_t _mc6821_read(mc6821_t* c, uint8_t addr) {
    uint8_t data = 0;
    switch (addr) {
        case MC6821_REG_CRA:
            data = c->pa.ctrl;
            c->pa.ctrl &= ~(MC6821_CR_IRQ1|MC6821_CR_IRQ2); // clear irq on read
            break;
        case MC6821_REG_CRB:
            data = c->pb.ctrl;
            c->pb.ctrl &= ~(MC6821_CR_IRQ1|MC6821_CR_IRQ2); // clear irq on read
            break;
        case MC6821_REG_PRA:
            if (c->pa.ctrl & MC6821_CR_PR)
                data = c->pa.pins;
            else
                data = c->pa.ddr;
            break;
        case MC6821_REG_PRB:
            if (c->pb.ctrl & MC6821_CR_PR)
                data = c->pb.pins;
            else
                data = c->pb.ddr;
            break;
    }
    return data;
}

/* write a register */
static void _mc6821_write(mc6821_t* c, uint8_t addr, uint8_t data) {
    switch (addr) {
        case MC6821_REG_CRA:
            data &= ~(MC6821_CR_IRQ1|MC6821_CR_IRQ2); // highest two bits are not writeable
            data |= c->pa.ctrl & (MC6821_CR_IRQ1|MC6821_CR_IRQ2);
            if ((data & MC6821_CR_C2_OUT) && !(data & MC6821_CR_C2_SET)) {
                assert(false); // this mode is not supported currently
            }
            c->pa.ctrl = data;
            break;
        case MC6821_REG_CRB:
            data &= ~(MC6821_CR_IRQ1|MC6821_CR_IRQ2); // highest two bits are not writeable
            data |= c->pb.ctrl & (MC6821_CR_IRQ1|MC6821_CR_IRQ2);
            if ((data & MC6821_CR_C2_OUT) && !(data & MC6821_CR_C2_SET)) {
                assert(false); // this mode is not supported currently
            }
            c->pb.ctrl = data;
            break;
        case MC6821_REG_PRA:
            if (c->pa.ctrl & MC6821_CR_PR)
                c->pa.outr = data;
            else
                c->pa.ddr = data;
            break;
        case MC6821_REG_PRB:
            if (c->pb.ctrl & MC6821_CR_PR)
                c->pb.outr = data;
            else
                c->pb.ddr = data;
            break;
    }
}

uint64_t mc6821_tick(mc6821_t* c, uint64_t pins) {
    if (pins & MC6821_CS) {
        uint8_t addr = pins & MC6821_RS_PINS;
        if (pins & MC6821_RW) {
            uint8_t data = _mc6821_read(c, addr);
            MC6821_SET_DATA(pins, data);
        } else {
            uint8_t data = MC6821_GET_DATA(pins);
            _mc6821_write(c, addr, data);
        }
    }
    /* FIXME: move tick above read/write? */
    pins = _mc6821_tick(c, pins);
    c->pins = pins;
    return pins;
}

#endif /* CHIPS_IMPL */
