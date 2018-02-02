#ifndef NESRESTGOTILEFINDER_H
#define NESRESTGOTILEFINDER_H
#include "GameObject.h"
#include "NearestFinder.h"

using namespace RattletrapEngine;

class NearestGOTileFinder : public NearestFinder<GameObject>, public Finder<GameObject>
{
	public:
		NearestGOTileFinder();
		float operator()(GameObject);
};

#endif // NESRESTGOTILEFINDER_H
