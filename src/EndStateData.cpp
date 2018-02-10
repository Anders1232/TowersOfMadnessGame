#include "EndStateData.h"
#include "GameStateDataType.h"

//using namespace RattletrapEngine;

EndStateData::EndStateData(bool playerVictory) {
	this-> playerVictory= playerVictory;
}

bool EndStateData::Is(StateDataType typeToCheck) const {
    return typeToCheck == GameStateDataType::STATE_DATA_END;
}

