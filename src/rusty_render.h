#ifndef RUSTY_RENDER_H
#define RUSTY_RENDER_H

#ifdef ALE_TESTING
#include "sokol/sokol_gfx.h"
#include "rusty.h"
#endif

V2f32 screen = {
    380, 650
};

extern void game_loop();

#endif
