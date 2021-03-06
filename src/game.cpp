#include <cstdlib>
#include <ctime>

#include "game.hpp"

#define WIDTH  800
#define HEIGHT 600
#define RENDER_QUANTUM 16

Game::Game() 
  : engine { Engine(WIDTH, HEIGHT) },
    gameScreen { GameScreen(&engine) },
    startScreen { StartScreen (&engine) }, 
    scoresScreen { ScoresScreen (&engine) } {
    scenes = std::vector<Scene *> {&startScreen, &gameScreen};
    currentScene = &startScreen;
    frames = ticks = 0; 
    lastDrawTime = lastTime = SDL_GetTicks64();
    gameScreen.maxScore = 0;
    srand(time(NULL));
    // SDL_Log("Game init ok");
}

void Game::run(){
    bool exiting = false;
    // SDL_Log("Game running");
    repaint();
    frames++;
    while (!exiting) {
	    Uint64 t = SDL_GetTicks64();
	    Uint64 dt = t - lastTime;

        // по два update на repaint
        if (dt < RENDER_QUANTUM / 2) {
            SDL_Delay(RENDER_QUANTUM/2 - dt);
            t = SDL_GetTicks64();
            dt = t - lastTime;
            continue;
        }
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
            switch (event.type) {
              case SDL_QUIT:
                exiting = true;
                break;
              case SDL_USEREVENT: 
                if (event.user.code == GO_PLAY) {
                    currentScene = &gameScreen;
                    currentScene->reset();
                }
                if (event.user.code == GO_START) {
                    currentScene = &startScreen;
                    currentScene->reset();
                } 
                if (event.user.code == GO_SCORES) {
                    currentScene = &scoresScreen;
                    currentScene->reset();
                } 
                if (event.user.code == UPD_SCORES) {
                    auto hiscores = &scoresScreen.scores;
                    hiscores->push_back(*((int*) event.user.data1));
                    std::sort(hiscores->begin(), hiscores->end(), std::greater<unsigned>{});
                    hiscores->pop_back();
                }
                break;
              default:
                currentScene -> handleEvent(event);
            }
        }
    }
    SDL_Log("Stats: %llu ticks and %llu frames in %llums @%f ticks per sec, %f fps", 
           ticks, frames, lastTime, 
           1000.0 * ticks / lastTime, 1000.0 * frames / lastTime);
}

void Game::repaint(){
    SDL_RenderClear(engine.renderer);
    for (auto& e : currentScene->entities) {
        if (e != currentScene->entities[0]){
            engine.draw(e);
        } else {
            engine.draw(e, (int)e->pos.x, (int)e->pos.y, WIDTH, HEIGHT);
            engine.draw(e, (int)e->pos.x + WIDTH, (int)e->pos.y, WIDTH, HEIGHT);
        }
    }   
    SDL_RenderPresent(engine.renderer);
}

int main(){
    Game game;
    game.run();
    return 0;
}
