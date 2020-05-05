The sources compile for Ubuntu 18.04 with gcc 8.4.0
For MacOS Mojave with Apple clang version 11.0.0 (clang-1100.0.33.8)

To build from sources:
- Create build dir (Important: build dir should have upper level than CMakeLists.txt dir)
- go to build dir and run "cmake ../TCPCommandServer" to generate make files. 
  If you need specify compiller - define variables, e.g. "cmake ../TCPCommandServer  -DCMAKE_CXX_COMPILER=<path_to_compiler> -DCMAKE_C_COMPILER=<path_to_compiler>"
- run "make -j4" to build sources.