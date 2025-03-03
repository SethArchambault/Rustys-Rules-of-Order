let project_dir = expand('<sfile>:p:h')
let g:ale_c_cc_options="/std:c++20 -DSOKOL_METAL -I ".project_dir."/src -I ".project_dir."/include -I ".project_dir."/include/imgui"



