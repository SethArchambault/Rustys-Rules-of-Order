set -e
mkdir -p temp

:<<'COMMENT'
shader_lib="metal_macos"
shader_arr=("texture.glsl" "color.glsl")
for shader in ${shader_arr[@]}; do
    echo "shader:${shader}"
    tools/sokol-shdc -i src/${shader} -o includes/${shader}.h -l $shader_lib 
done
COMMENT

:<<'COMMENT'
-DPOISON 
-DUSE_DBG_UI
lib/macos/libcimgui.a
lib/macos/libsokol.a
-framework AudioToolbox
lib/macos/sokol.mm
COMMENT

normal_build=(
-g
-fsanitize=address 
-I include 
-I include/imgui
-I include/sokol
src/main.cpp
-L lib/macos
-lsokol
-fobjc-arc
-Werror -Wall -Wextra -Wshadow -Wconversion
-Wno-unused-variable -Wno-unused-parameter -Wno-deprecated-declarations -Wno-unused-value 
-Wno-deprecated-enum-enum-conversion
-Wno-unused-function -Wno-missing-field-initializers -Wno-implicit-float-conversion 
-framework Metal -framework Cocoa -framework MetalKit -framework Quartz 
-ferror-limit=6 -O0 
-Wno-address-of-temporary
)

# you could compile just by including lib/macos/sokol.mm, but that adds a few seconds, so instead we 
# create lib/macos/libsokol.a this way:
# clang -I include -I include/sokol -I include/imgui -c lib/macos/sokol.mm -o sokol.o && ar -r lib/macos/libsokol.a sokol.o && rm sokol.o
# Note that imgui is added to the archive as well.

echo "normal build"
time clang++ -std=c++20 -o temp/main "${normal_build[@]}"
MallocNanoZone=0 temp/main

