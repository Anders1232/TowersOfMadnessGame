#ifndef TILEWEIGHTMAPASTAR_H
#define TILEWEIGHTMAPASTAR_H
#include "TileMap.h"
#include "Tile.h"

using namespace RattletrapEngine;

class TileWeightMapAStar : public AStarWeight<Tile>
{
public:
    TileWeightMapAStar(std::map<int, double> weightMap);
    float CalculateCost(Tile& tile);
    virtual bool IsTraversable(Tile& tile);
    virtual ~TileWeightMapAStar(void);
};

#endif // TILEWEIGHTMAPASTAR_H

