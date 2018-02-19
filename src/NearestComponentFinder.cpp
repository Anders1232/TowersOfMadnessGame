#include "NearestComponentFinder.h"

NearestComponentFinder::NearestComponentFinder(int componentType,Vec2 origin)
:componentType(componentType),
 origin(origin){}

NearestComponentFinder::NearestGOFinder()
{

}

float NearestComponentFinder::operator()(GameObject* go){
    Component& component = nullptr;
    if(nullptr != go){
        component = go->GetComponent(componentType);
        if(nullptr != component){
            return(origin.VecDistance(go->box.Center()).Magnitude());
        }
    }
}

void NearestGOFinder::setOrigin(Vec2 origin){
    this->origin = origin;
}
