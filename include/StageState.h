#ifndef STAGE_STATE_H
#define STAGE_STATE_H

#include <memory>
#include <vector>
#include <map>

#include "Game.h"
#include "ActionManager.h"
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
#include "Text.h"
#include "Component.h"

#define TOWERNAME_DEFAULT_TEXT " "
#define TOWERCOST_DEFAULT_TEXT " "
#define TOWERDAMAGE_DEFAULT_TEXT " "
#define TOWERDAMGETYPE_DEFAULT_TEXT " "

using std::vector;
using namespace RattletrapEngine;

class StageState: public State, public TileMapObserver, public NearestFinder<GameObject> {
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
		GameObject* FindNearest(Vec2 origin, Finder<GameObject> &finder, float range = std::numeric_limits<float>::max()) const;
		std::vector<GameObject*>* FindNearests(Vec2 origin,Finder<GameObject> &finder,float range) const;
		void AddCollider(Component& collider,GameObject& associated);
		void RemoveCollider(Component& collider);
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
		InputManager &inputManager;
		Music music;
		bool isLightning;
		bool isThundering;
		Timer lightningTimer;
		Color lightningColor;
		Sound nightSound;
		Sound thunderSound;
		Sound towerMenuSounds; /**< Som referente a compra, venda e construçao de torre. */
		Sound menuMove;
		GameObject *waveManagerGO;
		TileSet tileSet;
		TileMap<Tile> tileMap;/**< Mapa de tiles do jogo. */
		WaveManager *waveManager;/**< Referencia para a WaveManager, gerenciador de waves. Essa Referência existe aqui por motivos de perfornance, para não ter que procurá-lo todo Update.*/
		int frameRateCounter;
		std::map<Component*,GameObject*> collisionMap;
		
		float lightningInterval;

		vector<int> waves;//vetor de waves a ser lido no arquivo
		
		void InitializeObstacles(void);

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

		GameObject* openMenuBtnGO;

		GameObject* HUDcanvasGO;

		GameObject* towerBtn1GO;
		GameObject* towerBtn2GO;
		GameObject* towerBtn3GO;
		GameObject* towerBtn4GO;

		Button towerBtn1;
		Button towerBtn2;
		Button towerBtn3;
		Button towerBtn4;

		Button openMenuBtn;

		GameObject* towerNameGO;
		GameObject* towerCostGO;
		GameObject* towerDamageGO;
		GameObject* towerDamageTypeGO;

		GameObject* towerInfoGroupGO;
		GameObject* towersBtnGroupGO;

		Grouper towersInfoGroup;
		Grouper towersBtnGroup;




};

#include "EndState.h"

#endif // STAGE_STATE_H


