#ifndef PEWPEW_CAMERA_H_
#define PEWPEW_CAMERA_H_

#include <atomic>
#include <chrono>
#include <mutex>
#include <stop_token>
#include <vector>

#include "color.h"
#include "float.h"
#include "hittable.h"
#include "ray.h"
#include "vec3.h"

struct CameraSettings {
  int image_width;
  int image_height;
  int samples_per_pixel;
  int max_depth;
  Float fov;
  Point3 look_from;
  Point3 look_at;
  Vec3 view_up;
  Float defocus_angle;
  Float focus_distance;
};

class Camera {
 public:
  Camera(CameraSettings settings)
      : settings_{settings},
        is_rendering_{false},
        scanlines_rendered_{0},
        render_time_{0.0},
        num_color_components_{3} {}

  void Initialize();
  void Render(std::stop_token token, const Hittable& world);
  Float Progress() const;
  void CopyTo(int* buffer);

  const CameraSettings& settings() const { return settings_; }
  void set_settings(const CameraSettings& settings) { settings_ = settings; }
  bool is_rendering() const { return is_rendering_; }
  void set_is_rendering(bool is_rendering) { is_rendering_ = is_rendering; }
  double render_time() const { return render_time_; }

 private:
  Ray GetRay(int i, int j) const;
  Color RayColor(const Ray& ray, int depth, const Hittable& world) const;
  void WriteImage();
  Point3 SampleDefocusDisk() const;

  CameraSettings settings_;
  std::atomic<bool> is_rendering_;
  std::atomic<int> scanlines_rendered_;
  double render_time_;
  int num_color_components_;
  std::vector<int> pixel_data_;
  std::mutex pixel_data_mutex_;
  Float pixel_samples_scale_;
  Point3 center_;
  Vec3 u_;
  Vec3 v_;
  Vec3 w_;
  Vec3 pixel_delta_u_;
  Vec3 pixel_delta_v_;
  Point3 upper_left_pixel_location_;
  Vec3 defocus_disk_u_;
  Vec3 defocus_disk_v_;
};

#endif  // PEWPEW_CAMERA_H_