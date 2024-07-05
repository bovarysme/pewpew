#include "app.h"

#include <SDL2/SDL.h>

#include <functional>
#include <iostream>
#include <thread>

#include "camera.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

void App::Run() {
  bool success = Initialize();
  if (!success) {
    return;
  }

  bool show_demo_window = true;

  bool is_running = true;
  while (is_running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) {
        is_running = false;
      }
    }

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow(&show_demo_window);

    if (!camera_.is_rendering()) {
      camera_.set_is_rendering(true);
      if (rendering_thread_.joinable()) {
        rendering_thread_.join();
      }
      rendering_thread_ = std::jthread{
          std::bind_front(&Camera::Render, &camera_), std::ref(world_)};
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

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer_);

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

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(window_, renderer_);
  ImGui_ImplSDLRenderer2_Init(renderer_);

  return true;
}