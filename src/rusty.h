#ifndef TYPES_H
#define TYPES_H
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include "sokol/sokol_app.h"

#define ArrayCount(arr) (s32)sizeof(arr) / (s32)sizeof(arr[0])

#define BUFFER_MAX  50000

#define assert(expr) if(!(expr)) { printf("%s:%d %s() %s\n",__FILE__,__LINE__, __func__, #expr); *(volatile int *)0 = 0; }


#define FRAME_MAX 20

/// Types ////

typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32; // use this mostly
typedef int64_t     s64;
typedef int32_t     b32;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef size_t      usize;

typedef float       f32;
typedef double      f64;

typedef u8          b8;

/// Vectors

#define structdef(Name) \
    typedef struct Name Name; \
    struct Name


structdef(V2s32) {
    s32 x;
    s32 y;
};

structdef(V3s32) {
    union {
        s32 x;
        s32 r;
    };
    union {
        s32 y;
        s32 g;
    };
    union {
        s32 z;
        s32 b;
    };
};

structdef(V2f32) {
    f32 x;
    f32 y;
};

structdef(V3f32) {
    union {
        f32 x;
        f32 r;
    };
    union {
        f32 y;
        f32 g;
    };
    union {
        f32 z;
        f32 b;
    };
};

structdef(V4f32) {
    f32 r;
    f32 g;
    f32 b;
    f32 a;
};

structdef(V4s32) {
    s32 r;
    s32 g;
    s32 b;
    s32 a;
};


/*
structdef(Buffer) {
    u64 size;
    u8 *data;
};
typedef Buffer String;
*/



//
/// Memory
//


#define Kilobytes(count) (u64) (count * 1024)
#define Megabytes(count) (u64) (count * 1024 * 1024)
#define Gigabytes(count) (u64) (count * 1024 * 1024 * 1024)


structdef(Arena) {
    u8 *mem;
    u64 cap;
    u64 alloc_pos;
    u64 commit_pos;
    u64 high_water_mark;
};




// 
/// renderer
//

structdef(Bind) {
    u32 idx;
};




typedef struct {
    s32 id;
} Sprite_index;

#endif
