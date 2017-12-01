#include "StageState.h"
#include "AIArt.h"
#include "AIEngineer.h"
#include "AIMedic.h"
#include "AIQuimic.h"
#include "Camera.h"
#include "Collision.h"
#include "EndStateData.h"
#include "Enemy.h"
#include "Error.h"
#include "Game.h"
#include "GameResources.h"
#include "Obstacle.h"
#include "Vec2.h"
#include "PlayerData.h"
#include "RectTransform.h"
#include "BaseTile.h"

#define INCLUDE_SDL
#define INCLUDE_SDL_IMAGE
#include "SDL_include.h"

// Esse valores calculam o offset em relação ao canto superior esquedo da imagem daquilo que será renderizado
#define STATE_RENDER_X 0
#define STATE_RENDER_Y 0
#define FACE_LINEAR_SIZE 30
#define TIME_BETWEEN_SPAWNS (8.)
#define TOWER_LINEAR_SIZE 120
#define STAGE_STATE_DELTA_VOLUME (1) //11*11 = 121 ~128
#define CAM_START_X 300
#define CAM_START_Y 300
#define CAM_START_ZOOM -1.75
#define MAX_TIME_LIGHTINING_RISE 0.1 // s
#define MAX_TIME_LIGHTINING 0.3 // s
#define MAX_TIME_LIGHTINING_FADE 2 // s
#define LIGHTINING_MIN_INTERVAL 30 // s
#define LIGHTINING_MAX_INTERVAL 60 // s
#define TREE_1_TILESET_INDEX 70
#define TREE_2_TILESET_INDEX 71
#define TREE_3_TILESET_INDEX 72
#define POLE_TILESET_INDEX 73
#define BENCH_TILESET_INDEX 76

#define TOWER_INFO_TXT_COLOR	{199,159,224,255} // Purple-ish white
#define MONEY_TXT_COLOR			{186,179, 62,255}

StageState::StageState(void)
		: State()
		, inputManager(INPUT_MANAGER)
		, music("audio/trilha_sonora/loop_3_atualizado.ogg")
		, isLightning(false)
		, isThundering(false)
		, lightningTimer()
		, lightningColor(255, 255, 255, 0)
		, nightSound("audio/Ambiente/Barulho_noite.wav")
		, thunderSound("audio/Ambiente/Trovao.wav")
		, towerMenuSounds("audio/Acoes/Dinheiro1.wav")
		, frameRateCounter(0)
        , menuMove("audio/Interface/Click1.wav"){

	Resources::ChangeMusicVolume(0);
	Resources::ChangeSoundVolume(0);

	REPORT_I_WAS_HERE;
	music.Play(0);
	Camera::pos = Vec2(CAM_START_X, CAM_START_Y);
	Camera::ForceLogZoom(CAM_START_ZOOM);

    waveManagerGO = new GameObject();
    tileSet = new TileSet("map/tileSetDescriptor.txt",*waveManagerGO);
    waveManagerGO->AddComponent(tileSet);

    tileMap = new TileMap<Tile>(*waveManagerGO,"map/tileMap.txt",tileSet);
	tileMap->ObserveMapChanges(this);
    waveManagerGO->AddComponent(tileMap);
    GameResources::SetTileMap(&tileMap);

    waveManager= new WaveManager(tileMap, "assets/wave&enemyData.txt");

	waveManagerGO->AddComponent(waveManager);
	AddObject(waveManagerGO);
	
	tileMap.ObserveMapChanges(this);
	lightningInterval = rand() % (LIGHTINING_MAX_INTERVAL - LIGHTINING_MIN_INTERVAL) + LIGHTINING_MIN_INTERVAL;
	REPORT_DEBUG(" Proximo relampago sera em " << lightningInterval << " segundos.");
	InitializeObstacles();

	nightSound.Play(0);
	
	SetupUI();

	SetUILife(PLAYER_DATA_INSTANCE.GetLifes()/TOTAL_LIFES);
	if(waveManager->GetWaveTotalEnemies() > 0) {
		SetUIWaveProgress(waveManager->GetEnemiesLeft()/waveManager->GetWaveTotalEnemies());
	} else {
		SetUIWaveProgress(0.);
	}
	SetUIMoney(PLAYER_DATA_INSTANCE.GetGold());

}

