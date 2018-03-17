#include "TitleState.h"

#include "Camera.h"
#include "Game.h"
#include "InputManager.h"
#include "StageState.h"

#include <cfloat>

#define ECLIPSE_DURATION	6.5 // s
#define OVERLAY_FADEIN_DURATION	4 // s
#define MAX_SPEED			20 // px/s
#define MIN_SPEED			5 // px/s
#define DISABLED_COLOR		{ 70, 70, 70,100} // Dark Gray
#define ENABLED_COLOR		{164,133,166,255} // Purple
#define HIGHLIGHTED_COLOR	{227,196,230,255} // Purple-ish white
#define PRESSED_COLOR		{227,196,230,255} // Purple-ish white

TitleState::TitleState()
		: State()
		, speedNuvemA(std::rand() % (MAX_SPEED - MIN_SPEED) + MIN_SPEED)
		, speedNuvemB(std::rand() % (MAX_SPEED - MIN_SPEED) + MIN_SPEED)
		, introTimer()
		, titleMusic("audio/trilha_sonora/main_title_.ogg")
		, clickSound("audio/Interface/Click1.wav")
		// , playBtn(*playBtnGO)
		// , editorBtn(*editorBtnGO)
		// , configBtn(*configBtnGO)
		// , exitBtn(*exitBtnGO)
		, finishedEclipse(false)
		, finishedFadeIn(false)
		, forceEnd(false){

	Resources::ChangeMusicVolume(0);
	Resources::ChangeSoundVolume(0);
	
	SetupUI();
}

