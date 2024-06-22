#ifndef PEWPEW_CAMERA_H_
#define PEWPEW_CAMERA_H_

#include "color.h"
#include "float.h"
#include "hittable.h"
#include "ray.h"
#include "vec3.h"

class Camera {
 public:
  Camera(Float aspect_ratio, int image_width)
      : aspect_ratio_(aspect_ratio), image_width_(image_width) {}

  void Render(const Hittable& world);

 private:
  void Initialize();
  Color RayColor(const Ray& ray, const Hittable& world) const;

  Float aspect_ratio_;
  int image_width_;
  int image_height_;
  Point3 center_;
  Vec3 pixel_delta_u_;
  Vec3 pixel_delta_v_;
  Point3 upper_left_pixel_location_;
};

#endif  // PEWPEW_CAMERA_H_