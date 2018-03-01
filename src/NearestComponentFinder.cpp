#include "NearestComponentFinder.h"

NearestComponentFinder::NearestComponentFinder(int componentType,Vec2 origin)
:componentType(componentType),
 origin(origin){}

NearestComponentFinder::NearestComponentFinder()
{

}

float NearestComponentFinder::operator()(GameObject go){

    Component& component = go.GetComponent(componentType);
    return(origin.VecDistance(go.box.Center()).Magnitude());

}

void NearestComponentFinder::setOrigin(Vec2 origin){
    this->origin = origin;
}
