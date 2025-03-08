#pragma once
/*#
    # ui_mp1000.h

    Integrated debugging UI for mp1000.h

    Do this:
    ~~~C
    #define CHIPS_UI_IMPL
    ~~~
    before you include this file in *one* C++ file to create the
    implementation.

    Optionally provide the following macros with your own implementation

    ~~~C
    CHIPS_ASSERT(c)
    ~~~
        your own assert macro (default: assert(c))

    Include the following headers (and their depenencies) before including
    ui_mp1000.h both for the declaration and implementation.

    - mp1000.h
    - mem.h
    - ui_chip.h
    - ui_util.h
    - ui_settings.h
    - ui_mc6800.h
    - ui_mc6821.h
    - ui_mc6847.h
    - ui_audio.h
    - ui_display.h
    - ui_dasm.h
    - ui_dbg.h
    - ui_memedit.h
    - ui_memmap.h
    - ui_kbd.h
    - ui_snapshot.h

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

// reboot callback
typedef void (*ui_mp1000_boot_cb)(mp1000_t* sys);

// setup params for ui_mp1000_init()
typedef struct {
    mp1000_t* mp1000;             // pointer to mp1000_t instance to track
    ui_mp1000_boot_cb boot_cb; // reboot callback function
//    ui_dbg_texture_callbacks_t dbg_texture; // texture create/update/destroy callbacks
//    ui_dbg_debug_callbacks_t dbg_debug;
//    ui_dbg_keys_desc_t dbg_keys;        // user-defined hotkeys for ui_dbg_t
    ui_snapshot_desc_t snapshot;        // snapshot UI setup params
} ui_mp1000_desc_t;

typedef struct {
    mp1000_t* mp1000;
    int dbg_scanline;
    ui_mp1000_boot_cb boot_cb;
    ui_mc6800_t cpu;
    ui_mc6821_t pia[2];
    ui_mc6847_t vdg;
    ui_audio_t audio;
    ui_display_t display;
    ui_kbd_t kbd;
    ui_memmap_t memmap;
    ui_memedit_t memedit[4];
    //ui_dasm_t dasm[4];
    //ui_dbg_t dbg;
    ui_snapshot_t snapshot;
} ui_mp1000_t;

typedef struct {
    ui_display_frame_t display;
} ui_mp1000_frame_t;

void ui_mp1000_init(ui_mp1000_t* ui, const ui_mp1000_desc_t* desc);
void ui_mp1000_discard(ui_mp1000_t* ui);
void ui_mp1000_draw(ui_mp1000_t* ui, const ui_mp1000_frame_t* frame);
chips_debug_t ui_mp1000_get_debug(ui_mp1000_t* ui);
void ui_mp1000_save_settings(ui_mp1000_t* ui, ui_settings_t* settings);
void ui_mp1000_load_settings(ui_mp1000_t* ui, const ui_settings_t* settings);

#ifdef __cplusplus
} /* extern "C" */
#endif

/*-- IMPLEMENTATION (include in C++ source) ----------------------------------*/
#ifdef CHIPS_UI_IMPL
#ifndef __cplusplus
#error "implementation must be compiled as C++"
#endif
#include <string.h> /* memset */
#ifndef CHIPS_ASSERT
    #include <assert.h>
    #define CHIPS_ASSERT(c) assert(c)
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

