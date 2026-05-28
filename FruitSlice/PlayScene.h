#pragma once
#include "Scene.h"

class PlayScene : public Scene
{
public:
    PlayScene() = default;
    ~PlayScene() override = default;

    void Initialize() override;
    void Finalize() override;

    void Enter() override;
    void Leave() override;

    void Update(float deltaTime) override;
    void Render(HDC hDC) override;
};