#pragma once

struct Entity {
	entt::entity handle = entt::null;
	entt::registry* registry = nullptr;

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args) {
		return registry->emplace<T>(handle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& GetComponent() {
		return registry->get<T>(handle);
	}

	template<typename T>
	bool HasComponent() {
		return registry->all_of<T>(handle);
	}
};