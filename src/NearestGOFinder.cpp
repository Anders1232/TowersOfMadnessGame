#include "NearestGOFinder.h"

NearestGOFinder::NearestGOFinder(std::string targetType,Vec2 origin)
:origin(origin){}

NearestGOFinder::NearestGOFinder()
{

}

float NearestGOFinder::operator()(GameObject* go){
    if(nullptr != go){
        return(origin.VecDistance(go->box.Center()).Magnitude());
    }
}

void NearestGOFinder::setOrigin(Vec2 origin){
    this->origin = origin;
}
