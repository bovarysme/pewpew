#ifndef PEWPEW_APP_H_
#define PEWPEW_APP_H_

#include <SDL2/SDL.h>

#include <thread>

#include "camera.h"
#include "hittable_list.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

class App {
 public:
  App(Camera& camera, const HittableList& world)
      : camera_(camera), world_(world) {}

  ~App() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if (texture_ != nullptr) {
      SDL_DestroyTexture(texture_);
    }
    if (renderer_ != nullptr) {
      SDL_DestroyRenderer(renderer_);
    }
    if (window_ != nullptr) {
      SDL_DestroyWindow(window_);
    }
    SDL_Quit();
  }

  void Run();

 private:
  bool Initialize();

  Camera& camera_;
  const HittableList& world_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  SDL_Texture* texture_;
  std::jthread rendering_thread_;
};

#endif  // PEWPEW_APP_H_