#ifndef NESRESTGOTILEFINDER_H
#define NESRESTGOTILEFINDER_H
#include "GameObject.h"
#include "NearestFinder.h"

using namespace RattletrapEngine;

class NearestGOFinder : public Finder<GameObject*>
{
	public:
        NearestGOFinder(std::string targetType,Vec2 origin);
        void setOrigin(Vec2 origin);
        float operator()(GameObject*);
    private:
        std::string targetType;
        Vec2 origin;
};

#endif // NESRESTGOTILEFINDER_H
