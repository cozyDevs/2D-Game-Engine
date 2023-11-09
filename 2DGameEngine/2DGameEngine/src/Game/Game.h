#pragma once
#include "../ECS/ECS.h"
#include <SDL.h>
#include "../AssetManager/AssetStore.h"

const int FPS = 500;
const int MILLISECS_PER_FRAME = 1000 / FPS;


class Game {

	private:
		bool isRunning; //tracks if the application is running
		int millisecsPreviousFrame = 0;
		SDL_Window* window; //object of the sdl window
		SDL_Renderer* renderer;  //object of the sdl renderer

		std::unique_ptr<Registry> registry;
		std::unique_ptr<AssetStore> assetStore;

	public:
		Game();
		~Game();
		void Initialize();
		void Run();
		void Setup();
		void LoadLevel(int level);
		void ProcessInput();
		void Update();
		void Render();
		void Destroy();

		int windowWidth;
		int windowHeight;



};
