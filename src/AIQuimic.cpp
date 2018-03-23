#include <algorithm>
#include "AIQuimic.h"
#include "Shooter.h"

AIQuimic::AIQuimic(float speed, int dest, TileMap<Tile>& tileMap, GameObject &associated,WaveManager& wManager)
	:Component(associated),
	 speed(speed),
	 vecSpeed(Vec2(0.0,0.0)),
	 lastDistance(std::numeric_limits<float>::max()),
	 destTile(dest),
	 path(new std::vector<int>()),
	 pathIndex(0),
	 heuristic(new ManhattanDistance()),
	 tileWeightMap((*GameResources::GetWeightData("map/WeightData.txt"))[((Enemy&)associated).GetType()]),
	 tileMap(tileMap),
	 associated(associated),
	 waveManager(wManager),
	 randomMaxTimer(0),
	 finder(new NearestComponentFinder(GameComponentType::TOWER,associated.box.Center())),
	 shooter(new Shooter(associated, &((NearestFinder<GameObject>&)Game::GetInstance().GetCurrentState()), (Finder<GameObject>*)finder, GameComponentType::TOWER, 500000, 2.5, Shooter::TargetPolicy::ALWAYS_NEAREST, true, 500, 500000, "img/SpriteSheets/bomba_spritesheet.png",2,3.0))
	 {

	tileMap.ObserveMapChanges(this);
	associated.AddComponent(shooter);

	dfa[AIState::WAITING][AIEvent::PATH_FREE] = AIState::WALKING;
	dfa[AIState::WAITING][AIEvent::NONE] = AIState::WAITING;

	dfa[AIState::WALKING][AIEvent::STUN] = AIState::STUNNED;
	dfa[AIState::WALKING][AIEvent::PATH_BLOCKED] = AIState::SENDING_BOMB;
	dfa[AIState::WALKING][AIEvent::SMOKE] = AIState::WALKING_SLOWLY;
	dfa[AIState::WALKING][AIEvent::NONE] = AIState::WALKING;

	dfa[AIState::SENDING_BOMB][AIEvent::STUN] = AIState::STUNNED;
	dfa[AIState::SENDING_BOMB][AIEvent::PATH_FREE] = AIState::WALKING;
	dfa[AIState::SENDING_BOMB][AIEvent::NONE] = AIState::SENDING_BOMB;

	dfa[AIState::STUNNED][AIEvent::NOT_STUN] = AIState::WALKING;
	dfa[AIState::STUNNED][AIEvent::PATH_BLOCKED] = AIState::SENDING_BOMB;
	dfa[AIState::STUNNED][AIEvent::NONE] = AIState::STUNNED;

	dfa[AIState::WALKING_SLOWLY][AIEvent::NOT_SMOKE] = AIState::WALKING;
	dfa[AIState::WALKING_SLOWLY][AIEvent::PATH_BLOCKED] = AIState::SENDING_BOMB;
	dfa[AIState::WALKING_SLOWLY][AIEvent::STUN] = AIState::STUNNED;
	dfa[AIState::WALKING_SLOWLY][AIEvent::NONE] = AIState::WALKING_SLOWLY;

	actualState = AIState::WAITING;

}

AIQuimic::~AIQuimic(void){
	tileMap.RemoveObserver(this);
	delete heuristic;
	delete finder;
}

AIQuimic::AIEvent AIQuimic::ComputeEvents(){
	if(actualState == AIState::WAITING){
		if(path->empty()){
			return AIEvent::NONE;
		}
		else{
			return AIEvent::PATH_FREE;
		}
	}
	else if(actualState == AIState::WALKING){
		if(((Enemy*)associated.GetComponent(GameComponentType::ENEMY))->GetLastEvent() == Enemy::Event::STUN){// Aqui verifica-se a colisão com o elemento estonteante
			return AIEvent::STUN;
		}
		else if(pathIndex == path->size()){
			return AIEvent::PATH_BLOCKED;
		}
		else if(((Enemy&)associated).GetLastEvent() == Enemy::Event::SMOKE){
			return AIEvent::SMOKE;
		}
		else{return NONE;}
	}
	else if(actualState == AIState::WALKING_SLOWLY){
		if(((Enemy*)associated.GetComponent(GameComponentType::ENEMY))->GetLastEvent() == Enemy::Event::STUN){// Aqui verifica-se a colisão com o elemento estonteante
			return AIEvent::STUN;
		}
		else if(((Enemy*)associated.GetComponent(GameComponentType::ENEMY))->GetLastEvent() != Enemy::Event::SMOKE){// Aqui verifica-se o fim da colisão com o elemento de fumaça
			return AIEvent::NOT_SMOKE;
		}
		else if(pathIndex == path->size()){
			TEMP_REPORT_I_WAS_HERE;
			return AIEvent::PATH_BLOCKED;
		}
		else{return NONE;}
	}
	else if(actualState == AIState::SENDING_BOMB){
		if(((Enemy*)associated.GetComponent(GameComponentType::ENEMY))->GetLastEvent() == Enemy::Event::STUN){// Aqui verifica-se a colisão com o elemento estonteante
			return AIEvent::STUN;
		}
		else if(!path->empty()){
			return AIEvent::PATH_FREE;
		}
		else{return NONE;}
	}
	else if(actualState == AIState::STUNNED){
		if(((Enemy*)associated.GetComponent(GameComponentType::ENEMY))->GetLastEvent() != Enemy::Event::STUN){// Aqui verifica-se o fim da colisão com o elemento estonteante
			return AIEvent::NOT_STUN;
		}
		else if(pathIndex == path->size()){
			return AIEvent::PATH_BLOCKED;
		}
		else{return NONE;}
	}
	return NONE;
}

