#ifndef ENDSTATE_H
#define ENDSTATE_H

#include "EndStateData.h"
#include "GameObject.h"
#include "Music.h"
#include "Rect.h"
#include "Sprite.h"
#include "Text.h"
#include "Vec2.h"
#include "State.h"
#include "RectTransform.h"
#include "Button.h"
#include "Grouper.h"

using namespace RattletrapEngine;

#define END_STATE_FONT_SIZE (40)
#define END_STATE_DELTA_VOLUME (1) //11*11 = 121 ~128

class EndState: public State {
	public:
		EndState(EndStateData stateData);
		void Update(float dt);
		void Render() const;
		void Pause();
		void Resume();
		void StartLoop(void);
		void LoadAssets(void) const;
	private:
		void SetupUI(EndStateData stateData);
		void Close(void);
		void MainMenu(void);
		Music music;
		Music intro;

        GameObject* HUDcanvasGO;
        GameObject* menuBgGO;
        GameObject* venceuTextGO;
        GameObject* optionsGroupGO;
        GameObject* playBtnGO;
        GameObject* exitBtnGO;

        RectTransform* HUDcanvasRect;
        RectTransform* menuBgRect;
        RectTransform* venceuTextRect;
        RectTransform* optionsGroupRect;
        RectTransform* playBtnRect;
        RectTransform* exitBtnRect;

        Button exitBtn;
        Button playBtn;
        Grouper optionsGroup;
};

#endif // ENDSTATE_H
