#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
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

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool check = false;
        for (auto object: objList){
            if (object){
                check |= object->intersect(r, h, tmin);
            }
        }
        return check;
    }

    void addObject(int index, Object3D *obj) {
        objList.insert(objList.begin()+index, obj);
    }

    int getGroupSize() {
        return objList.size();
    }

private:
    std::vector<Object3D *> objList;
};

#endif
	
