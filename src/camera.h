#ifndef PEWPEW_CAMERA_H_
#define PEWPEW_CAMERA_H_

#include "color.h"
#include "float.h"
#include "hittable.h"
#include "ray.h"
#include "vec3.h"

class Camera {
 public:
  Camera(int image_width, int image_height, int samples_per_pixel,
         int max_depth)
      : image_width_(image_width),
        image_height_(image_height),
        samples_per_pixel_(samples_per_pixel),
        max_depth_(max_depth) {}

  void Render(const Hittable& world);

 private:
  void Initialize();
  Ray GetRay(int i, int j) const;
  Color RayColor(const Ray& ray, int depth, const Hittable& world) const;

  int image_width_;
  int image_height_;
  int samples_per_pixel_;
  int max_depth_;
  Float pixel_samples_scale_;
  Point3 center_;
  Vec3 pixel_delta_u_;
  Vec3 pixel_delta_v_;
  Point3 upper_left_pixel_location_;
};

#endif  // PEWPEW_CAMERA_H_