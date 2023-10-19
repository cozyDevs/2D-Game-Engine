#pragma once
#include <SDL.h>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;


class Game {

	private:
		bool isRunning; //tracks if the application is running
		int millisecsPreviousFrame = 0;
		SDL_Window* window; //object of the sdl window
		SDL_Renderer* renderer;  //object of the sdl renderer

	public:
		Game();
		~Game();
		void Initialize();
		void Run();
		void Setup();
		void ProcessInput();
		void Update();
		void Render();
		void Destroy();

		int windowWidth;
		int windowHeight;



};
