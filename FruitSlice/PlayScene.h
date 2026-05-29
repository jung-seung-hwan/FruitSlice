#pragma once
#include "Scene.h"
#include "RenderHelp.h"
#include <vector>    
#include <windows.h> 
#include "FruitSpawner.h"

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
    BitmapInfo* m_pSlashInfo = nullptr;
    BitmapInfo* m_pAppleBitmapInfo = nullptr;

    int m_screenWidth = 1024;
    int m_screenHeight = 720;

    // 직선 베기 변수
    bool m_bIsDragging = false;     // 드래그 여부
    POINT m_ptDragStart = { 0, 0 }; // 베기 시작 좌표
    POINT m_ptDragEnd = { 0, 0 };   // 베기 끝(현재 마우스) 좌표

    // 과일 메모리 풀과 스포너를 씬이 직접 관리
    ObjectPool<Fruit>* m_pFruitPool = nullptr;
    FruitSpawner* m_pFruitSpawner = nullptr;
};