static void _ui_mp1000_draw_menu(ui_mp1000_t* ui) {
    CHIPS_ASSERT(ui && ui->mp1000 && ui->boot_cb);
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("System")) {
            ui_snapshot_menus(&ui->snapshot);
            if (ImGui::MenuItem("Reset")) {
                mp1000_reset(ui->mp1000);
                //ui_dbg_reset(&ui->dbg);
            }
            if (ImGui::MenuItem("Cold Boot")) {
                ui->boot_cb(ui->mp1000);
                //ui_dbg_reboot(&ui->dbg);
            }
            if (ImGui::MenuItem("Remove Cart")) {
                mp1000_remove_cart(ui->mp1000);
            }
            /*
            if (ImGui::BeginMenu("Joystick")) {
                if (ImGui::MenuItem("None", 0, ui->mp1000->joystick_type == C64_JOYSTICKTYPE_NONE)) {
                    ui->mp1000->joystick_type = C64_JOYSTICKTYPE_NONE;
                }
                if (ImGui::MenuItem("Digital #1", 0, ui->mp1000->joystick_type == C64_JOYSTICKTYPE_DIGITAL_1)) {
                    ui->mp1000->joystick_type = C64_JOYSTICKTYPE_DIGITAL_1;
                }
                if (ImGui::MenuItem("Digital #2", 0, ui->mp1000->joystick_type == C64_JOYSTICKTYPE_DIGITAL_2)) {
                    ui->mp1000->joystick_type = C64_JOYSTICKTYPE_DIGITAL_2;
                }
                ImGui::EndMenu();
            }
            */
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Hardware")) {
            ImGui::MenuItem("Memory Map", 0, &ui->memmap.open);
            ImGui::MenuItem("Keyboard Matrix", 0, &ui->kbd.open);
            ImGui::MenuItem("Audio Output", 0, &ui->audio.open);
            ImGui::MenuItem("Display", 0, &ui->display.open);
            ImGui::MenuItem("MC 6800 (CPU)", 0, &ui->cpu.open);
            ImGui::MenuItem("MC 6821 #1 (PIA)", 0, &ui->pia[0].open);
            ImGui::MenuItem("MC 6821 #2 (PIA-IM)", 0, &ui->pia[1].open);
            ImGui::MenuItem("MC 6847 (VDG)", 0, &ui->vdg.open);
            ImGui::EndMenu();
        }/*
        if (ImGui::BeginMenu("Debug")) {
            ImGui::MenuItem("CPU Debugger", 0, &ui->dbg.ui.open);
            ImGui::MenuItem("Breakpoints", 0, &ui->dbg.ui.breakpoints.open);
            ImGui::MenuItem("Stopwatch", 0, &ui->dbg.ui.stopwatch.open);
            ImGui::MenuItem("Execution History", 0, &ui->dbg.ui.history.open);
            ImGui::MenuItem("Memory Heatmap", 0, &ui->dbg.ui.heatmap.open);
            if (ImGui::BeginMenu("Memory Editor")) {
                ImGui::MenuItem("Window #1", 0, &ui->memedit[0].open);
                ImGui::MenuItem("Window #2", 0, &ui->memedit[1].open);
                ImGui::MenuItem("Window #3", 0, &ui->memedit[2].open);
                ImGui::MenuItem("Window #4", 0, &ui->memedit[3].open);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Disassembler")) {
                ImGui::MenuItem("Window #1", 0, &ui->dasm[0].open);
                ImGui::MenuItem("Window #2", 0, &ui->dasm[1].open);
                ImGui::MenuItem("Window #3", 0, &ui->dasm[2].open);
                ImGui::MenuItem("Window #4", 0, &ui->dasm[3].open);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }*/
        ui_util_options_menu();
        ImGui::EndMainMenuBar();
    }
}

/* keep disassembler layer at the start */
#define _UI_MP1000_MEMLAYER_CPU    (0)     /* CPU visible mapping */
#define _UI_MP1000_MEMLAYER_RAM    (1)     /* RAM blocks */
#define _UI_MP1000_MEMLAYER_ROM    (2)     /* ROM blocks */
#define _UI_MP1000_MEMLAYER_VDG    (3)     /* VIC visible mapping */
#define _UI_MP1000_CODELAYER_NUM   (3)     /* number of valid layers for disassembler */
#define _UI_MP1000_MEMLAYER_NUM    (4)

static const char* _ui_mp1000_memlayer_names[_UI_MP1000_MEMLAYER_NUM] = {
    "CPU Mapped", "RAM Banks", "ROM Banks", "VDG Mapped"
};

