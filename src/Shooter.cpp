#include "Shooter.h"
#include "Game.h"
#include "Bullet.h"
#include "Error.h"

Shooter::Shooter(GameObject &associated,
                 NearestFinder<GameObject*> &nearestFinder,
                 Finder<GameObject*> &finder,
                 int targetType,
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
    nearestFinder(nearestFinder),
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
    return GameComponentType::SHOOTER == type;
}

void Shooter::Update(float dt){
	if(active){
		timerBetweetShoots.Update(dt);
		if(timerBetweetShoots.Get() > betweetShootsTime){
			timerBetweetShoots.Restart();
            if(nullptr == target || TargetPolicy::ALWAYS_NEAREST == policy){
                ((NearestComponentFinder&)finder).setOrigin(associated.box.Center());
                target = ((GameObject*)(nearestFinder.FindNearest(associated.box.Center(),finder, range)));
			}
			else if(target->IsDead()){
                ((NearestComponentFinder&)finder).setOrigin(associated.box.Center());
                target = ((GameObject*)(nearestFinder.FindNearest(associated.box.Center(),finder, range)));
			}
			//supoe-se aqui que já existe um algo e a políica de tipo é SHOOT_UNTIL_OUT_OF_RANGE
			else if( (target->box.Center()-associated.box.Center() ).Magnitude() > range){
                ((NearestComponentFinder&)finder).setOrigin(associated.box.Center());
                target = ((GameObject*)(nearestFinder.FindNearest(associated.box.Center(),finder, range)));
            }
			if(nullptr!= target){
				Vec2 origin= associated.box.Center();
				Vec2 startDistanceFromOrigin(associated.box.w/2, 0);
				float angle= (target->box.Center()-origin).Inclination();
				startDistanceFromOrigin= startDistanceFromOrigin.Rotate(angle);
				origin = origin + startDistanceFromOrigin;
                if(GameComponentType::TOWER == targetType){
                    associated.AddComponent(new Bomb(associated,origin.x, origin.y, angle, bulletSpeed, bulletMaxDistance, bulletSprite, targetType,bulletScale,0.2,bulletFrameRate));
				}
				else{
                    associated.AddComponent(new Bullet(associated,origin.x, origin.y, angle, bulletSpeed, bulletMaxDistance, bulletSprite, targetType,bulletScale,0.2,bulletFrameRate));
				}
			}
		}
	}
}

void Shooter::SetActive(bool active){
	this->active= active;
}

