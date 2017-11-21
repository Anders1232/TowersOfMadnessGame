#ifndef TILE_H
#define TILE_H
#include "GameObject.h"

class Tile : public BaseTile
{
    public:

    private:
        GameObject* obstacle;

        void setGO(GameObject obstacle);
}

#endif // TILE_H

