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
#include "Tileset.h"
#include "Timer.h"
#include "Tower.h"
#include "WaveManager.h"
#include "Obstacle.h"
#include "Sound.h"
#include "NearestGOFinder.h"

#define TOWERNAME_DEFAULT_TEXT " "
#define TOWERCOST_DEFAULT_TEXT " "
#define TOWERDAMAGE_DEFAULT_TEXT " "
#define TOWERDAMGETYPE_DEFAULT_TEXT " "

using std::vector;

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
		InputManager &inputManager;

		Music music;
		
		bool isLightning;
		bool isThundering;
		Timer lightningTimer;
		Color lightningColor;
		float lightningInterval;

		vector<int> waves;//vetor de waves a ser lido no arquivo
		
		void InitializeObstacles(void);
		std::vector<std::unique_ptr<Obstacle>> obstacleArray;
		void AddObstacle(Obstacle *obstacle);
		void RenderObstacleArray(void) const;
		Sound nightSound;
		Sound thunderSound;
		Sound towerMenuSounds; /**< Som referente a compra, venda e construçao de torre. */

		int frameRateCounter;
		Timer frameRateTimer;


		bool menuIsShowing;

        GameObject* HUDcanvas;

        GameObject* menuBg;
        GameObject* openMenuBtn;
		Sound menuMove;

        GameObject* towerInfoGroup;
        GameObject* towerName;
        GameObject* towerCost;
        GameObject* towerDamage;
        GameObject* towerDamageType;

        GameObject* towersBtnGroup;
        GameObject* towerBtn1;
        GameObject* towerBtn2;
        GameObject* towerBtn3;
        GameObject* towerBtn4;

        GameObject* health;
        GameObject* healthIcon;
        GameObject* healthbarBg;
        GameObject* healthbarBar;

        GameObject* wave;
        GameObject* waveIcon;
        GameObject* wavebarBg;
        GameObject* wavebarBar;

        GameObject* money;
        GameObject* moneyIcon;
        GameObject* moneyText;
};

#include "EndState.h"

#endif // STAGE_STATE_H