static uint8_t _ui_mp1000_mem_read(int layer, uint16_t addr, void* user_data) {
    CHIPS_ASSERT(user_data);
    ui_mp1000_t* ui = (ui_mp1000_t*) user_data;
    mp1000_t* mp1000 = ui->mp1000;
    /*
    switch (layer) {
        case _UI_C64_MEMLAYER_CPU:
            return mem_rd(&mp1000->mem_cpu, addr);
        case _UI_C64_MEMLAYER_RAM:
            return mp1000->ram[addr];
        case _UI_C64_MEMLAYER_ROM:
            if ((addr >= 0xA000) && (addr < 0xC000)) {
                return mp1000->rom_basic[addr - 0xA000];
            }
            else if ((addr >= 0xD000) && (addr < 0xE000)) {
                return mp1000->rom_char[addr - 0xD000];
            }
            else if (addr >= 0xE000) {
                return mp1000->rom_kernal[addr - 0xE000];
            }
            else {
                return 0xFF;
            }
            break;
        case _UI_C64_MEMLAYER_1541:
            if (ui->mp1000->c1541.valid) {
                return mem_rd(&ui->mp1000->c1541.mem, addr);
            }
            else {
                return 0xFF;
            }
            break;
        case _UI_C64_MEMLAYER_VIC:
            return mem_rd(&mp1000->mem_vic, addr);
        case _UI_C64_MEMLAYER_COLOR:
            if ((addr >= 0xD800) && (addr < 0xDC00)) {
                return mp1000->color_ram[addr - 0xD800];
            }
            else {
                return 0xFF;
            }
        default:
            return 0xFF;
    }
    */
    return 0;
}

static void _ui_mp1000_mem_write(int layer, uint16_t addr, uint8_t data, void* user_data) {
    CHIPS_ASSERT(user_data);
    ui_mp1000_t* ui = (ui_mp1000_t*) user_data;
    mp1000_t* mp1000 = ui->mp1000;
/*
    switch (layer) {
        case _UI_C64_MEMLAYER_CPU:
            mem_wr(&mp1000->mem_cpu, addr, data);
            break;
        case _UI_C64_MEMLAYER_RAM:
            mp1000->ram[addr] = data;
            break;
        case _UI_C64_MEMLAYER_ROM:
            if ((addr >= 0xA000) && (addr < 0xC000)) {
                mp1000->rom_basic[addr - 0xA000] = data;
            }
            else if ((addr >= 0xD000) && (addr < 0xE000)) {
                mp1000->rom_char[addr - 0xD000] = data;
            }
            else if (addr >= 0xE000) {
                mp1000->rom_kernal[addr - 0xE000] = data;
            }
            break;
        case _UI_C64_MEMLAYER_1541:
            if (ui->mp1000->c1541.valid) {
                mem_wr(&ui->mp1000->c1541.mem, addr, data);
            }
            break;
        case _UI_C64_MEMLAYER_VIC:
            mem_wr(&mp1000->mem_vic, addr, data);
            break;
        case _UI_C64_MEMLAYER_COLOR:
            if ((addr >= 0xD800) && (addr < 0xDC00)) {
                mp1000->color_ram[addr - 0xD800] = data;
            }
            break;
    }
*/
}

static void _ui_mp1000_update_memmap(ui_mp1000_t* ui) {
    CHIPS_ASSERT(ui && ui->mp1000);
    const mp1000_t* mp1000 = ui->mp1000;
/*
    bool all_ram = (mp1000->cpu_port & (C64_CPUPORT_HIRAM|C64_CPUPORT_LORAM)) == 0;
    bool basic_rom = (mp1000->cpu_port & (C64_CPUPORT_HIRAM|C64_CPUPORT_LORAM)) == (C64_CPUPORT_HIRAM|C64_CPUPORT_LORAM);
    bool kernal_rom = (mp1000->cpu_port & C64_CPUPORT_HIRAM) != 0;
    bool io_enabled = !all_ram && ((mp1000->cpu_port & C64_CPUPORT_CHAREN) != 0);
    bool char_rom = !all_ram && ((mp1000->cpu_port & C64_CPUPORT_CHAREN) == 0);
    ui_memmap_reset(&ui->memmap);
    ui_memmap_layer(&ui->memmap, "IO");
        ui_memmap_region(&ui->memmap, "IO REGION", 0xD000, 0x1000, io_enabled);
    ui_memmap_layer(&ui->memmap, "ROM");
        ui_memmap_region(&ui->memmap, "BASIC ROM", 0xA000, 0x2000, basic_rom);
        ui_memmap_region(&ui->memmap, "CHAR ROM", 0xD000, 0x1000, char_rom);
        ui_memmap_region(&ui->memmap, "KERNAL ROM", 0xE000, 0x2000, kernal_rom);
    ui_memmap_layer(&ui->memmap, "RAM");
        ui_memmap_region(&ui->memmap, "RAM", 0x0000, 0x10000, true);
*/
}

