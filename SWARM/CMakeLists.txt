cmake_minimum_required (VERSION 3.20)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (VCPKG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set (BUILD_TESTING OFF CACHE BOOL "" FORCE)

project (SWARM VERSION 0.1.0 LANGUAGES CXX)

# --- DEPENDECIES ---
find_package (directx-headers CONFIG REQUIRED)
find_package (directxtk12 CONFIG REQUIRED)
find_package (SDL3 CONFIG REQUIRED)
find_package (imgui CONFIG REQUIRED)
find_package (entt CONFIG REQUIRED)


# --- EXECUTABLES ---
add_executable (${CMAKE_PROJECT_NAME}
    "src/Core/main.cpp"
)
 
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Engine/
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    # DirectX
    Microsoft::DirectX-Headers
    Microsoft::DirectXTK12
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
    dxguid.lib

    # SDL3
    SDL3::SDL3

    # ImGui
    imgui::imgui

    # EnTT
    EnTT::EnTT
)
