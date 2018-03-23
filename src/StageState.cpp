#include "StageState.h"
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
		, menuMove("audio/Interface/Click1.wav")
		, waveManagerGO(new GameObject())
		, tileSet("map/tileSetDescriptor.txt",*waveManagerGO)
		, tileMap(*waveManagerGO,"map/tileMap.txt",&tileSet)
		, waveManager(new WaveManager(tileMap,"assets/wave&enemyData.txt",*waveManagerGO))
		, frameRateCounter(0)
	 {

	Resources::ChangeMusicVolume(0);
	Resources::ChangeSoundVolume(0);

	REPORT_I_WAS_HERE;
	music.Play(0);
	Camera::pos = Vec2(CAM_START_X, CAM_START_Y);
	Camera::ForceLogZoom(CAM_START_ZOOM);

	tileMap.ObserveMapChanges(this);
	waveManagerGO->AddComponent(&tileMap);
	GameResources::SetTileMap(&tileMap);

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

	ToggleMenu();
}

void StageState::SetupUI(){
	// Side Menu
	menuIsShowing = true;

	GameObject* go;
	RectTransform* rt;
	Sprite* sp;
	Button* btn;
	Text* txt;
	Grouper* gp;

	GameObject* canvasGO;
	GameObject* menuBGGO;
	GameObject* grouperGO;
	GameObject* statsGO;
	GameObject* statGroupGO;

	// Canvas
	go = new GameObject();
	rt = new RectTransform( *go, nullptr );
	rt->SetAnchors( Vec2(0.0, 0.0), Vec2(1.0, 1.0) );
	go->AddComponent( rt );
	AddObject( go );
	canvasGO = go;

	// MenuBG
	go = new GameObject();
	sp = new Sprite( "img/UI/HUD/menu.png", *go );
	go->AddComponent( sp );
	rt = new RectTransform( *go, canvasGO );
	rt->SetAnchors( Vec2(1., 0.5), Vec2(1., 0.5) );
	rt->SetCenterPin( Vec2(1.0, 0.5) );
	rt->SetOffsets( -sp->GetHeight()/2.0, -10.0, sp->GetHeight()/2.0, -(sp->GetWidth()+10.0) );
	rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
	rt->SetBehaviorType( RectTransform::BehaviorType::FIT );
	go->AddComponent( rt );
	AddObject( go );
	menuBGGO = go;
	menuBGRT = rt;

	// OpenMenuBtn
	go = new GameObject();
	sp = new Sprite( "img/UI/HUD/openmenu.png", *go );
	go->AddComponent( sp );
	btn = new Button( *go );
	btn->SetReleaseCallback( { [] ( void* ptr ) {
									( (StageState&) Game::GetInstance().GetCurrentState() ).ToggleMenu();
								}, nullptr } );
	btn->SetCallback( Button::State::HIGHLIGHTED, { [] (void* ptr) {
													Sprite* sp = static_cast<Sprite*>( ptr );
													sp->SetImage( "img/UI/HUD/openmenu-clicked.png" );
												}, sp } );
	btn->SetCallback( Button::State::ENABLED, { [] (void* ptr) {
													Sprite* sp = static_cast<Sprite*>( ptr );
													sp->SetImage( "img/UI/HUD/openmenu.png" );
												}, sp } );
	go->AddComponent( btn );
	rt = new RectTransform( *go, menuBGGO );
	rt->SetAnchors( {0.0, 0.0}, {0.0, 0.0} );
	rt->SetCenterPin( Vec2(1.0, 0.0) );
	rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
	rt->SetOffsets( +10, -10.0, sp->GetHeight()+10, -10.0 );
	rt->SetBehaviorType( RectTransform::BehaviorType::FILL );
	go->AddComponent( rt );
	go->rotation = 180;
	AddObject( go );
	menuBtnGO = go;

	// TowerInfo
		// - Grouper
		go = new GameObject();
		gp = new Grouper( *go );
		gp->MakeVerticalGroup();
		go->AddComponent( gp );
		rt = new RectTransform( *go, menuBGGO );
		rt->SetAnchors( Vec2(0.165, 0.05), Vec2(0.86, 0.425) );
		rt->SetOffsets( 5.0, -5.0, -5.0, 5.0 );
		go->AddComponent( rt );
		AddObject( go );
		grouperGO = go;

		// - TowerName
		go = new GameObject();
		txt = new Text( *go );
		txt->SetFont( "font/SHPinscher-Regular.otf" );
		txt->SetText( TOWERNAME_DEFAULT_TEXT );
		txt->SetColor( TOWER_INFO_TXT_COLOR );
		txt->SetFontSize( 95 );
		go->AddComponent( txt );
		rt = new RectTransform( *go, grouperGO );
		rt->SetKernelSize( txt->GetSize() );
		rt->SetBehaviorType( RectTransform::BehaviorType::FIT );
		go->AddComponent( rt );
		AddObject( go );
		gp->groupedElements.push_back( go );
		towerNameTXT = txt;
		
		// - TowerCost
		go = new GameObject();
		txt = new Text( *go );
		txt->SetFont( "font/SHPinscher-Regular.otf" );
		txt->SetText( TOWERCOST_DEFAULT_TEXT );
		txt->SetColor( TOWER_INFO_TXT_COLOR );
		txt->SetFontSize( 95 );
		go->AddComponent( txt );
		rt = new RectTransform( *go, grouperGO );
		rt->SetKernelSize( txt->GetSize() );
		rt->SetBehaviorType( RectTransform::BehaviorType::FIT );
		go->AddComponent( rt );
		AddObject( go );
		gp->groupedElements.push_back( go );
		towerCostTXT = txt;

		// - TowerDamage
		go = new GameObject();
		txt = new Text( *go );
		txt->SetFont( "font/SHPinscher-Regular.otf" );
		txt->SetText( TOWERDAMAGE_DEFAULT_TEXT );
		txt->SetColor( TOWER_INFO_TXT_COLOR );
		txt->SetFontSize( 95 );
		go->AddComponent( txt );
		rt = new RectTransform( *go, grouperGO );
		rt->SetKernelSize( txt->GetSize() );
		rt->SetBehaviorType( RectTransform::BehaviorType::FIT );
		go->AddComponent( rt );
		AddObject( go );
		gp->groupedElements.push_back( go );
		towerDamageTXT = txt;

		// - TowerDamageType
		go = new GameObject();
		txt = new Text( *go );
		txt->SetFont( "font/SHPinscher-Regular.otf" );
		txt->SetText( TOWERDAMGETYPE_DEFAULT_TEXT );
		txt->SetColor( TOWER_INFO_TXT_COLOR );
		txt->SetFontSize( 95 );
		go->AddComponent( txt );
		rt = new RectTransform( *go, grouperGO );
		rt->SetKernelSize( txt->GetSize() );
		rt->SetBehaviorType( RectTransform::BehaviorType::FIT );
		go->AddComponent( rt );
		AddObject( go );
		gp->groupedElements.push_back( go );
		towerDmgTypeTXT = txt;

	// TowerButtons
		// - Grouper
		go = new GameObject();
		gp = new Grouper( *go );
		gp->MakeGridGroup( Grouper::ConstraintType::FIXED_N_COLS, 2, Grouper::BehaviorOnLess::CENTER );
		gp->padding = Vec2( 10.0, 10.0 );
		go->AddComponent( gp );
		rt = new RectTransform( *go, menuBGGO );
		rt->SetAnchors( Vec2(0., 0.485), Vec2(1., 1.) );
		rt->SetOffsets( 0., -27.0, -30.0, 32.0 );
		go->AddComponent( rt );
		AddObject( go );
		grouperGO = go;

		// - SmokeTower
		go = new GameObject();
		sp = new Sprite( "img/UI/HUD/botaotorre.png", *go );
		go->AddComponent( sp );
		btn = new Button( *go );
		btn->SetCallback( Button::State::HIGHLIGHTED, { [] (void* ptr) {
														( (StageState&)Game::GetInstance().GetCurrentState() ).SetTowerInfoData( "Fumaca", "$30", "Slow", "Area" );
													}, nullptr } );
		btn->SetCallback( Button::State::PRESSED, { [] (void* ptr) {
														Sprite* sp = static_cast<Sprite*>(ptr);
														sp->SetImage( "img/UI/HUD/botaotorre-clicked.png" );
													}, sp } );
		btn->SetCallback( Button::State::ENABLED, { [] (void* ptr) {
														Sprite* sp = static_cast<Sprite*>(ptr);
														( (StageState&)Game::GetInstance().GetCurrentState() ).SetTowerInfoData();
														sp->SetImage( "img/UI/HUD/botaotorre.png" );
													}, sp } );
		btn->SetReleaseCallback( { [] (void* ptr) {
			( (StageState&)Game::GetInstance().GetCurrentState() ).CreateTower( Tower::TowerType::SMOKE );
		}, nullptr } );
		go->AddComponent( btn );
		rt = new RectTransform( *go, grouperGO );
		rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
		rt->SetBehaviorType( RectTransform::BehaviorType::FIT );
		go->AddComponent( rt );
		AddObject( go );
		gp->groupedElements.push_back( go );

		// - TentaclesTower
		go = new GameObject();
		sp = new Sprite( "img/UI/HUD/botaoantibomba.png", *go );
		go->AddComponent( sp );
		btn = new Button( *go );
		btn->SetCallback( Button::State::HIGHLIGHTED, { [] (void* ptr) {
														( (StageState&)Game::GetInstance().GetCurrentState() ).SetTowerInfoData( "Tentaculos", "$30", "1 tiro/2s", "Anti-Bomba" );
													}, nullptr } );
		btn->SetCallback( Button::State::PRESSED, { [] (void* ptr) {
														Sprite* sp = static_cast<Sprite*>(ptr);
														sp->SetImage( "img/UI/HUD/botaoantibomba-clicked.png" );
													}, sp } );
		btn->SetCallback( Button::State::ENABLED, { [] (void* ptr) {
														Sprite* sp = static_cast<Sprite*>(ptr);
														( (StageState&)Game::GetInstance().GetCurrentState() ).SetTowerInfoData();
														sp->SetImage( "img/UI/HUD/botaoantibomba.png" );
													}, sp } );
		btn->SetReleaseCallback( { [] (void* ptr) {
			( (StageState&)Game::GetInstance().GetCurrentState() ).CreateTower( Tower::TowerType::ANTIBOMB );
		}, nullptr } );
		go->AddComponent( btn );
		rt = new RectTransform( *go, grouperGO );
		rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
		rt->SetBehaviorType( RectTransform::BehaviorType::FIT );
		go->AddComponent( rt );
		AddObject( go );
		gp->groupedElements.push_back( go );

		// - CoilTower
		go = new GameObject();
		sp = new Sprite( "img/UI/HUD/botaochoque.png", *go );
		go->AddComponent( sp );
		btn = new Button( *go );
		btn->SetCallback( Button::State::HIGHLIGHTED, { [] (void* ptr) {
														( (StageState&)Game::GetInstance().GetCurrentState() ).SetTowerInfoData( "Bobina", "$30", "1 tiro/2s", "Dano" );
													}, nullptr } );
		btn->SetCallback( Button::State::PRESSED, { [] (void* ptr) {
														Sprite* sp = static_cast<Sprite*>(ptr);
														sp->SetImage( "img/UI/HUD/botaochoque-clicked.png" );
													}, sp } );
		btn->SetCallback( Button::State::ENABLED, { [] (void* ptr) {
														Sprite* sp = static_cast<Sprite*>(ptr);
														( (StageState&)Game::GetInstance().GetCurrentState() ).SetTowerInfoData();
														sp->SetImage( "img/UI/HUD/botaochoque.png" );
													}, sp } );
		btn->SetReleaseCallback( { [] (void* ptr) {
			( (StageState&)Game::GetInstance().GetCurrentState() ).CreateTower( Tower::TowerType::SMOKE );
		}, nullptr } );
		go->AddComponent( btn );
		rt = new RectTransform( *go, grouperGO );
		rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
		rt->SetBehaviorType( RectTransform::BehaviorType::FIT );
		go->AddComponent( rt );
		AddObject( go );
		gp->groupedElements.push_back( go );

		// - MonolithTower
		go = new GameObject();
		sp = new Sprite( "img/UI/HUD/botaostun.png", *go );
		go->AddComponent( sp );
		btn = new Button( *go );
		btn->SetCallback( Button::State::HIGHLIGHTED, { [] (void* ptr) {
														( (StageState&)Game::GetInstance().GetCurrentState() ).SetTowerInfoData( "Monolito", "$30", "Stun", "Area" );
													}, nullptr } );
		btn->SetCallback( Button::State::PRESSED, { [] (void* ptr) {
														Sprite* sp = static_cast<Sprite*>(ptr);
														sp->SetImage( "img/UI/HUD/botaostun-clicked.png" );
													}, sp } );
		btn->SetCallback( Button::State::ENABLED, { [] (void* ptr) {
														Sprite* sp = static_cast<Sprite*>(ptr);
														( (StageState&)Game::GetInstance().GetCurrentState() ).SetTowerInfoData();
														sp->SetImage( "img/UI/HUD/botaostun.png" );
													}, sp } );
		btn->SetReleaseCallback( { [] (void* ptr) {
			( (StageState&)Game::GetInstance().GetCurrentState() ).CreateTower( Tower::TowerType::ANTIBOMB );
		}, nullptr } );
		go->AddComponent( btn );
		rt = new RectTransform( *go, grouperGO );
		rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
		rt->SetBehaviorType( RectTransform::BehaviorType::FIT );
		go->AddComponent( rt );
		AddObject( go );
		gp->groupedElements.push_back( go );


	// Stats
		// - Grouper
		go = new GameObject();
		gp = new Grouper( *go );
		gp->MakeVerticalGroup();
		gp->padding = Vec2( 0.0, 5.0 );
		go->AddComponent( gp );
		rt = new RectTransform( *go, canvasGO );
		rt->SetAnchors( Vec2(0., 0.), Vec2(0.2, 0.125) );
		rt->SetOffsets( 20., 204.8, 75., 20. );
		go->AddComponent( rt );
		AddObject( go );
		statsGO = go;

		// - HealthGroup
		go = new GameObject();
		rt = new RectTransform( *go, statsGO );
		go->AddComponent( rt );
		AddObject( go );
		statGroupGO = go;
		gp->groupedElements.push_back(go);

			// -- HealthBarBG
			go = new GameObject();
			sp = new Sprite( "img/UI/HUD/hudvida.png", *go );
			sp->colorMultiplier = {0, 0, 0, 255};
			go->AddComponent( sp );
			rt = new RectTransform( *go, statGroupGO );
			rt->SetAnchors( Vec2(0.0, 0.15), Vec2(1.0, 0.85) );
			go->AddComponent( rt );
			AddObject( go );

			// -- HealthBar
			go = new GameObject();
			sp = new Sprite( "img/UI/HUD/hudvida.png", *go );
			sp->colorMultiplier = {180, 225, 149, 255};
			go->AddComponent( sp );
			rt = new RectTransform( *go, statGroupGO );
			rt->SetAnchors( Vec2(0.0, 0.15), Vec2(0.5, 0.85) );
			rt->SetOffsets( 5., -5., -5., 5.);
			go->AddComponent( rt );
			AddObject( go );
			lifeRT = rt;

			// -- HealthIcon
			go = new GameObject();
			sp = new Sprite( "img/UI/HUD/CoraçãoHUD_spritesheet.png", *go, false, 1./4., 8 );
			go->AddComponent( sp );
			rt = new RectTransform( *go, statGroupGO );
			rt->SetCenterPin( Vec2(.225, 0.5) );
			rt->SetAnchors( Vec2(0.0, 0.0), Vec2(0.0, 1.0) );
			rt->SetBehaviorType( RectTransform::BehaviorType::FILL );
			go->AddComponent( rt );
			AddObject( go );

		// - WaveGroup
		go = new GameObject();
		rt = new RectTransform( *go, statsGO );
		go->AddComponent( rt );
		AddObject( go );
		statGroupGO = go;
		gp->groupedElements.push_back(go);

			// -- WaveBarBG
			go = new GameObject();
			sp = new Sprite( "img/UI/HUD/hudwave.png", *go );
			sp->colorMultiplier = {0, 0, 0, 255};
			go->AddComponent( sp );
			rt = new RectTransform( *go, statGroupGO );
			rt->SetAnchors( Vec2(0.0, 0.15), Vec2(0.75, 0.85) );
			go->AddComponent( rt );
			AddObject( go );

			// -- WaveBar
			go = new GameObject();
			sp = new Sprite( "img/UI/HUD/hudwave.png", *go );
			sp->colorMultiplier = {154, 148, 104, 255};
			go->AddComponent( sp );
			rt = new RectTransform( *go, statGroupGO );
			rt->SetAnchors( Vec2(0.0, 0.15), Vec2(0.375, 0.85) );
			rt->SetOffsets( 5., -5., -5., 5.);
			go->AddComponent( rt );
			AddObject( go );
			waveRT = rt;

			// -- WaveIcon
			go = new GameObject();
			sp = new Sprite( "img/UI/HUD/inimigoHUD_spritesheet.png", *go, false, 1./4., 5 );
			go->AddComponent( sp );
			rt = new RectTransform( *go, statGroupGO );
			rt->SetCenterPin( Vec2(.225, 0.5) );
			rt->SetAnchors( Vec2(0.0, 0.0), Vec2(0.0, 1.0) );
			rt->SetBehaviorType( RectTransform::BehaviorType::FILL );
			go->AddComponent( rt );
			AddObject( go );

		// - MoneyGroup
		go = new GameObject();
		rt = new RectTransform( *go, statsGO );
		go->AddComponent( rt );
		AddObject( go );
		statGroupGO = go;
		gp->groupedElements.push_back(go);

			// -- MoneyText
			go = new GameObject();
			txt = new Text( *go );
			txt->SetFont( "font/SHPinscher-Regular.otf" );
			txt->SetColor( MONEY_TXT_COLOR );
			txt->SetFontSize( 95 );
			txt->SetText( "+Inf" );
			go->AddComponent( txt );
			rt = new RectTransform( *go, statGroupGO );
			rt->SetKernelSize( txt->GetSize() );
			rt->SetAnchors( Vec2(0.1, 0.1), Vec2(1.0, 0.9) );
			rt->SetCenterPin( Vec2(0.0, 0.5) );
			rt->SetOffsets( 0., -5., 0., 5.);
			rt->SetBehaviorType( RectTransform::BehaviorType::FIT );
			go->AddComponent( rt );
			AddObject( go );
			moneyTXT = txt;

			// -- MoneyIcon
			go = new GameObject();
			sp = new Sprite( "img/UI/HUD/spritesheetmoeda_HUD.png", *go, false, 1./4., 4 );
			go->AddComponent( sp );
			rt = new RectTransform( *go, statGroupGO );
			rt->SetCenterPin( Vec2(.225, 0.5) );
			rt->SetAnchors( Vec2(0.0, 0.0), Vec2(0.0, 1.0) );
			rt->SetBehaviorType( RectTransform::BehaviorType::FILL );
			go->AddComponent( rt );
			AddObject( go );
}

