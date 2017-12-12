#ifndef TILE_H
#define TILE_H
#include "GameObject.h"
#include "BaseTile.h"

class Tile{

    public:
        void SetGO(GameObject* obstacle);
        GameObject* GetGO();
        int GetTileType();

    private:
        GameObject* obstacle;
        int tileType;

};

#endif // TILE_H

