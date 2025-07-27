#run {
  make_directory_if_it_does_not_exist("temp");
  set_build_options_dc(.{
    do_output=false,
  });
  options := get_build_options();
  recreate_libsokol_from_jai := false;
  args := options.compile_time_command_line;
  for arg: args {
    if arg == {
      case "recompile"; {
        recreate_libsokol_from_jai = true;
      }
    }
  }

  if recreate_libsokol_from_jai {
    build_ok := build_cpp(
      "lib/macos/libsokol_from_jai",
      "lib/macos/sokol.mm",
      library_files = .[
      ],
      type = .STATIC_LIBRARY,
      extra = .[
        "-I", "include",
        "-I", "include/sokol",
        "-I", "include/imgui",
      ]
    );
    assert(build_ok);
  }
  build_ok := build_cpp(
    "temp/rusty_from_jai",
    "src/rusty.cpp",
    library_files = .[
    ],
    type = .EXECUTABLE,
    debug = true,
    extra = .[
      // optional
      //"-fsanitize=address",
      // required
      "-I", "include",
      "-L", "lib/macos",
      "-lsokol_from_jai",
      "-framework", "Metal", 
      "-framework", "Cocoa", 
      "-framework", "MetalKit", 
      "-framework", "Quartz",
      "-std=c++20",
    ]
  );
  assert(build_ok);
  run_command("temp/rusty_from_jai");
}
#import "Basic";
#import "File";
#import "Compiler";
#import "BuildCpp";
#import "Process";

/*
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
MallocNanoZone=0 temp/rusty
*/