void StageState::SetupUI(){
    Rect windowBox = Game::GetInstance().GetWindowDimensions();
    // Side Menu
    menuIsShowing = false;

    //HUDCanvas
    RectTransform* HUDCanvasRect = new RectTransform(HUDcanvasGO);
    HUDcanvasGO->AddComponent(HUDCanvasRect);
    AddObject(HUDcanvasGO);
    //menuBG
    RectTransform* menuBGRect = new RectTransform(menuBgGO,HUDcanvasGO);
    menuBGRect->SetBehaviorType(RectTransform::BehaviorType::FIT);
    menuBGRect->SetAnchors(Vec2(1., 0.5),Vec2(1., 0.5));
    Sprite* menuBGSP = new Sprite("img/UI/HUD/menu.png",menuBgGO);
    menuBGRect->SetOffsets(-10., (float)(menuBGSP->GetHeight()/2.),(float)(menuBGSP->GetWidth()-(float)10.), (float)(menuBGSP->GetHeight()/2.));
    menuBgGO->AddComponent(menuBGRect);
    menuBgGO->AddComponent(menuBGSP);
    AddObject(menuBgGO);
    //openMenuBtn
    RectTransform* openMenuBtnRect = new RectTransform(openMenuBtnGO,menuBgGO);
    openMenuBtnGO->AddComponent(openMenuBtnRect);
    openMenuBtn = Button(openMenuBtnGO);
    openMenuBtnRect->SetAnchors(Vec2(0., 0.),Vec2(0., 0.));
    //rect->SetOffsets((float)-(openMenuBtn.GetStateSprite(UIbutton::State::ENABLED).GetWidth()), (float)10.,0., openMenuBtn.GetStateSprite(UIbutton::State::ENABLED).GetHeight()+(float)10. );
    openMenuBtn.SetClickCallback(this, [] (void* ptr) {
                                                StageState* it = static_cast<StageState*>(ptr);
                                                it->RemoveComponent(ComponentType::SPRITE);
                                                Sprite* sp = new Sprite("img/UI/HUD/openmenu-clicked.png",openMenuBtn);
                                                it->AddComponent(sp);
                                                it->ToggleMenu();
                                            } );
    openMenuBtn.SetCallback(UIbutton::State::HIGHLIGHTED, this, [] (void* ptr) {
                                                                    StageState* it = static_cast<StageState*>(ptr);
                                                                    it->RemoveComponent(ComponentType::SPRITE);
                                                                    Sprite* sp = new Sprite("img/UI/HUD/openmenu.png",openMenuBtn);
                                                                    it->AddComponent(sp);
                                                                } );
    openMenuBtn.SetCallback(UIbutton::State::ENABLED, this, [] (void* ptr) {
                                                                    StageState* it = static_cast<StageState*>(ptr);
                                                                    it->RemoveComponent(ComponentType::SPRITE);
                                                                    Sprite* sp = new Sprite("img/UI/HUD/openmenu.png",openMenuBtn);
                                                                    it->AddComponent(sp);
                                                                } );
    AddObject(openMenuBtnGO);
    //towerInfoGroup
    //Faltamdo algo aqui?
    RectTransform* towerInfoGrpupRect = new RectTransform(towerInfoGroupGO,menuBgGO);
    towerInfoGrpupRect->SetAnchors( Vec2(0.165, 0.05),Vec2(0.86, 0.425);
    towerInfoGrpupRect->SetOffsets(5., 5.,-5., -5.);
    towersInfoGroup = Grouper(towerInfoGroupGO);
    towersBtnGroupGO->AddComponent(towersInfoGroup);
    towerInfoGroupGO->AddComponent(towerInfoGrpupRect);
    AddObject(towerInfoGroupGO);
    //towerName
    RectTransform* towerNameRect = new RectTransform(towerNameGO,towerInfoGroupGO);
    Text* towerNameText = new Text(towerNameGO);
    towerNameText->SetText(TOWERNAME_DEFAULT_TEXT);
    towerNameText->SetColor(TOWER_INFO_TXT_COLOR);
    towerNameText->SetFont("font/SHPinscher-Regular.otf");
    towerNameText->SetFontSize(95);
    towerNameGO->AddComponent(towerNameRect);
    towerNameGO->AddComponent(towerNameText);
    AddObject(towerNameGO);
    //towerCost
    RectTransform* TowerCostRect = new RectTransform(towerCostGO,towerInfoGroupGO);
    Text* towerCostText = new Text(towerCostGO);
    towerCostText->SetText(TOWERCOST_DEFAULT_TEXT);
    towerCostText->SetColor(TOWER_INFO_TXT_COLOR);
    towerCostText->SetFont("font/SHPinscher-Regular.otf");
    towerCostText->SetFontSize(95);
    towerCostGO->AddComponent(TowerCostRect);
    towerCostGO->AddComponent(towerCostText);
    AddObject(towerCostGO);
    //towerDamage
    RectTransform* towerDamageRect = new RectTransform(towerDamageGO,towerInfoGroupGO);
    Text* towerDamageText = new Text(towerDamageGO);
    towerDamageText->SetText(TOWERDAMAGE_DEFAULT_TEXT);
    towerDamageText->SetColor(TOWER_INFO_TXT_COLOR);
    towerDamageText->SetFont("font/SHPinscher-Regular.otf");
    towerDamageText->SetFontSize(95);
    towerDamageGO->AddComponent(towerDamageRect);
    towerDamageGO->AddComponent(towerDamageText);
    AddObject(towerDamageGO);
    //towerDamageType
    RectTransform* towerDamageTypeRect = new RectTransform(towerDamageTypeGO,towerInfoGroupGO);
    Text* towerDamageTypeText = new Text(towerDamageTypeGO);
    towerDamageTypeText->SetText(TOWERDAMGETYPE_DEFAULT_TEXT);
    towerDamageTypeText->SetColor(TOWER_INFO_TXT_COLOR);
    towerDamageTypeText->SetFont("font/SHPinscher-Regular.otf");
    towerDamageTypeText->SetFontSize(95);
    towerDamageTypeGO->AddComponent(towerDamageTypeRect);
    towerDamageTypeGO->AddComponent(towerDamageTypeText);
    AddObject(towerDamageTypeGO);

    towersInfoGroup.groupedElements.push_back(towerNameGO);
    towersInfoGroup.groupedElements.push_back(towerCostGO);
    towersInfoGroup.groupedElements.push_back(towerDamageGO);
    towersInfoGroup.groupedElements.push_back(towerDamageTypeGO);

    RectTransform* towersBtnGroupRect = new RectTransform(towersBtnGroupGO,nullptr);
    towersBtnGroupRect->SetAnchors(Vec2(0., 0.485),Vec2(1., 1.));
    towersBtnGroupRect->SetOffsets(32., 0.,-27., -30.);
    //towersBtnGroup.padding = Vec2(10., 10.);
    towersBtnGroup = Grouper(towersBtnGroupGO);
    towersBtnGroupGO->AddComponent(towersBtnGroupRect);
    towersBtnGroupGO->AddComponent(towersBtnGroup);
    towersBtnGroup.MakeGridGroup(UIgridGroup::ConstraintType::FIXED_N_COLS, 2, UIgridGroup::BehaviorOnLess::NORMAL);
    AddObject(towersBtnGroupGO);

    //towerBtn1
    RectTransform* towerBtn1Rect = new RectTransform(towerBtn1GO,towersBtnGroupGO);
    towerBtn1Rect->SetCenterPin(vec2(0.5, 0.));
    towerBtn1 = Button(towerBtn1GO);
    towerBtn1GO->AddComponent(towerBtn1Rect);
    towerBtn1GO->AddComponent(towerBtn1);
    AddObject(towerBtn1GO);

    towerBtn1.SetCallback(UIbutton::State::HIGHLIGHTED, this, [] (void* ptr) {
                                                                    stct = static_cast<btnCallbackArgAux*>(ptr);
                                                                    stct->ST->SetTowerInfoData("Fumaca", "$30", "Slow", "Area");
                                                                    stct->GO->RemoveComponent(ComponentType::SPRITE);
                                                                    Sprite* sp = new Sprite("img/UI/HUD/botaotorre.png",towerBtn1);
                                                                    stct->GO->AddComponent(sp);
																} );
	towerBtn1.SetCallback(UIbutton::State::ENABLED, this, [] (void* ptr) {
																	StageState* it = static_cast<StageState*>(ptr);
																	it->SetTowerInfoData();
																} );
	towerBtn1.SetClickCallback(this, [] (void* ptr) {
											StageState* it = static_cast<StageState*>(ptr);
											it->CreateTower(Tower::TowerType::SMOKE);
										} );

    //towerBtn2
    RectTransform* towerBtn2Rect = new RectTransform(towerBtn2,towersBtnGroupGO);
    rect->SetCenterPin(vec2(0.5, 0.));
    towerBtn2 = Button(towerBtn2GO);
    towerBtn2GO->AddComponent(towerBtn2Rect);
    towerBtn2GO->AddComponent(towerBtn2);
    AddObject(towerBtn2GO);

    towerBtn2.SetCallback(UIbutton::State::HIGHLIGHTED, this, [] (void* ptr) {
                                                                    stct = static_cast<btnCallbackArgAux*>(ptr)
                                                                    stct->ST->SetTowerInfoData("Tentaculos", "$30", "1 tiro/2s", "Anti-Bomba");
                                                                    stct->GO->RemoveComponent(ComponentType::SPRITE);
                                                                    Sprite* sp = new Sprite("img/UI/HUD/botaoantibomba.png",towerBtn2));
                                                                    stct->GO->AddComponent(sp);
																} );
	towerBtn2.SetCallback(UIbutton::State::ENABLED, this, [] (void* ptr) {
																	StageState* it = static_cast<StageState*>(ptr);
																	it->SetTowerInfoData();
																} );
	towerBtn2.SetClickCallback(this, [] (void* ptr) {
											StageState* it = static_cast<StageState*>(ptr);
											it->CreateTower(Tower::TowerType::ANTIBOMB);
										} );
    //towerBtn3
    RectTransform* towerBtn3Rect = new RectTransform(towerBtn3,towersBtnGroupGO);
    towerBtn3Rect->SetCenterPin(vec2(0.5, 0.));
    towerBtn3 = Button(towerBtn3GO);
    towerBtn3GO->AddComponent(towerBtn3Rect);
    towerBtn3GO->AddComponent(towerBtn3);
    AddObject(towerBtn3GO);

	towerBtn3.SetCallback(UIbutton::State::HIGHLIGHTED, this, [] (void* ptr) {
																	StageState* it = static_cast<StageState*>(ptr);
																	it->SetTowerInfoData("Bobina", "$30", "1 tiro/2s", "Dano");
																} );
	towerBtn3.SetCallback(UIbutton::State::ENABLED, this, [] (void* ptr) {
																	StageState* it = static_cast<StageState*>(ptr);
																	it->SetTowerInfoData();
																} );
	towerBtn3.SetClickCallback(this, [] (void* ptr) {
											StageState* it = static_cast<StageState*>(ptr);
											it->CreateTower(Tower::TowerType::SHOCK);
										} );
    //towerBtn4
    RectTransform* towerBtn4Rect = new RectTransform(towerBtn4,nullptr);
    towerBtn4GO->AddComponent(towerBtn4Rect);
    towerBtn4 = Button(towerBtn4GO);
    towerBtn4Rect->SetCenterPin(vec2(0.5, 0.));
    towerBtn4GO->AddComponent(towerBtn4Rect);
    towerBtn4GO->AddComponent(towerBtn4);
    AddObject(towerBtn4GO);
    
	towerBtn4.SetCallback(UIbutton::State::HIGHLIGHTED, this, [] (void* ptr) {
																	StageState* it = static_cast<StageState*>(ptr);
																	it->SetTowerInfoData("Monolito", "$30", "Stun", "Area");
																} );
	towerBtn4.SetCallback(UIbutton::State::ENABLED, this, [] (void* ptr) {
																	StageState* it = static_cast<StageState*>(ptr);
																	it->SetTowerInfoData();
																} );
	towerBtn4.SetClickCallback(this, [] (void* ptr) {
											StageState* it = static_cast<StageState*>(ptr);
											it->CreateTower(Tower::TowerType::STUN);
										} );

    towersBtnGroup.groupedElements.push_back(towerBtn1GO);
    towersBtnGroup.groupedElements.push_back(towerBtn2GO);
    towersBtnGroup.groupedElements.push_back(towerBtn3GO);
    towersBtnGroup.groupedElements.push_back(towerBtn4GO);

    //health
    RectTransform* healthRect = new RectTransform(healthGO,HUDcanvasGO);
    healthGO->AddComponent(healthRect);
    //healthIcon
    RectTransform* healthIconRect = new RectTransform(healthIconGO,healthGO);
    healthIconRect->SetBehaviorType(RectTransform::BehaviorType::FILL);
    Sprite* healthIconSP = new Sprite("img/UI/HUD/CoraçãoHUD_spritesheet.png",healthIconGO,false, 1./4, 8);
    healthIconRect->SetCenterPin(Vec2(.725, 0.5));
    healthIconRect->SetAnchors(Vec2(0., 0.1),Vec2(0., 0.9));
    healthIconGO->AddComponent(healthIconRect);
    healthIconGO->AddComponent(healthIconSP);
    AddObject(healthIconGO);
    //health
    healthGO.SetAnchors(Vec2((float)(30.+ healthIconSP->GetWidth())/(2*windowBox.x), (float)10./windowBox.y),Vec2((float)300./windowBox.x, (float)35./windowBox.y));
    healthGO.SetOffsets((float)(30.+healthIconSP->GetWidth())/2, 0.,120., 25. );
    AddObject(healthGO);
    //healthbarBg
    RectTransform* healthbarBgRect = new RectTransform(healthbarBgGO,healthGO);
    Sprite* healthbarBgSP = new Sprite("img/UI/HUD/hudvida.png",healthbarBgGO);
    rect->SetAnchors(Vec2(0., 0.3),Vec2(1., 0.7));
    //healthbarBg.GetSprite().colorMultiplier = {0, 0, 0, 255};
    healthbarBgGO->AddComponent(healthbarBgRect);
    healthbarBgGO->AddComponent(healthbarBgSP);
    AddObject(healthbarBgGO);
    //healthbarBar
    RectTransform* healthbarBarRect = new RectTransform(healthbarBarGO,healthGO);
    Sprite* healthbarBarSP = new Sprite("img/UI/HUD/hudvida.png",healthbarBarGO);
    Rect healthBox = rect->ComputeBox(healthbarBarRect->ComputeBoundingbox(Rect(0., 0., windowBox.x, windowBox.y)));
    healthbarBarRect->SetAnchors(Vec2((float)0., (float)0.3+2/healthBox.h),Vec2((float)1., (float)0.7-2/healthBox.h));
    healthbarBarRect->SetOffsets((float)2., 0.,(float)-2., 0.);
    //healthbarBar.GetSprite().colorMultiplier = {180, 225, 149, 255};
    healthbarBarGO->AddComponent(healthbarBarRect);
    healthbarBarGO->AddComponent(healthbarBarSP);
    AddObject(healthbarBarGO);
    //wave
    RectTransform* waveRect = new RectTransform(waveGO,HUDcanvasGO);
    waveRect->SetAnchors(Vec2((float)(30.+healthIconSP->GetWidth())/(2*windowBox.x), (float)35./windowBox.y),
                     Vec2((float)150./windowBox.x, (float)60./windowBox.y));
    waveGO->AddComponent(waveRect);
    AddObject(waveGO);
    //waveIcon
    RectTransform* waveIconRect = new RectTransform(waveIconGO,waveGO);
    waveIconRect->SetBehaviorType(RectTransform::BehaviorType::FILL);
    Sprite* waveIconSP = new Sprite("img/UI/HUD/inimigoHUD_spritesheet.png",waveIconGO,false, 1./4, 5);
    waveRect->SetOffsets((float)(30.+waveIconSP->GetWidth())/2, 25.,120., 50.);
    waveIconRect->SetCenterPin(Vec2(.725, 0.5));
    waveIconRect->SetAnchors(0., 0.1,0., 0.9);
    waveIconGO->AddComponent(waveIconRect);
    waveIconGO->AddComponent(waveIconSP);
    AddObject(waveIconGO);
    //wavebarBg
    RectTransform* wabebarBGRect = new RectTransform(wavebarBgGO,waveGO);
    Sprite* wavebarBgSP = new Sprite("img/UI/HUD/hudvida.png",wavebarBgGO);
    wabebarBGRect->SetAnchors(Vec2(0., 0.3),Vec2(1., 0.7));
    //wavebarBg.GetSprite().colorMultiplier = {0, 0, 0, 255};
    wavebarBgGO->AddComponent(wabebarBGRect);
    wavebarBgGO->AddComponent(wavebarBgSP);
    AddObject(wavebarBgGO);
    //wavebarBar
    RectTransform* waveBarRect = new RectTransform(wavebarBarGO,waveGO);
    Sprite* wavebarBarSP = new Sprite("img/UI/HUD/hudvida.png",wavebarBarGO);
    Rect waveBox = waveBarRect->ComputeBox(waveBarRect->ComputeBoundingbox(Rect(0., 0., windowBox.x, windowBox.y)));
    waveBarRect->SetAnchors(Vec2((float)0., (float)0.3+2/waveBox.h),Vec2((float)1., (float)0.7-2/waveBox.h));
    wavebarBarRect.SetOffsets((float)2., 0.,(float)-2., 0.);
    //wavebarBar.GetSprite().colorMultiplier = {154, 148, 104, 255};
    wavebarBarGO->AddComponent(wavebarBarRect);
    wavebarBarGO->AddComponent(wavebarBarSP);
    AddObject(wavebarBarGO);
    //money
    RectTransform* moneyRect = new RectTransform(moneyGO,HUDcanvasGO);
    moneyGO->AddComponent(moneyRect);
    AddObject(moneyGO);
    //moneyIcon
    RectTransform* moneyIconRect = new RectTransform(moneyIconGO,moneyGO);
    moneyIconRect->SetBehaviorType(RectTransform::BehaviorType::FILL);
    Sprite* moneyIconSP = new Sprite("img/UI/HUD/spritesheetmoeda_HUD.png",moneyIconGO,false, 1./4, 4);
    moneyRect->SetAnchors(Vec2((float)(30.+moneyIconSP->GetWidth())/(2*windowBox.x), (float)60./windowBox.y),
                         Vec2((float)150./windowBox.x, (float)85./windowBox.y));
    moneyRect->SetOffsets((float)(7.5+moneyIconSP->GetWidth()/2.), 60.,120., 70.);
    moneyIconRect->SetCenterPin(Vec2(1., 0.5));
    moneyIconRect->SetAnchors(Vec2(0., 0.),Vec2(0., 1.));
    moneyIconGO->AddComponent(moneyIconRect);
    moneyIconGO->AddComponent(moneyIconSP);
    AddObject(moneyIconGO);
    //moneyText
    RectTransform* moneyTextRect = new RectTransform(moneyTextGO,moneyGO);
    Text* moneyTextText = new Text(moneyTextGO);
    moneyTextText->SetText("+Inf");
    moneyTextText->SetColor(MONEY_TXT_COLOR);
    moneyTextText->SetFont("font/SHPinscher-Regular.otf");
    moneyTextText->SetFontSize(95);
    moneyTextRect->SetAnchors(Vec2(0., 0.),Vec2(1., 1.));
    moneyTextRect->SetOffsets(12.5, 0.,0., 0.);
    moneyTextRect->SetCenterPin(Vec2(0., .5);
    moneyTextGO->AddComponent(moneyTextRect);
    moneyTextGO->AddComponent(moneyTextText);
    AddObject(moneyTextGO);

}

StageState::~StageState(void) {
	std::cout<<WHERE<<"\tGameResources path hit rate: " << GameResources::GetPathHitRate()<<END_LINE;
	objectArray.clear();
	obstacleArray.clear();
	tileMap.RemoveObserver(this);
	GameResources::Clear();
	TEMP_REPORT_I_WAS_HERE;
	nightSound.Stop();
}

void StageState::Update(float dt){
	REPORT_I_WAS_HERE;
	if(ActionManager::EscapeAction()) {
		popRequested = true;
	}
	if(inputManager.QuitRequested()) {
		quitRequested = true;
	}

	//fazendo o prórpio loop de atualização ao invés do UpdateArray pois estamos fazendo checagens adicionais
    /*for(unsigned int cont = 0; cont < objectArray.size(); cont++) {
		objectArray.at(cont)->Update(dt);
		if(objectArray.at(cont)->IsDead()) {
            Como isso dev e ser feito agora?
              int64_t objOnTileMap= tileMap.Have(objectArray[cont].get());
			if(0 <= objOnTileMap){
				tileMap.RemoveGO(objOnTileMap);
			}

			objectArray.erase(objectArray.begin()+cont);
			cont--;
		}
    }*/

	if(!objectArray.empty()){
		for(uint count1 = 0; count1 < objectArray.size()-1; count1++) {
			for(uint count2 = count1+1; count2 < objectArray.size(); count2++) {
				if(Collision::IsColliding(objectArray[count1]->box, objectArray[count2]->box, objectArray[count1]->rotation, objectArray[count2]->rotation) ) {
					objectArray[count1]->NotifyCollision(*objectArray[count2]);
					objectArray[count2]->NotifyCollision(*objectArray[count1]);
				}
			}
		}
	}

	Camera::Update(dt);

	// Game Over Conditions
	if(waveManager->GetLifesLeft() == 0){
		popRequested = true;
		Game::GetInstance().Push(new EndState(EndStateData(false)));
	}else if(waveManager->Victory()){
		popRequested = true;
		Game::GetInstance().Push(new EndState(EndStateData(true)));
	}


	if(INPUT_MANAGER.KeyPress('r')) {
		popRequested = true;
		Game::GetInstance().Push(new EndState(EndStateData(true)));
	}
	if(INPUT_MANAGER.KeyPress('t')) {
		popRequested = true;
		Game::GetInstance().Push(new EndState(EndStateData(false)));
	}
	if(INPUT_MANAGER.KeyPress('q')) {
		Vec2 mousePos = Camera::ScreenToWorld(INPUT_MANAGER.GetMousePos());
		std::cout << WHERE << "O mouse está no tile " << tileMap.GetCoordTilePos(mousePos, true, 0) << ", cada layer tem " << tileMap.GetHeight()*tileMap.GetHeight() << " tiles." << END_LINE;

	}
	if(INPUT_MANAGER.MousePress(RIGHT_MOUSE_BUTTON)) {
		Vec2 mousePos = Camera::ScreenToWorld(INPUT_MANAGER.GetMousePos());
		int position = tileMap.GetCoordTilePos(mousePos, false, COLLISION_LAYER);
		GameObject *go = tileMap.GetGO(position);
		if(nullptr != go) {
			go->AddComponent(new DragAndDrop(tileMap, mousePos, *go));
			REPORT_I_WAS_HERE;
		}
	}
	if(INPUT_MANAGER.KeyPress('=')) {
		Game &game = Game::GetInstance();
		game.SetMaxFramerate(game.GetMaxFramerate()+5);
	}
	if(INPUT_MANAGER.KeyPress('-')) {
		Game &game = Game::GetInstance();
		game.SetMaxFramerate( ( (int64_t)game.GetMaxFramerate() )-5);
	}
	tileMap.ShowCollisionInfo(INPUT_MANAGER.IsKeyDown('g'));
	if(INPUT_MANAGER.IsKeyDown('[')){
		Resources::ChangeMusicVolume(-STAGE_STATE_DELTA_VOLUME);
	}
	if(INPUT_MANAGER.IsKeyDown(']')){
		Resources::ChangeMusicVolume(STAGE_STATE_DELTA_VOLUME);
	}
	if(INPUT_MANAGER.IsKeyDown(',')){
		Resources::ChangeSoundVolume(-STAGE_STATE_DELTA_VOLUME);
	}
	if(INPUT_MANAGER.IsKeyDown('.')){
		Resources::ChangeSoundVolume(STAGE_STATE_DELTA_VOLUME);
	}
	if(isLightning){
		if(!isThundering){
			thunderSound.Play(1);
			isThundering = true;
		}
		ShowLightning(dt);
	}
	else{
		isLightning = false;
		isThundering = false;
		lightningTimer.Update(dt);
		if(lightningTimer.Get() > lightningInterval){
			isLightning = true;
			lightningTimer.Restart();
			lightningInterval = rand() % (LIGHTINING_MAX_INTERVAL - LIGHTINING_MIN_INTERVAL) + LIGHTINING_MIN_INTERVAL;
			REPORT_DEBUG(" Proximo relampago sera em " << lightningInterval << " segundos.");
		}
	}

	SetUILife(PLAYER_DATA_INSTANCE.GetLifes()/TOTAL_LIFES);
	if(waveManager->GetWaveTotalEnemies() > 0) {
		SetUIWaveProgress(waveManager->GetEnemiesLeft()/waveManager->GetWaveTotalEnemies());
	} else {
		SetUIWaveProgress(0.);
	}
	SetUIMoney(PLAYER_DATA_INSTANCE.GetGold());

	REPORT_DEBUG("\tFrame rate: " << Game::GetInstance().GetCurrentFramerate() << "/" << Game::GetInstance().GetMaxFramerate());
	//depois isolar essa lógica num componente.
	frameRateTimer.Update(dt);
	frameRateCounter++;
	if(1. <= frameRateTimer.Get()){
		std::cout<<WHERE<<"\t Frame Rate: " << (float)frameRateCounter/frameRateTimer.Get()<<"/"<<Game::GetInstance().GetMaxFramerate()<< END_LINE;
		frameRateCounter=0;
		frameRateTimer.Restart();
	}
}

void StageState::Render(void) const {
	REPORT_I_WAS_HERE;
	bool highlighted = true;
	for(unsigned int cont=0; cont < objectArray.size(); cont++) {
		if(INPUT_MANAGER.GetMousePos().IsInRect(objectArray.at(cont)->GetWorldRenderedRect())){
			highlighted = false;
			break;
		}
	}
    //Como registro para chamadas a serem feitas em outras funções de tilemap
    //tileMap.Render(Vec2(0,0), false, highlighted ? Camera::ScreenToWorld(INPUT_MANAGER.GetMousePos()) : Vec2(-1, -1));
	REPORT_I_WAS_HERE;
	State::RenderArray();
	if(isLightning){
		SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), lightningColor.r, lightningColor.g, lightningColor.b, lightningColor.a);
		SDL_SetRenderDrawBlendMode(Game::GetInstance().GetRenderer(), SDL_BLENDMODE_BLEND);
		SDL_RenderFillRect(Game::GetInstance().GetRenderer(), NULL);
	}

	RenderUI();
}
//Comentado para futuro esclaricimento sobre compatibilidade retroativa
/*void StageState::RenderUI(void) const {
	// Se tivesse como ser estatico para a funcao mas uma para cada instancia, melhor ainda...
	// Mas como StageState nao teram instancias multiplas simultaneas, serve...
	static bool menuIsShowing = this->menuIsShowing;

	if(menuIsShowing) {
		menuBg.Render();
		// towerInfoGroup.Render(true);
		towerName.Render();
		towerCost.Render();
		towerDamage.Render();
		towerDamageType.Render();
		// towersBtnGroup.Render(true);
		towerBtn1.Render();
		towerBtn2.Render();
		towerBtn3.Render();
		towerBtn4.Render();
	}
	openMenuBtn.Render();

	// health.Render(true);
	healthbarBg.Render();
	healthbarBar.Render();
	healthIcon.Render();

	// wave.Render(true);
	wavebarBg.Render();
	wavebarBar.Render();
	waveIcon.Render();

	// money.Render(true);
	moneyIcon.Render();
	moneyText.Render();

	menuIsShowing = this->menuIsShowing;
}*/

