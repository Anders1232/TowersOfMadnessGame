#include "WaveManager.h"
#include "TileMap.h"
#include "InputManager.h"
#include "Vec2.h"
#include "Game.h"
#include "Enemy.h"
#include "GameResources.h"
#include "Error.h"
#include "PlayerData.h"

#define TIME_BETWEEN_SPAWN 0.8
#define TIME_BETWEEN_WAVES 5.0

int WaveManager::waveCount = 0;

WaveManager::WaveManager(TileMap<T> &tileMap, string waveFile)
		: tileMap(tileMap)
		, waveStartSound("audio/Acoes/Inicio de Wave.wav")
		, levelUpSound("audio/Acoes/Level Up.wav")
		, lostEnemySound("audio/Acoes/perdeu1.wav")
		, betweenWavesTimer()
		, waitingForTheNextWave(false) {
	endWave=true;
	enemiesLeft = 1;
	playerLifes = 30;
	REPORT_DEBUG2(1, "Buscando spawn points.");
	spawnGroups= tileMap.GetTileGroups(SPAWN_POINT);
	REPORT_DEBUG2(1, "Buscando end points.");
	endGroups= tileMap.GetTileGroups(END_POINT);
	wavesAndEnemysData = GameResources::GetWaveData("assets/wave&enemyData.txt");
	enemyIndex = 0;
	waveIndex=-1;
	totalWaves = wavesAndEnemysData->first.size();
	victory = false;
	// StartWave();
}

WaveManager::~WaveManager(){
	delete spawnGroups;
	delete endGroups;
}
/*
vector<vector<int>>* WaveManager::GetTileGroups(int tileType) const {
    vector<vector<int>> *tilePoints = new vector<vector<int>>();
    vector<int> foundTilePoints;
    uint countLimit = GetWidth()*GetHeight();
    int base = countLimit*COLLISION_LAYER;
    REPORT_I_WAS_HERE;

    for(uint i = 0; i < countLimit; i++) {
        int positionToBeSearch = base+i;
        if(tileType == tileMatrix[positionToBeSearch]) {
            foundTilePoints.push_back(positionToBeSearch%(GetWidth()*GetHeight()));
        }
    }
    REPORT_I_WAS_HERE;
    // Agora que tenho todos os spawn points vou agrupá-los de acordo com suas adjacências.
    if(foundTilePoints.empty()) {
        Error("Não foi encontrado spawn points!");
    }

    tilePoints->emplace_back();
    (*tilePoints)[0].push_back(foundTilePoints[0]);
    foundTilePoints.erase(foundTilePoints.begin());
    while(!foundTilePoints.empty()) {
        bool neighborFound = false;
        for(unsigned int i = 0; i < tilePoints->size(); i++) {
            vector<int> &vec = (*tilePoints)[i];
            if(
                    (std::find(vec.begin(), vec.end(), foundTilePoints[0]+1) != vec.end() )//posição à direita
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]-1) != vec.end() )//posição à esquerda
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]+GetWidth()) != vec.end() )// posição em cima
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]-GetWidth()) != vec.end() )//posição em baixo
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]-GetWidth()-1) != vec.end() )//diagonal supeior esquerda
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]-GetWidth()+1) != vec.end() )//diagonal supeior direita
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]+GetWidth()-1) != vec.end() )//diagonal inferior esquerda
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]+GetWidth()+1) != vec.end() )//diagonal inferior direita
            ){
                vec.push_back(foundTilePoints[0]);
                foundTilePoints.erase(foundTilePoints.begin());
                neighborFound = true;
                break;
            }
        }
        if(!neighborFound) {
            tilePoints->emplace_back();
            (*tilePoints)[tilePoints->size()-1].push_back(foundTilePoints[0]);
            foundTilePoints.erase(foundTilePoints.begin());
        }
        REPORT_I_WAS_HERE;
    }
#if DEBUG
    std::cout << WHERE << "\tNumero de tile groups achados: " << (*tilePoints).size() << END_LINE;
    for(uint i = 0; i < (*tilePoints).size(); i++) {
        std::cout << WHERE << "\tTile groups " << i <<" tem tamanho " << (*tilePoints)[i].size() << END_LINE;
        std::cout << WHERE << "\tTileWidth= " << GetWidth() << END_LINE;
        for(uint i2 = 0; i2 < (*tilePoints)[i].size(); i2++) {
            std::cout << WHERE << "\tSpawn point: " << (*tilePoints)[i][i2] << END_LINE;
        }
    }
#endif
    return tilePoints;
}*/

void WaveManager::StartWave(void){
	enemiesLeft=0;
	maxNumberOfEnemiesInSpawnPoint=0;
	int numberOfEnemiesInSpawnPoint;
	REPORT_I_WAS_HERE;
	for (uint i = 0; i < wavesAndEnemysData->first[waveIndex].spawnPointsData.size(); i++){
		numberOfEnemiesInSpawnPoint=0;
		for (uint j = 0; j < wavesAndEnemysData->first[waveIndex].spawnPointsData[i].enemySpawnData.size(); j++){
			enemiesLeft += wavesAndEnemysData->first[waveIndex].spawnPointsData[i].enemySpawnData[j].numberOfEnemies;
			numberOfEnemiesInSpawnPoint+= wavesAndEnemysData->first[waveIndex].spawnPointsData[i].enemySpawnData[j].numberOfEnemies;
		}
		if(numberOfEnemiesInSpawnPoint > maxNumberOfEnemiesInSpawnPoint){
			maxNumberOfEnemiesInSpawnPoint= numberOfEnemiesInSpawnPoint;
		}
	}
	waveTotalEnemies = enemiesLeft;
	enemyIndex = 0;
	endWave = false;
	++waveCount;
}


bool WaveManager::EndWave(void) const{
	return endWave;
}
 
