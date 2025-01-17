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
    chips_audio_descc_t audio;
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

    kbd_t kbd;                  // keyboard matrix state
    mem_t mem_cpu;              // CPU-visible memory mapping
    mem_t mem_vdg;              // VDG-visible memory mapping
    bool valid;
    chips_debug_t debug;

    uint8_t ram[1<<16];         // general ram
    uint8_t rom_bios[0x1000];   // 1 KB BIOS ROM image
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
int32_t mp1000_exec(mp1000_t* sys, uint32_t micro_seconds);
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

#endif /* CHIPS_IMPL */

