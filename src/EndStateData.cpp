#ifndef ENDSTATEDATA_H
#define ENDSTATEDATA_H

#include "StateData.h"
#include "GameStateDataType.h"

inline EndStateData::EndStateData(bool playerVictory) {
	this-> playerVictory= playerVictory;
}

inline bool EndStateData::Is(StateDataType typeToCheck) const {
    return typeToCheck == GameStateDataType::STATE_DATA_END;
}


#endif
