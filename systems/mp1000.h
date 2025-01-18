#pragma once
/*#
    # mp1000.h

    An APF MP-1000 emulator in a C header

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
        your own assert macro (default: assert(c))

    You need to include the following headers before including c64.h:

    - chips/chips_commmon.h
    - chips/mc6800.h
    - chips/mc6847.h
    - chips/mc6821.h
    - chips/kbd.h
    - chips/mem.h
    - chips/clk.h

    ## The APF MP-1000 / APF Imagination Machine

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
#include <stddef.h>
#include <stdalign.h>

#ifdef __cplusplus
extern "C" {
#endif

// bump snapshot version when c64_t memory layout changes
#define MP1000_SNAPSHOT_VERSION (1)

#define MP1000_FREQUENCY (894750)              // clock frequency in Hz
#define MP1000_MAX_AUDIO_SAMPLES (TODO)        // max number of audio samples in internal sample buffer
#define MP1000_DEFAULT_AUDIO_SAMPLES (TODO)     // default number of samples in internal sample buffer

// config parameters for mp1000_init()
typedef struct {
    chips_debug_t debug;
    chips_audio_desc_t audio;
    // ROM images
    struct {
        chips_range_t bios;
        chips_range_t basic;
    } roms;
} mp1000_desc_t;

// MP1000 emulator state
typedef struct {
    mc6800_t cpu;
    mc6847_t vdg;
    uint64_t pins;

    bool io_mapped;

    kbd_t kbd;                  // keyboard matrix state
    mem_t mem_cpu;              // CPU-visible memory mapping
    mem_t mem_vdg;              // VDG-visible memory mapping
    bool valid;
    chips_debug_t debug;

    uint8_t ram[1<<16];         // general ram
    uint8_t rom_bios[0x0800];   // 2 KB BIOS ROM image
    alignas(64) uint8_t fb[MC6847_FRAMEBUFFER_SIZE_BYTES];
} mp1000_t;

// initialize a new MP1000 instance
void mp1000_init(mp1000_t* sys, const mp1000_desc_t* desc);
// discard MP1000 instance
void mp1000_discard(mp1000_t* sys);
// reset MP1000 instance
void mp1000_reset(mp1000_t* sys);
// get framebuffer and display attributes
chips_display_info_t mp1000_display_info(mp1000_t* sys);
// tick MP1000 instance for a given number of microseconds, return number of ticks executed
uint32_t mp1000_exec(mp1000_t* sys, uint32_t micro_seconds);
// send a key-down event to the MP1000
void mp1000_key_down(mp1000_t* sys, int key_code);
// send a key-up event to the MP1000
void mp1000_key_up(mp1000_t* sys, int key_code);

#ifdef __cplusplus
} // extern "C"
#endif

/*-- IMPLEMENTATION ----------------------------------------------------------*/
#ifdef CHIPS_IMPL
#include <string.h> // memcpy, memset
#ifndef CHIPS_ASSERT
    #include <assert.h>
    #define CHIPS_ASSERT(c) assert(c)
#endif

#define _MP1000_SCREEN_WIDTH (256)
#define _MP1000_SCREEN_HEIGHT (192)

static void _mp1000_init_key_map(mp1000_t* sys);
static void _mp1000_init_memory_map(mp1000_t* sys);

#define _MP1000_DEFAULT(val,def) (((val) != 0) ? (val) : (def))

static uint64_t _dumb_cb_fetch(uint64_t pins, void* user_data) {

}

void mp1000_init(mp1000_t* sys, const mp1000_desc_t* desc) {
    CHIPS_ASSERT(sys && desc);
    if (desc->debug.callback.func) { CHIPS_ASSERT(desc->debug.stopped); }

    memset(sys, 0, sizeof(mp1000_t));
    sys->valid = true;
    sys->debug = desc->debug;
    CHIPS_ASSERT(desc->roms.bios.ptr && (desc->roms.bios.size == sizeof(sys->rom_bios)));
    memcpy(sys->rom_bios, desc->roms.bios.ptr, sizeof(sys->rom_bios));

    // initialize the hardware
    sys->io_mapped = true;

    sys->pins = mc6800_init(&sys->cpu);
    mc6847_init(&sys->vdg, &(mc6847_desc_t) {
        .tick_hz = MP1000_FREQUENCY,
        .framebuffer = {
            .ptr = sys->fb,
            .size = sizeof(sys->fb),
        },
        .user_data = sys,
        .fetch_cb = _dumb_cb_fetch,
    });
    _mp1000_init_key_map(sys);
    _mp1000_init_memory_map(sys);
}

