#include "Obstacle.h"
#include "Error.h"

Obstacle::Obstacle(GameObject& associated ,std::string path, Vec2 position) : Component(associated), sp(new Sprite(path,associated)){
    associated.box = position;

    associated.box.w = sp->GetWidth();
    associated.box.h = sp->GetHeight();
}

Obstacle::~Obstacle(){}

void Obstacle::Update(float dt ) {}

void Obstacle::NotifyCollision(Component &other){}

bool Obstacle::Is(int componentType) const{
    return GameComponentType::OBSTACLE == componentType;
}