/*
static int _ui_mp1000_eval_bp(ui_dbg_t* dbg_win, int trap_id, uint64_t pins, void* user_data) {
    (void)pins;
    CHIPS_ASSERT(user_data);
    ui_mp1000_t* ui = (ui_mp1000_t*) user_data;
    mp1000_t* mp1000 = ui->mp1000;

    int scanline = mp1000->vic.rs.v_count;
    for (int i = 0; (i < dbg_win->dbg.num_breakpoints) && (trap_id == 0); i++) {
        const ui_dbg_breakpoint_t* bp = &dbg_win->dbg.breakpoints[i];
        if (bp->enabled) {
            switch (bp->type) {
                case UI_DBG_BREAKTYPE_USER+0:
                    if ((ui->dbg_scanline != scanline) && (scanline == bp->val)) {
                        trap_id = UI_DBG_BP_BASE_TRAPID + i;
                    }
                    break;
                case UI_DBG_BREAKTYPE_USER+1:
                    if (ui->dbg_scanline != scanline) {
                        trap_id = UI_DBG_BP_BASE_TRAPID + i;
                    }
                    break;
                case UI_DBG_BREAKTYPE_USER+2:
                    if ((ui->dbg_scanline != scanline) && mp1000->vic.rs.badline) {
                        trap_id = UI_DBG_BP_BASE_TRAPID + i;
                    }
                    break;
                case UI_DBG_BREAKTYPE_USER+3:
                    if ((ui->dbg_scanline != scanline) && (scanline == 0)) {
                        trap_id = UI_DBG_BP_BASE_TRAPID + i;
                    }
                    break;
            }
        }
    }
    ui->dbg_scanline = scanline;
  return trap_id;
}*/

static const ui_chip_pin_t _ui_mp1000_cpu6800_pins[] = {
    { "D0",     0,      MC6800_D0 },
    { "D1",     1,      MC6800_D1 },
    { "D2",     2,      MC6800_D2 },
    { "D3",     3,      MC6800_D3 },
    { "D4",     4,      MC6800_D4 },
    { "D5",     5,      MC6800_D5 },
    { "D6",     6,      MC6800_D6 },
    { "D7",     7,      MC6800_D7 },
    { "RW",     9,      MC6800_RW },
    { "VMA",    10,     MC6800_VMA },
    { "IRQ",    11,     MC6800_IRQ },
    { "NMI",    12,     MC6800_NMI },
    { "HALT",   13,     MC6800_HALT },
    { "RESET",  14,     MC6800_RESET },
    { "A0",     15,     MC6800_A0 },
    { "A1",     16,     MC6800_A1 },
    { "A2",     17,     MC6800_A2 },
    { "A3",     18,     MC6800_A3 },
    { "A4",     19,     MC6800_A4 },
    { "A5",     20,     MC6800_A5 },
    { "A6",     21,     MC6800_A6 },
    { "A7",     22,     MC6800_A7 },
    { "A8",     23,     MC6800_A8 },
    { "A9",     24,     MC6800_A9 },
    { "A10",    25,     MC6800_A10 },
    { "A11",    26,     MC6800_A11 },
    { "A12",    27,     MC6800_A12 },
    { "A13",    28,     MC6800_A13 },
    { "A14",    29,     MC6800_A14 },
    { "A15",    30,     MC6800_A15 },
};

static const ui_chip_pin_t _ui_mp1000_pia_pins[] = {
    { "D0",     0,      MC6821_D0 },
    { "D1",     1,      MC6821_D1 },
    { "D2",     2,      MC6821_D2 },
    { "D3",     3,      MC6821_D3 },
    { "D4",     4,      MC6821_D4 },
    { "D5",     5,      MC6821_D5 },
    { "D6",     6,      MC6821_D6 },
    { "D7",     7,      MC6821_D7 },
    { "RS0",    8,      MC6821_RS0 },
    { "RS1",    9,     MC6821_RS1 },
    { "RW",     10,     MC6821_RW },
    { "CS",     11,     MC6821_CS },
    { "CA1",    12,     MC6821_CA1 },
    { "CA2",    13,     MC6821_CA2 },
    { "CB1",    14,     MC6821_CB1 },
    { "CB2",    15,     MC6821_CB2 },
    { "PA0",    16,     MC6821_PA0 },
    { "PA1",    17,     MC6821_PA1 },
    { "PA2",    18,     MC6821_PA2 },
    { "PA3",    19,     MC6821_PA3 },
    { "PA4",    20,     MC6821_PA4 },
    { "PA5",    21,     MC6821_PA5 },
    { "PA6",    22,     MC6821_PA6 },
    { "PA7",    23,     MC6821_PA7 },
    { "PB0",    24,     MC6821_PB0 },
    { "PB1",    25,     MC6821_PB1 },
    { "PB2",    26,     MC6821_PB2 },
    { "PB3",    27,     MC6821_PB3 },
    { "PB4",    28,     MC6821_PB4 },
    { "PB5",    29,     MC6821_PB5 },
    { "PB6",    30,     MC6821_PB6 },
    { "PB7",    31,     MC6821_PB7 },
};

