#ifndef NEARESTCOMPONENTFINDER
#define NEARESTCOMPONENTFINDER
#include "GameObject.h"
#include "NearestFinder.h"

using namespace RattletrapEngine;

class NearestComponentFinder : public Finder<GameObject*>
{
    public:
        NearestComponentFinder(int componentType,Vec2 origin);
        NearestComponentFinder();
        void setOrigin(Vec2 origin);
        float operator()(GameObject*);
    private:
        int componentType;
        Vec2 origin;
};

#endif // NEARESTCOMPONENTFINDER

