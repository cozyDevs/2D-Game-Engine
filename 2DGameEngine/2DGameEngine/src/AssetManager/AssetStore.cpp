#include "AssetStore.h"
#include "../Logger/Logger.h"
#include <SDL_Image.h>

AssetStore::AssetStore()
{
	Logger::Log("Asset store constructor called");
}

AssetStore::~AssetStore()
{
	// Always clear the assets when the destructor is called
	ClearAssets();
}

void AssetStore::ClearAssets()
{
	// Deallocate the textures
	for (auto texture : textures)
	{
		SDL_DestroyTexture(texture.second);
	}

	// Clear the map of texture assets
	textures.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath)
{
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Adding the texture to the map of textures
	textures.emplace(assetId, texture);
}

// Return the texture based on its id
SDL_Texture* AssetStore::GetTexture(const std::string& assetId)
{
	return textures[assetId];
}