void StageState::Pause(void) {
	nightSound.Stop();
	thunderSound.Stop();
}

void StageState::Resume(void) {
	nightSound.Play(0);
	Camera::pos = Vec2(CAM_START_X, CAM_START_Y);
	Camera::ForceLogZoom(CAM_START_ZOOM);
}

void StageState::ShowLightning(float dt){
	isLightning = true;
	lightningTimer.Update(dt);
	float newAlpha;
	if(lightningTimer.Get() < MAX_TIME_LIGHTINING_RISE){
		newAlpha = lightningColor.a + 256 * dt / MAX_TIME_LIGHTINING_RISE;
	}
	else if(lightningTimer.Get() >= MAX_TIME_LIGHTINING_RISE && lightningTimer.Get() < MAX_TIME_LIGHTINING_RISE+MAX_TIME_LIGHTINING){
		newAlpha = 255;
	}
	else if(lightningTimer.Get() >= MAX_TIME_LIGHTINING_RISE+MAX_TIME_LIGHTINING && lightningTimer.Get() < MAX_TIME_LIGHTINING_RISE+MAX_TIME_LIGHTINING+MAX_TIME_LIGHTINING_FADE){
		float fullTime = (MAX_TIME_LIGHTINING_RISE+MAX_TIME_LIGHTINING+MAX_TIME_LIGHTINING_FADE) - (MAX_TIME_LIGHTINING_RISE+MAX_TIME_LIGHTINING);
		newAlpha = lightningColor.a - 256* (dt / fullTime);
	}
	else{
		newAlpha = 0;
		isLightning = false;
		lightningTimer.Restart();
	}
	lightningColor.a = newAlpha > 255 ? 255 : newAlpha < 0 ? 0 : newAlpha;
}