static const ui_chip_pin_t _ui_mp1000_vdg_pins[] = {
    { "D0",     0,	MC6847_D0 },
    { "D1",     1,	MC6847_D1 },
    { "D2",     2,	MC6847_D2 },
    { "D3",     3,	MC6847_D3 },
    { "D4",     4,	MC6847_D4 },
    { "D5",     5,	MC6847_D5 },
    { "D6",     6,	MC6847_D6 },
    { "D7",     7,	MC6847_D7 },
    { "A/G",    9,	MC6847_AG },
    { "A/S",    10,     MC6847_AS },
    { "I/X",    11,     MC6847_INTEXT },
    { "INV",    12,     MC6847_INV },
    { "CSS",    13,     MC6847_CSS },
    { "GM0",    14,     MC6847_GM0 },
    { "GM1",    15,     MC6847_GM0 },
    { "GM2",    16,     MC6847_GM0 },
    { "GM3",    17,     MC6847_GM0 },
    { "A0",     18,     MC6847_A0 },
    { "A1",     19,     MC6847_A1 },
    { "A2",     20,     MC6847_A2 },
    { "A3",     21,     MC6847_A3 },
    { "A4",     22,     MC6847_A4 },
    { "A5",     23,     MC6847_A5 },
    { "A6",     24,     MC6847_A6 },
    { "A7",     25,     MC6847_A7 },
    { "A8",     26,     MC6847_A8 },
    { "A9",     27,     MC6847_A9 },
    { "A10",    28,     MC6847_A10 },
    { "A11",    29,     MC6847_A11 },
    { "A12",    30,     MC6847_A12 },
    { "FS",     32,     MC6847_FS },
    { "HS",     33,     MC6847_HS },
    { "RP",     34,     MC6847_RP }
};

