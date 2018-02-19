#ifndef BOMB
#define BOMB
#include "GameObject.h"
#include "Sprite.h"
#include "Game.h"
#include "Animation.h"
#include "Vec2.h"
#include "RectTransform.h"
#include "GameComponentType.h"
#include <string>

using namespace RattletrapEngine;

class Bomb: public Component
{
    public:
        Bomb(float x,float y,float angle,float speed,float maxDistance,std::string sprite,int targetType,float scale = 1.0,float frameTime = 1,int frameCount = 1);
        ~Bomb(void);
        void Update(float dt);
        void NotifyCollision(Component &other);
        bool Is(int type);
        int getTargetType();
    private:
        Sprite sp;
        Vec2 speed;
        float distanceLeft;
        int targetType;
        GameObject* animation;
};

#endif // BOMB

