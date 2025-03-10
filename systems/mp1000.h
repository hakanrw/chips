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

#define MP1000_FREQUENCY (894886)              // clock frequency in Hz
#define MP1000_MAX_AUDIO_SAMPLES (1024)        // TODO: max number of audio samples in internal sample buffer
#define MP1000_DEFAULT_AUDIO_SAMPLES (128)     // TODO: default number of samples in internal sample buffer

// special joystick bits
#define MP1000_LJOY_0     (0x80)
#define MP1000_LJOY_1     (0x81)
#define MP1000_LJOY_2     (0x82)
#define MP1000_LJOY_3     (0x83)
#define MP1000_LJOY_4     (0x84)
#define MP1000_LJOY_5     (0x85)
#define MP1000_LJOY_6     (0x86)
#define MP1000_LJOY_7     (0x87)
#define MP1000_LJOY_8     (0x88)
#define MP1000_LJOY_9     (0x89)
#define MP1000_LJOY_CLR   (0x8A)
#define MP1000_LJOY_ENTER (0x8B)
#define MP1000_LJOY_DOWN  (0x8C)
#define MP1000_LJOY_RIGHT (0x8D)
#define MP1000_LJOY_UP    (0x8E)
#define MP1000_LJOY_LEFT  (0x8F)

#define MP1000_RJOY_0     (0x90)
#define MP1000_RJOY_1     (0x91)
#define MP1000_RJOY_2     (0x92)
#define MP1000_RJOY_3     (0x93)
#define MP1000_RJOY_4     (0x94)
#define MP1000_RJOY_5     (0x95)
#define MP1000_RJOY_6     (0x96)
#define MP1000_RJOY_7     (0x97)
#define MP1000_RJOY_8     (0x98)
#define MP1000_RJOY_9     (0x99)
#define MP1000_RJOY_CLR   (0x9A)
#define MP1000_RJOY_ENTER (0x9B)
#define MP1000_RJOY_DOWN  (0x9C)
#define MP1000_RJOY_RIGHT (0x9D)
#define MP1000_RJOY_UP    (0x9E)
#define MP1000_RJOY_LEFT  (0x9F)

// special keyboard keys
#define MP1000_KEY_SPACE  (0x20)    // space
#define MP1000_KEY_RETURN (0x0D)    // return
#define MP1000_KEY_CTRL   (0x0E)    // ctrl
#define MP1000_KEY_BREAK  (0x03)    // break
#define MP1000_KEY_ESC    (0x03)    // escape
#define MP1000_KEY_RUBOUT (0x01)    // backspace

#define MP1000_VDG_AG_LAG_TICKS (2000)

// config parameters for mp1000_init()
typedef struct {
    chips_debug_t debug;
    chips_audio_desc_t audio;
    // ROM images
    struct {
        chips_range_t bios;
        chips_range_t basic;
        chips_range_t cart;
    } roms;
} mp1000_desc_t;