void StageState::NotifyTileMapChanged(int tilePosition){
	GameResources::NotifyTileMapChanged(tilePosition);
}

void StageState::ToggleMenu(void){
	menuIsShowing = !menuIsShowing;
	menuMove.Play(1);

    Rect menuBgOffsets = menuBgGO->GetComponent(ComponentType::RECT_TRANSFORM).GetOffsets();
    Vec2 menuBgDim = {(float)menuBgGO->GetComponent(ComponentType::SPRITE).GetWidth(), (float)menuBgGO->GetComponent(ComponentType::SPRITE).GetHeight()};
    if(menuIsShowing){
        menuBgGO->GetComponent(ComponentType::RECT_TRANSFORM).SetOffsets( {menuBgOffsets.x-menuBgDim.x, menuBgOffsets.y},
						   {menuBgOffsets.w-menuBgDim.x, menuBgOffsets.h});
	} else {
        menuBgGO->GetComponent(ComponentType::RECT_TRANSFORM).SetOffsets( {menuBgOffsets.x+menuBgDim.x, menuBgOffsets.y},
						   {menuBgOffsets.w+menuBgDim.x, menuBgOffsets.h});
	}
}

void StageState::SetTowerInfoData(string name, string cost, string damage, string damageType) {
    towerNameGO->GetComponent(ComponentType::TEXT).SetText(name);
    towerCostGO->GetComponent(ComponentType::TEXT).SetText(cost);
    towerDamageGO->GetComponent(ComponentType::TEXT).SetText(damage);
    towerDamageTypeGO->GetComponent(ComponentType::TEXT).SetText(damageType);
}

