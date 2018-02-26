#include "Bomb.h"
#include "Camera.h"
#include "Error.h"

Bomb::Bomb(GameObject& associated,float x, float y, float angle, float speed, float maxDistance, std::string sprite, std::string targetType, float scale, float frameTime, int frameCount)
    :Component(associated),
     targetType(targetType),
     speed(Vec2::FromPolarCoord(speed, angle)),
     distanceLeft(maxDistance),
     animation(new GameObject(){

    sp = new Sprite(sprite,associated,false,frameTime, frameCount);
    sp->SetScale(scale);
    associated.box.x= x;
    associated.box.y= y;
    associated.box.w= sp.GetWidth();
    associated.box.h= sp.GetHeight();
    associated.rotation= angle;
    associated.AddComponent(sp);
}

void Bomb::Update(float dt){

    if(0 >= distanceLeft){associated.RequestDelete();}

    associated.box = associated.box + speed*dt;
    distanceLeft-= speed.Magnitude()*dt;

}

Bomb::~Bomb(){
}

void Bomb::NotifyCollision(Component &other){
    if(other.Is(targetType)){
        distanceLeft= 0;
        if(other.Is(GameComponentType::TOWER)){
            animation->AddComponent(new Animation(box.x,box.y,rotation,"img/SpriteSheets/explosao_spritesheet.png",9,0.1,true));
            Game::GetInstance().GetCurrentState().AddObject(animation);
        }
        else if(other.Is(GameComponentType::BULLET)){
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

bool Bomb::Is(int type) const{

    return (GameComponentType::BOMB == type);

}

int Bomb::getTargetType(){
    return targetType;
}