void ui_mp1000_init(ui_mp1000_t* ui, const ui_mp1000_desc_t* ui_desc) {
    CHIPS_ASSERT(ui && ui_desc);
    CHIPS_ASSERT(ui_desc->mp1000);
    CHIPS_ASSERT(ui_desc->boot_cb);
    ui->mp1000 = ui_desc->mp1000;
    ui->boot_cb = ui_desc->boot_cb;
    ui_snapshot_init(&ui->snapshot, &ui_desc->snapshot);
    int x = 20, y = 20, dx = 10, dy = 10;
    {/*
        ui_dbg_desc_t desc = {0};
        desc.title = "CPU Debugger";
        desc.x = x;
        desc.y = y;
        desc.mc6800 = &ui->mp1000->cpu;
        desc.freq_hz = MP1000_FREQUENCY;
        desc.scanline_ticks = MC6847_HTOTAL;
        desc.frame_ticks = MC6847_HTOTAL * MC6847_VTOTAL;
        desc.read_cb = _ui_mp1000_mem_read;
        desc.break_cb = _ui_mp1000_eval_bp;
        desc.texture_cbs = ui_desc->dbg_texture;
        desc.debug_cbs = ui_desc->dbg_debug;
        desc.keys = ui_desc->dbg_keys;
        desc.user_data = ui;
        // custom breakpoint types
        desc.user_breaktypes[0].label = "Scanline at";
        desc.user_breaktypes[0].show_val16 = true;
        desc.user_breaktypes[1].label = "Next Scanline";
        desc.user_breaktypes[2].label = "Next Badline";
        desc.user_breaktypes[3].label = "Next Frame";
        ui_dbg_init(&ui->dbg, &desc);*/
    }
    x += dx; y += dy;
    {
        ui_mc6800_desc_t desc = {0};
        desc.title = "MC 6800";
        desc.cpu = &ui->mp1000->cpu;
        desc.x = x;
        desc.y = y;
        desc.h = 390;
        UI_CHIP_INIT_DESC(&desc.chip_desc, "6800", 30, _ui_mp1000_cpu6800_pins);
        ui_mc6800_init(&ui->cpu, &desc);
    }
    x += dx; y += dy;
    {
        ui_mc6821_desc_t desc = {0};
        desc.title = "MC 6821 #1 (PIA)";
        desc.pia = &ui->mp1000->pia;
        desc.x = x;
        desc.y = y;
        UI_CHIP_INIT_DESC(&desc.chip_desc, "6821", 32, _ui_mp1000_pia_pins);
        ui_mc6821_init(&ui->pia[0], &desc);
        x += dx; y += dy;
        desc.title = "MC 6821 #2 (PIA-IM)";
        desc.pia = &ui->mp1000->piam;
        desc.x = x;
        desc.y = y;
        ui_mc6821_init(&ui->pia[1], &desc);
    }
    x += dx; y += dy;
    {
        ui_mc6847_desc_t desc = {0};
        desc.title = "MC 6847 (VDG)";
        desc.mc6847 = &ui->mp1000->vdg;
        desc.x = x;
        desc.y = y;
        UI_CHIP_INIT_DESC(&desc.chip_desc, "6847", 35, _ui_mp1000_vdg_pins);
        ui_mc6847_init(&ui->vdg, &desc);
    }
    x += dx; y += dy;
    {
        ui_audio_desc_t desc = {0};
        desc.title = "Audio Output";
        desc.sample_buffer = ui->mp1000->audio.sample_buffer;
        desc.num_samples = ui->mp1000->audio.num_samples;
        desc.x = x;
        desc.y = y;
        ui_audio_init(&ui->audio, &desc);
    }
    x += dx; y += dy;
    {
        ui_display_desc_t desc = {0};
        desc.title = "Display";
        desc.x = x;
        desc.y = y;
        ui_display_init(&ui->display, &desc);
    }
    x += dx; y += dy;
    {
        ui_kbd_desc_t desc = {0};
        desc.title = "Keyboard Matrix";
        desc.kbd = &ui->mp1000->kbd;
        desc.layers[0] = "None";
        desc.layers[1] = "Shift";
        desc.layers[2] = "Ctrl";
        desc.x = x;
        desc.y = y;
        ui_kbd_init(&ui->kbd, &desc);
    }
    x += dx; y += dy;
    {
        ui_memedit_desc_t desc = {0};
        for (int i = 0; i < _UI_MP1000_MEMLAYER_NUM; i++) {
            desc.layers[i] = _ui_mp1000_memlayer_names[i];
        }
        desc.read_cb = _ui_mp1000_mem_read;
        desc.write_cb = _ui_mp1000_mem_write;
        desc.user_data = ui;
        static const char* titles[] = { "Memory Editor #1", "Memory Editor #2", "Memory Editor #3", "Memory Editor #4" };
        for (int i = 0; i < 4; i++) {
            desc.title = titles[i]; desc.x = x; desc.y = y;
            ui_memedit_init(&ui->memedit[i], &desc);
            x += dx; y += dy;
        }
    }
    x += dx; y += dy;
    {
        ui_memmap_desc_t desc = {0};
        desc.title = "Memory Map";
        desc.x = x;
        desc.y = y;
        ui_memmap_init(&ui->memmap, &desc);
    }/*
    x += dx; y += dy;
    {
        ui_dasm_desc_t desc = {0};
        for (int i = 0; i < _UI_MP1000_CODELAYER_NUM; i++) {
            desc.layers[i] = _ui_mp1000_memlayer_names[i];
        }
        desc.cpu_type = UI_DASM_CPUTYPE_MC6800;
        desc.start_addr = mem_rd16(&ui->mp1000->mem_cpu, 0xFFFC);
        desc.read_cb = _ui_mp1000_mem_read;
        desc.user_data = ui;
        static const char* titles[4] = { "Disassembler #1", "Disassembler #2", "Disassembler #2", "Dissassembler #3" };
        for (int i = 0; i < 4; i++) {
            desc.title = titles[i]; desc.x = x; desc.y = y;
            ui_dasm_init(&ui->dasm[i], &desc);
            x += dx; y += dy;
        }
    }*/
}

