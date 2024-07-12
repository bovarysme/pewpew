#ifndef PEWPEW_CAMERA_H_
#define PEWPEW_CAMERA_H_

#include <atomic>
#include <chrono>
#include <mutex>
#include <stop_token>
#include <vector>

#include "app_settings.h"
#include "color.h"
#include "float.h"
#include "hittable.h"
#include "ray.h"
#include "vec3.h"

enum class SettingsUpdateType;

struct CameraSettings {
  int image_width;
  int image_height;
  int samples_per_pixel_log2;
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
      : settings_{settings}, num_color_components_{3} {}

  void Initialize(SettingsUpdateType type);
  void InitializePhase();
  void Render(std::stop_token token, const Hittable& world);
  void StoreImage();
  Float Progress() const;
  void CopyTo(int* buffer);

  const CameraSettings& settings() const { return settings_; }
  void set_settings(const CameraSettings& settings) { settings_ = settings; }

  bool is_rendering() const { return is_rendering_; }
  bool done_rendering() const { return done_rendering_; }

  int current_phase() const { return current_phase_; }
  int last_phase() const { return last_phase_; }
  int current_phase_samples_per_pixel() const {
    return current_phase_samples_per_pixel_;
  }

  double global_render_time() const { return global_render_time_; }
  double phase_render_time() const { return phase_render_time_; }

 private:
  Ray GetRay(int i, int j) const;
  Color RayColor(const Ray& ray, int depth, const Hittable& world) const;
  void WriteImage();
  Point3 SampleDefocusDisk() const;

  CameraSettings settings_;
  const int num_color_components_;

  std::vector<Float> pixel_data_;
  std::vector<uint8_t> image_data_;
  std::mutex image_data_mutex_;

  std::atomic<bool> is_rendering_;
  std::atomic<bool> done_rendering_;

  int current_phase_;
  int last_phase_;
  int current_phase_samples_per_pixel_;
  int accumulated_samples_per_pixel_;
  Float pixel_samples_scale_;

  std::atomic<double> global_render_time_;
  std::atomic<double> phase_render_time_;
  std::atomic<int> scanlines_rendered_;

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