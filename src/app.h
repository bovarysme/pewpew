#ifndef PEWPEW_APP_H_
#define PEWPEW_APP_H_

#include <SDL2/SDL.h>

#include <string>
#include <thread>

#include "camera.h"
#include "hittable_list.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

struct AppSettings {
  int window_width;
  int window_height;

  bool enable_rendering;

  // Camera settings.
  float image_scale_factor;
  int samples_per_pixel_log2;
  int max_depth_log2;
  float fov;
  float look_from[3];
  float look_at[3];
  float view_up[3];
  float defocus_angle;
  float focus_distance;
};

CameraSettings ToCameraSettings(const AppSettings& settings);

enum class RenderingState {
  kStartRendering,
  kRendering,
  kRequestStop,
  kUpdateSettings,
  kIdle,
};

std::string RenderingStateToString(RenderingState state);

enum class SettingsUpdateType {
  kNoUpdates,
  kUpdateSettings,
  kUpdateTextureAndSettings,
};

class App {
 public:
  App(const AppSettings& settings, const HittableList& world)
      : settings_(settings),
        world_(world),
        camera_(ToCameraSettings(settings)),
        rendering_state_(RenderingState::kStartRendering),
        settings_update_requested_(false),
        settings_update_type_(SettingsUpdateType::kNoUpdates) {}

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
  bool Render();
  bool CreateTexture();
  void NextState();
  SettingsUpdateType ShowDebugWindow();

  AppSettings settings_;
  const HittableList& world_;
  Camera camera_;
  RenderingState rendering_state_;
  bool settings_update_requested_;
  SettingsUpdateType settings_update_type_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  SDL_Texture* texture_;
  std::jthread rendering_thread_;
};

#endif  // PEWPEW_APP_H_