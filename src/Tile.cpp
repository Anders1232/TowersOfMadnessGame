#include "Tile.h"

void Tile::SetGO(GameObject* obstacle){
    this->obstacle = obstacle;
}

Tile::Tile(int n): BaseTile(n){

}

GameObject* Tile::GetGO(){
    return obstacle;
}

