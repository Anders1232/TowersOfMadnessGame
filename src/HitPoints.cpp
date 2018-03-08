#include "HitPoints.h"

HitPoints::HitPoints(uint hp,GameObject &associated, float scaleX)
        : Component(associated)
        ,  healthBar(new Sprite("img/effect/health_bar.png",associated))
        , healthColor(new Sprite("img/effect/health_color.png",associated)){
			
	this->hp = (int)hp;
	maxHp = (int)hp;
    healthBar->SetScaleX(scaleX);
    healthColor->SetScaleX(scaleX);
    associated.AddComponent(healthBar);
    associated.AddComponent(healthColor);
}

HitPoints::~HitPoints(){
}


void HitPoints::Update(float dt){
    Color& c = healthColor->colorMultiplier;
	c.r = 255*(1-(float)hp/maxHp);
	c.b = 0;
	c.g = 255*((float)hp/maxHp);
}

bool HitPoints::Is(int type) const{
    return (GameComponentType::HIT_POINTS == type);
}

int HitPoints::GetHp(){
	return hp;
}

void HitPoints::Damage(int damage){
	hp -= damage;
}

void HitPoints::RequestDelete(void){
	hp = 0;
}

