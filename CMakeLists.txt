#Copyright (c) EofE Ultrasonics Co., Ltd., 2024
message(STATUS "CMake version: ${CMAKE_VERSION}")
cmake_minimum_required(VERSION 3.20)

find_package(Git)
if(NOT GIT_FOUND)
    message(WARNING "Git not found!")
endif()


# Get the current working branch
execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the closest tag
execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --abbrev=0
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Git branch: ${GIT_BRANCH}")
message(STATUS "Git commit: ${GIT_COMMIT_HASH}")
message(STATUS "Git tag: ${GIT_TAG}")

string(TIMESTAMP BUILDTIME %Y%m%d%H%M)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set (PROJECT echosounderapi)
project(${PROJECT})

## Sources
set(echosounderapi_src
    src/Echosounder.cpp 
    src/DualEchosounder.cpp 
    src/SingleEchosounder.cpp 
    src/EchosounderCWrapper.cpp 
    src/ISonar.cpp 
    modules/serial/src/serial.cc
)

if(WIN32)
    list(APPEND echosounderapi_src modules/serial/src/impl/win.cc)
else() # UNIX/IOS
    list(APPEND echosounderapi_src modules/serial/src/impl/unix.cc)
endif()


include_directories(include modules/serial/include)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(BUILD_SHARED_LIBS)
message(STATUS "Build Shared Library")
add_library(${PROJECT_NAME} SHARED ${echosounderapi_src})
add_compile_definitions(DLL_LIB)
else()
message(STATUS "Build Static Library")
add_library(${PROJECT_NAME} ${echosounderapi_src})
endif()

#Examples
add_executable(example_detect examples/detect/detect.c)
add_dependencies(example_detect ${PROJECT_NAME})
target_link_libraries(example_detect ${PROJECT_NAME})

add_executable(example_work examples/work/work.c)
add_dependencies(example_work ${PROJECT_NAME})
target_link_libraries(example_work ${PROJECT_NAME})

add_compile_definitions(_UNICODE UNICODE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/exe)
