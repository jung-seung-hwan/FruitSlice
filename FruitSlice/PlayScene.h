#pragma once
#include "Scene.h"
#include "RenderHelp.h"

namespace renderHelp
{
    class BitmapInfo;
}

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

private:
    using BitmapInfo = renderHelp::BitmapInfo;
    BitmapInfo* m_pBackgroundBitmapInfo = nullptr;

    int m_screenWidth = 1024;
    int m_screenHeight = 720;
};