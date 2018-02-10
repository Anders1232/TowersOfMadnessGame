#include "NearestGOFinder.h"

NearestGOFinder::NearestGOFinder(std::string targetType,Vec2 origin)
:targetType(targetType),
 origin(origin){}

float NearestGOFinder::operator()(GameObject* go){
    if(nullptr != go){
        if(go->Is(targetType)){
            return(origin.VecDistance(go->box.Center()).Magnitude());
        }
    }
}
