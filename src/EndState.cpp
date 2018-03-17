#include "EndState.h"
#include "InputManager.h"
#include "StageState.h"
#include "Camera.h"

#include "Game.h"

#include <string>

#define COLOR_WIN		{183,156,245,255}
#define COLOR_LOSE		{132,132,132,100}

#define DISABLED_COLOR		{ 70, 70, 70,100} // Dark Gray
#define ENABLED_COLOR		{183,156,245,255} // Purple
#define HIGHLIGHTED_COLOR	{227,196,230,255} // Purple-ish white
#define PRESSED_COLOR		{227,196,230,255} // Purple-ish white

// QUE GAMBIARRA ABSURDA D:
void StartFinalLoop() {
	static_cast<EndState&>(Game::GetInstance().GetCurrentState()).StartLoop();
}

EndState::EndState(EndStateData stateData)
		: music("audio/tela_de_vitoria_derrota/loop_tela_vitoria_derrota.ogg")
        , intro( (stateData.playerVictory) ? "audio/tela_de_vitoria_derrota/vitoria.ogg" : "audio/tela_de_vitoria_derrota/derrota.ogg")
        ,HUDcanvasGO(new GameObject)
        ,menuBgGO(new GameObject)
        ,venceuTextGO(new GameObject)
        ,optionsGroupGO(new GameObject)
        ,playBtnGO(new GameObject)
        ,exitBtnGO(new GameObject)
        ,exitBtn(*exitBtnGO)
        ,playBtn(*playBtnGO)
        ,optionsGroup(*optionsGroupGO){
	Resources::ChangeMusicVolume(0);
	Resources::ChangeSoundVolume(0);

	intro.Play(1);
	Mix_HookMusicFinished(StartFinalLoop);

	SetupUI(stateData);
}

void EndState::SetupUI(EndStateData stateData){

    //HUDcanvas
    HUDcanvasRect = new RectTransform(*HUDcanvasGO,nullptr);
    HUDcanvasGO->AddComponent(HUDcanvasRect);
    //bg
    menuBgRect = new RectTransform(*menuBgGO,HUDcanvasGO);
    menuBgRect->SetBehaviorType(RectTransform::BehaviorType::FIT);
    Sprite* sp = new Sprite(stateData.playerVictory ? "img/UI/end-game/win.jpg" : "img/UI/end-game/lose.jpg",*menuBgGO);
    menuBgGO->AddComponent(menuBgRect);
    menuBgGO->AddComponent(sp);
    //venceuText
    venceuTextRect = new RectTransform(*venceuTextGO,HUDcanvasGO);
    Text* venceuTextComponent = new Text(*venceuTextGO);
    venceuTextComponent->SetText(stateData.playerVictory ? std::string("Vit") + (char)0xF3 /*ó*/ + "ria" : "Derrota");
    venceuTextComponent->SetColor(COLOR_WIN);
    venceuTextComponent->SetFont("font/SHPinscher-Regular.otf");
    venceuTextComponent->SetFontSize(95);
    venceuTextGO->AddComponent(venceuTextRect);
    venceuTextGO->AddComponent(venceuTextComponent);
    //optionsGroup ***Terminar***
    optionsGroupRect = new RectTransform(*optionsGroupGO,HUDcanvasGO);
    optionsGroupGO->AddComponent(optionsGroupRect);
    //playBtn
    playBtnRect = new RectTransform(*playBtnGO,optionsGroupGO);
    Text* t = new Text(*playBtnGO);
    t->SetText("Menu Principal");
    t->SetColor({255,255,255,255});
    t->SetFont("font/SHPinscher-Regular.otf");
    t->SetFontSize(95);
    playBtnGO->AddComponent(playBtnRect);

    //bg.GetSprite().colorMultiplier = {255, 255, 255, 200};

    venceuTextRect->SetAnchors(Vec2(0.2, 0.),Vec2(0.8, 0.4));
	if( stateData.playerVictory) {
        venceuTextComponent->SetColor( COLOR_WIN );
	} else {
        venceuTextComponent->SetColor( COLOR_LOSE );
	}

    //optionsGroupRect.padding = 30.;
    optionsGroupRect->SetAnchors(Vec2(0.15, 0.5),Vec2(0.85, 0.8));
    optionsGroup.groupedElements.push_back(playBtnGO);
    optionsGroup.groupedElements.push_back(exitBtnGO);

    //playBtn.ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);
    playBtn.SetReleaseCallback({ [] (void* ptr) {
                                    EndState* endState = static_cast<EndState*>(ptr);
									endState->MainMenu();
                                },this} );
    //exitBtn("font/SHPinscher-Regular.otf", 95, Text::TextStyle::BLENDED, {255,255,255,255}, "Sair")
    exitBtnRect = new RectTransform(*exitBtnGO,optionsGroupGO);
    Text* exitBtnTextComponent = new Text(*exitBtnGO);
    exitBtnTextComponent->SetText(stateData.playerVictory ? std::string("Vit") + (char)0xF3 /*ó*/ + "ria" : "Derrota");
    exitBtnGO->AddComponent(exitBtnTextComponent);
    exitBtnGO->AddComponent(exitBtnRect);
    //exitBtn.ConfigColors(DISABLED_COLOR, ENABLED_COLOR, HIGHLIGHTED_COLOR, PRESSED_COLOR);

    exitBtn.SetReleaseCallback({[] (void* ptr) {
                                    EndState* endState = static_cast<EndState*>(ptr);
									endState->Close();
                                },this} );
}

void EndState::Update(float dt) {
	if(INPUT_MANAGER.QuitRequested()) {
		quitRequested = true;
	}

    State::Update(dt);
}

void EndState::Render(){

}

void EndState::Pause(){}

void EndState::Resume() {
	Camera::ForceLogZoom(0.0);
	Camera::pos = Vec2(0, 0);
}

void EndState::StartLoop() {
	music.Play(0);
	Mix_HookMusicFinished(NULL);
}

void EndState::LoadAssets(void) const{
	Resources::GetImage("img/UI/end-game/win.jpg");
	Resources::GetImage("img/UI/end-game/lose.jpg");
	Resources::GetFont("font/SHPinscher-Regular.otf", 32);
	Resources::GetMusic("audio/tela_de_vitoria_derrota/vitoria.ogg");
	Resources::GetMusic("audio/tela_de_vitoria_derrota/derrota.ogg");
	Resources::GetMusic("audio/tela_de_vitoria_derrota/loop_tela_vitoria_derrota.ogg");
}

void EndState::Close(void) {
	quitRequested = true;
}

void EndState::MainMenu(void) {
	popRequested = true;
}
