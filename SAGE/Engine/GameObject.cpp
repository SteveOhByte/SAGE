#include "GameObject.h"

#include <chrono>
#include <random>

GameObject::~GameObject()
{
	components.clear();
}

void GameObject::GenerateUID(const std::vector<GameObject*>& gameObjects)
{
	std::mt19937 rng; // Mersenne Twister 19937 generator
	std::uniform_int_distribution<unsigned long long> dist(10000000, 99999999); // 8-digit range
        
	// Use current time as seed for true randomness
	rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	unsigned long long newId;

	do {
		newId = dist(rng); // Generate a new ID

		// Check if the generated ID is unique among existing gameObjects
	} while (!IsUniqueId(newId, gameObjects));

	uid = newId; // Assign the unique ID
}

bool GameObject::IsUniqueId(const unsigned long long newId, const std::vector<GameObject*>& gameObjects)
{
	for (const auto& gameObject : gameObjects) {
		if (gameObject->uid == newId) {
			return false; // ID is not unique
		}
	}
	return true; // ID is unique
}