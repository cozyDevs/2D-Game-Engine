//Kosi Chima and Nick tsakir

#include "Game.h"
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

Game::Game() 
{
	std::cout << "Game constructor has been called" << std::endl;
}

Game::~Game()
{
	std::cout << "Game destructor has been called" << std::endl;
}

void Game::Initialize() 
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "Error initializing SDL" << std::endl;
		return;
	}
	SDL_DisplayMode displayMode; 
	SDL_GetCurrentDisplayMode(0, &displayMode);
	windowWidth = displayMode.w;
	windowHeight = displayMode.h;
	 window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
	if (!window) {
		std::cerr << "Error creating SDL window" << std::endl;
		return;
	}
	 renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		std::cerr << "Error creating the SDL renderer" << std::endl;
		return;
	}
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); // try setting sdlfullscreenwindowdesktop is issues persist
	isRunning = true;

}


void Game::ProcessInput()
{
	SDL_Event sdlEvent;
	SDL_PollEvent(&sdlEvent);
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
	playerVelocity = glm::vec2(0.5, 0.0);
	
}

void Game::Update()
{
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}
	//Store current frame time
   millisecsPreviousFrame = SDL_GetTicks();
	
	playerPosition.x += playerVelocity.x;
	playerPosition.y += playerVelocity.y;
	
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