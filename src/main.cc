#include <SDL2/SDL.h>

#include <memory>
#include <thread>

#include "camera.h"
#include "color.h"
#include "dielectric.h"
#include "hittable_list.h"
#include "lambertian.h"
#include "material.h"
#include "metal.h"
#include "sphere.h"
#include "vec3.h"

int main(int argc, char** argv) {
  HittableList world;
  Lambertian ground_material{Color{0.5, 0.5, 0.5}};
  world.Add(
      std::make_shared<Sphere>(Point3{0, -1000, 0}, 1000, &ground_material));

  std::vector<std::unique_ptr<Material>> materials;
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      Float choose_mat = RandomFloat();
      Float center_x = a + 0.9 * RandomFloat();
      Float center_z = b + 0.9 * RandomFloat();
      Point3 center{center_x, 0.2, center_z};

      if ((center - Point3{4, 0.2, 0}).length() > 0.9) {
        if (choose_mat < 0.8) {
          // Diffuse.
          Color albedo = Color::Random() * Color::Random();
          materials.push_back(std::make_unique<Lambertian>(albedo));
          world.Add(
              std::make_shared<Sphere>(center, 0.2, materials.back().get()));
        } else if (choose_mat < 0.95) {
          // Metal.
          Color albedo = Color::Random(0.5, 1);
          Float fuzz = RandomFloat(0, 0.5);
          materials.push_back(std::make_unique<Metal>(albedo, fuzz));
          world.Add(
              std::make_shared<Sphere>(center, 0.2, materials.back().get()));
        } else {
          // Glass.
          materials.push_back(std::make_unique<Dielectric>(1.5));
          world.Add(
              std::make_shared<Sphere>(center, 0.2, materials.back().get()));
        }
      }
    }
  }

  Dielectric material1{1.5};
  world.Add(std::make_shared<Sphere>(Point3{0, 1, 0}, 1.0, &material1));

  Lambertian material2{Color{0.4, 0.2, 0.1}};
  world.Add(std::make_shared<Sphere>(Point3{-4, 1, 0}, 1.0, &material2));

  Metal material3{Color{0.7, 0.6, 0.5}, 0.0};
  world.Add(std::make_shared<Sphere>(Point3{4, 1, 0}, 1.0, &material3));

  const int width = 640;
  const int height = 360;

  CameraSettings settings{
      .image_width = width,
      .image_height = height,
      .samples_per_pixel = 1,
      .max_depth = 8,

      .fov = 20,
      .look_from = Point3{13, 2, 3},
      .look_at = Point3{0, 0, 0},
      .view_up = Vec3{0, 1, 0},

      .defocus_angle = 0.6,
      .focus_distance = 10.0,
  };

  Camera camera{settings};

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Error calling SDL_Init: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_Window* window = SDL_CreateWindow("pewpew", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, width, height,
                                        SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    std::cerr << "Error calling SDL_CreateWindow: " << SDL_GetError()
              << std::endl;
    return -1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    std::cerr << "Error calling SDL_CreateRenderer: " << SDL_GetError()
              << std::endl;
    return -1;
  }

  SDL_Texture* texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, width, height);
  if (texture == NULL) {
    std::cerr << "Error calling SDL_CreateTexture: " << SDL_GetError()
              << std::endl;
    return -1;
  }

  bool run = true;
  std::thread thread;
  while (run) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        run = false;
      }
    }

    if (!camera.is_rendering()) {
      camera.set_is_rendering(true);
      if (thread.joinable()) {
        thread.join();
      }
      thread = std::thread{&Camera::Render, &camera, std::ref(world)};
    }

    void* pixels;
    int pitch;
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0) {
      std::cerr << "Error calling SDL_LockTexture: " << SDL_GetError()
                << std::endl;
      break;
    }

    camera.CopyTo(static_cast<int*>(pixels));
    SDL_UnlockTexture(texture);

    if (SDL_RenderClear(renderer) < 0) {
      std::cerr << "Error calling SDL_RenderClear: " << SDL_GetError()
                << std::endl;
      break;
    }

    if (SDL_RenderCopy(renderer, texture, NULL, NULL) < 0) {
      std::cerr << "Error calling SDL_RenderCopy: " << SDL_GetError()
                << std::endl;
      break;
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}