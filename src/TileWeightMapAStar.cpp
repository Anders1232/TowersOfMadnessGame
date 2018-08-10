#include "TileWeightMapAStar.h"

using namespace RattletrapEngine;

TileWeightMapAStar::TileWeightMapAStar(std::map<int, double> weightMap) : weightMap(weightMap)
{
}

float TileWeightMapAStar::CalculateCost(Tile& tile){
    return weightMap[tile.GetTileSetIndex()];
}
bool TileWeightMapAStar::IsTraversable(Tile& tile){
    return(tile.GetGO() == nullptr &&
			tile.GetTileSetIndex() != END_POINT &&
			tile.GetTileSetIndex() != SPAWN_POINT &&
			tile.GetTileSetIndex() != TILE_VAZIO);
}

TileWeightMapAStar::~TileWeightMapAStar(void){

}

