#include "Bullet.h"
#include "Camera.h"
#include "Error.h"

#include "Bomb.h"
#include "Camera.h"
#include "Error.h"

Bullet::Bullet(float x, float y, float angle, float speed, float maxDistance, std::string sprite, std::string targetType, float scale, float frameTime, int frameCount)
    :targetType(targetType),
     speed(Vec2::FromPolarCoord(speed, angle)),
     distanceLeft(maxDistance),
     animation(new GameObject()){

    sp = new Sprite(sprite,associated,false,frameTime, frameCount);
    sp->SetScale(scale);
    associated.box.x= x;
    associated.box.y= y;
    associated.box.w= sp.GetWidth();
    associated.box.h= sp.GetHeight();
    associated.rotation= angle;
    associated.AddComponent(sp);
}

void Bullet::Update(float dt){

    if(0 >= distanceLeft){associated.RequestDelete();}

    associated.box = associated.box + speed*dt;
    distanceLeft-= speed.Magnitude()*dt;

}

Bullet::~Bullet(){
}

void Bullet::NotifyCollision(Component &other){
    if(other.Is(targetType)){
        distanceLeft= 0;
        if(other.Is(GameComponentType::TOWER)){
            animation->AddComponent(new Animation(box.x,box.y,rotation,"img/SpriteSheets/explosao_spritesheet.png",9,0.1,true));
            Game::GetInstance().GetCurrentState().AddObject(animation);
        }
        else if(other.Is(GameComponentType::BOMB)){
            animation->AddComponent(new Animation(box.x,box.y,rotation,"img/SpriteSheets/anti-bomba_ativ_spritesheet.png",11,0.1,true));
            Game::GetInstance().GetCurrentState().AddObject(animation);
        }
        else{
            animation->AddComponent(new Animation(box.x,box.y,rotation,"./img/SpriteSheets/explosao_spritesheet.png",9,0.1,true));
            Game::GetInstance().GetCurrentState().AddObject(animation);
        }
        associated.RequestDelete();
    }
}

bool Bullet::Is(int type){

    return (GameComponentType::BULLET == type);

}

int Bomb::getTargetType(){
    return targetType;
}
