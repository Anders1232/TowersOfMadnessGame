#ifndef TILE_H
#define TILE_H
#include "GameObject.h"
#include "BaseTile.h"

class Tile : public BaseTile{

    public:
        void SetGO(GameObject* obstacle);
        GameObject* GetGO();
        Tile(int n);
        Tile():BaseTile(-1){}

    private:
        GameObject* obstacle;
        int tileType;

};

#endif // TILE_H

