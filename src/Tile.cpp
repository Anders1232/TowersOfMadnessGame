#include "Tile.h"

Tile::Tile(){}
Tile::Tile(int tileType){
    this->tileType = tileType;
}
void Tile::SetGO(GameObject* obstacle){
    this->obstacle = obstacle;
}
GameObject* Tile::GetGO(){
    return obstacle;
}
int Tile::GetTileType(){
    return tileType;
}
