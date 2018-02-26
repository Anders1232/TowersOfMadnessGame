#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"
#include "Sprite.h"
#include "Game.h"
#include "Animation.h"
#include "Vec2.h"
#include "RectTransform.h"
#include "GameComponentType.h"
#include <string>

using namespace RattletrapEngine;

#define BULLET_VEL 210
#define BULLET_REACH 1000

using namespace RattletrapEngine;

class Bullet: public Component
{
    public:
        Bullet(GameObject& associated,float x,float y,float angle,float speed,float maxDistance,std::string sprite,int targetType,float scale = 1.0,float frameTime = 1,int frameCount = 1);
        ~Bullet(void);
        void Update(float dt);
        void NotifyCollision(Component &other);
        bool Is(int type) const;
        int getTargetType();
    private:
        Sprite sp;
        Vec2 speed;
        float distanceLeft;
        int targetType;
        GameObject* animation;
};

#endif
