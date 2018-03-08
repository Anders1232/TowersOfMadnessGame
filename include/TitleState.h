#ifndef TITLESTATE_H
#define TITLESTATE_H

#include "ActionManager.h"
#include "Rect.h"
#include "State.h"
#include "Sound.h"
#include "Music.h"
#include "Button.h"
#include "Grouper.h"
#include "Timer.h"
#include "RectTransform.h"

using namespace RattletrapEngine;

class TitleState: public State {
	public:
		TitleState(void);
		void Update(float dt);
		void Render(void) const;
		void Pause(void);
		void Resume(void);
		void Play(void);
		void Exit(void);
		void LoadAssets(void) const;
	private:
		void SetupUI(void);
		void UpdateUI(float dt);
		void RenderUI() const;
		void MoveClouds(float dt);
		float speedNuvemA;
		float speedNuvemB;

		Timer introTimer;
        Music titleMusic;
        Sound clickSound;

        GameObject* canvasGO;
        GameObject* bgGO;
        GameObject* luaGO;
        GameObject* nuvemAGO;
        GameObject* nuvemBGO;
        GameObject* iccGO;
        GameObject* overlayGO;
        GameObject* titleGO;
        GameObject* optionsGroupGO;
        GameObject* playBtnGO;
        GameObject* editorBtnGO;
        GameObject* configBtnGO;
        GameObject* exitBtnGO;

        Grouper optionsGroup;

        Button playBtn;
        Button editorBtn;
        Button configBtn;
        Button exitBtn;

        bool finishedEclipse;
        bool finishedFadeIn;
        bool forceEnd;

};

#endif // TITLESTATE_H
