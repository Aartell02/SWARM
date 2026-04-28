#pragma once
#include <glm/glm.hpp>
#include <string>

struct TransformComponent {
    glm::vec3 GridPosition{ 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale{ 1.0f, 1.0f, 0.0f };
};

struct TagComponent {
    std::string Tag;
};

struct ColorComponent {
    glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
};