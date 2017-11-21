#ifndef TILE_H
#define TILE_H
#include "GameObject.h"
#include "BaseTile.h"

class Tile : public BaseTile{

    private:
        GameObject* obstacle;

        void setGO(GameObject obstacle);
};

#endif // TILE_H

