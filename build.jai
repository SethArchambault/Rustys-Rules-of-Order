
#run {
  set_build_options_dc(.{
    do_output=false,
  });
  build_ok := build_cpp(
    "temp/rusty_from_jai",
    "src/rusty.cpp",
    compiler_executable_path = "clang++",
    library_files = .[
    ],
    type = .EXECUTABLE,
    extra = .[
      "-I", "include",
      "-L", "lib/macos",
      "-lsokol",
      "-framework", "Metal", 
      "-framework", "Cocoa", 
      "-framework", "MetalKit", 
      "-framework", "Quartz",
      "-std=c++20",
    ]
  );
  assert(build_ok);

  //opts: Generate_Bindings_Options;
  //array_add(*opts.libraries, .{filename="c_file"});
  //array_add(*opts.extra_clang_arguments, "-x", "c++");
  //array_add(*opts.source_files, "c_file.h");



  run_command("temp/rusty_from_jai");
}
#import "Basic";
#import "Compiler";
#import "BuildCpp";
#import "Process";
#import "Bindings_Generator";

/*
set -e
mkdir -p temp

normal_build=(
-g
-fsanitize=address 
-I include 
-I include/imgui
-I include/sokol
src/rusty.cpp
-L lib/macos
-lsokol
-fobjc-arc
-Werror -Wall -Wextra -Wshadow -Wconversion
-Wno-unused-variable -Wno-unused-parameter -Wno-deprecated-declarations -Wno-unused-value 
-Wno-unused-but-set-parameter
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
time clang++ -std=c++20 -o temp/rusty "${normal_build[@]}"
MallocNanoZone=0 temp/rusty
*/
