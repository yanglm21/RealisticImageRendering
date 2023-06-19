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
    Plane(const Vector3f &normal, float d, Material *m)
        : Object3D(m), normal(normal.normalized()), d(d) {
        uaxis = Vector3f::cross(Vector3f::UP, normal);
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f Rd = r.getDirection().normalized();

        // 平行
        float cos = Vector3f::dot(normal, Rd);
        if (fabs(cos) < 1e-6) return false;

        float t = (d - Vector3f::dot(normal, r.getOrigin())) / Vector3f::dot(normal, Rd);
        if (t > tmin && t < h.getT()){
            Vector3f o(r.getOrigin()), dir(r.getDirection());
            Vector3f p(o + dir * t);
            float u = Vector3f::dot(p - d * normal, uaxis);
            float v = p.y();
            h.set(t, material, getNormal(u, v), material->getColor(u, v), p);
        }else {
            return false;
        }
    }

    Vector3f getNormal(float u, float v) {
        Vector2f grad(0);
        float f = material->bump.getDisturb(u, v, grad);
        if (fabs(f) < FLT_EPSILON) return normal;
        if (uaxis.squaredLength() < FLT_EPSILON) return normal;
        return Vector3f::cross(uaxis + normal * grad[0],
                               Vector3f::UP + normal * grad[1])
            .normalized();
    }

protected:
    Vector3f normal, uaxis;
    float d;
};

#endif //PLANE_H
		