void TitleState::SetupUI(void) {
	Rect winSize = Game::GetInstance().GetWindowDimensions();

	GameObject* go;
	RectTransform* rt;
	Sprite* sp;
	Text* txt;
	Grouper* gp;
	Button* btn;

	GameObject* canvasGO;
	GameObject* grouperGO;

	// Canvas
	go = new GameObject();
	rt = new RectTransform(*go, nullptr);
	rt->SetBehaviorType(RectTransform::BehaviorType::STRETCH);
	rt->SetAnchors({0.0, 0.0}, {1.0, 1.0});
	rt->SetKernelSize(Vec2(1024,600));
	go->AddComponent(rt);
	AddObject(go);
	canvasGO = go;

	// BG
	go = new GameObject();
	sp = new Sprite("img/UI/main-menu/bg.png", *go);
	go->AddComponent(sp);
	rt = new RectTransform(*go, canvasGO);
	rt->SetBehaviorType(RectTransform::BehaviorType::STRETCH);
	rt->SetAnchors({0.0, 0.0}, {1.0, 1.0});
	rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
	go->AddComponent(rt);
	AddObject(go);

	// Lua
	go = new GameObject();
	sp = new Sprite("img/UI/main-menu/spritesheetlua.png", *go, false, ECLIPSE_DURATION/8., 8);
	sp->SetScale(0.75);
	go->AddComponent(sp);
	rt = new RectTransform(*go, canvasGO);
	rt->SetBehaviorType(RectTransform::FIT);
	rt->SetAnchors(Vec2((float)(0.5 - (sp->GetWidth()/2.+30.)/winSize.x),(float)(60./winSize.y)),
					   Vec2((float)(0.5 + (sp->GetWidth()/2.-30.)/winSize.x), (float)(60. + sp->GetHeight())/winSize.y ));
	rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
	go->AddComponent(rt);
	AddObject(go);
	luaGO = go;

	// Nuvem A
	go = new GameObject();
	sp = new Sprite("img/UI/main-menu/nuvemA.png", *go);
	sp->SetScale(0.6);
	go->AddComponent(sp);
	rt = new RectTransform(*go, canvasGO);
	rt->SetBehaviorType(RectTransform::BehaviorType::FILL);
	rt->SetCenterPin(Vec2(0.5, 0.1));
	rt->SetAnchors( Vec2(0., (float)20./winSize.y),
					   Vec2((float)sp->GetWidth()/winSize.x, (float)(20.+sp->GetHeight())/winSize.y ) );
	rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
	go->AddComponent(rt);
	AddObject(go);
	nuvemAGO = go;

	// Nuvem B
	go = new GameObject();
	sp = new Sprite("img/UI/main-menu/nuvemB.png", *go);
	sp->SetScale(0.75);
	go->AddComponent(sp);
	rt = new RectTransform(*go, canvasGO);
	rt->SetBehaviorType(RectTransform::BehaviorType::FILL);
	rt->SetAnchors(Vec2((float)(1. - (sp->GetWidth()+110.)/winSize.x), (float)(70./winSize.y)),
					  Vec2((float)(1. - 110./winSize.x), (float)(70.+sp->GetHeight())/winSize.y));
	rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
	go->AddComponent(rt);
	AddObject(go);
	nuvemBGO = go;

	// ICC
	go = new GameObject();
	sp = new Sprite("img/UI/main-menu/icc.png", *go);
	go->AddComponent(sp);
	rt = new RectTransform(*go, canvasGO);
	rt->SetBehaviorType(RectTransform::BehaviorType::STRETCH);
	rt->SetAnchors(Vec2(0., (float)(80./winSize.y)),Vec2(1., 1.));
	rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
	go->AddComponent(rt);
	AddObject(go);

	// Overlay
	go = new GameObject();
	sp = new Sprite("img/UI/main-menu/overlay.png", *go);
	sp->colorMultiplier = {255,255,255,0};
	go->AddComponent(sp);
	rt = new RectTransform(*go, canvasGO);
	rt->SetBehaviorType(RectTransform::BehaviorType::STRETCH);
	rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
	go->AddComponent(rt);
	AddObject(go);
	overlayGO = go;

	// Title
	go = new GameObject();
	sp = new Sprite("img/UI/main-menu/spritesheettitle.png", *go, false, 1./5., 5);
	sp->SetScale(0.7);
	go->AddComponent(sp);
	rt = new RectTransform(*go, canvasGO);
	rt->SetAnchors(Vec2((float)(0.5 - (sp->GetWidth()/2.)/winSize.x), (float)(60./winSize.y)),
					 Vec2((float)(0.5 + (sp->GetWidth()/2.)/winSize.x), (float)(60. + sp->GetHeight())/winSize.y ) );
	rt->SetBehaviorType(RectTransform::BehaviorType::FIT);
	rt->SetKernelSize( Vec2(sp->GetWidth(), sp->GetHeight()) );
	go->AddComponent(rt);
	AddObject(go);
	titleGO = go;

	// OptionsGroup
	go = new GameObject();
	rt = new RectTransform(*go, canvasGO);
	rt->SetAnchors(Vec2(0.3, 0.45),Vec2(0.7, 0.9));
	go->AddComponent(rt);
	gp = new Grouper(*go);
	go->AddComponent(gp);
	AddObject(go);
	grouperGO = go;
	
	// PlayBtn
	go = new GameObject();
	txt = new Text(*go);
	txt->SetFont("font/SHPinscher-Regular.otf");
	txt->SetText("Jogar");
	txt->SetColor({255,255,255,255});
	txt->SetFontSize(95);
	go->AddComponent(txt);
	btn = new Button(*go);
	// btn->ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);
	btn->SetCallback(Button::State::ENABLED, { [] (void* caller) {
													TitleState* titleState = static_cast<TitleState*>(caller);
													titleState->Play();
												}, this
											} );
	go->AddComponent(btn);
	rt = new RectTransform(*go, grouperGO);
	rt->SetKernelSize( txt->GetSize() );
	go->AddComponent(rt);
	AddObject(go);
	gp->groupedElements.push_back(go);
	
	// EditorBtn
	go = new GameObject();
	txt = new Text(*go);
	txt->SetFont("font/SHPinscher-Regular.otf");
	txt->SetText("Editor de Fases");
	txt->SetColor({255,255,255,255});
	txt->SetFontSize(95);
	go->AddComponent(txt);
	btn = new Button(*go);
	// btn->ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);
	go->AddComponent(btn);
	rt = new RectTransform(*go, grouperGO);
	rt->SetKernelSize( txt->GetSize() );
	go->AddComponent(rt);
	AddObject(go);
	gp->groupedElements.push_back(go);
	
<<<<<<< HEAD
	// ConfigBtn
	go = new GameObject();
	txt = new Text(*go);
	txt->SetFont("font/SHPinscher-Regular.otf");
	txt->SetText("Configuracoes");
	txt->SetColor({255,255,255,255});
	txt->SetFontSize(95);
	go->AddComponent(txt);
	btn = new Button(*go);
	// btn->ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);
	go->AddComponent(btn);
	rt = new RectTransform(*go, grouperGO);
	rt->SetKernelSize( txt->GetSize() );
	go->AddComponent(rt);
	AddObject(go);
	gp->groupedElements.push_back(go);

	// ExitBtn
	go = new GameObject();
	txt = new Text(*go);
	txt->SetFont("font/SHPinscher-Regular.otf");
	txt->SetText("Sair");
	txt->SetColor({255,255,255,255});
	txt->SetFontSize(95);
	go->AddComponent(txt);
	btn = new Button(*go);
	// btn->ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);
	btn->SetCallback(Button::State::ENABLED, { [] (void* caller) {
													TitleState* titleState = static_cast<TitleState*>(caller);
													titleState->Exit();
												}, this
											} );
	go->AddComponent(btn);
	rt = new RectTransform(*go, grouperGO);
	rt->SetKernelSize( txt->GetSize() );
	go->AddComponent(rt);
	AddObject(go);
	gp->groupedElements.push_back(go);
=======
    //exitBtn.ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);//O que fazer aqui?
    exitBtn.SetCallback(Button::State::ENABLED, { [] (void* caller) {
                                                       TitleState* titleState = static_cast<TitleState*>(caller);
                                                       titleState->Exit();
                                                                     }, this } );

    optionsGroup.groupedElements.push_back(playBtnGO);
    optionsGroup.groupedElements.push_back(editorBtnGO);
    optionsGroup.groupedElements.push_back(configBtnGO);
    optionsGroup.groupedElements.push_back(exitBtnGO);
	
	AddObject(editorBtnGO);
	AddObject(configBtnGO);
	AddObject(optionsGroupGO);
>>>>>>> Colocada correções feitas ontem

	titleMusic.Play(0);
}

