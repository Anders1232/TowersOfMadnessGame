#ifndef GAMECOMPONENTTYPE
#define GAMECOMPONENTTYPE
#include "ComponentType.h"
/**
    \brief Tipos de Componentes

    Enumeração com os tipos de componentes possíveis, usado no método Is que todo componente deve implementar.
    Foi colocado em um arquivo separado por problemas de inclusão circular entre GameObject e Component.
*/

enum GameComponentType{
    DRAG_AND_DROP=ComponentType::COMPONENT_TYPE_SIZE,
    HIT_POINTS,
    AI_GO_DOWN,
    AI_GO_TO,
    AI_PRINT_PATH,
    AI_ART,
    AI_MEDIC,
    AI_QUIMIC,
    AI_ENGINEER,
    WAVE_MANAGER,
    SHOOTER,
    STUN_AURA,
    SLOW_AURA,
    HEAL_AURA,
    PLAYER_DATA,
    ENEMY,
    GAME_COMPONENT_TYPE_SIZE
};

#endif // GAMECOMPONENTTYPE
