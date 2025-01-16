#pragma once
/*#
    # ui_ay38910.h

    Debug visualization for AY-3-8910 sound chip.

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
        - ay38910.h
        - ui_chip.h

    Include the following headers before including the *implementation*:
        - imgui.h
        - ay38910.h
        - ui_chip.h
        - ui_util.h
        - ui_settings.h

    All string data provided to the ui_ay38910_init() must remain alive until
    until ui_ay38910_discard() is called!

    ## zlib/libpng license

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

/* setup parameters for ui_ay38910_init()
    NOTE: all string data must remain alive until ui_ay38910_discard()!
*/
typedef struct {
    const char* title;          /* window title */
    ay38910_t* ay;              /* pointer to ay38910_t instance to track */
    int x, y;                   /* initial window pos */
    int w, h;                   /* initial window size or zero for default size */
    bool open;                  /* initial open state */
    ui_chip_desc_t chip_desc;   /* chip visualization desc */
} ui_ay38910_desc_t;

typedef struct {
    const char* title;
    ay38910_t* ay;
    float init_x, init_y;
    float init_w, init_h;
    bool open;
    bool last_open;
    bool valid;
    ui_chip_t chip;
} ui_ay38910_t;

void ui_ay38910_init(ui_ay38910_t* win, const ui_ay38910_desc_t* desc);
void ui_ay38910_discard(ui_ay38910_t* win);
void ui_ay38910_draw(ui_ay38910_t* win);
void ui_ay38910_save_settings(ui_ay38910_t* win, ui_settings_t* settings);
void ui_ay38910_load_settings(ui_ay38910_t* win, const ui_settings_t* settings);

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

void ui_ay38910_init(ui_ay38910_t* win, const ui_ay38910_desc_t* desc) {
    CHIPS_ASSERT(win && desc);
    CHIPS_ASSERT(desc->title);
    CHIPS_ASSERT(desc->ay);
    memset(win, 0, sizeof(ui_ay38910_t));
    win->title = desc->title;
    win->ay = desc->ay;
    win->init_x = (float) desc->x;
    win->init_y = (float) desc->y;
    win->init_w = (float) ((desc->w == 0) ? 440 : desc->w);
    win->init_h = (float) ((desc->h == 0) ? 420 : desc->h);
    win->open = win->last_open = desc->open;
    win->valid = true;
    ui_chip_init(&win->chip, &desc->chip_desc);
}

void ui_ay38910_discard(ui_ay38910_t* win) {
    CHIPS_ASSERT(win && win->valid);
    win->valid = false;
}