void TitleState::Update(float dt) {
	State::Update( dt );
	if(INPUT_MANAGER.QuitRequested()) {
		quitRequested = true;
	}
	if(INPUT_MANAGER.KeyRelease(ESCAPE_KEY)) {
		forceEnd = true;
	}

	introTimer.Update(dt);
	if(forceEnd || (!finishedEclipse && introTimer.Get() >= ECLIPSE_DURATION)) {
		finishedEclipse = true;
		((Sprite*)(luaGO->GetComponent(ComponentType::SPRITE)))->SetFrameTime(FLT_MAX);
		Color& c = ((Sprite*)(overlayGO->GetComponent(ComponentType::SPRITE)))->colorMultiplier;
		c.a = 180;
		introTimer.Restart();
	}
	if(forceEnd || (finishedEclipse && ! finishedFadeIn && introTimer.Get() >= OVERLAY_FADEIN_DURATION)) {
		finishedFadeIn = true;
		Color& c = ((Sprite*)(titleGO->GetComponent(ComponentType::SPRITE)))->colorMultiplier;
		c.a = 255;
		introTimer.Restart();
	}
	forceEnd = false;

	UpdateUI(dt);
	State::Update(dt);
}

void TitleState::UpdateUI(float dt) {
	Rect winSize(0., 0., Game::GetInstance().GetWindowDimensions().x, Game::GetInstance().GetWindowDimensions().y);

	if(!finishedEclipse) {
		Color& c = ((Sprite*)(overlayGO->GetComponent(ComponentType::SPRITE)))->colorMultiplier;
		c.a = (unsigned char)(180*introTimer.Get()/ECLIPSE_DURATION);
	}

	if(finishedEclipse && !finishedFadeIn) {
		Color& c = ((Sprite*)(titleGO->GetComponent(ComponentType::SPRITE)))->colorMultiplier;
		c.a = 255*introTimer.Get()/OVERLAY_FADEIN_DURATION;
	}

	MoveClouds(dt);
}

void TitleState::MoveClouds(float dt) {
	Vec2 winSize = Game::GetInstance().GetWindowDimensions();

	Rect box = nuvemAGO->box;
	Rect offsets = ((RectTransform*)(nuvemAGO->GetComponent(ComponentType::RECT_TRANSFORM)))->GetOffsets();
	if (box.x + box.w < 0) {
		offsets.x += winSize.x + ((Sprite*)(nuvemAGO->GetComponent(ComponentType::SPRITE)))->GetWidth();
		offsets.w += winSize.x + ((Sprite*)(nuvemAGO->GetComponent(ComponentType::SPRITE)))->GetWidth();
		speedNuvemA = std::rand() % (MAX_SPEED - MIN_SPEED) + MIN_SPEED;
	}
	((RectTransform*)(nuvemAGO->GetComponent(ComponentType::RECT_TRANSFORM)))->SetOffsets(offsets.x-dt*speedNuvemA, offsets.y,offsets.w-dt*speedNuvemA, offsets.h);

	box = nuvemBGO->box;
	offsets = ((RectTransform*)(nuvemBGO->GetComponent(ComponentType::RECT_TRANSFORM)))->GetOffsets();
	if (box.x + box.w < 0) {
		offsets.x += winSize.x + ((Sprite*)(nuvemBGO->GetComponent(ComponentType::SPRITE)))->GetWidth();
		offsets.w += winSize.x + ((Sprite*)(nuvemBGO->GetComponent(ComponentType::SPRITE)))->GetWidth();
		speedNuvemB = std::rand() % (MAX_SPEED - MIN_SPEED) + MIN_SPEED;
	}
	((RectTransform*)(nuvemBGO->GetComponent(ComponentType::RECT_TRANSFORM)))->SetOffsets(offsets.x-dt*speedNuvemB, offsets.y,offsets.w-dt*speedNuvemB, offsets.h);
}

void TitleState::Pause(void) {
}

void TitleState::Resume(void) {
	Camera::ForceLogZoom(0.0);
	Camera::pos = Vec2(0, 0);
}

void TitleState::Play(void) {
	clickSound.Play(1);
	Game::GetInstance().Push(new StageState());
}

void TitleState::Exit() {
	clickSound.Play(1);
	popRequested = true;
}

void TitleState::LoadAssets(void) const{
	Resources::GetImage("img/UI/main-menu/bg.png");
	Resources::GetImage("img/UI/main-menu/lua.png");
	Resources::GetImage("img/UI/main-menu/nuvemA.png");
	Resources::GetImage("img/UI/main-menu/nuvemB.png");
	Resources::GetImage("img/UI/main-menu/icc.png");
	Resources::GetImage("img/UI/main-menu/overlay.png");
	Resources::GetImage("img/UI/main-menu/spritesheettitle.png");
	Resources::GetMusic("audio/trilha_sonora/main_title_.ogg");
	Resources::GetMusic("audio/Interface/Click1.wav");
	Resources::GetFont("font/SHPinscher-Regular.otf", 95);
}
