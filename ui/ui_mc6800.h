#pragma once
/*#
    # ui_mc6800.h

    Debug visualization UI for mc6800.h

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

    Include the following headers before the including the *declaration*:
        - mc6800.h
        - ui_chip.h

    Include the following headers before including the *implementation*:
        - imgui.h
        - mc6800.h
        - ui_chip.h
        - ui_util.h
        - ui_settings.h

    All strings provided to ui_mc6800_init() must remain alive until
    ui_mc6800_discard() is called!

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

/* setup parameters for ui_mc6800_init()
    NOTE: all string data must remain alive until ui_mc6800_discard()!
*/
typedef struct {
    const char* title;          /* window title */
    mc6800_t* cpu;               /* mc6800_t instance to track */
    int x, y;                   /* initial window position */
    int w, h;                   /* initial window width and height */
    bool open;                  /* initial open state */
    ui_chip_desc_t chip_desc;   /* chips visualization desc */
} ui_mc6800_desc_t;

typedef struct {
    const char* title;
    mc6800_t* cpu;
    float init_x, init_y;
    float init_w, init_h;
    bool open;
    bool last_open;
    bool valid;
    ui_chip_t chip;
} ui_mc6800_t;

void ui_mc6800_init(ui_mc6800_t* win, const ui_mc6800_desc_t* desc);
void ui_mc6800_discard(ui_mc6800_t* win);
void ui_mc6800_draw(ui_mc6800_t* win);
void ui_mc6800_save_settings(ui_mc6800_t* win, ui_settings_t* settings);
void ui_mc6800_load_settings(ui_mc6800_t* win, const ui_settings_t* settings);

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

void ui_mc6800_init(ui_mc6800_t* win, const ui_mc6800_desc_t* desc) {
    CHIPS_ASSERT(win && desc);
    CHIPS_ASSERT(desc->title);
    CHIPS_ASSERT(desc->cpu);
    memset(win, 0, sizeof(ui_mc6800_t));
    win->title = desc->title;
    win->cpu = desc->cpu;
    win->init_x = (float) desc->x;
    win->init_y = (float) desc->y;
    win->init_w = (float) ((desc->w == 0) ? 360 : desc->w);
    win->init_h = (float) ((desc->h == 0) ? 320 : desc->h);
    win->open = win->last_open = desc->open;
    win->valid = true;
    ui_chip_init(&win->chip, &desc->chip_desc);
}

void ui_mc6800_discard(ui_mc6800_t* win) {
    CHIPS_ASSERT(win && win->valid);
    win->valid = false;
}

static void _ui_mc6800_regs(ui_mc6800_t* win) {
    mc6800_t* cpu = win->cpu;
    ImGui::Text("A:  %02X", cpu->A);
    ImGui::Text("B:  %02X", cpu->B);
    ImGui::Text("IX:  %04X", cpu->IX);
    ImGui::Text("SP:  %04X", cpu->SP);
    const uint8_t f = cpu->P;
    char f_str[9] = {
        (f & MC6800_CF) ? 'C':'-',
        (f & MC6800_VF) ? 'V':'-',
        (f & MC6800_ZF) ? 'Z':'-',
        (f & MC6800_NF) ? 'N':'-',
        (f & MC6800_IF) ? 'I':'-',
        (f & MC6800_HF) ? 'H':'-',
        (f & MC6800_XF) ? 'X':'-',
        (f & MC6800_YF) ? 'Y':'-',
        0
    };
    ImGui::Text("P:  %02X %s", f, f_str);
    ImGui::Text("PC: %04X", cpu->PC);
    ImGui::Separator();
}

void ui_mc6800_draw(ui_mc6800_t* win) {
    CHIPS_ASSERT(win && win->valid && win->cpu);
    ui_util_handle_window_open_dirty(&win->open, &win->last_open);
    if (!win->open) {
        return;
    }
    ImGui::SetNextWindowPos(ImVec2(win->init_x, win->init_y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(win->init_w, win->init_h), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(win->title, &win->open)) {
        ImGui::BeginChild("##mc6800_chip", ImVec2(176, 0), true);
        ui_chip_draw(&win->chip, win->cpu->PINS);
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("##mc6800_regs", ImVec2(0, 0), true);
        _ui_mc6800_regs(win);
        ImGui::EndChild();
    }
    ImGui::End();
}

void ui_mc6800_save_settings(ui_mc6800_t* win, ui_settings_t* settings) {
    CHIPS_ASSERT(win && settings);
    ui_settings_add(settings, win->title, win->open);
}

void ui_mc6800_load_settings(ui_mc6800_t* win, const ui_settings_t* settings) {
    CHIPS_ASSERT(win && settings);
    win->open = ui_settings_isopen(settings, win->title);
}
#endif /* CHIPS_UI_IMPL */
