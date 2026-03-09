# CMakeList.txt : CMake project for RPGVTT, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (RPGVTT VERSION 0.1.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (SDL2 CONFIG REQUIRED)
find_package (glm CONFIG REQUIRED)
find_package (entt CONFIG REQUIRED)
find_package (imgui CONFIG REQUIRED)
set(IMGUI_VERSION "v1.91.9") # Musi zgadzać się z wersją z vcpkg
set(IMGUI_URL "https://raw.githubusercontent.com/ocornut/imgui/${IMGUI_VERSION}/backends")

set(BACKEND_FILES
    "imgui_impl_sdl2.h"
    "imgui_impl_sdl2.cpp"
    "imgui_impl_sdlrenderer2.h"
    "imgui_impl_sdlrenderer2.cpp"
)

# Upewniamy się, że folder Thirdparty istnieje
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui")

foreach(FILE IN LISTS BACKEND_FILES)
    set(LOCAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/${FILE}")
    
    # Pobieraj tylko wtedy, gdy pliku jeszcze nie ma na dysku
    if(NOT EXISTS "${LOCAL_PATH}")
        message(STATUS "Pobieranie ${FILE} z repozytorium ImGui...")
        file(DOWNLOAD "${IMGUI_URL}/${FILE}" "${LOCAL_PATH}" SHOW_PROGRESS)
    endif()
endforeach()

add_executable (${CMAKE_PROJECT_NAME} 
    "src/Core/main.cpp" 
    "src/Core/Window.cpp"
    "thirdparty/imgui/imgui_impl_sdlrenderer2.cpp"
    "thirdparty/imgui/imgui_impl_sdl2.cpp"
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/Engine/)
target_include_directories(${CMAKE_PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    $<TARGET_NAME:SDL2::SDL2main>
    $<TARGET_NAME:SDL2::SDL2>
    glm::glm
    EnTT::EnTT
    imgui::imgui 
)
