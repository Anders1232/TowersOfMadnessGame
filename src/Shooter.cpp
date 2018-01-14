#include "Shooter.h"
#include "Game.h"
#include "Bullet.h"
#include "Error.h"

Shooter::Shooter(GameObject &associated,
        NearestFinder<GameObject> &nearestFinder,
        Finder<GameObject> &finder,
		std::string targetType,
		float range,
		float betweetShootsTime,
		TargetPolicy policy,
		bool active,
		float bulletSpeed,
		float bulletMaxDistance,
		std::string bulletSprite,
		int frameRate,
		float bulletScale):
	associated(associated),
    nearestFinder(nearestFinder)
	finder(finder),
	active(active),
	targetType(targetType),
	range(range),
	betweetShootsTime(betweetShootsTime),
	target(nullptr),
	policy(policy),
	bulletFrameRate(frameRate),
	bulletScale(bulletScale),
	bulletSpeed(bulletSpeed),
	bulletMaxDistance(bulletMaxDistance),
    bulletSprite(bulletSprite),
    Component(associated){
}

bool Shooter::Is(int type) const{
	return ComponentType::SHOOTER == type;
}

void Shooter::Update(float dt){
	if(active){
		timerBetweetShoots.Update(dt);
		if(timerBetweetShoots.Get() > betweetShootsTime){
			timerBetweetShoots.Restart();
			if(nullptr == target || TargetPolicy::ALWAYS_NEAREST == policy){
                target= nearestFinder.FindNearestGO(associated.box.Center(),finder, targetType, range);
			}
			else if(target->IsDead()){
                target= nearestFinder.FindNearestGO(associated.box.Center(),finder, targetType, range);
			}
			//supoe-se aqui que já existe um algo e a políica de tipo é SHOOT_UNTIL_OUT_OF_RANGE
			else if( (target->box.Center()-associated.box.Center() ).Magnitude() > range){
                target= nearestFinder.FindNearestGO(associated.box.Center(),finder, targetType, range);
			}
			if(nullptr!= target){
				Vec2 origin= associated.box.Center();
				Vec2 startDistanceFromOrigin(associated.box.w/2, 0);
				float angle= (target->box.Center()-origin).Inclination();
				startDistanceFromOrigin= startDistanceFromOrigin.Rotate(angle);
				origin = origin + startDistanceFromOrigin;
				if("Tower" == targetType){
					Game::GetInstance().GetCurrentState().AddObject(new Bullet(origin.x, origin.y, angle, bulletSpeed, bulletMaxDistance, bulletSprite, targetType,bulletScale,0.2,bulletFrameRate, true));
				}
				else{
					Game::GetInstance().GetCurrentState().AddObject(new Bullet(origin.x, origin.y, angle, bulletSpeed, bulletMaxDistance, bulletSprite, targetType,bulletScale,0.2,bulletFrameRate));
				}
			}
		}
	}
}

void Shooter::SetActive(bool active){
	this->active= active;
}

