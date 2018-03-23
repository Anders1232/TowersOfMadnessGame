#include "Tower.h"

#include "Camera.h"
#include "DragAndDrop.h"
#include "Error.h"
#include "Shooter.h"
#include "Game.h"
#include "StageState.h"
#include "Aura.h"

#define SORTEAR_TORRES

Tower::Tower(TowerType type, Vec2 pos, Vec2 tileSize, int hp,GameObject& associated):Component(associated){
	sp = new Sprite(type == TowerType::SMOKE ? "img/tower/torre_fumaca.png" :
			type == TowerType::ANTIBOMB ? "img/SpriteSheets/anti-bomba.png" :
			type == TowerType::STUN ? "img/tower/torrestun.png" :
			type == TowerType::SHOCK ? "img/SpriteSheets/torrechoque_lvl1.png" :
			"",
			associated,
			true,
			0.25,
			type == TowerType::SMOKE ? 1:
			type == TowerType::ANTIBOMB ? 9:
			type == TowerType::STUN ? 1:
			type == TowerType::SHOCK ? 8: 1);
	sp->ScaleX(tileSize.x/sp->GetWidth());
	sp->ScaleY(tileSize.y/sp->GetHeight());
	associated.AddComponent(sp);
	associated.box.x = pos.x;
	associated.box.y = pos.y;
	associated.box.w = sp->GetWidth();
	associated.box.h = sp->GetHeight();
	NearestFinder<GameObject>* stageState = &dynamic_cast< NearestFinder<GameObject>& >(Game::GetInstance().GetCurrentState());

	switch(type){
		case TowerType::SMOKE:
			finder = new NearestComponentFinder(GameComponentType::ENEMY, associated.box.Center());
			associated.AddComponent(new Aura(associated, Enemy::Event::SMOKE, 400, 7.0, stageState, dynamic_cast< Finder<GameObject>* >(finder)));
			break;
		case TowerType::ANTIBOMB:
			finder = new NearestComponentFinder(GameComponentType::BULLET, associated.box.Center());
			associated.AddComponent(new Shooter(associated, stageState, dynamic_cast< Finder<GameObject>* >(finder), GameComponentType::BOMB, 5000, 2.0, Shooter::TargetPolicy::ALWAYS_NEAREST, true, 500, 5000, "img/SpriteSheets/anti-bomba_idle.png", 11, 1));
			break;
		case TowerType::STUN:
			finder = new NearestComponentFinder(GameComponentType::ENEMY, associated.box.Center());
			associated.AddComponent(new Aura(associated, Enemy::Event::STUN, 400, 7.0, stageState, dynamic_cast< Finder<GameObject>* >(finder)));
			break;
		case TowerType::SHOCK:
			finder = new NearestComponentFinder(GameComponentType::ENEMY, associated.box.Center());
			associated.AddComponent(new Shooter(associated, stageState, dynamic_cast< Finder<GameObject>* >(finder), GameComponentType::ENEMY, 5000, 2.0, Shooter::TargetPolicy::ALWAYS_NEAREST, true, 1500, 5000, "img/SpriteSheets/bullet_choquelvl1.png", 4, 1));
			break;
		case TowerType::COMPUTATION:
			break;
	}

	hitpoints = new HitPoints(hp,associated);
	associated.AddComponent(hitpoints);
}

Tower::~Tower() {
	delete finder;
}

void Tower::Damage(int damage) {
	hitpoints->Damage(damage);
}

void Tower::Update(float dt ) {

	if( 0 >= hitpoints->GetHp()){associated.RequestDelete();}

}
void Tower::Render(void) {
}

Rect Tower::GetWorldRenderedRect() const {
	return Camera::WorldToScreen(associated.box);
}

bool Tower::Is(int componentType) const{
	return GameComponentType::TOWER == componentType;
}

void Tower::NotifyCollision(Component &object){
	if(object.Is(GameComponentType::BOMB)){
		if(((Bomb&)object).getTargetType() == GameComponentType::TOWER){
			hitpoints->Damage(TOWER_BULLET_DAMAGE);
		}
	}
}
