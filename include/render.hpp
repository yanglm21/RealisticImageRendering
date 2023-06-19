#ifndef RENDER_H
#define RENDER_H

#include <cmath>
#include <cstring>
#include <iostream>
#include <string>

#include "camera.hpp"
#include "group.hpp"
#include "hit.hpp"
#include "image.hpp"
#include "light.hpp"
#include "ray.hpp"
#include "scene_parser.hpp"
#include "utils.hpp"
#include "hit_kdtree.hpp"

using namespace std;

class SPPM {
public:
    const SceneParser scene;
    int numRounds, numPhotons, ckpt_interval;
    std::string outdir;
    int w, h;
    Camera* camera;
    vector<Hit*> hitPoints;
    HitKDTree* hitKDTree;
    vector<Object3D*> illuminants;
    Group* group;

    SPPM(const SceneParser& scene, int numRounds, int numPhotons, int ckpt,
         const char* dir) : scene(scene), numRounds(numRounds), numPhotons(numPhotons),
                            ckpt_interval(ckpt), outdir(dir) {
        camera = scene.getCamera();
        group = scene.getGroup();
        illuminants = group->getIlluminant();
        w = camera->getWidth();
        h = camera->getHeight();
        hitKDTree = nullptr;
        for (int u = 0; u < w; ++u)
            for (int v = 0; v < h; ++v) hitPoints.push_back(new Hit());
        cout << "Width: " << w << " Height: " << h << endl;
    }

    ~SPPM() {
        for (int u = 0; u < w; ++u)
            for (int v = 0; v < h; ++v) delete hitPoints[u * w + v];
        delete hitKDTree;
    }

    void forward(Ray ray, Hit* hit) {
        int depth = 0;
        Vector3f attenuation(1, 1, 1);
        while (true) {
            if (++depth > TRACE_DEPTH || attenuation.max() < 1e-3) return;
            hit->t = INF;
            if (!group->intersect(ray, *hit)) {
                hit->fluxLight += hit->attenuation*scene.getBackgroundColor();
                return;
            }
            ray.origin += ray.direction * (*hit).t;
            Material* material = (*hit).material;
            Vector3f N(hit->normal);
            float type = RND2;
            if (type <= material->type.x()) {  // Diffuse
                hit->attenuation = attenuation * hit->color;
                hit->fluxLight += hit->attenuation * material->emission;
                return;
            } else if (type <= material->type.x() + material->type.y()) {
                float cost = Vector3f::dot(ray.direction, N);
                ray.direction = (ray.direction - N * (cost * 2)).normalized();
            } else {
                float n = material->refr;
                float R0 = ((1.0 - n) * (1.0 - n)) / ((1.0 + n) * (1.0 + n));
                if (Vector3f::dot(N, ray.direction) > 0) {  // inside the medium
                    N.negate();
                    n = 1 / n;
                }
                n = 1 / n;
                float cost1 =
                    -Vector3f::dot(N, ray.direction);  // cosine theta_1
                float cost2 =
                    1.0 - n * n * (1.0 - cost1 * cost1);  // cosine theta_2
                float Rprob =
                    R0 + (1.0 - R0) * pow(1.0 - cost1,
                                          5.0);   // Schlick-approximation
                if (cost2 > 0 && RND2 > Rprob) {  // refraction direction
                    ray.direction =
                        ((ray.direction * n) + (N * (n * cost1 - sqrt(cost2))))
                            .normalized();
                } else {  // reflection direction
                    ray.direction =
                        (ray.direction + N * (cost1 * 2)).normalized();
                }
            }
            attenuation = attenuation * hit->color;
        }
    }

