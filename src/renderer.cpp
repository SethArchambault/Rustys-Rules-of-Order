
#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <math.h>
#include<time.h>
#define _USE_MATH_DEFINES
#include "types.h"
#include "memory.h"
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"
#include "imgui/imgui.h"
#include "sokol/util/sokol_imgui.h"

#define imgui ImGui

static struct {
    sg_pass_action pass_action;
} state;

static void init(void) {

    sg_setup(sg_desc{
        .logger = {
            .func = slog_func
        },
        .environment = sglue_environment()
    });

    simgui_setup(simgui_desc_t{ 0 });


    sg_pass_action action = {
        .colors = {{ 
            .load_action = SG_LOADACTION_CLEAR, 
            .clear_value = { 0.0f, 0.5f, 1.0f, 1.0 } 
        }}
    };
    state.pass_action = action;
}

void sa_begin(ImVec2 pos, ImVec2 size, const char * title) {
    ImGui::SetNextWindowPos(pos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(size, ImGuiCond_Once);
    ImGui::Begin(title, 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
}
#define sa_button ImGui::Button
#define sa_text ImGui::Text

extern void game_loop();
static void frame(void) {
    simgui_new_frame(simgui_frame_desc_t{
        .width = sapp_width(),
        .height = sapp_height(),
        .delta_time = sapp_frame_duration(),
        .dpi_scale = sapp_dpi_scale(),
    });

    game_loop();
    sg_begin_pass(sg_pass{ 
        .action = state.pass_action, 
        .swapchain = sglue_swapchain() 
    });
    simgui_render();
    sg_end_pass();
    sg_commit();
}

static void cleanup(void) {
    simgui_shutdown();
    sg_shutdown();
}

static void event(const sapp_event* ev) {
    simgui_handle_event(ev);
    if (ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
        if (ev->key_code == SAPP_KEYCODE_ESCAPE) {
            sapp_request_quit();
        }
    }
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    return sapp_desc{
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .width = 500,
        .height = 600,
        .window_title = "Rusty's Rules",
        .icon = {
            .sokol_default = true,
        },
        .logger = {
            .func = slog_func
        }
    };
}

