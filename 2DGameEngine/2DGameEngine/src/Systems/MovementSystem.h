#pragma once

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Logger/Logger.h"

class MovementSystem : public System
{
	public:
		MovementSystem()
		{
			RequireComponent<TransformComponent>();
			RequireComponent<RigidBodyComponent>();
		}

		// Update function for movement system, takes delta time as an argument
		void Update(double deltaTime)
		{
			// Loop through all entities to see which have the relevant components
			for (auto entity : GetSystemEntities())
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				const auto rigidBody = entity.GetComponent<RigidBodyComponent>();

				transform.position.x += rigidBody.velocity.x * deltaTime;
				transform.position.y += rigidBody.velocity.y * deltaTime;

				Logger::Log("Position x: " + std::to_string(transform.position.x) + " position y: " + std::to_string(transform.position.y));
			}

		}
};