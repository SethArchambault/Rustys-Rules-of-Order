@echo off
echo %time%

IF NOT EXIST temp/wasm mkdir temp/wasm
cmd /c emcc -o temp/wasm/index.html -g src/main.c lib/wasm/libcimgui.a lib/wasm/libsokol.a -DSOKOL_WGPU -I include -ferror-limit=4 -O0 -Wno-emcc -sINITIAL_MEMORY=5242880 --shell-file=include/shell.html -sALLOW_MEMORY_GROWTH -sUSE_WEBGL2 
echo %time%

emrun temp/wasm/index.html
