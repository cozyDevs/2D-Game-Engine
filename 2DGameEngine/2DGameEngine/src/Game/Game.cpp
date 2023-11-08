//Kosi Chima and Nick tsakir

#include "Game.h"
#include "../Logger/Logger.h"
#include <iostream>
#include <SDL.h>
#include <SDL_Image.h>
#include <glm/glm.hpp>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

Game::Game() 
{
	isRunning = false;
	registry = std::make_unique<Registry>();
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


void Game::Setup() {
	playerPosition = glm::vec2(10.0, 20.0);
	playerVelocity = glm::vec2(109.0, 0.0);

	// Adding systems to the game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();

	// Creating entities 
	Entity tank = registry->CreateEntity();
	Entity square = registry->CreateEntity();

	// Adding components to entities 
	tank.AddComponent<TransformComponent>(glm::vec2(10.0, 20.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(50, 0));
	tank.AddComponent<SpriteComponent>(10, 10);

	square.AddComponent<TransformComponent>(glm::vec2(10.0, 20.0), glm::vec2(1.0, 1.0), 0.0);
	square.AddComponent<RigidBodyComponent>(glm::vec2(0, 50));
	square.AddComponent<SpriteComponent>(50, 50);
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


	// Receive the movement system update function and pass in delta time
	registry->GetSystem<MovementSystem>().Update(deltaTime);

	// Update registry function that manages entities waiting to be created or destroyed
	// Called at the end of the frame
	registry->Update();
	
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
	registry->GetSystem<RenderSystem>().Update(renderer);

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