void WaveManager::Update(float dt){
	if(EndWave()){
		if(totalWaves==waveCount){ //Check Game over Condition
			//Ao invés de não fazer nada deve-ser informar o fim de jogo
			victory = true;
			return;
		}else{
			if(!waitingForTheNextWave){
				waitingForTheNextWave= true;
				betweenWavesTimer.Restart();
			}
			else{
				betweenWavesTimer.Update(dt);
				if(TIME_BETWEEN_WAVES < betweenWavesTimer.Get()){
					++waveIndex;
					waveStartSound.Play(1);
					StartWave();
				}
			}
			REPORT_I_WAS_HERE;
		}
	}else{
		if(enemyIndex <= maxNumberOfEnemiesInSpawnPoint){
			spawnTimer.Update(dt);
			if(TIME_BETWEEN_SPAWN < spawnTimer.Get()){ // spawn cooldown
				//spawn 1 enemy at each existing spawn group
				WaveData currentWave = wavesAndEnemysData->first[waveIndex];
				for (uint i = 0; i < currentWave.spawnPointsData.size(); i++){
					uint enemiesCounter= enemyIndex;
					uint indexOfTheEnemyToSpawn=0;
					bool breaked= false;
					REPORT_I_WAS_HERE;
					while(enemiesCounter >= currentWave.spawnPointsData.at(i).enemySpawnData.at(indexOfTheEnemyToSpawn).numberOfEnemies){
						REPORT_I_WAS_HERE;
						if(indexOfTheEnemyToSpawn>= currentWave.spawnPointsData.at(i).enemySpawnData.size()){
							REPORT_I_WAS_HERE;
							breaked= true;
							REPORT_I_WAS_HERE;
							break;
						}
						REPORT_I_WAS_HERE;
						enemiesCounter-= currentWave.spawnPointsData.at(i).enemySpawnData.at(indexOfTheEnemyToSpawn).numberOfEnemies;
						indexOfTheEnemyToSpawn++;
						REPORT_I_WAS_HERE;
						if(indexOfTheEnemyToSpawn>= currentWave.spawnPointsData.at(i).enemySpawnData.size()){
							REPORT_I_WAS_HERE;
							breaked= true;
							REPORT_I_WAS_HERE;
							break;
						}
						REPORT_I_WAS_HERE;
					}
					if(breaked){
						continue;
					}
					REPORT_DEBUG("\t indexOfTheEnemyToSpawn= " << indexOfTheEnemyToSpawn);
					
					EnemySpawnData &enemyToSpawn= currentWave.spawnPointsData.at(i).enemySpawnData.at(indexOfTheEnemyToSpawn);
					REPORT_DEBUG("\t enemyIndex= " << enemyToSpawn.enemyIndex);
					REPORT_DEBUG("\t baseHP= " << enemyToSpawn.baseHP);
					REPORT_DEBUG("\t endPoint= " << enemyToSpawn.endPoint);
					int spawnPosition = rand()% ( (*spawnGroups).at(i) ).size();
					int endPosition= rand()% ( ( (*endGroups).at(enemyToSpawn.endPoint) ).size());
					int endTilePosition= (*endGroups).at(enemyToSpawn.endPoint).at(endPosition);
					SpawnEnemy( (*spawnGroups).at(i).at(spawnPosition), enemyToSpawn.enemyIndex, enemyToSpawn.baseHP, endTilePosition, indexOfTheEnemyToSpawn );
				}
				REPORT_I_WAS_HERE;
				spawnTimer.Restart();
				enemyIndex++;
			}
			REPORT_I_WAS_HERE;
		}
		REPORT_I_WAS_HERE;
	}
	REPORT_I_WAS_HERE;
	if (0 >= enemiesLeft){
		endWave = true;
		float income = (waveCount* 5)+100;
		levelUpSound.Play(1);
		PLAYER_DATA_INSTANCE.GoldUpdate(income);
	}
}

void WaveManager::SpawnEnemy(int tileMapPosition, int enemyId, uint baseHP, uint endPoint, uint indexOfTheEnemyToSpawn ){
	EnemyData &currentWaveEnemyData = wavesAndEnemysData->second[indexOfTheEnemyToSpawn];
	REPORT_DEBUG("\t currentWaveEnemyData.scale= ("<<currentWaveEnemyData.scaleX<<","<<currentWaveEnemyData.scaleY<<")" );
	Vec2 tileSize= tileMap.GetTileSize();
	Vec2 spawnPosition;
	REPORT_I_WAS_HERE;
	spawnPosition.x = (tileMapPosition%tileMap.GetWidth() ) * tileSize.x;
	REPORT_I_WAS_HERE;
	spawnPosition.y = (tileMapPosition/tileMap.GetWidth() ) * tileSize.y;
	Enemy* enemy = new Enemy(spawnPosition, enemyIndex, currentWaveEnemyData, baseHP, endPoint, tileMap, *this);
	Game::GetInstance().GetCurrentState().AddObject(enemy);
}

bool WaveManager::Is(ComponentType type) const{
	return type == WAVE_MANAGER;
}

void WaveManager::NotifyEnemyGotToHisDestiny(void){
	--playerLifes;
	PLAYER_DATA_INSTANCE.DecrementLife();
	lostEnemySound.Play(1);
}

void WaveManager::NotifyEnemyGotKilled(void){
	--enemiesLeft;
	PLAYER_DATA_INSTANCE.IncrementKills();
}

int WaveManager::GetLifesLeft(void){
	return playerLifes;
}

int WaveManager::GetEnemiesLeft(void){
	return enemiesLeft;
}

int WaveManager::GetWaveTotalEnemies(void) {
 return waveTotalEnemies;
}

bool WaveManager::Victory(void){
	return victory;
}