void StageState::CreateTower(Tower::TowerType towerType) {
	ToggleMenu();

	if( PLAYER_DATA_INSTANCE.GetGold() >= 30 ) {
		Vec2 mousePos = Camera::ScreenToWorld(INPUT_MANAGER.GetMousePos())-Vec2(TOWER_LINEAR_SIZE/2, TOWER_LINEAR_SIZE/2);
		Tower *newTower = new Tower(towerType, mousePos, Vec2(TOWER_LINEAR_SIZE, TOWER_LINEAR_SIZE), TOWER_BASE_HP);
		newTower->AddComponent(new DragAndDrop(tileMap, mousePos, *newTower, false, false));
		AddObject(newTower);
		PLAYER_DATA_INSTANCE.GoldUpdate(-30, false);
		towerMenuSounds.Play(1);
	} else {
		printf("Not enough gold! Need more gold!\n");
	}
}

void StageState::SetUILife(float lifePercent) {
	lifePercent = (lifePercent < 0) ? 0 : ((lifePercent > 1) ? 1 : lifePercent);
    Rect oldAnchor = healthbarBarGO->GetComponent(ComponentType::RECT_TRANSFORM).GetAnchors();
    healthbarBarGO->GetComponent(ComponentType::RECT_TRANSFORM).SetAnchors( {oldAnchor.x, oldAnchor.y},
							 {lifePercent, oldAnchor.h} );
}

