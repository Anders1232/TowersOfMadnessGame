#ifndef NEARESTCOMPONENTFINDER_H
#define NEARESTCOMPONENTFINDER_H
#include "GameObject.h"
#include "NearestFinder.h"
#include <cmath>

using namespace RattletrapEngine;

class NearestComponentFinder : public Finder<GameObject>
{
    public:
        NearestComponentFinder(int componentType,Vec2 origin);
        NearestComponentFinder();
        void setOrigin(Vec2 origin);
        float operator()(GameObject* go);
    private:
        int componentType;
        Vec2 origin;
};

#endif // NEARESTCOMPONENTFINDER_H

