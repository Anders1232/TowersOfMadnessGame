#ifndef TILEWEIGHTMAPASTAR_H
#define TILEWEIGHTMAPASTAR_H
#include "TileMap.h"
#include "Tile.h"
#include <map>

#define TILE_VAZIO (-1)
#define SPAWN_POINT (75)
#define COLLISION_LAYER (1)
#define END_POINT (74)

using namespace RattletrapEngine;

class TileWeightMapAStar : public AStarWeight<Tile>
{
public:
    TileWeightMapAStar(std::map<int, double> weightMap);
    float CalculateCost(Tile& tile);
    virtual bool IsTraversable(Tile& tile);
    virtual ~TileWeightMapAStar(void);
private:
    std::map<int, double> weightMap;
};

#endif // TILEWEIGHTMAPASTAR_H

