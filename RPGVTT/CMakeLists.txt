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

add_executable (RPGVTT "src/main.cpp" "src/Window.cpp")

target_include_directories(RPGVTT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(RPGVTT PRIVATE
    $<TARGET_NAME:SDL2::SDL2main> $<TARGET_NAME:SDL2::SDL2>
    glm::glm
    EnTT::EnTT
    imgui::imgui
)