#pragma once

#include "Scene.h"
#include "RenderHelp.h"

namespace renderHelp
{
    class BitmapInfo;
}

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

private:
    using BitmapInfo = renderHelp::BitmapInfo;
    BitmapInfo* m_pBackgroundBitmapInfo = nullptr;

    // 시작 버튼 영역 및 상태 관리 변수
    RECT m_startButtonRect = {};
    bool m_isButtonHovered = false;

    int m_screenWidth = 1024;
    int m_screenHeight = 720;
};

