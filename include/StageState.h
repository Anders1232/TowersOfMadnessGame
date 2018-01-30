#ifndef STAGE_STATE_H
#define STAGE_STATE_H

#include <memory>
#include <vector>

#include "ActionManager.h"
#include "AIGoDown.h"
#include "AIPrintPath.h"
#include "DragAndDrop.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Music.h"
#include "Obstacle.h"
#include "Sprite.h"
#include "State.h"
#include "TileMap.h"
#include "Timer.h"
#include "Tower.h"
#include "WaveManager.h"
#include "Obstacle.h"
#include "Sound.h"
#include "NearestGOFinder.h"
#include "Grouper.h"
#include "Tile.h"
#include "Button.h"

#define TOWERNAME_DEFAULT_TEXT " "
#define TOWERCOST_DEFAULT_TEXT " "
#define TOWERDAMAGE_DEFAULT_TEXT " "
#define TOWERDAMGETYPE_DEFAULT_TEXT " "

using std::vector;
//using namespace RattletrapEngine;

class StageState: public State, public TileMapObserver, public NearestGOFinder {
	public:
		StageState(void);
		~StageState(void);
		void Update(float dt);
		void Render(void) const;
		void Pause(void);
		void Resume(void);
		void LoadAssets(void) const;
		void ShowLightning(float dt);
		void SetUILife(float lifePercent);
		void SetUIWaveProgress(float waveProgressPercent);
		void SetUIMoney(int coins);
		void NotifyTileMapChanged(int tilePosition);
		GameObject* FindNearestGO(Vec2 origin, std::string targetType, float range= std::numeric_limits<float>::max());
		std::vector<GameObject*>* FindNearestGOs(Vec2 origin, std::string targetType, float range= std::numeric_limits<float>::max());
	private:
		void SetupUI(void);
		void UpdateUI(float dt);
		void RenderUI(void) const;
		void ToggleMenu(void);
		void SetTowerInfoData(string name = TOWERNAME_DEFAULT_TEXT,
							  string cost = TOWERCOST_DEFAULT_TEXT,
							  string damage = TOWERDAMAGE_DEFAULT_TEXT,
							  string damageType = TOWERDAMGETYPE_DEFAULT_TEXT
		);
		void CreateTower(Tower::TowerType towerType);
		TileSet tileSet;
        GameObject *waveManagerGO;
        Sound menuMove;
        TileMap<Tile> tileMap;/**< Mapa de tiles do jogo. */
		InputManager &inputManager;

		Music music;
		
		bool isLightning;
		bool isThundering;
		Timer lightningTimer;
		Color lightningColor;
		float lightningInterval;

		WaveManager *waveManager;/**< Referencia para a WaveManager, gerenciador de waves. Essa Referência existe aqui por motivos de perfornance, para não ter que procurá-lo todo Update.*/
		vector<int> waves;//vetor de waves a ser lido no arquivo
		
		void InitializeObstacles(void);
		Sound nightSound;
		Sound thunderSound;
		Sound towerMenuSounds; /**< Som referente a compra, venda e construçao de torre. */

		int frameRateCounter;
		Timer frameRateTimer;


		bool menuIsShowing;

        GameObject* healthGO;
        GameObject* healthIconGO;
        GameObject* healthbarBgGO;
        GameObject* healthbarBarGO;

        GameObject* waveGO;
        GameObject* waveIconGO;
        GameObject* wavebarBgGO;
        GameObject* wavebarBarGO;

        GameObject* moneyGO;
        GameObject* moneyIconGO;
        GameObject* moneyTextGO;

        GameObject* menuBgGO;

        GameObject* HUDcanvasGO;

        GameObject* towerBtn1GO;
        GameObject* towerBtn2GO;
        GameObject* towerBtn3GO;
        GameObject* towerBtn4GO;

        Button towerBtn1;
        Button towerBtn2;
        Button towerBtn3;
        Button towerBtn4;

        GameObject* towersBtnGroupGO;

        GameObject* openMenuBtnGO;
		Button openMenuBtn;

        GameObject* towerInfoGroupGO;
        GameObject* towerNameGO;
        GameObject* towerCostGO;
        GameObject* towerDamageGO;
        GameObject* towerDamageTypeGO;


        Grouper towersInfoGroup;
        Grouper towersBtnGroup;



};

struct btnCallbackArgAux{
    StageState* ST;
    GameObject* GO;
};

#include "EndState.h"

#endif // STAGE_STATE_H


