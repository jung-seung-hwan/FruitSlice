#pragma once

#include "Scene.h"

class TitleScene : public Scene
{
public:
    TitleScene() = default;
    ~TitleScene() override = default;

    void Initialize() override;
    void Finalize() override;

    void Enter() override;
    void Leave() override;

    void Update(float deltaTime) override;
    void Render(HDC hDC) override;
};

