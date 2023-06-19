#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() : radius(0), center(0, 0, 0){
        // unit ball at the center
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material), center(center), radius(radius) {
        // 
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        //
        Vector3f Rd = r.getDirection().normalized(); // 标准化后的光线方向向量
        Vector3f origin = r.getOrigin();
        Vector3f l = center - origin;   // 光线指向球心的向量
        float tp = Vector3f::dot(l, Rd);
        float l_square = l.squaredLength();
        float r_square = radius * radius;

        // d > r，不相交
        float d = sqrt(fabs(l_square - tp * tp));
        if (d > radius){
            return false;
        }

        float t_prime = r_square - d * d;
        float t = tp - t_prime;
        if (t > tmin && t < h.getT()){
            Vector3f OP(t * Rd + origin - center);
            Vector3f normal = OP.normalized();
            float u = 0.5 + atan2(normal.x(), normal.z()) / (2 * M_PI);
            float v = 0.5 - asin(normal.y()) / M_PI;
            h.set(t, material, getNormal(normal, OP, u, v),
              material->getColor(u, v), t * Rd + origin);
            return true;
        } else {
            return false;
        }
    }

    Vector3f getNormal(const Vector3f &n, const Vector3f &p, float u, float v) {
        Vector2f grad(0);
        float f = material->bump.getDisturb(u, v, grad);
        if (fabs(f) < FLT_EPSILON) return n;
        float phi = u * 2 * M_PI, theta = M_PI - v * M_PI;
        Vector3f pu(-p.z(), 0, p.x()),
            pv(p.y() * cos(phi), -radius * sin(theta), p.y() * sin(phi));
        if (pu.squaredLength() < FLT_EPSILON) return n;
        return Vector3f::cross(pu + n * grad[0] / (2 * M_PI),
                               pv + n * grad[1] / M_PI)
            .normalized();
    }

protected:
    Vector3f center;
    float radius;
};


#endif
