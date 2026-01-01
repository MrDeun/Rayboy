cmake_minimum_required(VERSION 3.17.0)
project(Rayboy VERSION 0.1.0 LANGUAGES C CXX)
include(FetchContent)

if(UNIX)
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)
endif()

set(FETCHCONTENT_QUIET OFF)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)
# ===============
# ==== IMGUI ====
# ===============
# Tries to find the library locally and if it doesn't find it, get it from Github
message(STATUS "Getting Dear ImGui from Github")
FetchContent_Declare(
  ImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.91.3
)
FetchContent_MakeAvailable(imgui)
# Add library files
add_library(
  imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
)
# Set it's include directory
target_include_directories(imgui INTERFACE ${imgui_SOURCE_DIR})

# ================
# ==== RAYLIB ====
# ================
message(STATUS "Getting raylib from Github")
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

# ====================
# ==== RAYLIB_CPP ====
# ====================
message(STATUS "Getting raylib-cpp from Github")
FetchContent_Declare(
  raylib_cpp
  GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
  GIT_TAG v5.0.2
)
FetchContent_MakeAvailable(raylib_cpp)

# =================
# ==== RLIMGUI ====
# =================
message(STATUS "Getting rlImGui from Github")
FetchContent_Declare(
  rlImGui
  GIT_REPOSITORY https://github.com/raylib-extras/rlImGui
  # Git Tag can be a release or a commit (like this)
  GIT_TAG c7303258e045b4698c3e0b930b71956b5bbe7dff
)
FetchContent_MakeAvailable(rlImGui)
# For libraries not compiled with CMake, it's necessary to manually add the source files and set the include directory
add_library(rlimgui STATIC ${rlimgui_SOURCE_DIR}/rlImGui.cpp)
target_include_directories(rlimgui INTERFACE ${rlimgui_SOURCE_DIR})
# Tells the compiler that one library depends on other
target_link_libraries(rlimgui PRIVATE imgui raylib)


message(STATUS "Fetching fmt from Github")
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

add_executable(Rayboy src/main.cpp
    src/lib/bus.cpp
    src/lib/cart.cpp
    src/lib/cpu.cpp
    src/lib/emu.cpp
    src/lib/instruction.cpp
    src/lib/ppu.cpp
    src/lib/timer.cpp
    src/lib/cpu_proc.cpp
    src/lib/cpu_fetch.cpp
    src/lib/cpu_regs.cpp
    src/lib/ram.cpp
    src/lib/stack.cpp
    src/lib/interupt.cpp
    src/lib/io.cpp
    src/lib/dbg.cpp
    src/lib/ui.cpp
    src/lib/dma.cpp
    src/lib/common.cpp

)
target_link_libraries(Rayboy PRIVATE imgui rlimgui raylib raylib_cpp fmt::fmt)
