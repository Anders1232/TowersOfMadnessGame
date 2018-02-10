#ifndef ENDSTATEDATA_H
#define ENDSTATEDATA_H

#include "StateData.h"
#include "GameStateDataType.h"

using namespace RattletrapEngine;

class EndStateData: StateData {
	public:
		bool Is(StateDataType typeToCheck) const;
		EndStateData(bool playerVictory);
		bool playerVictory;
};

#endif
