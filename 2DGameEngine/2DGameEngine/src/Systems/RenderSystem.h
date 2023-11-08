#pragma once

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL.h>
#include "../Logger/Logger.h"

class RenderSystem : public System
{
	public:
		RenderSystem()
		{
			RequireComponent<TransformComponent>();
			RequireComponent<SpriteComponent>();
		}

		// Update function for movement system, takes delta time as an argument
		void Update(SDL_Renderer* renderer)
		{
			// Loop through all entities to see which have the relevant components
			for (auto entity : GetSystemEntities())
			{
				const auto transform = entity.GetComponent<TransformComponent>();
				const auto sprite = entity.GetComponent<SpriteComponent>();


				SDL_Rect objRect =
				{
					static_cast<int>(transform.position.x),
					static_cast<int>(transform.position.y),
					sprite.width,
					sprite.height
				};
				
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderFillRect(renderer, &objRect);

			}

		}
};