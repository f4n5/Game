cmake_minimum_required(VERSION 3.20)
project(GgWare CXX)

set(CMAKE_CXX_STANDARD 17)

# ── Sources ───────────────────────────────────────────────
file(GLOB IMGUI_SRC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_win32.cpp
    imgui/backends/imgui_impl_dx11.cpp
)

add_executable(GgWare WIN32
    main.cpp
    Config.cpp
    ${IMGUI_SRC}
)

target_include_directories(GgWare PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui/backends
    ${CMAKE_SOURCE_DIR}/include   # nlohmann/json.hpp
)

target_link_libraries(GgWare PRIVATE
    d3d11
    dxgi
    d3dcompiler
)

# Windows-only
target_compile_definitions(GgWare PRIVATE UNICODE _UNICODE)
