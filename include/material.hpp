#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "texture.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0, Vector3f t = Vector3f(1, 0, 0), 
                      const Vector3f &e_color = Vector3f::ZERO, float r = 1,
                      const char *textureFile = "", const char *bumpFile = "") :
            diffuseColor(d_color), specularColor(s_color), shininess(s), emission(e_color),
          refr(r),
          type(t),
          texture(textureFile),
          bump(bumpFile) {
        
    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        // 
        Vector3f N = hit.getNormal(), V = -ray.getDirection().normalized();
        Vector3f Lx = dirToLight.normalized();
        Vector3f Rx = (2 * (Vector3f::dot(Lx, N)) * N - Lx).normalized();
        Vector3f shaded =
            lightColor *
            (diffuseColor * relu(Vector3f::dot(Lx, N)) +
             specularColor * (pow(relu(Vector3f::dot(V, Rx)), shininess)));
        return shaded;
    }

    Vector3f diffuseColor;
    Vector3f specularColor; // 镜面反射系数
    Vector3f emission;       // 发光系数
    float shininess;    // 高光指数
    float refr;              // 折射率
    Vector3f type;           // 种类
    Texture texture, bump;   // 纹理
    float relu(float x) { return std::max((float)0, x); }
};


#endif // MATERIAL_H