static void _ui_ay38910_draw_state(ui_ay38910_t* win) {
    ay38910_t* ay = win->ay;
    if (ImGui::BeginTable("##ay_channels", 4)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 86);
        ImGui::TableSetupColumn("ChnA", ImGuiTableColumnFlags_WidthFixed, 32);
        ImGui::TableSetupColumn("ChnB", ImGuiTableColumnFlags_WidthFixed, 32);
        ImGui::TableSetupColumn("ChnC", ImGuiTableColumnFlags_WidthFixed, 32);
        ImGui::TableHeadersRow();
        ImGui::TableNextColumn();
        ImGui::Text("Tone Period"); ImGui::TableNextColumn();
        for (int i = 0; i < 3; i++) {
            ImGui::Text("%04X", ay->tone[i].period); ImGui::TableNextColumn();
        }
        ImGui::Text("Tone Count"); ImGui::TableNextColumn();
        for (int i = 0; i < 3; i++) {
            ImGui::Text("%04X", ay->tone[i].counter); ImGui::TableNextColumn();
        }
        ImGui::Text("Tone Bit"); ImGui::TableNextColumn();
        for (int i = 0; i < 3; i++) {
            ImGui::Text("%s", ay->tone[i].bit ? "ON":"OFF"); ImGui::TableNextColumn();
        }
        ImGui::Text("Volume"); ImGui::TableNextColumn();
        for (int i = 0; i < 3; i++) {
            ImGui::Text("%X", ay->reg[AY38910_REG_AMP_A + i] & 0x0F); ImGui::TableNextColumn();
        }
        ImGui::Text("Ampl Ctrl"); ImGui::TableNextColumn();
        for (int i = 0; i < 3; i++) {
            const uint8_t v = ay->reg[AY38910_REG_AMP_A + i] & 0x10;
            ImGui::Text("%s", v ? "ENV":"VOL"); ImGui::TableNextColumn();
        }
        ImGui::Text("Mix Tone"); ImGui::TableNextColumn();
        for (int i = 0; i < 3; i++) {
            const uint8_t v = (ay->enable >> i) & 1;
            ImGui::Text("%s", v ? "OFF":"ON"); ImGui::TableNextColumn();
        }
        ImGui::Text("Mix Noise"); ImGui::TableNextColumn();
        for (int i = 0; i < 3; i++) {
            const uint8_t v = (ay->enable >> (i+3)) & 1;
            ImGui::Text("%s", v ? "OFF":"ON"); ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("##ay_noise", 2)) {
        ImGui::TableSetupColumn("Noise", ImGuiTableColumnFlags_WidthFixed, 86);
        ImGui::TableHeadersRow();
        ImGui::TableNextColumn();
        ImGui::Text("Period"); ImGui::TableNextColumn();
        ImGui::Text("%02X (reg:%02X)", ay->noise.period, ay->reg[AY38910_REG_PERIOD_NOISE]); ImGui::TableNextColumn();
        ImGui::Text("Count"); ImGui::TableNextColumn();
        ImGui::Text("%02X", ay->noise.counter); ImGui::TableNextColumn();
        ImGui::Text("Rand"); ImGui::TableNextColumn();
        ImGui::Text("%05X", ay->noise.rng); ImGui::TableNextColumn();
        ImGui::Text("Bit"); ImGui::TableNextColumn();
        ImGui::Text("%s", ay->noise.bit ? "ON":"OFF"); ImGui::TableNextColumn();
        ImGui::EndTable();
    }
    if (ImGui::BeginTable("##ay_envelope", 2)) {
        ImGui::TableSetupColumn("Envelope", ImGuiTableColumnFlags_WidthFixed, 86);
        ImGui::TableHeadersRow();
        ImGui::TableNextColumn();
        ImGui::Text("Period"); ImGui::TableNextColumn();
        ImGui::Text("%04X (reg:%04X)", ay->env.period, (ay->reg[AY38910_REG_ENV_PERIOD_COARSE]<<8)|ay->reg[AY38910_REG_ENV_PERIOD_FINE]); ImGui::TableNextColumn();
        ImGui::Text("Count"); ImGui::TableNextColumn();
        ImGui::Text("%04X", ay->env.counter); ImGui::TableNextColumn();
        ImGui::Text("Amplitude"); ImGui::TableNextColumn();
        ImGui::Text("%02X", ay->env.shape_state); ImGui::TableNextColumn();
        ImGui::EndTable();
    }
    const int num_ports = (ay->type==AY38910_TYPE_8910) ? 2 : ((ay->type == AY38910_TYPE_8912) ? 1 : 0);
    const int max_ports = 2;
    if (ImGui::BeginTable("##ay_ports", max_ports + 1)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 96);
        ImGui::TableSetupColumn("PortA", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableSetupColumn("PortA", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableHeadersRow();
        ImGui::TableNextColumn();
        ImGui::Text("In/Out Dir"); ImGui::TableNextColumn();
        int i;
        for (i = 0; i < num_ports; i++) {
            const uint8_t v = (ay->enable >> (i+6)) & 1;
            ImGui::Text("%s", v ? "OUT":"IN"); ImGui::TableNextColumn();
        }
        for (; i < max_ports; i++) {
            ImGui::Text("-"); ImGui::TableNextColumn();
        }
        ImGui::Text("Data Store"); ImGui::TableNextColumn();
        for (i = 0; i < num_ports; i++) {
            ImGui::Text("%02X", (i == 0) ? ay->port_a : ay->port_b); ImGui::TableNextColumn();
        }
        for (; i < max_ports; i++) {
            ImGui::Text("-"); ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
}

void ui_ay38910_draw(ui_ay38910_t* win) {
    CHIPS_ASSERT(win && win->valid && win->title && win->ay);
    ui_util_handle_window_open_dirty(&win->open, &win->last_open);
    if (!win->open) {
        return;
    }
    ImGui::SetNextWindowPos(ImVec2(win->init_x, win->init_y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(win->init_w, win->init_h), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(win->title, &win->open)) {
        ImGui::BeginChild("##ay_chip", ImVec2(176, 0), true);
        ui_chip_draw(&win->chip, win->ay->pins);
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("##ay_state", ImVec2(0, 0), true);
        _ui_ay38910_draw_state(win);
        ImGui::EndChild();
    }
    ImGui::End();
}

void ui_ay38910_save_settings(ui_ay38910_t* win, ui_settings_t* settings) {
    CHIPS_ASSERT(win && settings);
    ui_settings_add(settings, win->title, win->open);
}

void ui_ay38910_load_settings(ui_ay38910_t* win, const ui_settings_t* settings) {
    CHIPS_ASSERT(win && settings);
    win->open = ui_settings_isopen(settings, win->title);
}

#endif /* CHIPS_UI_IMPL */