void StageState::SetUIWaveProgress(float waveProgressPercent) {
	waveProgressPercent = (waveProgressPercent < 0) ? 0 : ((waveProgressPercent > 1) ? 1 : waveProgressPercent);
    Rect oldAnchor = wavebarBarGO->GetComponent(ComponentType::RECT_TRANSFORM).GetAnchors();
    wavebarBarGO->GetComponent(ComponentType::RECT_TRANSFORM).SetAnchors( {oldAnchor.x, oldAnchor.y},
							 {waveProgressPercent, oldAnchor.h} );
}

void StageState::SetUIMoney(int coins) {
    moneyTextGO->GetComponent(ComponentType::TEXT).SetText(std::to_string(coins));
}

vector<vector<int>>* StageState::GetTileGroups(int tileType) const{
    TileMap tileMap = (TileMap)waveManagerGO->GetComponent(ComponentType::TILEMAP);
    vector<vector<int>> *tilePoints = new vector<vector<int>>();
    vector<int> foundTilePoints;
    REPORT_I_WAS_HERE;

    for(uint i = 0; i < tileMap.GetWidth() * tileMap.GetHeight(); i++) {
        if(tileType == tileMap.AtLayer(i,COLLISION_LAYER)){
            foundTilePoints.push_back(i);
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
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]+tileMap.GetWidth()) != vec.end() )// posição em cima
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]-tileMap.GetWidth()) != vec.end() )//posição em baixo
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]-tileMap.GetWidth()-1) != vec.end() )//diagonal supeior esquerda
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]-tileMap.GetWidth()+1) != vec.end() )//diagonal supeior direita
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]+tileMap.GetWidth()-1) != vec.end() )//diagonal inferior esquerda
                    || (std::find(vec.begin(), vec.end(), foundTilePoints[0]+tileMap.GetWidth()+1) != vec.end() )//diagonal inferior direita
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
}

