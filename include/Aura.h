#ifndef AURA_H
#define AURA_H

#include "Enemy.h"
#include "Component.h"
#include "Timer.h"
#include "NearestComponentFinder.h"
#include "GameComponentType.h"

#include <string>

using namespace RattletrapEngine;

class Aura: public Component
{
	public:
        Aura(GameObject& associated, Enemy::Event auraType, float auraRange, float timeBetweetNotifications, NearestFinder<GameObject> &nearestFinder,
             Finder<GameObject> &finder);
		~Aura(void){};
		void Update(float dt);
		void Render(void);
		bool Is(int type) const;
	private:
		GameObject &associated;
		Enemy::Event auraType;
		float auraRange;
		Sprite* sp;
		Sprite* sp2;
		float timeBetweetNotifications;
		Timer notificationTimer;
        NearestFinder<GameObject> &nearestFinder;
        Finder<GameObject> &finder;
};

#endif // AURA_H
