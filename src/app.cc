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

CameraSettings ToCameraSettings(const AppSettings& settings) {
  return CameraSettings{
      .image_width =
          static_cast<int>(settings.window_width * settings.image_scale_factor),
      .image_height = static_cast<int>(settings.window_height *
                                       settings.image_scale_factor),
      .samples_per_pixel = 1 << settings.samples_per_pixel_log2,
      .max_depth = 1 << settings.max_depth_log2,
      .fov = settings.fov,
      .look_from = Point3{settings.look_from},
      .look_at = Point3{settings.look_at},
      .view_up = Vec3{settings.view_up},
      .defocus_angle = settings.defocus_angle,
      .focus_distance = settings.focus_distance,
  };
}

std::string RenderingStateToString(RenderingState state) {
  std::string result;
  switch (state) {
    case RenderingState::kStartRendering:
      result = "kStartRendering";
      break;
    case RenderingState::kRendering:
      result = "kRendering";
      break;
    case RenderingState::kRequestStop:
      result = "kRequeststop";
      break;
    case RenderingState::kUpdateSettings:
      result = "kUpdateSettings";
      break;
    case RenderingState::kIdle:
      result = "kIdle";
      break;
    default:
      result = "Unknown";
  }

  return result;
}

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

    SettingsUpdateType last_update_type = ShowDebugWindow();
    if (last_update_type > settings_update_type_) {
      settings_update_requested_ = true;
      settings_update_type_ = last_update_type;
    }

    NextState();

    if (rendering_state_ == RenderingState::kStartRendering &&
        !camera_.is_rendering()) {
      camera_.set_is_rendering(true);
      camera_.Initialize();
      rendering_thread_ = std::jthread{
          std::bind_front(&Camera::Render, &camera_), std::ref(world_)};
    } else if (rendering_state_ == RenderingState::kRequestStop &&
               !rendering_thread_.get_stop_source().stop_requested()) {
      rendering_thread_.get_stop_source().request_stop();
    } else if (rendering_state_ == RenderingState::kUpdateSettings) {
      camera_.set_settings(ToCameraSettings(settings_));
      camera_.Initialize();

      if (settings_update_type_ ==
          SettingsUpdateType::kUpdateTextureAndSettings) {
        SDL_DestroyTexture(texture_);
        bool success = CreateTexture();
        if (!success) {
          break;
        }
      }

      settings_update_requested_ = false;
      settings_update_type_ = SettingsUpdateType::kNoUpdates;
    }

    bool success = Render();
    if (!success) {
      break;
    }
  }
}

bool App::Initialize() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Error calling SDL_Init: " << SDL_GetError() << std::endl;
    return false;
  }

  window_ = SDL_CreateWindow("pewpew", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, settings_.window_width,
                             settings_.window_height, SDL_WINDOW_RESIZABLE);
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

  bool success = CreateTexture();
  if (!success) {
    return false;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(window_, renderer_);
  ImGui_ImplSDLRenderer2_Init(renderer_);

  return true;
}

bool App::CreateTexture() {
  texture_ = SDL_CreateTexture(
      renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
      camera_.settings().image_width, camera_.settings().image_height);
  if (texture_ == nullptr) {
    std::cerr << "Error calling SDL_CreateTexture: " << SDL_GetError()
              << std::endl;
    return false;
  }

  return true;
}

void App::NextState() {
  switch (rendering_state_) {
    case RenderingState::kStartRendering:
      if (camera_.is_rendering()) {
        rendering_state_ = RenderingState::kRendering;
      } else {
        rendering_state_ = RenderingState::kStartRendering;
      }
      break;
    case RenderingState::kRendering:
      if (settings_update_requested_) {
        rendering_state_ = RenderingState::kRequestStop;
      } else if (camera_.is_rendering()) {
        rendering_state_ = RenderingState::kRendering;
      } else {
        rendering_state_ = RenderingState::kStartRendering;
      }
      break;
    case RenderingState::kRequestStop:
      if (camera_.is_rendering()) {
        rendering_state_ = RenderingState::kRequestStop;
      } else {
        rendering_state_ = RenderingState::kUpdateSettings;
      }
      break;
    case RenderingState::kUpdateSettings:
      if (settings_update_requested_) {
        rendering_state_ = RenderingState::kUpdateSettings;
      } else if (settings_.enable_rendering) {
        rendering_state_ = RenderingState::kStartRendering;
      } else {
        rendering_state_ = RenderingState::kIdle;
      }
      break;
    case RenderingState::kIdle:
      if (settings_update_requested_) {
        rendering_state_ = RenderingState::kUpdateSettings;
      } else {
        rendering_state_ = RenderingState::kIdle;
      }
      break;
  }
}

