#include "game.hpp"

#define WIDTH  800
#define HEIGHT 600
#define RENDER_QUANTUM 16

Game::Game() 
  : engine { Engine(WIDTH, HEIGHT) },
    level { Scene(WIDTH, HEIGHT) } { 
    currentScene = &level;
    loadTextures(currentScene);
    
    ticks = 0; 
    repaint();
    frames = 1;
    lastDrawTime = lastTime = SDL_GetTicks64();

    score = highScore = 0;
    SDL_Log("Game init ok");
}

void Game::loadTextures(Scene* scene){
	for (auto& e : currentScene -> entities) {
		engine.loadEntityTexture(e);
	}
}

void Game::run(){
    bool exiting = false;
    while (!exiting) {
	    Uint64 t = SDL_GetTicks64();
	    Uint64 dt = t - lastTime;
    	currentScene -> update(dt);
        ticks++;
        lastTime = t;
    
    	if (t - lastDrawTime >= RENDER_QUANTUM) {
        	repaint();
        	frames++;
        	lastDrawTime = t;
    	}

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exiting = true;
                break;
            } else {
                currentScene -> handleEvent(event);
            }
        }
    }
    std::cout << ticks << " ticks and "
      << frames << " frames in " 
      << lastTime << "ms, @" 
      << 1000.0 * ticks / lastTime << " ticks per sec\n" 
      << 1000.0 * frames / lastTime << " fps\n";
}

void Game::repaint(){
    SDL_Renderer* rend = engine.renderer;
    SDL_RenderClear(rend);
    for (auto& e : currentScene->entities) {
        engine.draw(e);
        if (e == &currentScene->bg) {
            engine.draw(e, e->px+WIDTH, e->py);
        }
    }
    SDL_RenderPresent(rend);
}