StageState::~StageState(void) {
	std::cout<<WHERE<<"\tGameResources path hit rate: " << GameResources::GetPathHitRate()<<END_LINE;
	objectArray.clear();
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

	if(collisionMap.size() >= 2){
		for(std::map<Component*,GameObject*>::iterator it1 = collisionMap.begin(); it1 != collisionMap.end();++ it1) {
			for(std::map<Component*,GameObject*>::iterator it2 = it1 ++; it2 != collisionMap.end();++ it2) {
				if(collisionMap.at(it1->first) != collisionMap.at(it2->first)){
					if(Collision::IsColliding(collisionMap.at(it1->first)->box, collisionMap.at(it2->first)->box,collisionMap.at(it1->first)->rotation,collisionMap.at(it2->first)->rotation) ) {
						it1->first->NotifyCollision(*it2->first);
						it2->first->NotifyCollision(*it1->first);
					}
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
		Tile tile = tileMap.AtLayer(position,COLLISION_LAYER);
		GameObject *go = (GameObject*)tile.GetGO();
		if(nullptr != go) {
			go->AddComponent(new DragAndDrop<Tile>(tileMap, mousePos, *go));
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

	State::Update(dt);
}

void StageState::Render(void) const {
	REPORT_I_WAS_HERE;
	State::Render();
	if(isLightning){
		SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), lightningColor.r, lightningColor.g, lightningColor.b, lightningColor.a);
		SDL_SetRenderDrawBlendMode(Game::GetInstance().GetRenderer(), SDL_BLENDMODE_BLEND);
		SDL_RenderFillRect(Game::GetInstance().GetRenderer(), NULL);
	}
	REPORT_I_WAS_HERE;
}

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

	Rect offs = menuBGRT->GetOffsets();
	menuBGRT->SetOffsets( offs.y, -offs.x, offs.h, -offs.w );
	menuBtnGO->rotation = 180 - menuBtnGO->rotation;
}

void StageState::SetTowerInfoData(string name, string cost, string damage, string damageType) {
	towerNameTXT->SetText(name);
	towerCostTXT->SetText(cost);
	towerDamageTXT->SetText(damage);
	towerDmgTypeTXT->SetText(damageType);
}

void StageState::CreateTower(Tower::TowerType towerType) {
	ToggleMenu();

	if( PLAYER_DATA_INSTANCE.GetGold() >= 30 ) {
		Vec2 mousePos = Camera::ScreenToWorld(INPUT_MANAGER.GetMousePos())-Vec2(TOWER_LINEAR_SIZE/2, TOWER_LINEAR_SIZE/2);
		GameObject* GO = new GameObject();
		GO->AddComponent(new Tower(towerType, mousePos, Vec2(TOWER_LINEAR_SIZE, TOWER_LINEAR_SIZE), TOWER_BASE_HP,*GO));
		GO->AddComponent(new DragAndDrop<Tile>(tileMap, mousePos, *GO, false, false));
		AddObject(GO);
		PLAYER_DATA_INSTANCE.GoldUpdate(-30, false);
		towerMenuSounds.Play(1);
	} else {
		printf("Not enough gold! Need more gold!\n");
	}
}

void StageState::SetUILife(float lifePercent) {
	lifePercent = (lifePercent < 0) ? 0 : ((lifePercent > 1) ? 1 : lifePercent);
	Rect oldAnchor = lifeRT->GetAnchors();
	lifeRT->SetAnchors(Vec2(oldAnchor.x, oldAnchor.y),Vec2(lifePercent, oldAnchor.h));
}

void StageState::SetUIWaveProgress(float waveProgressPercent) {
	waveProgressPercent = (waveProgressPercent < 0) ? 0 : ((waveProgressPercent > 1) ? 1 : waveProgressPercent);
	Rect oldAnchor = waveRT->GetAnchors();
	waveRT->SetAnchors( Vec2(oldAnchor.x, oldAnchor.y), Vec2(waveProgressPercent*0.75, oldAnchor.h) );
}

void StageState::SetUIMoney(int coins) {
	moneyTXT->SetText( std::to_string(coins) );
}

void StageState::InitializeObstacles(void){

	//70 a 72 3 tipos de arvores
	//73 poste
	//76 banco

	int index;
	TileMap<Tile>* tileMap = (TileMap<Tile>*)waveManagerGO->GetComponent(ComponentType::TILEMAP);
	int mapWidth= tileMap->GetWidth();
	Vec2 tileSize= tileMap->GetTileSize();
	int tileWidth= tileSize.x;
	int tileHeight= tileSize.y;
	std::array<vector<vector<int>>*, 3> treeTiles;
	treeTiles[0] = waveManager->GetTileGroups(TREE_1_TILESET_INDEX);
	treeTiles[1] = waveManager->GetTileGroups(TREE_2_TILESET_INDEX);
	treeTiles[2] = waveManager->GetTileGroups(TREE_3_TILESET_INDEX);
	vector<vector<int>>* poleTiles = waveManager->GetTileGroups(POLE_TILESET_INDEX);
	vector<vector<int>>* benchTiles = waveManager->GetTileGroups(BENCH_TILESET_INDEX);
	for(uint count = 0; count < treeTiles.size(); count++){
		vector<vector<int>> &treeGroup= *(treeTiles[count]);
		for(uint i = 0; i < treeGroup.size(); i++){
			vector<int> &treeTilesVector= treeGroup[i];
			for(uint j = 0; j < treeTilesVector.size(); j++){
				Obstacle* tree= nullptr;
				GameObject* ObstGO = new GameObject();
				index = treeTilesVector[j];
				Vec2 offset(0,0);
				if(treeTilesVector.size() <= (j+1) ){
					//checar as alternativas gerará um seg fault
					tree = new Obstacle(*ObstGO,"./img/obstacle/arvore1.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
				}
				else{
					auto baixo= std::find(treeTilesVector.begin(), treeTilesVector.end(),treeTilesVector[j]+tileMap->GetWidth());
					if(baixo != treeTilesVector.end()){
						//tem um tile em baixo
						if(treeTilesVector[j+1] == (index+1) ){
							//tem uma linha e uma coluna a partir do tile sendo processado
							bool isSqare=false;
							if( (baixo+1) != treeTilesVector.end()){
								if(*(baixo+1) == (*baixo)+1){
									//é um quadrado
									isSqare = true;
									tree = new Obstacle(*ObstGO,"./img/obstacle/arvore4_1.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
									treeTilesVector.erase(baixo+1);
									treeTilesVector.erase(baixo);
									treeTilesVector.erase(treeTilesVector.begin()+(j+1) );
								}
							}
							if(!isSqare){
								//é uma coluna
								tree = new Obstacle(*ObstGO,"./img/obstacle/arvore4_2.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
								treeTilesVector.erase(baixo);
							}
						}
						if(nullptr != tree) {
							offset = Vec2(ObstGO->box.w/2, ObstGO->box.h/2);
						}
					}
					if(nullptr == tree){
						if(treeTilesVector[j+1] == index+1){
							//é uma linha
							tree = new Obstacle(*ObstGO,"./img/obstacle/arvore1_2.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
							treeTilesVector.erase(treeTilesVector.begin()+(j+1) );
						}
						else{
							//é apenas um tile
							tree = new Obstacle(*ObstGO,"./img/obstacle/arvore1_1.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
						}
					}
				}
				if(nullptr != tree){
					int tilePos= tileMap->GetCoordTilePos(ObstGO->box.Center(), false, 0);
					REPORT_DEBUG("\tInserting the gameObject at position " << tilePos);
					Tile& tile = tileMap->AtLayer(tilePos,COLLISION_LAYER);
					tile.SetGO(ObstGO);
					ObstGO->AddComponent(tree);
					AddObject(ObstGO);
					ObstGO->box = ObstGO->box - offset;
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
			GameObject* ObstGOPole = new GameObject();
			Obstacle* pole = new Obstacle(*ObstGOPole,"./img/obstacle/poste_aceso.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
			int tilePos= tileMap->GetCoordTilePos(ObstGOPole->box.Center(), false, 0);
			REPORT_DEBUG("\tInserting the gameObject at position " << tilePos);
			Tile& tile = tileMap->AtLayer(tilePos,COLLISION_LAYER);
			tile.SetGO(ObstGOPole);
			ObstGOPole->AddComponent(pole);
			AddObject(ObstGOPole);
			ObstGOPole->box = ObstGOPole->box - Vec2((float)10.5*ObstGOPole->box.w/16., (float)5.5*ObstGOPole->box.h/8.);
		}
	}
	delete poleTiles;
	for(uint i = 0; i < benchTiles->size(); i++){
		for(uint j = 0; j < benchTiles->at(i).size(); j++){
			index = benchTiles->at(i)[j];
			GameObject* ObstGOBench = new GameObject();
			Obstacle* bench = new Obstacle(*ObstGOBench,"./img/obstacle/banco_h.png", Vec2(index%mapWidth*tileWidth, index/mapWidth*tileHeight));
			int tilePos= tileMap->GetCoordTilePos(ObstGOBench->box.Center(), false, 0);
			REPORT_DEBUG("\tInserting the gameObject at position " << tilePos);
			Tile& tile = tileMap->AtLayer(tilePos,COLLISION_LAYER);
			tile.SetGO(ObstGOBench);
			ObstGOBench->AddComponent(bench);
			AddObject(ObstGOBench);
		}
	}
	delete benchTiles;
}

GameObject* StageState::FindNearest(Vec2 origin, Finder<GameObject>* finder, float range) const{

	GameObject* closerObj = nullptr;
	double closerObjDistance = std::numeric_limits<double>::max();

	for(unsigned int i = 0; i < objectArray.size(); i ++){
		float tempRes= (*finder)(objectArray[i].get());
		if(tempRes < closerObjDistance){
			closerObj = (objectArray[i].get());
			closerObjDistance = tempRes;
		}
	}
	return(closerObj);
}


std::vector<GameObject*>* StageState::FindNearests(Vec2 origin,Finder<GameObject>* finder,float range) const{
	vector<GameObject*> *objectsInRange = new vector<GameObject*>();
	for(unsigned int i = 0; i < objectArray.size(); i ++){
		float distance = (*finder)(objectArray[i].get());
		if(distance <= range){
			objectsInRange->push_back(objectArray[i].get());
		}
	}
	return(objectsInRange);
}

void StageState::AddCollider(Component& collider,GameObject& associated){
	//std::make_unique só funciona para c++14.Além de que essa implementação de mapa está muito obscura.Desse modo essa otimização
	//de lógica de colisões fica para outra oportunidade
	//std::shared_ptr<Component> c = std::make_shared<Component>(std::move(std::make_unique<Component>(std::move(&collider))));
	//std::shared_ptr<GameObject> g = std::make_shared<GameObject>(std::move(std::make_unique<Component>(std::move(&associated))));

	collisionMap[&collider] = &associated;
}
void StageState::RemoveCollider(Component& collider){
	collisionMap.erase(&collider);
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
	Resources::GetImage("./img/UI/HUD/botaostun.png");
	Resources::GetImage("./img/UI/HUD/botaostun-clicked.png");
	Resources::GetImage("./img/UI/HUD/botaochoque.png");
	Resources::GetImage("./img/UI/HUD/botaochoque-clicked.png");
	Resources::GetImage("./img/UI/HUD/botaoantibomba.png");
	Resources::GetImage("./img/UI/HUD/botaoantibomba-clicked.png");
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