static uint64_t _mp1000_tick(mp1000_t* sys, uint64_t pins) {
    // tick the CPU
    pins = mc6800_tick(&sys->cpu, pins);
    const uint16_t addr = MC6800_GET_ADDR(pins);

    // those pins are set each tick by the PIAs and VDG
    pins &= ~(MC6800_IRQ|MC6800_NMI);

    /*  address decoding

        When the VMA pin is active (during bad lines), no CPU/chip
        communication takes place starting with the first read access.
    */
    bool mem_access = false;
    uint64_t vdg_pins = pins & MC6800_PIN_MASK;
    uint64_t pia_pins = pins & MC6800_PIN_MASK;
    uint64_t piam_pins = pins & MC6800_PIN_MASK;
    // Memory access (VMA is set)
    if (pins & MC6800_VMA) {
        if (sys->io_mapped) {
            if (addr >= 0x2000 && addr <= 0x3FFF) {
                pia_pins |= 0;//M6526_CS; // TODO: Fix for PIA
            }
            else if (addr >= 0x6000 && addr <= 0x63FF) {
                piam_pins |= 0;//M6526_CS; // TODO: Fix for PIA
            }
            else if (addr >= 0x6400 && addr <= 0x67FF) {
                // Manual pg.23: External IO devices
                // TODO
            }
            else {
                mem_access = true;
            }
        }
        else {
            mem_access = true;
        }
    }

    /* tick PIA
    */
    {
        // this has direct effect on vdg's graphics mode
        // TODO
        // for now, force alphanumeric mode
        vdg_pins &= ~(MC6847_AG|MC6847_AS|MC6847_INTEXT|MC6847_INV);
    }

    /* tick the VDG display chip (4x freq.)
    */
    for (int i = 0; i < 4; i++)
    {
        vdg_pins = mc6847_tick(&sys->vdg, vdg_pins);
        uint8_t vres = mem_rd(&sys->mem_vdg, MC6847_GET_ADDR(vdg_pins));
        MC6847_SET_DATA(vdg_pins, vres);
    }

    if (mem_access) {
        if (pins & MC6800_RW) {
            // memory read
            MC6800_SET_DATA(pins, mem_rd(&sys->mem_cpu, addr));
        }
        else {
            // memory write
            mem_wr(&sys->mem_cpu, addr, MC6800_GET_DATA(pins));
        }
    }

    return pins;
}

static void _mp1000_init_memory_map(mp1000_t* sys) {
    // separate memory mapping for CPU and VDG
    mem_init(&sys->mem_cpu);
    mem_init(&sys->mem_vdg);

    int i;
    for (i = 0; i < (1<<16); i++) {
        sys->ram[i] = 0x00;
    }
    CHIPS_ASSERT(i == 0x10000);

    /* setup the initial CPU memory map
    */
    mem_map_ram(&sys->mem_cpu, 0, 0x0000, 0x2000, sys->ram);
    mem_map_ram(&sys->mem_cpu, 0, 0xA000, 0x2000, sys->ram+0xA000);
    mem_map_rom(&sys->mem_cpu, 0, 0x4000, 0x0800, sys->rom_bios);
    mem_map_rom(&sys->mem_cpu, 0, 0xF800, 0x0800, sys->rom_bios);

    /* setup the separate VDG map
    */
    mem_map_rom(&sys->mem_vdg, 0, 0x0000, 0x400, sys->ram+0x200); // 0x400 should actually be 0x200
}

static void _mp1000_init_key_map(mp1000_t* sys) {

}

chips_display_info_t mp1000_display_info(mp1000_t* sys) {
    chips_display_info_t res = {
        .frame = {
            .dim = {
                .width = MC6847_FRAMEBUFFER_WIDTH,
                .height = MC6847_FRAMEBUFFER_HEIGHT,
            },
            .bytes_per_pixel = 1,
            .buffer = {
                .ptr = sys ? sys->fb : 0,
                .size = MC6847_FRAMEBUFFER_SIZE_BYTES,
            }
        },
        //.palette = m_dbg_palette(),
    };
    //if (sys) {
    //    res.screen = mc6847_screen(&sys->vdg);
    //}
    //else {
        res.screen = (chips_rect_t){
            .x = 0,
            .y = 0,
            .width = _MP1000_SCREEN_WIDTH,
            .height = _MP1000_SCREEN_HEIGHT
        };
    //};
    CHIPS_ASSERT(((sys == 0) && (res.frame.buffer.ptr == 0)) || ((sys != 0) && (res.frame.buffer.ptr != 0)));
    return res;
}

void mp1000_key_down(mp1000_t* sys, int key_code) {

}

void mp1000_key_up(mp1000_t* sys, int key_code) {

}

uint32_t mp1000_exec(mp1000_t* sys, uint32_t micro_seconds) {
    CHIPS_ASSERT(sys && sys->valid);
    uint32_t num_ticks = clk_us_to_ticks(MP1000_FREQUENCY, micro_seconds);
    uint64_t pins = sys->pins;
    if (0 == sys->debug.callback.func) {
        // run without debug callback
        for (uint32_t ticks = 0; ticks < num_ticks; ticks++) {
            pins = _mp1000_tick(sys, pins);
        }
    }
    else {
        // run with debug callback
        for (uint32_t ticks = 0; (ticks < num_ticks) && !(*sys->debug.stopped); ticks++) {
            pins = _mp1000_tick(sys, pins);
            sys->debug.callback.func(sys->debug.callback.user_data, pins);
        }
    }
    sys->pins = pins;
    kbd_update(&sys->kbd, micro_seconds);
    return num_ticks;
}

#endif /* CHIPS_IMPL */

