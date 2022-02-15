#pragma once

#include <SDL.h>

#include "ecs/ecs.h"

class Game {
 public:
  Game();

  bool initialize();
  void terminate();

  void run(bool isSmokeTest = false);

 private:
  static const int kScreenWidth = 1280;
  static const int kScreenHeight = 720;
  static const int kTicksPerUpdate = 16;
  static const int kSmokeTestDuration = 1000;

  bool isRunning_;

  SDL_Window* window_;
  SDL_Renderer* renderer_;
  ECS ecs_;
};
