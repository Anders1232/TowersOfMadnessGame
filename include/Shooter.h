#ifndef SHOOTER_H
#define SHOOTER_H

#include <string>
#include "Component.h"
#include "GameObject.h"
#include "NearestFinder.h"
#include "Timer.h"
#include "Tile.h"

using namespace RattletrapEngine;

class Shooter : public Component{
	public:
		enum TargetPolicy{
			ALWAYS_NEAREST,
			SHOOT_UNTIL_OUT_OF_RANGE
		};
		Shooter(GameObject &associated,
                NearestFinder<GameObject> &nearestFinder,
                Finder<GameObject> &finder,
				std::string targetType,
				float range,
				float betweetShootsTime,
				Shooter::TargetPolicy policy,
				bool active,
				float bulletSpeed,
				float bulletMaxDistance,
				std::string bulletSprite,
				int frameRate,
				float bulletScale);
		void Update(float dt);
        bool Is(int type) const;
		void SetActive(bool active);
	private:
		GameObject &associated;
        NearestFinder<GameObject> &nearestFinder;
        Finder<GameObject> &finder;
		bool active;
		std::string targetType;
		float range;
		float betweetShootsTime;
		Timer timerBetweetShoots;
		GameObject *target;
		Shooter::TargetPolicy policy;
		int bulletFrameRate;
		float bulletScale;
		
		//agora os argumentos que precisam ser enviados ao construtor de bullet
		float bulletSpeed;
		float bulletMaxDistance;
		std::string bulletSprite;
};

#endif // SHOOTER_H
