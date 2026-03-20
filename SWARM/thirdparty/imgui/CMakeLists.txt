cmake_minimum_required(VERSION 3.20)
project(imgui_wrapper)

set(IMGUI_VERSION "v1.91.9")
set(IMGUI_URL "https://raw.githubusercontent.com/ocornut/imgui/${IMGUI_VERSION}/backends")
set(BACKEND_FILES 
    "imgui_impl_sdl2.h" 
    "imgui_impl_sdl2.cpp" 
    "imgui_impl_opengl3.h" 
    "imgui_impl_opengl3.cpp"
    "imgui_impl_opengl3_loader.h"
)

foreach(FILE IN LISTS BACKEND_FILES)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
        message(STATUS "Pobieranie backendu ImGui: ${FILE}")
        file(DOWNLOAD "${IMGUI_URL}/${FILE}" "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
    endif()
endforeach()

add_library(imgui_backends STATIC 
    "imgui_impl_sdl2.cpp"
    "imgui_impl_opengl3.cpp"
)

# Mówimy CMake'owi, gdzie leżą nagłówki (.h) backendów,
# żeby główny projekt mógł z nich korzystać.
target_include_directories(imgui_backends PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Podłączamy zależności do backendów.
# Użycie PUBLIC sprawia, że główny projekt (RPGVTT) podłączając imgui_backends,
# dostanie automatycznie dostęp do rdzenia ImGui, SDL2, OpenGL i GLAD!
target_link_libraries(imgui_backends PUBLIC
    imgui::imgui
    $<TARGET_NAME:SDL2::SDL2main>
    $<TARGET_NAME:SDL2::SDL2>
    OpenGL::GL
    glad::glad
)