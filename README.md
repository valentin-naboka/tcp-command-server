This is a demo task server which accepts commands. The command is ended with '\n'. The arguments are separated with spaces.
The server responds "OK" - if the command is successfully executed, otherwise "ERROR" will be responded. There is no handshake.

The code does not use exceptions.
It uses "select" for async wait on multiple connections.

Supported command:

'exit'
Do nothing and terminates current session.

'version'
Prints TCP server’s version.

The sources compile for Ubuntu 18.04 with gcc 8.4.0
For MacOS Mojave with Apple clang version 11.0.0 (clang-1100.0.33.8)

To build from sources:
- Create build dir (Important: build dir should have upper level than CMakeLists.txt dir)
- go to build dir and run "cmake ../TCPCommandServer" to generate make files. 
  If you need specify compiller - define variables, e.g. "cmake ../TCPCommandServer  -DCMAKE_CXX_COMPILER=<path_to_compiler> -DCMAKE_C_COMPILER=<path_to_compiler>"
- run "make -j4" to build sources.
