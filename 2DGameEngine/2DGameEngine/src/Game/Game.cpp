//Kosi Chima and Nick tsakir

#include "Game.h"
#include "../Logger/Logger.h"
#include <iostream>
#include <fstream>
#include <SDL.h>
#include <SDL_Image.h>
#include <glm/glm.hpp>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"

Game::Game() 
{
	isRunning = false;
	registry = std::make_unique<Registry>();
	assetStore = std::make_unique<AssetStore>();
	Logger::Log("Game constructor called!");
	
}

Game::~Game()
{
	Logger::Log("Game destructor called!");

}

void Game::Initialize() 
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		Logger::Err("Error Initializing SDL");

		return;
	}
	SDL_DisplayMode displayMode; 
	SDL_GetCurrentDisplayMode(0, &displayMode);
	windowWidth = displayMode.w;
	windowHeight = displayMode.h;
	 window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
	if (!window) {
		Logger::Err("error creating SDL window.");
		return;
	}
	 renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		Logger::Err("Error creating SDL renderer.");
		return;
	}
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); // try setting sdlfullscreenwindowdesktop is issues persist
	isRunning = true;

}


void Game::ProcessInput()
{
	SDL_Event sdlEvent;
	//SDL_PollEvent(&sdlEvent);
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}
			break;

		}
	}
}

glm::vec2 playerPosition;
glm::vec2 playerVelocity;

void Game::LoadLevel(int level)
{
	playerPosition = glm::vec2(10.0, 20.0);
	playerVelocity = glm::vec2(109.0, 0.0);

	// Adding systems to the game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<AnimationSystem>();


	// Adding assets to the asset store
	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper.png");
	assetStore->AddTexture(renderer, "crab-image", "./assets/images/blue_crab_attack.png");
	assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
	assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

	
	// Load the tilemap
	int tileSize = 32;
	double tileScale = 2.0;
	int mapNumCol = 25;
	int mapNumRows = 20;

	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	for (int y = 0; y < mapNumRows; y++)
	{
		for (int x = 0; x < mapNumCol; x++)
		{
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = registry->CreateEntity();
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), (y * (tileScale * tileSize))), glm::vec2(tileScale, tileScale), 0.0);
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);
		}

	}

	mapFile.close();
	

	// Creating entities 
	//Entity tank = registry->CreateEntity();
	Entity chopper = registry->CreateEntity();
	Entity crab = registry->CreateEntity();

	// Adding components to entities 
	//tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	//tank.AddComponent<RigidBodyComponent>(glm::vec2(50, 0));
	//tank.AddComponent<SpriteComponent>("tank-image", 64, 64, 1);

	chopper.AddComponent<TransformComponent>(glm::vec2(20.0, 200.0), glm::vec2(4.0, 4.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(50, 0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.AddComponent<AnimationComponent>(2, 10, true);


	Entity radar = registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1);
	radar.AddComponent<AnimationComponent>(8, 5, true);

	// Crab entity
	crab.AddComponent<TransformComponent>(glm::vec2(150.0, 80.0), glm::vec2(0.7, 0.7), 90.0);
	crab.AddComponent<RigidBodyComponent>(glm::vec2(50, 0));
	crab.AddComponent<SpriteComponent>("crab-image", 762, 733, 1);
}

void Game::Setup() {
	LoadLevel(1);
}

void Game::Update()
{
	//if we are too fast, waste some time until we reach the Millisecs fper frame
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}

	//The difference in ticks since the last frame, converted into seconds.
	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
	//Store current frame time
   millisecsPreviousFrame = SDL_GetTicks();
	
	playerPosition.x += playerVelocity.x * deltaTime;
	playerPosition.y += playerVelocity.y * deltaTime;

	// Update registry function that manages entities waiting to be created or destroyed
	// Called at the end of the frame
	registry->Update();

	// Receive the movement system update function and pass in delta time
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update();
	
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 0, 151, 218, 255); //Change back to 255,0,0 if needed
	SDL_RenderClear(renderer); 

	SDL_Surface* surface = IMG_Load("./assets/images/blue_crab_attack.png");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	SDL_Rect dstRect = { static_cast<int>(playerPosition.x), static_cast<int>(playerPosition.y), 132, 132 }; // destination that the texture should be placed on

	SDL_RenderCopy(renderer, texture, NULL, &dstRect); //null because we want the entire source image not just a piece
	SDL_DestroyTexture(texture);

	// Update the systems that need to render
	registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

	SDL_RenderPresent(renderer); 
	// SDL uses two render buffers(back and front) back enables developer editing, front buffer displays the SDL screen, helpful to avoid render glitches, it is important to select chosen renderer.
}

void Game::Run()
{
	Setup();
	while (isRunning) {
		ProcessInput();
		Update();
		Render();
	} 
}

void Game::Destroy() 
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}