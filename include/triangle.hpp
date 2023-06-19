#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
#include <cfloat>
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
		an = Vector3f::ZERO;
        bn = Vector3f::ZERO;
        cn = Vector3f::ZERO;
		cen = (a + b + c) / 3;
		nSet = false;
        tSet = false;
		d = Vector3f::dot(normal, a);
        bound[0] = minE(minE(a, b), c);
        bound[1] = maxE(maxE(a, b), c);
    }

	bool intersect(const Ray& ray, Hit& hit) override {
		Vector3f o(ray.getOrigin()), dir(ray.getDirection());
        Vector3f v0v1 = vertices[1] - vertices[0];
        Vector3f v0v2 = vertices[2] - vertices[0];
        Vector3f pvec = Vector3f::cross(dir, v0v2);
        float det = Vector3f::dot(v0v1, pvec);
        if (fabs(det) < FLT_EPSILON) return false;
        float invDet = 1 / det;
        Vector3f tvec = o - vertices[0];
        float u = Vector3f::dot(tvec, pvec) * invDet;
        if (u < 0 || u > 1) return false;
        Vector3f qvec = Vector3f::cross(tvec, v0v1);
        float v = Vector3f::dot(dir, qvec) * invDet;
        if (v < 0 || u + v > 1) return false;
        float t = Vector3f::dot(v0v2, qvec) * invDet;
        if (t <= 0 || t > hit.getT()) return false;
        Vector3f p(o + dir * t);
        getUV(p, u, v);
        hit.set(t, material, getNorm(p), material->getColor(u, v), p);
        return true;
	}

	void getUV(const Vector3f& p, float& u, float& v) {
        if (!tSet) return;
        Vector3f va = (vertices[0] - p), vb = (vertices[1] - p), vc = (vertices[2] - p);
        float ra = Vector3f::cross(vb, vc).length(),
              rb = Vector3f::cross(vc, va).length(),
              rc = Vector3f::cross(va, vb).length();
        Vector2f uv = (ra * at + rb * bt + rc * ct) / (ra + rb + rc);
        u = uv.x();
        v = uv.y();
    }

	void setVNorm(const Vector3f& anorm, const Vector3f& bnorm,
                  const Vector3f& cnorm) {
        an = anorm;
        bn = bnorm;
        cn = cnorm;
        nSet = true;
    }

	Vector3f getNorm(const Vector3f& p) {
        if (!nSet) return normal;
        Vector3f va = (vertices[0] - p), vb = (vertices[1] - p), vc = (vertices[2] - p);
        float ra = Vector3f::cross(vb, vc).length(),
              rb = Vector3f::cross(vc, va).length(),
              rc = Vector3f::cross(va, vb).length();
        return (ra * an + rb * bn + rc * cn).normalized();
    }

	void setVT(const Vector2f& _at, const Vector2f& _bt, const Vector2f& _ct) {
        at = _at;
        bt = _bt;
        ct = _ct;
        tSet = true;
    }

	bool nSet = false;
    bool tSet = false;
	Vector3f cen;
	Vector3f normal;
	Vector3f vertices[3];
	Vector2f at, bt, ct;
    Vector3f an, bn, cn;
    Vector3f bound[2];
	float d;
	
protected:
	bool inTriangle(const Vector3f& p) {
        return Vector3f::dot(Vector3f::cross((vertices[1] - p), (vertices[2] - p)), normal) >= -1e-6 &&
               Vector3f::dot(Vector3f::cross((vertices[2] - p), (vertices[0] - p)), normal) >= -1e-6 &&
               Vector3f::dot(Vector3f::cross((vertices[0] - p), (vertices[1] - p)), normal) >= -1e-6;
    }
};

#endif //TRIANGLE_H
