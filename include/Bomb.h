#ifndef BOMB_H
#define BOMB_H

#include <string>

#include "GameObject.h"
#include "Sprite.h"
#include "Game.h"
#include "Animation.h"
#include "Vec2.h"
#include "RectTransform.h"
#include "GameComponentType.h"

using namespace RattletrapEngine;

class Bomb: public Component
{
    public:
        Bomb(GameObject& associated,float x,float y,float angle,float speed,float maxDistance,std::string sprite,int targetType,float scale = 1.0,float frameTime = 1,int frameCount = 1);
        ~Bomb(void);
        void Update(float dt);
        void NotifyCollision(Component &other);
        bool Is(int type) const;
        int getTargetType();
    private:
        Sprite *sp;
        Vec2 speed;
        int targetType;
        float distanceLeft;
        GameObject* animation;
};

#endif // BOMB_H