bool App::Render() {
  if (SDL_RenderClear(renderer_) < 0) {
    std::cerr << "Error calling SDL_RenderClear: " << SDL_GetError()
              << std::endl;
    return false;
  }

  void* pixels;
  int pitch;
  if (SDL_LockTexture(texture_, nullptr, &pixels, &pitch) < 0) {
    std::cerr << "Error calling SDL_LockTexture: " << SDL_GetError()
              << std::endl;
    return false;
  }

  camera_.CopyTo(static_cast<int*>(pixels));
  SDL_UnlockTexture(texture_);

  if (SDL_RenderCopy(renderer_, texture_, nullptr, nullptr) < 0) {
    std::cerr << "Error calling SDL_RenderCopy: " << SDL_GetError()
              << std::endl;
    return false;
  }

  ImGui::Render();
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer_);

  SDL_RenderPresent(renderer_);

  return true;
}

SettingsUpdateType App::ShowDebugWindow() {
  bool has_texture_update = false;
  bool has_settings_update = false;

  ImGui::Begin("Debug");

  ImGui::SeparatorText("Render status");

  ImGui::Text("State: %s", RenderingStateToString(rendering_state_).c_str());

  has_settings_update |=
      ImGui::Checkbox("Rendering", &settings_.enable_rendering);

  ImGui::ProgressBar(camera_.progress(), ImVec2(0.0f, 0.0f));
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("Frame progress");

  ImGui::SeparatorText("Camera settings");

  ImGui::Text("Window size: %dx%d", settings_.window_width,
              settings_.window_height);
  ImGui::Text("Image size: %dx%d", camera_.settings().image_width,
              camera_.settings().image_height);
  has_texture_update |=
      ImGui::DragFloat("Scale factor", &settings_.image_scale_factor,
                       /*v_speed=*/0.1f, /*v_min=*/0.1f,
                       std::numeric_limits<float>::max(), "%.1fx");

  const int max_int_log2 = 30;
  std::string samples_per_pixel =
      std::to_string(1 << settings_.samples_per_pixel_log2);
  has_settings_update |=
      ImGui::DragInt("Samples per pixel", &settings_.samples_per_pixel_log2,
                     /*v_speed=*/0.1f,
                     /*v_min=*/0, max_int_log2, samples_per_pixel.c_str());

  std::string max_depth = std::to_string(1 << settings_.max_depth_log2);
  has_settings_update |=
      ImGui::DragInt("Max depth", &settings_.max_depth_log2, /*v_speed=*/0.1f,
                     /*v_min=*/0, max_int_log2, max_depth.c_str());

  has_settings_update |=
      ImGui::DragFloat("FOV", &settings_.fov, /*v_speed=*/1.0f,
                       /*v_min=*/1.0f,
                       /*v_max=*/179.0f, "%.f deg");

  has_settings_update |=
      ImGui::DragFloat3("Look from", settings_.look_from, /*v_speed=*/0.1f);
  has_settings_update |=
      ImGui::DragFloat3("Look at", settings_.look_at, /*v_speed=*/0.1f);
  has_settings_update |=
      ImGui::DragFloat3("View up", settings_.view_up, /*v_speed=*/0.1f);

  has_settings_update |=
      ImGui::DragFloat("Defocus angle", &settings_.defocus_angle,
                       /*v_speed=*/0.1f,
                       /*v_min=*/0.0f, /*v_max=*/179.0f, "%.1f deg");
  has_settings_update |= ImGui::DragFloat(
      "Focus distance", &settings_.focus_distance,
      /*v_speed=*/0.1f,
      /*v_min=*/0.1f, std::numeric_limits<float>::max(), "%.1f");

  ImGui::End();

  if (has_texture_update) {
    return SettingsUpdateType::kUpdateTextureAndSettings;
  } else if (has_settings_update) {
    return SettingsUpdateType::kUpdateSettings;
  } else {
    return SettingsUpdateType::kNoUpdates;
  }
}