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

		GameObject* luaGO;
		GameObject* overlayGO;
		GameObject* titleGO;
		GameObject* nuvemAGO;
		GameObject* nuvemBGO;

		bool finishedEclipse;
		bool finishedFadeIn;
		bool forceEnd;

};

#endif // TITLESTATE_H