    void backward(Ray ray, const Vector3f& color, long long int seed=-1) {
        int depth = 0;
        Vector3f attenuation = color * Vector3f(250, 250, 250);
        while (true) {
            if (++depth > TRACE_DEPTH || attenuation.max() < 1e-3) return;
            Hit hit;
            if (!group->intersect(ray, hit)) return;
            ray.origin += ray.direction * hit.t;
            Material* material = hit.material;
            Vector3f N(hit.normal);
            float type = RND2;
            if (type <= material->type.x()) {  // Diffuse
                hitKDTree->update(hitKDTree->root, hit.p, attenuation,
                                  ray.direction);
                ray.direction = diffDir(N, -1, seed);
            } else if (type <= material->type.x() + material->type.y()) {
                float cost = Vector3f::dot(ray.direction, N);
                ray.direction = (ray.direction - N * (cost * 2)).normalized();
            } else {
                float n = material->refr;
                float R0 = ((1.0 - n) * (1.0 - n)) / ((1.0 + n) * (1.0 + n));
                if (Vector3f::dot(N, ray.direction) > 0) {  // inside the medium
                    N.negate();
                    n = 1 / n;
                }
                n = 1 / n;
                float cost1 =
                    -Vector3f::dot(N, ray.direction);  // cosine theta_1
                float cost2 =
                    1.0 - n * n * (1.0 - cost1 * cost1);  // cosine theta_2
                float Rprob =
                    R0 + (1.0 - R0) * pow(1.0 - cost1,
                                          5.0);   // Schlick-approximation
                if (cost2 > 0 && RND2 > Rprob) {  // refraction direction
                    ray.direction =
                        ((ray.direction * n) + (N * (n * cost1 - sqrt(cost2))))
                            .normalized();
                } else {  // reflection direction
                    ray.direction =
                        (ray.direction + N * (cost1 * 2)).normalized();
                }
            }
            attenuation = attenuation * hit.color;
        }
    }

    void render() {
        time_t start = time(NULL);
        Vector3f color = Vector3f::ZERO;
        for (int round = 0; round < numRounds; ++round) {
            float elapsed = (time(NULL) - start),
                  progress = (1. + round) / numRounds;
            fprintf(stderr,
                    "\rRendering (%d/%d Rounds) %5.2f%% Time: %.2f/%.2f sec\n",
                    round + 1, numRounds, progress * 100., elapsed,
                    elapsed / progress);
#pragma omp parallel for schedule(dynamic, 1)
            for (int x = 0; x < w; ++x) {
                for (int y = 0; y < h; ++y) {
                    Ray camRay =
                        camera->generateRay(Vector2f(x + RND, y + RND));
                    hitPoints[x * h + y]->reset(-camRay.direction);
                    forward(camRay, hitPoints[x * h + y]);
                }
            }
            setHitKDTree();
            int photonsPerLight = numPhotons / illuminants.size();
// photon tracing pass
#pragma omp parallel for schedule(dynamic, 1)
            for (int i = 0; i < photonsPerLight; ++i) {
                for (int j = 0;j < illuminants.size(); ++j) {
                    // cout << i << " "<< j << " In" <<endl;
                    Ray ray = illuminants[j]->randomRay(-1, (long long)round * numPhotons + (round + 1) * w * h + i);
                    // cout << i << " "<< j << " Out" <<endl;
                    backward(ray, illuminants[j]->material->emission, (long long)round * numPhotons + i);
                } 
            }
            if ((round + 1) % ckpt_interval == 0) {
                char filename[100];
                sprintf(filename, "ckpt-%d.bmp", round + 1);
                save(filename, round + 1, numPhotons);
            }
        }
        save("result.bmp", numRounds, numPhotons);
    }

    void save(std::string filename, int numRounds, int numPhotons) {
        Image outImg(w, h);
        for (int u = 0; u < w; ++u)
            for (int v = 0; v < h; ++v) {
                Hit* hit = hitPoints[u * h + v];
                outImg.SetPixel(
                    u, v,
                    hit->flux / (M_PI * hit->r2 * numPhotons * numRounds) +
                        hit->fluxLight / numRounds);
            }
        outImg.SaveBMP((outdir + "/" + filename).c_str());
    }

    void setHitKDTree() {
        if (hitKDTree) delete hitKDTree;
        hitKDTree = new HitKDTree(&hitPoints);
    }
};

#endif