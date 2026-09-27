cmake_minimum_required(VERSION 3.21)
project(resamp_by_cyzenn_03dl VERSION 0.1.2 LANGUAGES CXX)

if(NOT WIN32)
  message(FATAL_ERROR "This project targets 32-bit Windows only.")
endif()
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR "Build with the Win32/x86 generator, not x64.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
  minhook
  GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
  GIT_TAG v1.3.4
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(minhook)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.92.9b
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(resamp_by_cyzenn SHARED
  src/dllmain.cpp
  src/plugin.cpp
  src/samp_dl.cpp
  src/renderer.cpp
  src/features.cpp
  src/stability.h

  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
)

target_include_directories(resamp_by_cyzenn PRIVATE
  src
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

target_compile_definitions(resamp_by_cyzenn PRIVATE
  WIN32_LEAN_AND_MEAN
  NOMINMAX
  _CRT_SECURE_NO_WARNINGS
)

if(MSVC)
  target_compile_options(resamp_by_cyzenn PRIVATE /W4 /permissive- /EHsc)
endif()

target_link_libraries(resamp_by_cyzenn PRIVATE
  minhook
  d3d9
  user32
)

set_target_properties(resamp_by_cyzenn PROPERTIES
  PREFIX ""
  OUTPUT_NAME "resamp++"
  SUFFIX ".asi"
)
