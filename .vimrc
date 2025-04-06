let project_dir = expand('<sfile>:p:h')
let g:ale_cpp_cc_options="-DALE_TESTING -std=c++20 -DSOKOL_METAL -I ".project_dir."/src -I ".project_dir."/include -I ".project_dir."/include/imgui"



