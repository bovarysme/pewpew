#ifndef PEWPEW_VEC3_H_
#define PEWPEW_VEC3_H_

#include <cmath>

#include "float.h"
#include "utils.h"

class Vec3 {
 public:
  Vec3() : e_{0, 0, 0} {}
  Vec3(Float e0, Float e1, Float e2) : e_{e0, e1, e2} {}

  Vec3& operator+=(const Vec3& value) {
    e_[0] += value.x();
    e_[1] += value.y();
    e_[2] += value.z();

    return *this;
  }

  Vec3& operator-=(const Vec3& value) {
    e_[0] -= value.x();
    e_[1] -= value.y();
    e_[2] -= value.z();

    return *this;
  }

  Vec3& operator*=(const Vec3& value) {
    e_[0] *= value.x();
    e_[1] *= value.y();
    e_[2] *= value.z();

    return *this;
  }

  Vec3& operator*=(double value) {
    e_[0] *= value;
    e_[1] *= value;
    e_[2] *= value;

    return *this;
  }

  Vec3& operator/=(double value) {
    e_[0] /= value;
    e_[1] /= value;
    e_[2] /= value;

    return *this;
  }

  static Vec3 Random(double min, double max) {
    return Vec3{RandomFloat(min, max), RandomFloat(min, max),
                RandomFloat(min, max)};
  }

  Float x() const { return e_[0]; }
  Float y() const { return e_[1]; }
  Float z() const { return e_[2]; }

  Float length_squared() const {
    return e_[0] * e_[0] + e_[1] * e_[1] + e_[2] * e_[2];
  }

  Float length() const { return std::sqrt(length_squared()); }

  bool near_zero() const {
    const Float epsilon = 1e-8;
    return (std::fabs(e_[0]) < epsilon) && (std::fabs(e_[1]) < epsilon) &&
           (std::fabs(e_[2]) < epsilon);
  }

 private:
  Float e_[3];
};

using Point3 = Vec3;

inline Vec3 operator-(const Vec3& value) {
  return Vec3{-value.x(), -value.y(), -value.z()};
}

inline Vec3 operator+(Vec3 lhs, const Vec3& rhs) { return lhs += rhs; }

inline Vec3 operator-(Vec3 lhs, const Vec3& rhs) { return lhs -= rhs; }

inline Vec3 operator*(Vec3 lhs, const Vec3& rhs) { return lhs *= rhs; }

inline Vec3 operator*(Vec3 lhs, double rhs) { return lhs *= rhs; }

inline Vec3 operator*(double lhs, Vec3 rhs) { return rhs *= lhs; }

inline Vec3 operator/(Vec3 lhs, double rhs) { return lhs /= rhs; }

inline Float Dot(const Vec3& lhs, const Vec3& rhs) {
  return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
}

inline Vec3 UnitVector(const Vec3& value) { return value / value.length(); }

inline Vec3 RandomInUnitSphere() {
  while (true) {
    const Vec3 p = Vec3::Random(-1, 1);
    if (p.length_squared() < 1) {
      return p;
    }
  }
}

inline Vec3 RandomUnitVector() { return UnitVector(RandomInUnitSphere()); }

inline Vec3 RandomOnHemisphere(const Vec3& normal) {
  const Vec3 on_unit_sphere = RandomUnitVector();
  return Dot(on_unit_sphere, normal) > 0.0 ? on_unit_sphere : -on_unit_sphere;
}

#endif  // PEWPEW_VEC3_H_