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
            Vector3f normal = (t * Rd + origin - center).normalized();
            h.set(t, material, normal);
            return true;
        } else {
            return false;
        }
    }

protected:
    Vector3f center;
    float radius;
};


#endif
