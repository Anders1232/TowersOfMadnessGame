#ifndef DRAGANDDROP_H
#define DRAGANDDROP_H

#include "Component.h"
#include "TileMap.h"
#include "Sound.h"
/**
    \brief Classe responsável por arrastar e posicionar Objetos.

    O DragAndDrop é um tipo de Componente. O objetivo do DragAndDrop é selecionar um objeto arrastavel por meio de Input e posicioná-lo em uma tile. E assim, atualizar o mapa de Tiles.
*/
template <class T>
class DragAndDrop : public Component{
    public:
        /**
            \brief Construtor.
            \param map Usado para inserir o objeto no tileMap.
            \param draggin Informa se o objeto esta sendo arrastado.
            \param redrag Verdadeiro se for um redrag, falso caso seja um drag inicial.
            \param forceDrag
            \param dragOnActionHold Tem como objetivo verificar se o arrastar é enquanto o botão da ação é segurado ou se pressiona uma vez para arrastar e outra vez para soltar.

            Instancia o DragAndDrop, informando se o objeto está sendo arrastado.
        */
        DragAndDrop(TileMap<T> &map, Vec2 associatedInitialPos, GameObject &associated, bool redrag = true, bool dragOnActionHold = true);
        /**
            \brief Atualiza o estado do objeto em arrasto.

            Nesse método a lógica do drag and drop é implementada. Se o botão do drag estiver sendo segurado o objeto é arrastado mantendo seu centro onde o mouse está. Quando o botão relativo ao drag and drop é liberado chama-se o tileMap para iserir o GameObject no tile onde o mouse está.
        */
        void Update(float dt);
        /**
            \brief Informa que o tipo deste Compomente é DragAndDrop.
            \param CompomentType A ser comparado se é igual ao Tipo desta classe.

            Retorna Verdadeiro se CompomentType também é do tipo DragAndDrop, caso contrário retorna falso.
        */
        bool Is(ComponentType) const;
    private:
        InsertGO(GameObject* obj,Vec2 initialPos);
        InsertGO(GameObject* obj, bool checkCollision = true);
        bool dragOnHold;/**< [Não utilizado no momento] Armazena a informação se o botão deve ser segurado para arrastar, ou apenas pressiona uma vez para arrastar e outra vez para soltar.*/
        Vec2 associatedInitialPos;/**<Vec2 no qual o será armazenada a posição do objeto associado no instante do drag.*/
        TileMap<T> &tileMap;/**< TileMap no qual o GameObject será inserido após o drag.*/
        bool redrag;/**<Informa se é um drag inicial ou um redrag*/
        Sound dragNDrop;
};

//implementação:

#include "Camera.h"
#include "Error.h"
#include "InputManager.h"
#include "TileMap.h"

template <class T>
DragAndDrop<T>::DragAndDrop(TileMap<T> &map,Vec2 associatedInitialPos, GameObject &associated, bool redrag, bool dragOnActionHold)
            : Component(associated)
            , dragOnHold(dragOnActionHold)
            , associatedInitialPos(associatedInitialPos)
            , tileMap(map)
            , redrag(redrag)
            , dragNDrop("audio/Acoes/Consertando1.wav") {
}

bool DragAndDrop::InsertGO(GameObject* obj,Vec2 initialPos){

    Vec2 mousePos = Camera::ScreenToWorld(InputManager::GetInstance().GetMousePos());
    int position = GetCoordTilePos(mousePos, false, 0);
    REPORT_DEBUG("\t position = " << position << "\t of " << mapHeight*mapWidth << " tiles.");
    if(0 > position) {
        std::cout << WHERE << "[ERROR] Tried to put the gameObject on an invalid tileMap position." << END_LINE;
    }
    int initialTile = map.GetCoordTilePos(initialPos, false, 0);
    if(-1 == map.AtLayer(position, COLLISION_LAYER)) {
        REPORT_DEBUG("\tInserting the gameObject at position " << position);
        map.At(position,COLLISION_LAYER).setGO(obj);
        int line = position / GetWidth();
        int column = position % GetWidth();
        obj->box.x = column*map.GetTileSize().x;
        obj->box.y = line*map.GetTileSize().y;
        map.At(initialTile,COLLISION_LAYER).setGO(nullptr);//Retira da posicao inicial
        //TODO: aqui ajudar a box para ficar exatamente no tileMap
    }
    else {
        int line = initialTile / GetWidth();
        int column = initialTile % GetWidth();
        obj->box.x = column*map.GetTileSize().x;
        obj->box.y = line*map.GetTileSize().y;
    }
}

bool DragAndDrop::InsertGO(GameObject* obj, bool checkCollision) {
    Vec2 mousePos = Camera::ScreenToWorld(InputManager::GetInstance().GetMousePos());
    int position = GetCoordTilePos(mousePos, false, 0);
    REPORT_DEBUG("\t position = " << position << "\t of " << mapHeight*mapWidth << " tiles.");
    if(0 > position) {
        std::cout << WHERE << "[ERROR] Tried to put the gameObject on an invalid tileMap position." << END_LINE;
        obj->RequestDelete();
    }
    if(checkCollision){
        if(-1 == AtLayer(position, COLLISION_LAYER)) {
            REPORT_DEBUG("\tInserting the gameObject at position " << position);
            map.At(position,COLLISION_LAYER).setGO(obj);
            int line = position / GetWidth();
            int column = position % GetWidth();
            obj->box.x = column*map.GetTileSize().x;
            obj->box.y = line*map.GetTileSize().y;
            //TODO: aqui ajudar a box para ficar exatamente no tileMap
            map.ReportChanges(position);
        }
        else if(0 > AtLayer(position, COLLISION_LAYER)) {
            REPORT_DEBUG("\ttentado inserir objeto em posição inválida, pois nela está" << tileMatrix[position+(COLLISION_LAYER * mapWidth*mapHeight)]);
            obj->RequestDelete();
        }
        else {
            REPORT_DEBUG("\ttentado inserir objeto em posição já ocupada!");
            obj->RequestDelete();
        }
    }
    else{
        int tilePos= GetCoordTilePos(obj->box.Center(), false, 0);
        REPORT_DEBUG("\tInserting the gameObject at position " << tilePos);
        map.At(position,COLLISION_LAYER).setGO(obj);
        int line = tilePos / GetWidth();
        int column = tilePos % GetWidth();
        obj->box.x = column*map.GetTileSize().x;
        obj->box.y = line*map.GetTileSize().y;
        //TODO: aqui ajudar a box para ficar exatamente no tileMap
    }
}

template <class T>
void DragAndDrop<T>::Update(float dt) {
    InputManager &inputManager= InputManager::GetInstance();
    if(inputManager.MouseRelease(RIGHT_MOUSE_BUTTON)) {
        bool success = false;
        if(redrag) {
            success = tileMap.InsertGO(&associated, associatedInitialPos);
        } else {
            success = tileMap.InsertGO(&associated);
        }
        if(success) { dragNDrop.Play(1); }
        associated.RemoveComponent(DRAG_AND_DROP);
    } else if(inputManager.IsMouseDown(RIGHT_MOUSE_BUTTON) || !dragOnHold) {
        Vec2 mousePos = Camera::ScreenToWorld(inputManager.GetMousePos() );
        associated.box = mousePos-Vec2(associated.box.w/2, associated.box.h/2);
    }
}

template <class T>
bool DragAndDrop<T>::Is(ComponentType type) const {
    return ComponentType::DRAG_AND_DROP == type;
}



#endif // DRAGANDDROP_H
