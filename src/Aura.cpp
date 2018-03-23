#include "Aura.h"

Aura::Aura(GameObject &associated,
			Enemy::Event auraType,
			float auraRange,
			float timeBetweetNotifications,
		   NearestFinder<GameObject>* nearestFinder,
		   Finder<GameObject>* finder)
	:Component(associated),
		associated(associated),
		auraType(auraType),
		auraRange(auraRange),
		sp (nullptr),
		sp2 (nullptr),
		timeBetweetNotifications(timeBetweetNotifications),
		nearestFinder(nearestFinder),
		finder(finder){
	if(Enemy::Event::SMOKE == auraType){
		sp = new Sprite(std::string("img/SpriteSheets/aura_spritesheet.png"), associated, false, 0.3f, 7);
		sp->colorMultiplier = Color(179, 150, 120);
		sp2 = new Sprite(std::string("img/SpriteSheets/aura_spritesheet.png"), associated, false, 0.3f, 7);
		sp2->colorMultiplier = Color(179, 150, 120);
		sp2->SetFrame(3);
	}
	else if(Enemy::Event::STUN == auraType){
		sp = new Sprite(std::string("img/SpriteSheets/stun_spritesheet.png"), associated, false, 0.3f, 7);
		sp2 = new Sprite(std::string("img/SpriteSheets/stun_spritesheet.png"), associated, false, 0.3f, 7);
		sp2->SetFrame(3);
	}
	else if(Enemy::Event::HEALER == auraType){
		sp = new Sprite(std::string("img/SpriteSheets/aura_spritesheet.png"), associated, false, 0.3f, 7);
	}
	sp->ScaleX(2*auraRange/(float)sp->GetWidth());
	sp->ScaleY(2*auraRange/(float)sp->GetHeight());
	sp->colorMultiplier.a= 110;
	if(Enemy::Event::HEALER != auraType){
		sp2->ScaleX(2*auraRange/(float)sp2->GetWidth());
		sp2->ScaleY(2*auraRange/(float)sp2->GetHeight());
		sp2->colorMultiplier.a=110;
	}

	associated.AddComponent(sp);
	associated.AddComponent(sp2);
}

void Aura::Update(float dt){
	notificationTimer.Update(dt);
	if(notificationTimer.Get() > timeBetweetNotifications){
		notificationTimer.Restart();
		vector<GameObject *> *enemiesInRange = ((vector<GameObject *>*)(nearestFinder->FindNearests(associated.box.Center(), finder, auraRange)));
		for(uint i=0; i< enemiesInRange->size(); i++){
			( (Enemy*)((*enemiesInRange)[i]) )->NotifyEvent(auraType);
		}
		delete enemiesInRange;
	}
}

void Aura::Render(void){
	Vec2 startPoint = associated.box;
	startPoint = startPoint - Vec2((-associated.box.w + sp->GetWidth())/2, (-associated.box.w + sp->GetHeight())/2);
	sp->Render();
	if(Enemy::Event::HEALER != auraType){
		startPoint= associated.box;
		startPoint= startPoint -  Vec2((-associated.box.w + sp->GetWidth())/2, (-associated.box.w + sp->GetHeight())/2);
		sp2->Render();
	}
}

bool Aura::Is(int type) const{
	if(Enemy::Event::SMOKE == auraType){
		return GameComponentType::SLOW_AURA== type;
	}
	else if(Enemy::Event::STUN == auraType){
		return GameComponentType::STUN_AURA == type;
	}
	else if(Enemy::Event::HEALER == auraType){
		return GameComponentType::HEAL_AURA == type;
	}
	else{
		Error("\t Should not get here!");
	}
}

