#ifndef HITPOINTS_H
#define HITPOINTS_H

#include "Component.h"
#include "Sprite.h"
#include "Error.h"
#include "GameComponentType.h"
#include "GameObject.h"

using namespace RattletrapEngine;

class HitPoints : public Component{
	public:
		HitPoints(uint baseHp,GameObject &associated, float scaleX = 0);
		~HitPoints();
		void Update(float dt);
		void RequestDelete(void);
		void Damage(int damage);
        bool Is(int type) const;
		int GetHp();
	private:
		int hp;
		int maxHp;
        Sprite *healthBar;
        Sprite *healthColor;
};

#endif // HITPOINTS_H