void AIQuimic::Update(float dt){
	AIEvent actualTransition = ComputeEvents();
	actualState = dfa[actualState][actualTransition];
	if(actualState == AIState::WAITING){
		Vec2 originCoord= associated.box.Center();
		path= GameResources::GetPath(((Enemy*)associated.GetComponent(GameComponentType::ENEMY))->GetType(), heuristic, tileMap.GetCoordTilePos(originCoord, false, 0), destTile, "map/WeightData.txt");
		actualTileweight = tileWeightMap.at(tileMap.AtLayer((*path).at(pathIndex),WALKABLE_LAYER).GetTileSetIndex());
		pathIndex = 0;
	}
	if(actualState == AIState::WALKING){
		if(pathIndex != path->size() && path->size() > 0){
			tempDestination = Vec2(tileMap.GetTileSize().x * ((*path).at(pathIndex) % tileMap.GetWidth()),tileMap.GetTileSize().y*((*path).at(pathIndex) / tileMap.GetWidth()));
			float distance = associated.box.Center().VecDistance(tempDestination).Magnitude();
			if((vecSpeed.MemberMult(dt)).Magnitude() >= distance || lastDistance < distance){
				Vec2 movement= tempDestination-Vec2(associated.box.w/2, associated.box.h/2);
				associated.box.x = movement.x;
				associated.box.y = movement.y;
				pathIndex++;
				if(pathIndex != path->size()){
					tempDestination = Vec2(tileMap.GetTileSize().x * ((*path).at(pathIndex) % tileMap.GetWidth()),tileMap.GetTileSize().y*((*path).at(pathIndex) / tileMap.GetWidth()));
					lastDistance = associated.box.Center().VecDistance(tempDestination).Magnitude();
					actualTileweight = tileWeightMap.at(tileMap.AtLayer((*path).at(pathIndex),WALKABLE_LAYER).GetTileSetIndex());
					if(associated.box.Center().VecDistance(tempDestination).Magnitude() > 0){
						vecSpeed = associated.box.Center().VecDistance(tempDestination).Normalize().MemberMult(speed / actualTileweight);
					}
					else{vecSpeed = Vec2(0,0);}
				}
			}
			else if(vecSpeed.Magnitude() == 0.0){
				actualTileweight = tileWeightMap.at(tileMap.AtLayer((*path).at(pathIndex),WALKABLE_LAYER).GetTileSetIndex());
				if(associated.box.Center().VecDistance(tempDestination).Magnitude() > 0){
					vecSpeed = associated.box.Center().VecDistance(tempDestination).Normalize().MemberMult(speed / actualTileweight);
				}
				else{vecSpeed = Vec2(0,0);}
			}
			else{
				associated.box.x = (associated.box.Center().x + (vecSpeed.MemberMult(dt)).x - associated.box.w/2);
				associated.box.y = (associated.box.Center().y + (vecSpeed.MemberMult(dt)).y - associated.box.h/2);
				lastDistance = distance;
			}
			if((*path)[path->size() - 1] != destTile){
				shooter->SetActive(true);
			}
			else{
				shooter->SetActive(false);
			}
		}
	}
	else if(actualState == AIState::WALKING_SLOWLY){
		shooter->SetActive(false);
		if(pathIndex != path->size() && path->size() > 0){
			tempDestination = Vec2(tileMap.GetTileSize().x * ((*path).at(pathIndex) % tileMap.GetWidth()),tileMap.GetTileSize().y*((*path).at(pathIndex) / tileMap.GetWidth()));
			float distance = associated.box.Center().VecDistance(tempDestination).Magnitude();
			if((vecSpeed.MemberMult(dt)).Magnitude() >= distance || lastDistance < distance){
				Vec2 movement= tempDestination-Vec2(associated.box.w/2, associated.box.h/2);
				associated.box.x = movement.x;
				associated.box.y = movement.y;
				pathIndex++;
				if(pathIndex != path->size()){
					tempDestination = Vec2(tileMap.GetTileSize().x * ((*path).at(pathIndex) % tileMap.GetWidth()),tileMap.GetTileSize().y*((*path).at(pathIndex) / tileMap.GetWidth()));
					lastDistance = associated.box.Center().VecDistance(tempDestination).Magnitude();
					actualTileweight = tileWeightMap.at(tileMap.AtLayer((*path).at(pathIndex),WALKABLE_LAYER).GetTileSetIndex()) * 3;
					if(associated.box.Center().VecDistance(tempDestination).Magnitude() > 0){
						vecSpeed = associated.box.Center().VecDistance(tempDestination).Normalize().MemberMult(speed /actualTileweight);
					}
					else{vecSpeed = Vec2(0,0);}
				}
			}
			else if(vecSpeed.Magnitude() == 0.0){
				actualTileweight = tileWeightMap.at(tileMap.AtLayer((*path).at(pathIndex),WALKABLE_LAYER).GetTileSetIndex()) * 3;
				if(associated.box.Center().VecDistance(tempDestination).Magnitude() > 0){
					vecSpeed = associated.box.Center().VecDistance(tempDestination).Normalize().MemberMult(speed / actualTileweight);
				}
				else{vecSpeed = Vec2(0,0);}
			}
			else{
				associated.box.x = (associated.box.Center().x + (vecSpeed.MemberMult(dt)).x - associated.box.w/2);
				associated.box.y = (associated.box.Center().y + (vecSpeed.MemberMult(dt)).y - associated.box.h/2);
				lastDistance = distance;
			}
		}
	}
	else if(actualState == AIState::SENDING_BOMB){
		if(tileMap.GetCoordTilePos(associated.box.Center(), false, 0) != destTile){
			shooter->SetActive(true);
			if(getPathTimer.Get() > randomMaxTimer){
				getPathTimer.Restart();
				randomMaxTimer = rand()%3;
				Vec2 originCoord= associated.box.Center();
				path= GameResources::GetPath(((Enemy*)associated.GetComponent(GameComponentType::ENEMY))->GetType(), heuristic, tileMap.GetCoordTilePos(originCoord, false, 0), destTile, "map/WeightData.txt");
				pathIndex = 0;
				if(path->size() > 0){
					tempDestination = Vec2(tileMap.GetTileSize().x * ((*path).at(pathIndex) % tileMap.GetWidth()),tileMap.GetTileSize().y*((*path).at(pathIndex) / tileMap.GetWidth()));
					if(associated.box.Center().VecDistance(tempDestination).Magnitude() > 0){
						vecSpeed = associated.box.Center().VecDistance(tempDestination).Normalize().MemberMult(speed / actualTileweight);
					}
					else{vecSpeed = Vec2(0,0);}
					lastDistance = std::numeric_limits<float>::max();
				}
			}
			getPathTimer.Update(dt);
		}
		else{
			associated.box.x = (associated.box.Center().x + (vecSpeed.MemberMult(dt)).x - associated.box.w/2);
			associated.RequestDelete();
			waveManager.NotifyEnemyGotToHisDestiny();
		}
	}
	else if(actualState == AIState::STUNNED){
		shooter->SetActive(false);
		//Aqui executa animações do efeito estonteante
	}
	if(tileMap.GetCoordTilePos(associated.box, false, 0) == destTile){
		associated.RequestDelete();
		waveManager.NotifyEnemyGotToHisDestiny();
	}
}

void AIQuimic::NotifyTileMapChanged(int tilePosition){
	if(path->end() != std::find(path->begin()+pathIndex, path->end(), tilePosition)){
		Vec2 originCoord= associated.box.Center();
		path= GameResources::GetPath(((Enemy*)associated.GetComponent(GameComponentType::ENEMY))->GetType(), heuristic, tileMap.GetCoordTilePos(originCoord, false, 0), destTile, "map/WeightData.txt");
		pathIndex = 0;
		if(path->size() > 0){
			tempDestination = Vec2(tileMap.GetTileSize().x * ((*path).at(pathIndex) % tileMap.GetWidth()),tileMap.GetTileSize().y*((*path).at(pathIndex) / tileMap.GetWidth()));
			if(associated.box.Center().VecDistance(tempDestination).Magnitude() > 0){
				vecSpeed = associated.box.Center().VecDistance(tempDestination).Normalize().MemberMult(speed / actualTileweight);
			}
			else{vecSpeed = Vec2(0,0);}
			lastDistance = std::numeric_limits<float>::max();
		}
	}
}

bool AIQuimic::Is(int type) const{
	return (type == GameComponentType::AI_QUIMIC);
}
