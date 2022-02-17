#pragma once

#include "ecs/ecs.h"

class RenderSprite {
 public:
  static void render(ECS& ecs, SDL_Renderer* renderer, float delay) {
    Physics* physics = ecs.physics();
    Transform* transform = ecs.transform();
    Collider* collider = ecs.collider();
    Sprite* sprite = ecs.sprite();

    for (size_t i = 0; i < ecs.allIds.size(); i++) {
      int id = ecs.allIds[i];

      if (sprite[id].texture == nullptr) continue;

      SDL_Rect rect;

      if (delay > 0) {
        Vector2 velocity = physics[id].velocity * delay;
        Vector2 position = transform[id].position + velocity;

        SDL_Point point = Utils::topLeftPoint(position, collider[id].rect);

        rect = {point.x, point.y, collider[id].rect.w, collider[id].rect.h};
      } else {
        rect = collider[id].rect;
      }

      SDL_RenderCopy(renderer, sprite[id].texture, nullptr, &rect);
    }
  }
};