void ui_mp1000_discard(ui_mp1000_t* ui) {
    CHIPS_ASSERT(ui && ui->mp1000);
    ui_mc6800_discard(&ui->cpu);
    ui_mc6821_discard(&ui->pia[0]);
    ui_mc6821_discard(&ui->pia[1]);
    ui_mc6847_discard(&ui->vdg);
    ui_kbd_discard(&ui->kbd);
    ui_audio_discard(&ui->audio);
    ui_display_discard(&ui->display);
    ui_memmap_discard(&ui->memmap);
    for (int i = 0; i < 4; i++) {
        ui_memedit_discard(&ui->memedit[i]);
        //ui_dasm_discard(&ui->dasm[i]);
    }
    //ui_dbg_discard(&ui->dbg);
    ui->mp1000 = 0;
}

void ui_mp1000_draw(ui_mp1000_t* ui, const ui_mp1000_frame_t* frame) {
    CHIPS_ASSERT(ui && ui->mp1000 && frame);
    _ui_mp1000_draw_menu(ui);
    if (ui->memmap.open) {
        _ui_mp1000_update_memmap(ui);
    }
    ui_audio_draw(&ui->audio, ui->mp1000->audio.sample_pos);
    ui_display_draw(&ui->display, &frame->display);
    ui_kbd_draw(&ui->kbd);
    ui_mc6800_draw(&ui->cpu);
    ui_mc6821_draw(&ui->pia[0]);
    ui_mc6821_draw(&ui->pia[1]);
    ui_mc6847_draw(&ui->vdg);
    ui_memmap_draw(&ui->memmap);
    for (int i = 0; i < 4; i++) {
        ui_memedit_draw(&ui->memedit[i]);
        //ui_dasm_draw(&ui->dasm[i]);
    }
    //ui_dbg_draw(&ui->dbg);
}

chips_debug_t ui_mp1000_get_debug(ui_mp1000_t* ui) {
    CHIPS_ASSERT(ui);
    chips_debug_t res = {};
    //res.callback.func = (chips_debug_func_t)ui_dbg_tick;
    //res.callback.user_data = &ui->dbg;
    //res.stopped = &ui->dbg.dbg.stopped;
    return res;
}

void ui_mp1000_save_settings(ui_mp1000_t* ui, ui_settings_t* settings) {
    CHIPS_ASSERT(ui && settings);
    ui_mc6800_save_settings(&ui->cpu, settings);
    for (int i = 0; i < 2; i++) {
        ui_mc6821_save_settings(&ui->pia[i], settings);
    }
    ui_mc6847_save_settings(&ui->vdg, settings);
    ui_audio_save_settings(&ui->audio, settings);
    ui_display_save_settings(&ui->display, settings);
    ui_kbd_save_settings(&ui->kbd, settings);
    ui_memmap_save_settings(&ui->memmap, settings);
    for (int i = 0; i < 4; i++) {
        ui_memedit_save_settings(&ui->memedit[i], settings);
    }
    for (int i = 0; i < 4; i++) {
        //ui_dasm_save_settings(&ui->dasm[i], settings);
    }
    //ui_dbg_save_settings(&ui->dbg, settings);
}

void ui_mp1000_load_settings(ui_mp1000_t* ui, const ui_settings_t* settings) {
    CHIPS_ASSERT(ui && settings);
    ui_mc6800_load_settings(&ui->cpu, settings);
    for (int i = 0; i < 2; i++) {
        ui_mc6821_load_settings(&ui->pia[i], settings);
    }
    ui_mc6847_load_settings(&ui->vdg, settings);
    ui_audio_load_settings(&ui->audio, settings);
    ui_display_load_settings(&ui->display, settings);
    ui_kbd_load_settings(&ui->kbd, settings);
    ui_memmap_load_settings(&ui->memmap, settings);
    for (int i = 0; i < 4; i++) {
        ui_memedit_load_settings(&ui->memedit[i], settings);
    }
    for (int i = 0; i < 4; i++) {
        //ui_dasm_load_settings(&ui->dasm[i], settings);
    }
    //ui_dbg_load_settings(&ui->dbg, settings);
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif /* CHIPS_UI_IMPL */
