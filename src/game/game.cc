#include "game.h"

#include <SDL_image.h>
#include <SDL_ttf.h>

#include <sstream>

#include "services/locator.h"
#include "texture/texture.h"

Game::Game() : isRunning_(true) {
  window_ = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kScreenWidth,
                             kScreenHeight, SDL_WINDOW_SHOWN);

  renderer_ = SDL_CreateRenderer(
      window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

bool Game::initialize() {
  bool success = true;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    Locator::logger().print("SDL could not initialize!");
    success = false;
  }

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    Locator::logger().print("SDL_image could not initialize!");
    success = false;
  }

  // Initialize SDL_ttf
  if (TTF_Init() == -1) {
    Locator::logger().print("SDL_ttf could not initialize!");
    success = false;
  }

  // Initialize entities
  ecs_.initialize(kScreenWidth, kScreenHeight, renderer_);

  return success;
}

void Game::terminate() {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  TTF_Quit();
  SDL_Quit();
}

void Game::run(bool isSmokeTest) {
#ifdef DEBUG
  Uint64 startTime = SDL_GetTicks64();
  Uint64 totalFrames = 0;
  TTF_Font* font =
      TTF_OpenFont("../../data/fonts/PressStart2P-Regular.ttf", 32);
  SDL_Color color = {255, 255, 255, 255};
  std::stringstream text;
  Texture texture;
#endif

  Uint64 previous = SDL_GetTicks64();
  Uint64 lag = 0;

  SDL_Event event;

  while (isRunning_) {
    Uint64 current = SDL_GetTicks64();
    Uint64 elapsed = current - previous;
    previous = current;
    lag += elapsed;

    // Process input
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        isRunning_ = false;
      }

      ecs_.input(event);
    }

    // Update state
    while (lag >= kTicksPerUpdate) {
      ecs_.update();
      lag -= kTicksPerUpdate;

#ifdef DEBUG
      Uint64 currentTime = SDL_GetTicks64();
      float totalSeconds = static_cast<float>(currentTime - startTime) / 1000;

      float fps = static_cast<float>(totalFrames) / totalSeconds;

      text.str("");
      text << "Average FPS " << fps;

      texture.load_text(renderer_, font, text.str().c_str(), color);
#endif
    }

    // Render graphics
    float delay = static_cast<float>(lag) / kTicksPerUpdate;
    ecs_.render(renderer_, delay);

#ifdef DEBUG
    texture.render(renderer_, 0, 0);
    ++totalFrames;
#endif

    SDL_RenderPresent(renderer_);

    // Handle testing
    if (isSmokeTest && current > kSmokeTestDuration) {
      isRunning_ = false;
    }
  }
}

void Game::benchmark() {
  Locator::logger().print("Starting benchmark...");

  int totalUpdates = 1000;

  Uint64 ecsStartTime = SDL_GetTicks64();
  for (int i = 0; i < totalUpdates; i++) {
    ecs_.update();
    ecs_.render(renderer_, 0.0f);
    SDL_RenderPresent(renderer_);
  }
  Uint64 ecsEndTime = SDL_GetTicks64() - ecsStartTime;

  Locator::logger().print("ECS total time (ms):");
  Locator::logger().print(std::to_string(ecsEndTime).c_str());
}