// MP1000 emulator state
typedef struct {
    mc6800_t cpu;
    mc6847_t vdg;
    mc6821_t pia;
    mc6821_t piam;
    uint64_t pins;

    bool io_mapped;

    kbd_t joy;                  // 2 controllers matrix state
    kbd_t kbd;                  // keyboard matrix state
    mem_t mem_cpu;              // CPU-visible memory mapping
    mem_t mem_vdg;              // VDG-visible memory mapping
    bool valid;
    chips_debug_t debug;
    uint32_t ticks;
    uint32_t vdg_ag_countdown;
    bool vdg_last_ag;
    bool cb1;

    struct {
        chips_audio_callback_t callback;
        int num_samples;
        int sample_pos;
        float sample_buffer[MP1000_MAX_AUDIO_SAMPLES];
    } audio;

    uint8_t ram[1<<16];         // general ram
    uint8_t rom_bios[0x0800];   // 2 KB BIOS ROM image
    uint8_t rom_basic[0x1000];  // 2 KB BASIC ROM image (optional)
    uint8_t rom_cart[0x2000];   // 2 KB cartridge image (optional)

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
// save a snapshot, patches pointers to zero and offsets, returns snapshot version
uint32_t mp1000_save_snapshot(mp1000_t* sys, mp1000_t* dst);
// load a snapshot, returns false if snapshot versions don't match
bool mp1000_load_snapshot(mp1000_t* sys, uint32_t version, mp1000_t* src);
// tick MP1000 instance for a given number of microseconds, return number of ticks executed
uint32_t mp1000_exec(mp1000_t* sys, uint32_t micro_seconds);
// load a cartridge
bool mp1000_insert_cart(mp1000_t* sys, chips_range_t data);
// remove cartridge
bool mp1000_remove_cart(mp1000_t* sys);
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

static uint64_t _mp1000_vdg_fetch(uint64_t pins, void* user_data);
static void _mp1000_init_key_map(mp1000_t* sys);
static void _mp1000_init_memory_map(mp1000_t* sys);

#define _MP1000_DEFAULT(val,def) (((val) != 0) ? (val) : (def))

void mp1000_init(mp1000_t* sys, const mp1000_desc_t* desc) {
    CHIPS_ASSERT(sys && desc);
    if (desc->debug.callback.func) { CHIPS_ASSERT(desc->debug.stopped); }

    memset(sys, 0, sizeof(mp1000_t));
    sys->valid = true;
    sys->debug = desc->debug;
    sys->audio.callback = desc->audio.callback;
    sys->audio.num_samples = _MP1000_DEFAULT(desc->audio.num_samples, MP1000_DEFAULT_AUDIO_SAMPLES);
    CHIPS_ASSERT(sys->audio.num_samples <= MP1000_MAX_AUDIO_SAMPLES);
    CHIPS_ASSERT(desc->roms.bios.ptr && (desc->roms.bios.size == sizeof(sys->rom_bios)));
    memcpy(sys->rom_bios, desc->roms.bios.ptr, sizeof(sys->rom_bios));

    memset(sys->rom_basic, 0, sizeof(sys->rom_basic));
    if (desc->roms.basic.ptr != NULL) {
        CHIPS_ASSERT(desc->roms.basic.size == sizeof(sys->rom_basic));
        memcpy(sys->rom_basic, desc->roms.basic.ptr, sizeof(sys->rom_basic));
    }

    memset(sys->rom_cart, 0, sizeof(sys->rom_cart));
    if (desc->roms.cart.ptr != NULL) {
        bool cart_2k = desc->roms.cart.size == 2048;
        bool cart_4k = desc->roms.cart.size == 4096;
        bool cart_8k = desc->roms.cart.size == 8192;
        CHIPS_ASSERT(cart_2k || cart_4k || cart_8k);
        memcpy(sys->rom_cart, desc->roms.cart.ptr, desc->roms.cart.size);
    }

    // initialize the hardware
    sys->io_mapped = true;
    sys->cb1 = false;

    sys->pins = mc6800_init(&sys->cpu);
    mc6847_init(&sys->vdg, &(mc6847_desc_t) {
        .tick_hz = MP1000_FREQUENCY,
        .framebuffer = {
            .ptr = sys->fb,
            .size = sizeof(sys->fb),
        },
        .user_data = sys,
        .fetch_cb = _mp1000_vdg_fetch,
    });
    mc6821_init(&sys->pia);
    mc6821_init(&sys->piam);
    _mp1000_init_key_map(sys);
    _mp1000_init_memory_map(sys);
}

void mp1000_discard(mp1000_t* sys) {
    CHIPS_ASSERT(sys && sys->valid);
    sys->valid = false;
}

void mp1000_reset(mp1000_t* sys) {
    CHIPS_ASSERT(sys && sys->valid);
    sys->pins |= MC6800_RESET;
    mc6821_reset(&sys->pia);
    mc6821_reset(&sys->piam);
    mc6847_reset(&sys->vdg);
}

static uint64_t _mp1000_tick(mp1000_t* sys, uint64_t pins) {
    // tick the CPU
    pins = mc6800_tick(&sys->cpu, pins);
    /*const */uint16_t addr = MC6800_GET_ADDR(pins);
    sys->ticks++;

    //printf("PC %04x SP %04x   [%02x %02x]\n", sys->cpu.PC, sys->cpu.SP, mem_rd(&sys->mem_cpu, 0x01c9), mem_rd(&sys->mem_cpu, 0x01ca));

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
                pia_pins |= MC6821_CS;
            }
            else if (addr >= 0x6000 && addr <= 0x63FF) {
                piam_pins |= MC6821_CS;
            }
            else if (addr >= 0x6400 && addr <= 0x67FF) {
                // Manual pg.23: External IO devices
                // TODO
                //assert(false);
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
        const uint8_t pa = ~kbd_scan_columns(&sys->joy); // TODO
        const uint8_t pb = 0x00; // TODO
        MC6821_SET_PAB(pia_pins, pa, pb);

        if (sys->cb1) pia_pins |= MC6821_CB1;

        pia_pins = mc6821_tick(&sys->pia, pia_pins);
        const uint8_t joy_lines = (~MC6821_GET_PB(pia_pins))&0xF;
        //fprintf(stderr, "joy lines: %8b\n", joy_lines);
        kbd_set_active_lines(&sys->joy, joy_lines);

        if (pia_pins & MC6821_IRQ) {
            pins |= MC6821_IRQ;
        }
        if ((pia_pins & (MC6821_CS|MC6821_RW)) == (MC6821_CS|MC6821_RW)) {
            pins = MC6800_COPY_DATA(pins, pia_pins);
        }

        vdg_pins &= ~(MC6847_AG|MC6847_AS|MC6847_INTEXT|MC6847_INV|MC6847_GM0);
        vdg_pins |= (MC6847_GM2|MC6847_GM1);

        if (pia_pins & MC6821_PB6) {
            vdg_pins |= MC6847_GM0;
        }

        /* TODO: explain/implement this better
         *
         * ! Arbitrary lag !
         * due to strict timing issues, i implemented this hack which
         * waits for some more time before switching to full graphics mode.
         * if we do not have this lag/countdown, the score tab at the top
         * will not be shown, as full graphics switch happens too early.
         * TODO: investigate
         */
        if (pia_pins & MC6821_PB7) {
            if (!sys->vdg_last_ag) sys->vdg_ag_countdown = MP1000_VDG_AG_LAG_TICKS;
            if (sys->vdg_ag_countdown <= 0) vdg_pins |= MC6847_AG;
            sys->vdg_last_ag = true;
        }
        else {
            sys->vdg_last_ag = false;
        }

        if (sys->vdg_ag_countdown > 0) sys->vdg_ag_countdown--;
    }

    /* tick PIA-IM
    */
    {
        const uint8_t pa = ~kbd_scan_columns(&sys->kbd); // TODO
        const uint8_t pb = 0x00; // TODO
        MC6821_SET_PAB(piam_pins, pa, pb);

        piam_pins = mc6821_tick(&sys->piam, piam_pins);
        const uint8_t kbd_lines = 1<<((MC6821_GET_PB(piam_pins))&0x7); // rightmost 3 lines
        kbd_set_active_lines(&sys->kbd, kbd_lines);

        if (piam_pins & MC6821_IRQ) {
            pins |= MC6821_IRQ;
        }
	if ((piam_pins & (MC6821_CS|MC6821_RW)) == (MC6821_CS|MC6821_RW)) {
            pins = MC6800_COPY_DATA(pins, piam_pins);
        //fprintf(stderr, "pia: %8b\n", MC6821_GET_PB(piam_pins));
        //fprintf(stderr, "kbd lines: %8b\n", kbd_lines);
        //fprintf(stderr,"piam_a %b\n", sys->piam.pa.ctrl);
        //fprintf(stderr,"piam_a ddr %b\n", sys->piam.pa.ddr);
        //fprintf(stderr,"piam_b %b\n", sys->piam.pb.ctrl);
        //fprintf(stderr,"piam_b ddr %b\n", sys->piam.pb.ddr);
        }
    }

    /* tick the VDG display chip (4x freq.) TODO
    */
    /*for (int i = 0; i < 3; i++)*/
    {
        vdg_pins = mc6847_tick(&sys->vdg, vdg_pins);

        sys->cb1 = !(vdg_pins & MC6847_FS);
    }

    if (mem_access) {
        if (addr >= 0x400 && addr <= 0x1FFF) {
            addr = addr % 0x400; // wrap memory 0x0400-0x1FFF
        }
        if (pins & MC6800_RW) {
            // memory read
            MC6800_SET_DATA(pins, mem_rd(&sys->mem_cpu, addr));
        }
        else {
            // memory write
            if (addr >= 0x4000 && addr <= 0x5FFF)
                assert(false); // rom write
            mem_wr(&sys->mem_cpu, addr, MC6800_GET_DATA(pins));
        }
    }

    return pins;
}

