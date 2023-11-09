#pragma once
#include <map>
#include <string>
#include <SDL.h>


class AssetStore
{
	// Create map for different asset types
	private:
		std::map<std::string, SDL_Texture*> textures;

	public:
		AssetStore();
		~AssetStore();

		void ClearAssets();
		void AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath);
		SDL_Texture* GetTexture(const std::string& assetId);

};