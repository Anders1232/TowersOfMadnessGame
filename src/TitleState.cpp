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
        , canvasGO(new GameObject())
        , bgGO(new GameObject())
        , luaGO(new GameObject())
        , nuvemAGO(new GameObject())
        , nuvemBGO(new GameObject())
        , iccGO(new GameObject())
        , overlayGO(new GameObject())
        , titleGO(new GameObject())
        , optionsGroupGO(new GameObject())
        , playBtnGO(new GameObject())
        , editorBtnGO(new GameObject())
        , configBtnGO(new GameObject())
        , exitBtnGO(new GameObject())
        , optionsGroup(*optionsGroupGO)
        , playBtn(*playBtnGO)
        , editorBtn(*editorBtnGO)
        , configBtn(*configBtnGO)
        , exitBtn(*exitBtnGO)
        , finishedEclipse(false)
        , finishedFadeIn(false)
        , forceEnd(false){

	Resources::ChangeMusicVolume(0);
	Resources::ChangeSoundVolume(0);
	
	SetupUI();
}

void TitleState::SetupUI(void) {
    Rect winSize = Game::GetInstance().GetWindowDimensions();

    RectTransform* canvasRect = new RectTransform(*canvasGO,nullptr);
    canvasGO->AddComponent(canvasRect);
    canvasRect->SetBehaviorType(RectTransform::BehaviorType::STRETCH);
    canvasRect->SetKernelSize(Vec2(1024,600));
    AddObject(canvasGO);

    RectTransform* bgRect = new RectTransform(*bgGO,canvasGO);
    bgRect->SetBehaviorType(RectTransform::BehaviorType::STRETCH);
    Sprite* bgSP = new Sprite("img/UI/main-menu/bg.png",*bgGO);
    bgGO->AddComponent(bgRect);
    bgGO->AddComponent(bgSP);
    AddObject(bgGO);

    RectTransform* luaRect = new RectTransform(*luaGO,canvasGO);
    luaRect->SetBehaviorType(RectTransform::FIT);
    Sprite* luaSP = new Sprite("img/UI/main-menu/spritesheetlua.png",*luaGO,false, ECLIPSE_DURATION/8., 8);
    luaSP->SetScale(0.75);
    luaRect->SetAnchors(Vec2((float)(0.5 - (luaSP->GetWidth()/2.+30.)/winSize.x),(float)(60./winSize.y)),
                       Vec2((float)(0.5 + (luaSP->GetWidth()/2.-30.)/winSize.x), (float)(60. + luaSP->GetHeight())/winSize.y ));
    luaGO->AddComponent(luaRect);
    bgGO->AddComponent(luaSP);
    AddObject(luaGO);

    RectTransform* nuvemARect = new RectTransform(*nuvemAGO,canvasGO);
    nuvemARect->SetBehaviorType(RectTransform::BehaviorType::FILL);
    Sprite* nuvemASP = new Sprite("img/UI/main-menu/nuvemA.png",*nuvemAGO);
    nuvemARect->SetCenterPin(Vec2(0.5, 0.1));
    nuvemASP->SetScale(0.6);
    nuvemARect->SetAnchors( Vec2(0., (float)20./winSize.y),
                       Vec2((float)nuvemASP->GetWidth()/winSize.x, (float)(20.+nuvemASP->GetHeight())/winSize.y ) );
    nuvemAGO->AddComponent(nuvemARect);
    nuvemAGO->AddComponent(nuvemASP);
    AddObject(nuvemAGO);

    RectTransform* nuvemBRect = new RectTransform(*nuvemBGO,canvasGO);
    nuvemBRect->SetBehaviorType(RectTransform::BehaviorType::FILL);
    Sprite* nuvemBSP = new Sprite("img/UI/main-menu/nuvemB.png",*nuvemBGO);
    nuvemBSP->SetScale(0.75);
    nuvemBRect->SetAnchors(Vec2((float)(1. - (nuvemBSP->GetWidth()+110.)/winSize.x), (float)(70./winSize.y)),
                      Vec2((float)(1. - 110./winSize.x), (float)(70.+nuvemBSP->GetHeight())/winSize.y));
    nuvemBGO->AddComponent(nuvemBRect);
    nuvemBGO->AddComponent(nuvemBSP);
    AddObject(nuvemBGO);

    RectTransform* iccRect = new RectTransform(*iccGO,canvasGO);
    iccRect->SetBehaviorType(RectTransform::BehaviorType::STRETCH);
    iccRect->SetAnchors(Vec2(0., (float)(80./winSize.y)),Vec2(1., 1.));
    Sprite* iccSP = new Sprite("img/UI/main-menu/icc.png",*iccGO);
    iccGO->AddComponent(iccRect);
    iccGO->AddComponent(iccSP);
    AddObject(iccGO);

    RectTransform* overlayRect = new RectTransform(*overlayGO,canvasGO);
    overlayRect->SetBehaviorType(RectTransform::BehaviorType::STRETCH);
    Sprite* overlaySP = new Sprite("img/UI/main-menu/overlay.png",*overlayGO);
    overlaySP->colorMultiplier = {255,255,255,0};
    overlayGO->AddComponent(overlayRect);
    nuvemAGO->AddComponent(overlaySP);
    AddObject(overlayGO);


    RectTransform* titleRect = new RectTransform(*titleGO,canvasGO);
    titleRect->SetBehaviorType(RectTransform::BehaviorType::FIT);
    Sprite* titleSP = new Sprite("img/UI/main-menu/spritesheettitle.png",*titleGO,false,1./5., 5);
    titleSP->SetScale(0.7);
    titleRect->SetAnchors(Vec2((float)(0.5 - (titleSP->GetWidth()/2.)/winSize.x), (float)(60./winSize.y)),
                     Vec2((float)(0.5 + (titleSP->GetWidth()/2.)/winSize.x), (float)(60. + titleSP->GetHeight())/winSize.y ) );
    titleGO->AddComponent(titleRect);
    titleGO->AddComponent(titleSP);
    AddObject(titleGO);

    Text* playBtnText = new Text(*playBtnGO);
    playBtnText->SetText("Jogar");
    playBtnText->SetColor({255,255,255,255});
    playBtnText->SetFont("font/SHPinscher-Regular.otf");
    playBtnText->SetFontSize(95);
    playBtnGO->AddComponent(playBtnText);
    AddObject(playBtnGO);

    Text* exitBtnText = new Text(*exitBtnGO);
    exitBtnText->SetText("Sair");
    exitBtnText->SetColor({255,255,255,255});
    exitBtnText->SetFont("font/SHPinscher-Regular.otf");
    exitBtnText->SetFontSize(95);
    exitBtnGO->AddComponent(exitBtnText);
    AddObject(exitBtnGO);
	

    optionsGroupGO->AddComponent(&optionsGroup);
    RectTransform* optionsGroupRect = new RectTransform(*optionsGroupGO,canvasGO);
    optionsGroupRect->SetAnchors(Vec2(0.3, 0.45),Vec2(0.7, 0.9));
	
    //playBtn.ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);//O que fazer aqui?
    playBtn.SetCallback(Button::State::ENABLED, { [] (void* caller) {
                                                      TitleState* titleState = static_cast<TitleState*>(caller);
                                                      titleState->Play();
                                                                    }, this } );
	
    //editorBtn.ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);//O que fazer aqui?
	
    //configBtn.ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);//O que fazer aqui?
	
    //exitBtn.ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);//O que fazer aqui?
    exitBtn.SetCallback(Button::State::ENABLED, { [] (void* caller) {
                                                       TitleState* titleState = static_cast<TitleState*>(caller);
                                                       titleState->Exit();
                                                                     }, this } );

    optionsGroup.groupedElements.push_back(playBtnGO);
    optionsGroup.groupedElements.push_back(editorBtnGO);
    optionsGroup.groupedElements.push_back(configBtnGO);
    optionsGroup.groupedElements.push_back(exitBtnGO);

	titleMusic.Play(0);
}

void TitleState::Update(float dt) {
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

/* Não funciona mais.Avaliar como reimplementar isso
void TitleState::RenderUI(void) const {
	bg.Render();
	lua.Render();
	nuvemA.Render();
	nuvemB.Render();
	icc.Render();
	overlay.Render();
	if(finishedEclipse) {
		title.Render();
		if(finishedFadeIn) {
			// optionsGroup.Render(true);
			playBtn.Render();
			editorBtn.Render();
			configBtn.Render();
			exitBtn.Render();
		}
	}
}*/

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

void TitleState::Render() const{}
