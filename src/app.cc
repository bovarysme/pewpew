#include "app.h"

#include <SDL2/SDL.h>

#include <functional>
#include <iostream>
#include <limits>
#include <string>
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

    ShowDebugWindow();

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

  window_ =
      SDL_CreateWindow("pewpew", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       window_width_, window_height_, SDL_WINDOW_RESIZABLE);
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

  texture_ = SDL_CreateTexture(
      renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
      camera_.settings().image_width, camera_.settings().image_height);
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

void App::ShowDebugWindow() {
  static bool is_rendering = true;
  static float scale_factor = 0.5;

  static int image_width = 640;
  static int image_height = 360;
  static int samples_per_pixel_log2 = 0;
  static int max_depth_log2 = 3;
  static float fov = 20;
  static float look_from[3] = {13, 2, 3};
  static float look_at[3] = {0, 0, 0};
  static float view_up[3] = {0, 1, 0};
  static float defocus_angle = 0.6;
  static float focus_distance = 10.0;

  ImGui::Begin("Debug");

  ImGui::SeparatorText("Render status");

  ImGui::Checkbox("Rendering", &is_rendering);

  ImGui::ProgressBar(camera_.progress(), ImVec2(0.0f, 0.0f));
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("Frame progress");

  ImGui::SeparatorText("Camera settings");

  ImGui::Text("Window size: %dx%d", window_width_, window_height_);
  ImGui::Text("Image size: %dx%d", image_width, image_height);
  ImGui::DragFloat("Scale factor", &scale_factor, /*v_speed=*/0.1f,
                   /*v_min=*/0.1f, std::numeric_limits<float>::max(), "%.1fx");

  const int max_int_log2 = 30;
  std::string samples_per_pixel = std::to_string(1 << samples_per_pixel_log2);
  ImGui::DragInt("Samples per pixel", &samples_per_pixel_log2, /*v_speed=*/0.1f,
                 /*v_min=*/0, max_int_log2, samples_per_pixel.c_str());

  std::string max_depth = std::to_string(1 << max_depth_log2);
  ImGui::DragInt("Max depth", &max_depth_log2, /*v_speed=*/0.1f, /*v_min=*/0,
                 max_int_log2, max_depth.c_str());

  ImGui::DragFloat("FOV", &fov, /*v_speed=*/1.0f, /*v_min=*/1.0f,
                   /*v_max=*/179.0f, "%.f deg");

  ImGui::DragFloat3("Look from", look_from, /*v_speed=*/0.1f);
  ImGui::DragFloat3("Look at", look_at, /*v_speed=*/0.1f);
  ImGui::DragFloat3("View up", view_up, /*v_speed=*/0.1f);

  ImGui::DragFloat("Defocus angle", &defocus_angle, /*v_speed=*/0.1f,
                   /*v_min=*/0.0f, /*v_max=*/179.0f, "%.1f deg");
  ImGui::DragFloat("Focus distance", &focus_distance, /*v_speed=*/0.1f,
                   /*v_min=*/0.1f, std::numeric_limits<float>::max(), "%.1f");

  ImGui::End();
}