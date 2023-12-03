#include "LevelLoader.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include <fstream>
#include "Game.h"
#include "sol/sol.hpp""
#include <string>

LevelLoader::LevelLoader()
{



}

LevelLoader::~LevelLoader()
{



}

void LevelLoader::LoadLevel(sol::state& lua, std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& assetStore,
	SDL_Renderer* renderer, int levelNumber)
{
	sol::load_result script = lua.script_file("./assets/scripts/level" + std::to_string(levelNumber) + ".lua");


	// Check syntax of the script

	if (!script.valid())
	{
		sol::error err = script;
		std::string errorMessage = err.what();
		Logger::Err("Error loading the lua script: " + errorMessage);
		return;
	}

	lua.script_file("./assets/scripts/level" + std::to_string(levelNumber) + ".lua");

	sol::table level = lua["Level"];

	// Reading the level assets from the Lua
	sol::table assets = level["assets"];


	// Adding assets through the Lua code
	int i = 0;
	while (true)
	{
		sol::optional<sol::table> hasAsset = assets[i];
		if (hasAsset == sol::nullopt)
		{
			break;
		}


		sol::table asset = assets[i];
		
		std::string assetType = asset["type"];
		std::string assetId = asset["id"];
		if (assetType == "texture")
		{
			assetStore->AddTexture(renderer, assetId, asset["file"]);
		}

		i++;
	}

	// Setting up the tilemap through Lua
	sol::table map = level["tilemap"];
	std::string mapFilePath = map["map_file"];
	std::string mapTextureAssetId = map["texture_asset_id"];
	int mapNumRows = map["num_rows"];
	int mapNumCols = map["num_cols"];
	int tileSize = map["tile_size"];
	double mapScale = map["scale"];
	std::fstream mapFile;
	mapFile.open(mapFilePath);
	for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = registry->CreateEntity();
			tile.AddComponent<TransformComponent>(glm::vec2(x * (mapScale * tileSize), y * (mapScale * tileSize)), glm::vec2(mapScale, mapScale), 0.0);
			tile.AddComponent<SpriteComponent>(mapTextureAssetId, tileSize, tileSize, 0, srcRectX, srcRectY);
		}
	}
	mapFile.close();
	Game::mapWidth = mapNumCols * tileSize * mapScale;
	Game::mapHeight = mapNumRows * tileSize * mapScale;


	// Adding the entities and components through the Lua script
	sol::table entities = level["entities"];
	i = 0;
	while (true) {
		sol::optional<sol::table> hasEntity = entities[i];
		if (hasEntity == sol::nullopt) {
			break;
		}

		sol::table entity = entities[i];

		Entity newEntity = registry->CreateEntity();

		/*
		sol::optional<std::string> tag = entity["tag"];
		if (tag != sol::nullopt) {
			newEntity.Tag(entity["tag"]);
		}

		sol::optional<std::string> group = entity["group"];
		if (group != sol::nullopt) {
			newEntity.Group(entity["group"]);
		}
		*/

		sol::optional<sol::table> hasComponents = entity["components"];
		if (hasComponents != sol::nullopt) {

			sol::optional<sol::table> transform = entity["components"]["transform"];
			if (transform != sol::nullopt) {
				newEntity.AddComponent<TransformComponent>(
					glm::vec2(
						entity["components"]["transform"]["position"]["x"],
						entity["components"]["transform"]["position"]["y"]
					),
					glm::vec2(
						entity["components"]["transform"]["scale"]["x"].get_or(1.0),
						entity["components"]["transform"]["scale"]["y"].get_or(1.0)
					),
					entity["components"]["transform"]["rotation"].get_or(0.0)
				);
			}

			// Adding the RigidBody component and values
			sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
			if (rigidbody != sol::nullopt) {
				newEntity.AddComponent<RigidBodyComponent>(
					glm::vec2(
						entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
						entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
					)
				);
			}

			// Sprite components and values
			sol::optional<sol::table> sprite = entity["components"]["sprite"];
			if (sprite != sol::nullopt) {
				newEntity.AddComponent<SpriteComponent>(
					entity["components"]["sprite"]["texture_asset_id"],
					entity["components"]["sprite"]["width"],
					entity["components"]["sprite"]["height"],
					entity["components"]["sprite"]["z_index"].get_or(1),
					entity["components"]["sprite"]["src_rect_x"].get_or(0),
					entity["components"]["sprite"]["src_rect_y"].get_or(0)
				);
			}

			// Animation component and values
			sol::optional<sol::table> animation = entity["components"]["animation"];
			if (animation != sol::nullopt) {
				newEntity.AddComponent<AnimationComponent>(
					entity["components"]["animation"]["num_frames"].get_or(1),
					entity["components"]["animation"]["speed_rate"].get_or(1)
				);
			}

		}

		i++;
	}
	// Adding assets to the asset store
	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper.png");
	assetStore->AddTexture(renderer, "crab-image", "./assets/images/blue_crab_attack.png");
	assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
	assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");



	// Creating entities 
	//Entity tank = registry->CreateEntity();
	Entity chopper = registry->CreateEntity();
	Entity crab = registry->CreateEntity();
	Entity radar = registry->CreateEntity();

	// Adding components to entities 
	//tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	//tank.AddComponent<RigidBodyComponent>(glm::vec2(50, 0));
	//tank.AddComponent<SpriteComponent>("tank-image", 64, 64, 1);

	chopper.AddComponent<TransformComponent>(glm::vec2(20.0, 200.0), glm::vec2(4.0, 4.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(50, 0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.AddComponent<AnimationComponent>(2, 10, true);

	radar.AddComponent<TransformComponent>(glm::vec2(Game::windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1);
	radar.AddComponent<AnimationComponent>(8, 5, true);

	// Crab entity
	crab.AddComponent<TransformComponent>(glm::vec2(150.0, 80.0), glm::vec2(0.7, 0.7), 90.0);
	crab.AddComponent<RigidBodyComponent>(glm::vec2(50, 0));
	crab.AddComponent<SpriteComponent>("crab-image", 762, 733, 1);

}