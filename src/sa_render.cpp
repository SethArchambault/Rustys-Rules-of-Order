
#ifdef ALE_TESTING
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"
#include "rusty.h"
#include "sa_types.h"
#include "imgui/imgui.h"
#include "sokol/util/sokol_imgui.h"
#include "sa_render.h"
#endif

static void init(void) {
    sg_setup(sg_desc{
        .logger = {
            .func = slog_func
        },
        .environment = sglue_environment()
    });
    simgui_setup(simgui_desc_t{ 0 });
}

void render_begin(ImVec2 pos, ImVec2 size, const char * title) {
    ImGui::SetNextWindowPos(pos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(size, ImGuiCond_Once);
    ImGui::Begin(title, 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
}

static void frame(void) {
    simgui_new_frame(simgui_frame_desc_t{
        .width = sapp_width(),
        .height = sapp_height(),
        .delta_time = sapp_frame_duration(),
        .dpi_scale = sapp_dpi_scale(),
    });

    game_loop();
    sg_begin_pass(sg_pass{ 
        .action = {
            .colors = {{ 
                .load_action = SG_LOADACTION_CLEAR, 
                .clear_value = { 0.0f, 0.0f, 0.0f, 1.0 } 
            }}
        },
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
    if (ev->type == SAPP_EVENTTYPE_RESIZED) {
        game_resized();
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
        .width = (s32)project_config.screen.x,
        .height = (s32)project_config.screen.y,
        .window_title = project_config.title,
        .icon = {
            .sokol_default = true,
        },
        .logger = {
            .func = slog_func
        }
    };
}

