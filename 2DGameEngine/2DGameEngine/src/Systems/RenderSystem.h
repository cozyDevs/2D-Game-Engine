#pragma once

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL.h>
#include "../Logger/Logger.h"
#include "../AssetManager/AssetStore.h"
#include <algorithm>

class RenderSystem : public System
{
	public:
		RenderSystem()
		{
			RequireComponent<TransformComponent>();
			RequireComponent<SpriteComponent>();
		}
 

		// Update function for movement system, takes delta time as an argument
		void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore)
		{
			// Helper struct for sorting the entities
			struct RenderableEntity
			{
				TransformComponent transformComponent;
				SpriteComponent spriteComponent;
			};

			std::vector<RenderableEntity> renderableEntities;

			// Sort entities by z-index
			for (auto entity : GetSystemEntities())
			{
				RenderableEntity renderableEntity;
				renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
				renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
				renderableEntities.emplace_back(renderableEntity);

			}

			std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b)
				{
					return a.spriteComponent.zIndex < a.spriteComponent.zIndex;
				});

			// Loop through all entities to see which have the relevant components
			for (auto entity : renderableEntities)
			{
				const auto transform = entity.transformComponent;
				const auto sprite = entity.spriteComponent;


				// Source rectangle for subsections of the texture
				SDL_Rect srcRect = sprite.srcRect;
				
				// Setting destination rectangle with the positon and scale of the sprite and transform components
				SDL_Rect dstRect = {
					static_cast<int>(transform.position.x),
					static_cast<int>(transform.position.y),
					static_cast<int>(sprite.width * transform.scale.x),
					static_cast<int>(sprite.height * transform.scale.y)
				};

					// Copy the asset with the rotation included
				SDL_RenderCopyEx(
					renderer,
					assetStore->GetTexture(sprite.assetId),
					&srcRect,
					&dstRect,
					transform.rotation,
					NULL,
					SDL_FLIP_NONE
					);
			}

		}
};