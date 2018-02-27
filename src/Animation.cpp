#include "Animation.h"
#include "Camera.h"

Animation::Animation(GameObject& associated
            float x, float y, float rotation,
            string sprite, int frameCount,
            float frameTime, bool ends
            ) : Component(associated), endTimer(), timeLimit(frameCount*frameTime)
            , oneTimeOnly(ends){
        sp = new Sprite(sprite,associated, false, frameTime, frameCount );
        associated.AddComponent(sp);
        associated.box = Vec2(x - sp.GetWidth()/2, y - sp.GetHeight()/2);
        associated.rotation = rotation;
    }

    void Animation::Update(float dt){
        if(oneTimeOnly) {
            if(endTimer.Get() > timeLimit) {
                associated.RequestDelete();
            }
        }
        endTimer.Update(dt);
    }

    Rect Animation::GetWorldRenderedRect(void) const {
        return Camera::WorldToScreen(box);
    }

    void Animation::NotifyCollision(GameObject &other) {}

    bool Animation::Is(int componentType) const  {
        return componentType == GameComponentType::ANIMATION;
    }

}
