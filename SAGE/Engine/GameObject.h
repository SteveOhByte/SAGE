#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Components/Component.h"

class Sage;
enum class ComponentType;

class GameObject
{
	friend bool operator==(const GameObject& lhs, const GameObject& rhs) { return lhs.uid == rhs.uid; }
	friend bool operator!=(const GameObject& lhs, const GameObject& rhs) { return !(lhs == rhs); }

public:
	explicit GameObject(std::string name)
		: name(std::move(name)) {}
	explicit GameObject(int numObjectsInScene)
		: name("GameObject" + std::to_string(numObjectsInScene)) {}
	GameObject() = default;

	~GameObject();

	void AddComponent(Component* component)
	{
		component->gameObject = this;
		
		components.push_back(std::unique_ptr<Component>(component));
	}

	void RemoveComponent(Component* component)
	{
		components.erase(std::remove_if(components.begin(),
		                                components.end(),
		                                [component](const std::unique_ptr<Component>& pComponent)
		                                {
			                                return pComponent.get() == component;
		                                }),
		                 components.end());
	}

	// HasComponent method:
	template <typename T>
	bool HasComponent() const
	{
		for (const auto& pComponent : components)
		{
			T* casted = dynamic_cast<T*>(pComponent.get());
			if (casted != nullptr)
				return true;
		}
		return false;
	}

	template <typename T>
	T* GetComponent() const
	{
		for (const auto& pComponent : components)
		{
			T* casted = dynamic_cast<T*>(pComponent.get());
			if (casted != nullptr)
			{
				return casted;
			}
		}
		return nullptr;
	}

	template <typename T>
	std::vector<T*> GetComponentsOfType() const
	{
		std::vector<T*> results;
		for (const auto& pComponent : components)
		{
			T* casted = dynamic_cast<T*>(pComponent.get());
			if (casted != nullptr)
			{
				results.push_back(casted);
			}
		}
		return results;
	}

	std::vector<std::unique_ptr<Component>>& GetComponents() { return components; }

	// Active controls both the update and render loop
	bool IsActive() const { return isActive; }
	void SetActive(bool active) { isActive = active; }

	// Rendered only controls the render loop
	bool IsRendered() const { return isRendered; }
	void SetRendered(bool rendered) { isRendered = rendered; }

	unsigned long long GetUID() const { return uid; }
	void GenerateUID(const std::vector<GameObject*>& gameObjects);
	
	std::string name;
	std::vector<std::unique_ptr<Component>> components;

	std::vector<std::string> GetTags() const { return tags; }
	void AddTag(const std::string& tag) { tags.push_back(tag); }
	void RemoveTag(const std::string& tag) { tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end()); }
	bool HasTag(const std::string& tag) const { return std::find(tags.begin(), tags.end(), tag) != tags.end(); }
	
	float2 referenceResolution = float2(-1, -1);
	
	Sage* sage;
private:
	bool isActive = true;
	bool isRendered = true;
	unsigned long long uid;
	std::vector<std::string> tags;

	bool IsUniqueId(unsigned long long newId, const std::vector<GameObject*>& gameObjects);
};