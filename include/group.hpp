#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "object_kdtree.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {

    }

    explicit Group (int num_objects): objList(num_objects){

    }

    ~Group() override {

    }

    bool intersect(const Ray &r, Hit &h) { return kdTree->intersect(r, h); }

    void addObject(int index, Object3D *obj) {
        objList.insert(objList.begin()+index, obj);
    }

    int getGroupSize() {
        return objList.size();
    }

    vector<Object3D *> getIlluminant() const {
        vector<Object3D *> illuminant;
        for (int i = 0; i < objList.size(); ++i)
            if (objList[i]->material->emission != Vector3f::ZERO)
                illuminant.push_back(objList[i]);
        return illuminant;
    }

private:
    ObjectKDTree *kdTree;
    std::vector<Object3D *> objList;
};

#endif
	
