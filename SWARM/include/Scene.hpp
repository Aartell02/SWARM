#pragma once
#include <entt/entt.hpp>
#include "components/Entity.hpp"
#include "components/Tag.hpp"

class Scene {
public:
	Entity CreateEntity(const std::string& name = "Entity") {
		entt::entity handle = m_registry.create();
		Entity entity{ handle, &m_registry };
		entity.AddComponent<Tag>(name);
		return entity;
	}
	void Update();

	entt::registry m_registry;

};