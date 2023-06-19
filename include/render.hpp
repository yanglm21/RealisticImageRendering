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
};

#endif