static uint64_t _mp1000_vdg_fetch(uint64_t pins, void* user_data) {
    mp1000_t* sys = (mp1000_t*) user_data;
    /*
        Fetch data into the VDG.
    */
    uint16_t addr = MC6847_GET_ADDR(pins);
    pins &= ~(MC6847_AS|MC6847_INV);

    if (pins & MC6847_AG) { // full graphics
        uint8_t x = addr % 32;
        uint8_t y = addr / 32;
        uint8_t yo = y / 16;

        uint8_t objmap = mem_rd(&sys->mem_cpu, yo*32+x);
        uint8_t obj = objmap & 31;
        if (yo == 11) obj = x;
        //if (obj == 23) obj = (sys->ticks / 50000) % 32;
        uint8_t data = mem_rd(&sys->mem_vdg, obj*16 + y%16);
        //if (obj != 20) data = 0xFF;
//        if (obj == 23) {
//            for (int i = 0; i < 16; i++) printf("%02x ", mem_rd(&sys->mem_vdg, 23*16+i));
//            printf("\n");
//        }
        MC6847_SET_DATA(pins, /*x==0 ? (yo%2?0xFF:00):*/data);
    }
    else {
        uint8_t data = mem_rd(&sys->mem_vdg, addr);
        MC6847_SET_DATA(pins, data);

        if (data & 128) { // semigraphics
            pins |= MC6847_AS;
        }
        else { // internal alphanum
            if (data & 64) pins |= MC6847_INV;
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
    mem_map_rom(&sys->mem_cpu, 0, 0x6800, 0x1000, sys->rom_basic);
    mem_map_rom(&sys->mem_cpu, 0, 0x8000, 0x2000, sys->rom_cart);

    mem_map_rom(&sys->mem_cpu, 0, 0xF800, 0x0800, sys->rom_bios);

    /* setup the separate VDG map
    */
    mem_map_rom(&sys->mem_vdg, 0, 0x0000, 0x400, sys->ram+0x200); // 0x400 should actually be 0x200
}

static void _mp1000_init_key_map(mp1000_t* sys) {
    // Many thanks:
    // https://github.com/mamedev/mame/blob/9a8d04701166990c91cad6b13d2814ada60f6c1e/src/mame/apf/apf.cpp#L407-L445

    /* Left and right controllers */

    kbd_init(&sys->joy, 1);

    kbd_register_key(&sys->joy, MP1000_RJOY_1   , 0, 0, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_0   , 1, 0, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_4   , 2, 0, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_7   , 3, 0, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_1   , 4, 0, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_0   , 5, 0, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_4   , 6, 0, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_7   , 7, 0, 0);

    kbd_register_key(&sys->joy, MP1000_RJOY_DOWN , 0, 1, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_RIGHT, 1, 1, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_UP   , 2, 1, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_LEFT , 3, 1, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_DOWN , 4, 1, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_RIGHT, 5, 1, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_UP   , 6, 1, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_LEFT , 7, 1, 0);

    kbd_register_key(&sys->joy, MP1000_RJOY_3   , 0, 2, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_CLR , 1, 2, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_6   , 2, 2, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_9   , 3, 2, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_3   , 4, 2, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_CLR , 5, 2, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_6   , 6, 2, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_9   , 7, 2, 0);

    kbd_register_key(&sys->joy, MP1000_RJOY_2    , 0, 3, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_ENTER, 1, 3, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_5    , 2, 3, 0);
    kbd_register_key(&sys->joy, MP1000_RJOY_8    , 3, 3, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_2    , 4, 3, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_ENTER, 5, 3, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_5    , 6, 3, 0);
    kbd_register_key(&sys->joy, MP1000_LJOY_8    , 7, 3, 0);

    /* Imagination machine keyboard */

    kbd_init(&sys->kbd, 1);

    const char* keymap =
        // no shift
        "XZQ2A1WS"
        "CVR3F4ED"
        "NBT6G5YH"
        "M,I7K8UJ"
        "/.O0L9P;"
        " :   -  "
        "        "

        // shift
        "   \" !  "
        "   # $  "
        "^  & %  "
        "]< '[(  "
        "?>_ \\)@+"
        " *   =  "
        "        ";
    CHIPS_ASSERT(strlen(keymap) == 112);
    // shift is column 0, line 6
    kbd_register_modifier(&sys->kbd, 0, 0, 6);
    // ctrl is column 2, line 6
    kbd_register_modifier(&sys->kbd, 1, 2, 6);
    for (int shift = 0; shift < 2; shift++) {
        for (int column = 0; column < 8; column++) {
            for (int line = 0; line < 7; line++) {
                int c = keymap[shift*56 + line*8 + column];
                if (c != ' ') {
                    kbd_register_key(&sys->kbd, c, column, line, shift?(1<<0):0);
                }
            }
        }
    }

    // special keys
    kbd_register_key(&sys->kbd, MP1000_KEY_SPACE , 0, 5, 0);
    kbd_register_key(&sys->kbd, MP1000_KEY_RETURN, 2, 5, 0);
    kbd_register_key(&sys->kbd, MP1000_KEY_CTRL  , 2, 6, 0);
    kbd_register_key(&sys->kbd, MP1000_KEY_ESC   , 1, 6, 0);
    kbd_register_key(&sys->kbd, MP1000_KEY_RUBOUT, 7, 5, 0);
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
        .screen = {
            .x = 0,
            .y = 0,
            .width = MC6847_DISPLAY_WIDTH,
            .height = MC6847_DISPLAY_HEIGHT,
        },
        .palette = {
            .ptr = sys ? sys->vdg.hwcolors : 0,
            .size = MC6847_HWCOLOR_NUM * sizeof(uint32_t)
        }

    };
    CHIPS_ASSERT(((sys == 0) && (res.frame.buffer.ptr == 0)) || ((sys != 0) && (res.frame.buffer.ptr != 0)));
    return res;
}

uint32_t mp1000_save_snapshot(mp1000_t* sys, mp1000_t* dst) {
    CHIPS_ASSERT(sys && dst);
    *dst = *sys;
    //chips_debug_snapshot_onsave(&dst->debug);
    chips_audio_callback_snapshot_onsave(&dst->audio.callback);
    mc6800_snapshot_onsave(&dst->cpu);
    mc6847_snapshot_onsave(&dst->vdg);
    mem_snapshot_onsave(&dst->mem_cpu, sys);
    mem_snapshot_onsave(&dst->mem_vdg, sys);
    return MP1000_SNAPSHOT_VERSION;
}

bool mp1000_load_snapshot(mp1000_t* sys, uint32_t version, mp1000_t* src) {
    CHIPS_ASSERT(sys && src);
    if (version != MP1000_SNAPSHOT_VERSION) {
        return false;
    }
    static mp1000_t im;
    im = *src;
    //chips_debug_snapshot_onload(&im.debug, &sys->debug);
    chips_audio_callback_snapshot_onload(&im.audio.callback, &sys->audio.callback);
    mc6800_snapshot_onload(&im.cpu, &sys->cpu);
    mc6847_snapshot_onload(&im.vdg, &sys->vdg);
    mem_snapshot_onload(&im.mem_cpu, sys);
    mem_snapshot_onload(&im.mem_vdg, sys);
    *sys = im;
    return true;
}

void mp1000_key_down(mp1000_t* sys, int key_code) {
    CHIPS_ASSERT(sys);
    uint8_t m = 0;

    if (key_code >= 0x80)
        kbd_key_down(&sys->joy, key_code);
    else
        kbd_key_down(&sys->kbd, key_code);
}

void mp1000_key_up(mp1000_t* sys, int key_code) {
    CHIPS_ASSERT(sys);

    if (key_code >= 0x80)
        kbd_key_up(&sys->joy, key_code);
    else
        kbd_key_up(&sys->kbd, key_code);
}

bool mp1000_insert_cart(mp1000_t* sys, chips_range_t data) {
    CHIPS_ASSERT(sys && sys->valid && data.ptr);
    bool cart_2k = data.size == 2048;
    bool cart_4k = data.size == 4096;
    bool cart_8k = data.size == 8192;
    CHIPS_ASSERT(cart_2k || cart_4k || cart_8k);
    memcpy(sys->rom_cart, data.ptr, data.size);

    mp1000_reset(sys);

    return true;
}

bool mp1000_remove_cart(mp1000_t* sys) {
    memset(sys->rom_cart, 0, 0x2000);
    mp1000_reset(sys);

    return true;
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
    kbd_update(&sys->joy, micro_seconds);
    kbd_update(&sys->kbd, micro_seconds);
    return num_ticks;
}

#endif /* CHIPS_IMPL */

