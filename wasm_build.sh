set -e

mkdir -p temp

:<<'COMMENT'
shader_lib="glsl300es"
shader_arr=("texture.glsl" "color.glsl")
for shader in ${shader_arr[@]}; do
    echo "shader:${shader}"
    tools/sokol-shdc -i src/${shader} -o includes/${shader}.h -l $shader_lib 
done
COMMENT


let memory_needed=5242880
# Must be aligned by 64 KiB
let page_size=65536 # 1024 * 64
# INITIAL_MEMORY must be larger than TOTAL_STACK, was 131072 (TOTAL_STACK=5242880)
# cannot grow more than 582483968 (555.5mb)
let memory_padding=$((page_size - (memory_needed % page_size)))
let memory_needed_aligned=$((memory_needed + memory_padding)) # aligned by pages size
echo "memory needed(aligned): $memory_needed_aligned"

options=(
-g
src/main.cpp
lib/wasm/libcimgui.a
lib/wasm/libsokol.a
-I include 
-ferror-limit=4 -O0 
-Wno-emcc 
-sINITIAL_MEMORY=${memory_needed_aligned}
--shell-file=src/shell.html
-sALLOW_MEMORY_GROWTH
-DSOKOL_GLES3 
-sUSE_WEBGL2
) 
:<<'COMMENT'
-static-libsan
-Wno-address-of-temporary
-Werror -Wall -Wextra -Wshadow -Wconversion
-Wno-unused-variable -Wno-unused-parameter -Wno-deprecated-declarations -Wno-unused-value 
-Wno-unused-function -Wno-missing-field-initializers -Wno-implicit-float-conversion 
--preload-file ./data@/data
-Wno-c99-designator -Wno-shorten-64-to-32 -Wno-reorder-init-list
COMMENT

time emcc -o temp/wasm/index.html "${options[@]}" 
if [ $# == 0 ]; then
    emrun temp/wasm/index.html
fi

