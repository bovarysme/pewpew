#include "app.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <thread>

#include "camera.h"

void App::Run() {
  bool success = Initialize();
  if (!success) {
    return;
  }

  bool is_running = true;
  while (is_running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        is_running = false;
      }
    }

    if (!camera_.is_rendering()) {
      camera_.set_is_rendering(true);
      if (rendering_thread_.joinable()) {
        rendering_thread_.join();
      }
      rendering_thread_ =
          std::thread{&Camera::Render, &camera_, std::ref(world_)};
    }

    void* pixels;
    int pitch;
    if (SDL_LockTexture(texture_, nullptr, &pixels, &pitch) < 0) {
      std::cerr << "Error calling SDL_LockTexture: " << SDL_GetError()
                << std::endl;
      break;
    }

    camera_.CopyTo(static_cast<int*>(pixels));
    SDL_UnlockTexture(texture_);

    if (SDL_RenderClear(renderer_) < 0) {
      std::cerr << "Error calling SDL_RenderClear: " << SDL_GetError()
                << std::endl;
      break;
    }

    if (SDL_RenderCopy(renderer_, texture_, nullptr, nullptr) < 0) {
      std::cerr << "Error calling SDL_RenderCopy: " << SDL_GetError()
                << std::endl;
      break;
    }

    SDL_RenderPresent(renderer_);
  }
}

bool App::Initialize() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Error calling SDL_Init: " << SDL_GetError() << std::endl;
    return false;
  }

  const int width = camera_.settings().image_width;
  const int height = camera_.settings().image_height;

  window_ =
      SDL_CreateWindow("pewpew", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, height, SDL_WINDOW_RESIZABLE);
  if (window_ == nullptr) {
    std::cerr << "Error calling SDL_CreateWindow: " << SDL_GetError()
              << std::endl;
    return false;
  }

  renderer_ = SDL_CreateRenderer(
      window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer_ == nullptr) {
    std::cerr << "Error calling SDL_CreateRenderer: " << SDL_GetError()
              << std::endl;
    return false;
  }

  texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING, width, height);
  if (texture_ == nullptr) {
    std::cerr << "Error calling SDL_CreateTexture: " << SDL_GetError()
              << std::endl;
    return false;
  }

  return true;
}