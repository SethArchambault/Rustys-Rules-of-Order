// same as sokol.m, but compiled as Objective-C++
#define SOKOL_IMPL
#define SOKOL_D3D11
/* this is only needed for the debug-inspection headers */
#define SOKOL_TRACE_HOOKS
/* sokol 3D-API defines are provided by build options */
#include "sokol_app.h"
#include "sokol_gfx.h"
//#include "sokol_time.h"
//#include "sokol_audio.h"
//#include "sokol_fetch.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "sokol_log.h"
#include "sokol_glue.h"
#include "imgui/imgui.h"
#include "imgui/imgui.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_tables.cpp"
#include "imgui/imgui_widgets.cpp"
#include "sokol/util/sokol_imgui.h"
#include "imgui/backends/imgui_impl_dx12.cpp"
