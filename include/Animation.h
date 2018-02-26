#ifndef ANIMATION
#define ANIMATION
#include "GameObject.h"
#include "Component.h"
#include "Sprite.h"
#include "Timer.h"
#include "GameComponentType.h"

using namespace RattletrapEngine;
    /**
        \brief Classe que modela animações.

        As animações são um tipo de Componente que não se colidem e cujo objetivo é basicamente rodar uma animação e depois sumir, a não ser no caso dela ter que ser executada indefinidamente em loop.
    */
class Animation: public Component {
    public:
            /**
                \brief Construtor
                \param x Coordenada horizontal a partir da qual a animação deve ser renderizada.
                \param y Coordenada vertical a partir da qual a animação deve ser renderizada.
                \param rotation Ângulo de rotação da animação.
                \param sprite Arquivo com spriteSheet da animação.
                \param frameCount quantidade de sub-imagens na animação.
                \param frameTime duração para cada frame da animação.
                \param ends Verdadeiro se a animação deve rodar apenas uma vez, falso se a animação deve ser rodar indefinidamente.

                Instancia uma animação com os argumentos dados. E inicializa um timer para contar por quanto tempo a animação deve rodar antes de ser destruída, no caso sela não ter que ser rodada enternamente.
                Caso a animação não deva rodar eternamente o tempo limite para a existência dessa animação é o produto de frameCount por frameTime.
            */
        Animation
            (
                GameObject& associated,
                float x,
                float y,
                float rotation,
                string sprite,
                int frameCount,
                float frameTime,
                bool ends = true
            );
            /**
                \brief Atualiza estado.

                Atualiza o estado da animação e a contagem de tempo do timer.
            */
            void Update(float dt);
            /**
                \brief Renderiza animação.

                Renderiza a animação na tela.
            */
            void Render(void);

            /**
                \brief informa o tipo de Component
                \return Verdadeiro se o argumento for GameComponentType::Animation, falso caso contrário.

                Usado pelos objetos que colidem com a animação para saber que tipo de GameObject é.
            */
            bool Is(int type);
            /**
                \brief Obtém Rect informando a posição renderizada da animação.

                Obtém Rect informando a posição renderizada, computando zoom, escala e posição da câmera.
            */
            Rect GetWorldRenderedRect(void) const;
    private:
            Timer endTimer;/**< Temporizador cronomenta o tempo de existência da animação. Usado para verificar se a animação deve ser destruída.*/
            float timeLimit;/**< Armazena o tempo que animação leva para rodar complemante uma vez. No caso em que a animação não deve rodar em loop esse é o tempo de a animação deve durar.*/
            bool oneTimeOnly;/**< Verdadeiro se essa animação deve ser rodada apenas uma vez, falso se deve ser rodada em loop indefinidamente.*/
            Sprite *sp;/**< Sprite sheet da animação.*/
};

#endif // ANIMATION