void StageState::InitializeObstacles(void){

    //70 a 72 3 tipos de arvores
    //73 poste
    //76 banco

	int index;
    TileMap& tileMap = (TileMap&)waveManagerGO->GetComponent(ComponentType::TILEMAP);
    int mapWidth= tileMap.GetWidth();
    Vec2 tileSize= tileMap.GetTileSize();
	int tileWidth= tileSize.x;
	int tileHeight= tileSize.y;
	std::array<vector<vector<int>>*, 3> treeTiles;
    treeTiles[0] = GetTileGroups(TREE_1_TILESET_INDEX);
    treeTiles[1] = GetTileGroups(TREE_2_TILESET_INDEX);
    treeTiles[2] = GetTileGroups(TREE_3_TILESET_INDEX);
    vector<vector<int>>* poleTiles = GetTileGroups(POLE_TILESET_INDEX);
    vector<vector<int>>* benchTiles = GetTileGroups(BENCH_TILESET_INDEX);
	for(uint count = 0; count < treeTiles.size(); count++){
		vector<vector<int>> &treeGroup= *(treeTiles[count]);
		for(uint i = 0; i < treeGroup.size(); i++){
			vector<int> &treeTilesVector= treeGroup[i];
			for(uint j = 0; j < treeTilesVector.size(); j++){
				Obstacle* tree= nullptr;
				index = treeTilesVector[j];
				Vec2 offset(0,0);
				if(treeTilesVector.size() <= (j+1) ){
					//checar as alternativas gerará um seg fault
                    tree = new Obstacle("./img/obstacle/arvore1.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
				}
				else{
                    auto baixo= std::find(treeTilesVector.begin(), treeTilesVector.end(),treeTilesVector[j]+tileMap.GetWidth());
					if(baixo != treeTilesVector.end()){
						//tem um tile em baixo
						if(treeTilesVector[j+1] == (index+1) ){
							//tem uma linha e uma coluna a partir do tile sendo processado
							bool isSqare=false;
							if( (baixo+1) != treeTilesVector.end()){
								if(*(baixo+1) == (*baixo)+1){
									//é um quadrado
									isSqare = true;
									tree = new Obstacle("./img/obstacle/arvore4_1.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
									treeTilesVector.erase(baixo+1);
									treeTilesVector.erase(baixo);
									treeTilesVector.erase(treeTilesVector.begin()+(j+1) );
								}
							}
							if(!isSqare){
								//é uma coluna
								tree = new Obstacle("./img/obstacle/arvore4_2.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
								treeTilesVector.erase(baixo);
							}
						}
						if(nullptr != tree) {
							offset = Vec2(tree->box.w/2, tree->box.h/2);
						}
					}
					if(nullptr == tree){
						if(treeTilesVector[j+1] == index+1){
							//é uma linha
							tree = new Obstacle("./img/obstacle/arvore1_2.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
							treeTilesVector.erase(treeTilesVector.begin()+(j+1) );
						}
						else{
							//é apenas um tile
							tree = new Obstacle("./img/obstacle/arvore1_1.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
						}
					}
				}
				if(nullptr != tree){
                    int tilePos= tileMap.GetCoordTilePos(tree->box.Center(), false, 0);
                    REPORT_DEBUG("\tInserting the gameObject at position " << tilePos);
                    Tile& tile = tileMap.AtLayer(tilePos,COLLISION_LAYER);
                    tile.setObject(tree);
                    AddObject(tree);
					tree->box = tree->box - offset;
				}
				else{
					REPORT_DEBUG2(1, "\t[WARNING] Couldn't place a tree on tileMap!");
				}
			}
		}
	}
	delete treeTiles[0];
	delete treeTiles[1];
	delete treeTiles[2];
	for(uint i = 0; i < poleTiles->size(); i++){
		for(uint j = 0; j < poleTiles->at(i).size(); j++){
			index = poleTiles->at(i)[j];
			Obstacle* pole = new Obstacle("./img/obstacle/poste_aceso.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
            int tilePos= tileMap.GetCoordTilePos(pole->box.Center(), false, 0);
            REPORT_DEBUG("\tInserting the gameObject at position " << tilePos);
            Tile& tile = tileMap.AtLayer(tilePos,COLLISION_LAYER);
            tile.setGO(pole);
            AddObject(pole);
			pole->box = pole->box - Vec2((float)10.5*pole->box.w/16., (float)5.5*pole->box.h/8.);
		}
	}
	delete poleTiles;
	for(uint i = 0; i < benchTiles->size(); i++){
		for(uint j = 0; j < benchTiles->at(i).size(); j++){
			index = benchTiles->at(i)[j];
			Obstacle* bench = new Obstacle("./img/obstacle/banco_h.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
            int tilePos= tileMap.GetCoordTilePos(bench->box.Center(), false, 0);
            REPORT_DEBUG("\tInserting the gameObject at position " << tilePos);
            Tile& tile = tileMap.AtLayer(tilePos,COLLISION_LAYER);
            tile.setGO(bench);
            AddObject(bench);
		}
	}
	delete benchTiles;
}

GameObject* StageState::FindNearestGO(Vec2 origin, std::string targetType, float range){
	GameObject* closerObj = nullptr;
	double closerObjDistance = std::numeric_limits<double>::max();
	for(unsigned int i = 0; i < objectArray.size(); i ++){
		std::unique_ptr<GameObject> &gameObjectInAnalisis= objectArray[i];
		if(nullptr != gameObjectInAnalisis){
			if(gameObjectInAnalisis->Is(targetType)){
				double distance = origin.VecDistance(gameObjectInAnalisis->box.Center()).Magnitude();
				if(distance < closerObjDistance && distance <= range){
					closerObjDistance = distance;
					closerObj = gameObjectInAnalisis.get();
				}
			}
		}
	}
	return(closerObj);
}

std::vector<GameObject*>* StageState::FindNearestGOs(Vec2 origin, std::string targetType, float range){
	vector<GameObject*> *objectsInRange= new vector<GameObject*>();
	for(unsigned int i = 0; i < objectArray.size(); i ++){
		std::unique_ptr<GameObject> &gameObjectInAnalisis= objectArray[i];
		if(nullptr != gameObjectInAnalisis){
			if(gameObjectInAnalisis->Is(targetType)){
				double distance = origin.VecDistance(gameObjectInAnalisis->box.Center()).Magnitude();
				if(distance <= range){
					objectsInRange->push_back(gameObjectInAnalisis.get());
				}
			}
		}
	}
	return(objectsInRange);
}

void StageState::LoadAssets(void) const{
	Resources::GetImage("./map/tileset_vf.png");
	Resources::GetImage("./img/UI/HUD/menu.png");
	Resources::GetImage("./img/UI/HUD/CoraçãoHUD_spritesheet.png");
	Resources::GetImage("./img/UI/HUD/inimigoHUD_spritesheet.png");
	Resources::GetImage("./img/UI/HUD/spritesheetmoeda_HUD.png");
	Resources::GetImage("./img/UI/HUD/hudvida.png");
	Resources::GetImage("./img/UI/HUD/openmenu.png");
	Resources::GetImage("./img/UI/HUD/openmenu-clicked.png");
	Resources::GetImage("./img/UI/HUD/botaotorre.png");
	Resources::GetImage("./img/UI/HUD/botaotorre-clicked.png");
	Resources::GetImage("./img/obstacle/arvore1.png");
	Resources::GetImage("./img/obstacle/arvore2.png");
	Resources::GetImage("./img/obstacle/arvore3.png");
	Resources::GetImage("./img/obstacle/arvore4.png");
	Resources::GetImage("./img/obstacle/banco_h.png");
	Resources::GetImage("./img/obstacle/posteLuz.png");
	Resources::GetImage("./img/tower/torre_fumaca.png");
	Resources::GetImage("./img/tower/torrefumaca.png");
	Resources::GetImage("./img/tower/torre-anti-bomba.png");
	Resources::GetImage("./img/tower/torrestun.png");
	Resources::GetImage("./img/tower/torrechoque_lvl1.png");
	Resources::GetImage("./img/enemy/perna_tras.png");
	Resources::GetImage("./img/enemy/cabeca_tras.png");
	Resources::GetImage("./img/enemy/cabelo_tras.png");
	Resources::GetImage("./img/enemy/torso_tras.png");
	Resources::GetImage("./img/enemy/perna_dir.png");
	Resources::GetImage("./img/enemy/cabeca_dir.png");
	Resources::GetImage("./img/enemy/cabelo_dir.png");
	Resources::GetImage("./img/enemy/torso_dir.png");
	Resources::GetImage("./img/enemy/perna_frente.png");
	Resources::GetImage("./img/enemy/cabeca_frente.png");
	Resources::GetImage("./img/enemy/cabelo_frente.png");
	Resources::GetImage("./img/enemy/torso_frente.png");
	Resources::GetImage("./img/enemy/perna_esq.png");
	Resources::GetImage("./img/enemy/cabeca_esq.png");
	Resources::GetImage("./img/enemy/cabelo_esq.png");
	Resources::GetImage("./img/enemy/torso_esq.png");
	Resources::GetImage("./img/SpriteSheets/explosao_spritesheet.png");
	Resources::GetMusic("./audio/trilha_sonora/loop_1.ogg");
	Resources::GetSound("./audio/Acoes/Inicio de Wave.wav");
	Resources::GetSound("./audio/Acoes/Consertando1.wav");
	Resources::GetSound("./audio/Ambiente/Barulho_noite.wav");
	Resources::GetSound("./audio/Ambiente/Trovao.wav");
	Resources::GetSound("./audio/Ambiente/andando2.wav");
	Resources::GetSound("./audio/Interface/Click1.wav");
	Resources::GetFont("./font/SHPinscher-Regular.otf", 95);
	// Resources::GetImage();
	// Resources::GetMusic();
	// Resources::GetSound();
	// Resources::GetFont();
}
