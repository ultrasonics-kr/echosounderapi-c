# echosounderapi-c Echologger Echosounder API for C/C++/C#
====================================================================================

Prerequisites
-------------

- `Git` version control system

Git can be downloaded from [Git Webpage](https://git-scm.com/downloads)

- `CMake` build system

Git can be downloaded from [CMake download webpage](https://cmake.org/download/)

- optional `Ninja` utility

Can be downloaded from [Ninja webpage](https://ninja-build.org/)

Build tools
-----------

- `ARM GCC` GCC GNU toolchain > 7.0.0 must be used for Linux build
- `Visual Studio` > 2013 or `MSYS64` for Windows build
- optional `Ninja` utility

Build command for Linux:

    git clone --recursive https://github.com/ultrasonics-kr/echosounderapi-c.git
    cd echosounderapi-c
    mkdir build
    cd build
    cmake ..
    make all
    make install

    // or build using Ninja
    
    git clone --recursive https://github.com/ultrasonics-kr/echosounderapi-c.git
    cd echosounderapi-c
    mkdir build
    cd build
    cmake -GNinja ..
    ninja    
    
Build command for Windows:

    git clone --recursive https://github.com/ultrasonics-kr/echosounderapi-c.git
    cd echosounderapi-c
    mkdir build
    cd build
    cmake ..
    // in case of Visual Studio toolchain cmake produce a solution file, which can be used to make binaries later on
    // in case of MSYS cmake produce a 'Makefile' file, which can be used by `make` utility
    
Binary files can be found at the /exe or build folder
