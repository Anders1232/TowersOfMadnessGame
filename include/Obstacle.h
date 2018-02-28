#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "GameObject.h"
#include "TileMap.h"
#include "Sprite.h"
#include "Rect.h"
#include "Camera.h"
#include "Vec2.h"
#include "GameComponentType.h"

#include <string>

using namespace RattletrapEngine;

class Obstacle : public Component{
	public:
        Obstacle(GameObject& associated,std::string path, Vec2 position);
		~Obstacle(void);
		void Update(float dt);
        void NotifyCollision(Component &other);
        bool Is(int componentType) const;
	private:
        Sprite *sp;
};

#endif // OBSTACLE_H
