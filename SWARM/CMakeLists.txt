cmake_minimum_required (VERSION 3.20)

project (SWARM VERSION 0.1.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (VCPKG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set (BUILD_TESTING OFF CACHE BOOL "" FORCE)

# --- DEPENDECIES ---
find_package (SDL2 CONFIG REQUIRED)
find_package (glm CONFIG REQUIRED)
find_package (entt CONFIG REQUIRED)
find_package (imgui CONFIG REQUIRED)
find_package (OpenGL REQUIRED)
find_package (glad CONFIG REQUIRED)

# --- EXECUTABLES ---
add_executable (${CMAKE_PROJECT_NAME}
    "src/Core/main.cpp" 
    
 )
 
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Engine/
)

 add_subdirectory(thirdparty/imgui)


target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    imgui_backends
    glm::glm
    EnTT::EnTT
)
