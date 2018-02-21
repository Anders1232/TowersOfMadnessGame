#include "GameObject.h"
#include "Sprite.h"
#include "Rect.h"
#include "TileMap.h"
#include "Vec2.h"
#include "HitPoints.h"
#include "Bullet.h"
#include "NearestComponentFinder.h"

#ifndef TOWER_H
#define TOWER_H

#define TOWER_BASE_HP (100)
#define TOTAL_TOWER_TYPES 5
#define TOWER_BULLET_DAMAGE 10

using namespace RattletrapEngine;


class Tower : public Component{
	public:
		/**
			\brief Tipos de Torres

			Enumeração com os tipos de torres possíveis.
		*/
		typedef enum TowerType : int{
			SMOKE=0,
			ANTIBOMB,
			STUN,
			SHOCK,
			COMPUTATION
		} TowerType;
		
        Tower(TowerType type, Vec2 pos, Vec2 tileSize,int hp,GameObject& associated);
		void Damage(int damage);
		void Update(float dt );
		void Render(void);
        void NotifyCollision(Component &other);
		Rect GetWorldRenderedRect(void) const;
        bool Is(int componentType) const;
		~Tower();
	private:
        Sprite *sp;
		HitPoints *hitpoints;
		bool isDraging;
        NearestComponentFinder finder;
};

#endif // TOWER_H
