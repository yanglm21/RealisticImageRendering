#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane() : normal(Vector3f::UP), d(0){

    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m), normal(normal), d(d) {
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f Rd = r.getDirection().normalized();

        // 平行
        float cos = Vector3f::dot(normal, Rd);
        if (fabs(cos) < 1e-6) return false;

        float t = (d - Vector3f::dot(normal, r.getOrigin())) / Vector3f::dot(normal, Rd);
        if (t > tmin && t < h.getT()){
            h.set(t, material, normal);
        }else {
            return false;
        }
    }

protected:
    Vector3f normal;
    float d;
};

#endif //PLANE_H
		

