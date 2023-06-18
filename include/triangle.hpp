#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
        normal = Vector3f::cross((vertices[1] - vertices[0]), (vertices[2] - vertices[0])).normalized();
    }

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		Vector3f Rd = ray.getDirection();

        // 平行
        float cos = Vector3f::dot(normal, Rd);
        if (fabs(cos) < 1e-6) return false;

		float t = (Vector3f::dot(normal, vertices[0]) - Vector3f::dot(normal, ray.getOrigin())) / Vector3f::dot(normal, Rd);
		if (!(t > tmin && t < hit.getT())){
			return false;
		}
		Vector3f p(ray.getOrigin() + Rd * t);
		if (!inTriangle(p)){
			return false;
		}
		hit.set(t, material, normal);
        return true;
	}
	Vector3f normal;
	Vector3f vertices[3];
	
protected:
	bool inTriangle(const Vector3f& p) {
        return Vector3f::dot(Vector3f::cross((vertices[1] - p), (vertices[2] - p)), normal) >= -1e-6 &&
               Vector3f::dot(Vector3f::cross((vertices[2] - p), (vertices[0] - p)), normal) >= -1e-6 &&
               Vector3f::dot(Vector3f::cross((vertices[0] - p), (vertices[1] - p)), normal) >= -1e-6;
    }
};

#endif //TRIANGLE_H
