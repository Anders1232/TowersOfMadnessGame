#ifndef TILE_H
#define TILE_H
#include "GameObject.h"
#include "BaseTile.h"

class Tile : public BaseTile{

    public:
        void setGO(GameObject obstacle);

    private:
        GameObject* obstacle;

};

#endif // TILE_H

