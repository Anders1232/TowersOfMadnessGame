#include "TileWeightMapAStar.h"

using namespace RattletrapEngine;

TileWeightMapAStar::TileWeightMapAStar(std::map<int, double> weightMap) : weightMap(weightMap)
{
}

float CalculateCost(Tile& tile){
    return weightMap[tile.GetTileSetIndex()]
}
virtual bool IsTraversable(Tile& tile){
    return(tile.GetGO() == nullptr && tile.GetTileSetIndex() != && tile.GetTileSetIndex() != );
}
virtual ~TileWeightMapAStar(void){

}

