#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>
#include <vecmath.h>
#include <iostream>

#include "stb_image.hpp"

using std::string;

struct Texture {  // 纹理
    unsigned char *pic;
    int w, h, c;

    Texture(const char *textureFile) {
        if (strlen(textureFile) > 0) {
            pic = stbi_load(textureFile, &w, &h, &c, 0);
            printf("Texture file: %s loaded. Size: %dx%dx%d\n", textureFile, w, h,
                c);
        } else {
            pic = nullptr;
        }
    }

    Vector3f getColor(float u, float v) const {
        if (!pic) {
            return Vector3f::ZERO;
        }
        u -= int(u);
        v -= int(v);
        u = u < 0 ? 1 + u : u;
        v = v < 0 ? 1 + v : v;
        u = u * w;
        v = h * (1 - v);
        int iu = (int)u, iv = (int)v;
        float alpha = u - iu, beta = v - iv;
        Vector3f ret(0);
        ret += (1 - alpha) * (1 - beta) * getColor(iu, iv);
        ret += alpha * (1 - beta) * getColor(iu + 1, iv);
        ret += (1 - alpha) * beta * getColor(iu, iv + 1);
        ret += alpha * beta * getColor(iu + 1, iv + 1);
        return ret;
    }

    Vector3f getColor(int idx) const {
        return Vector3f(pic[idx + 0], pic[idx + 1], pic[idx + 2]) / 255.;
    }

    Vector3f getColor(int u, int v) const;
    float getDisturb(float u, float v, Vector2f &grad) const {
        if (!pic) return 0;
        float disturb = getGray(getIdx(u, v));
        float du = 1.0 / w, dv = 1.0 / h;
        grad[0] = w * (getGray(getIdx(u + du, v)) - getGray(getIdx(u - du, v))) / 2.0;
        grad[1] = h * (getGray(getIdx(u, v + dv)) - getGray(getIdx(u, v - dv))) / 2.0;
        return disturb;
    }

    inline int getIdx(float u, float v) const {
        int pw = int(u * w + w) % w, ph = int(v * h + h) % h;
        return ph * w * c + pw * c;
    }

    inline float getGray(int idx) const {
        return (pic[idx] / 255. - 0.5) * 2;
    }
};

#endif  // !TEXTURE_H