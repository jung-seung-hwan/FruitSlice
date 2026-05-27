#pragma once
#include "NzWndBase.h"
#include "Utillity.h"

class GameTimer;
class GameObjectBase;
class GameObject;

namespace renderHelp
{
    class BitmapInfo;
}

// NzWndBase 클래스 상속
class MyFirstWndGame : public NzWndBase
{
public:
    MyFirstWndGame() = default;
    // NzWndBase 해제 시 MyFirstWndGame도 해제
    ~MyFirstWndGame() override = default;

    bool Initialize();
    void Run();
    void Finalize();

private:
    void Update();
    void Render();

    // override로 부모의 동작을 교체
    void OnResize(int width, int height) override;
    void OnClose() override;

    void OnMouseMove(int x, int y);
    void OnLButtonDown(int x, int y);
    void OnRButtonDown(int x, int y);

    void FixedUpdate();
    void LogicUpdate();
    void HandlePlayerEnemyCollision();

    void CreatePlayer();
    void CreateEnemy();

    void UpdatePlayerInfo();
    void UpdateEnemyInfo();
    void EnemyCollision();

    GameObject* GetPlayer() const { return (GameObject*)m_GameObjectPtrTable[0]; }
    int m_mouseCheck = 0;

private:
    HDC m_hFrontDC = nullptr;
    HDC m_hBackDC = nullptr;
    HBITMAP m_hBackBitmap = nullptr;
    HBITMAP m_hDefaultBitmap = nullptr;


    // 게임 타이머를 사용하여 프레임을 관리하는 예시.
    GameTimer* m_pGameTimer = nullptr;
    float m_fDeltaTime = 0.0f;
    float m_fFrameCount = 0.0f;

    // 게임 오브젝트를 관리하는 컨테이너.
    GameObjectBase** m_GameObjectPtrTable = nullptr;

    struct MOUSE_POS
    {
        int x = 0;
        int y = 0;

        bool operator!=(const MOUSE_POS& other) const
        {
            return (x != other.x || y != other.y);
        }
    };

    MOUSE_POS m_MousePos = { 0, 0 };
    MOUSE_POS m_MousePosPrev = { 0, 0 };

    MOUSE_POS m_PlayerTargetPos = { 0, 0 };
    MOUSE_POS m_EnemySpawnPos = { 0, 0 };

    using BitmapInfo = renderHelp::BitmapInfo;
    BitmapInfo* m_pPlayerBitmapInfo = nullptr;
    BitmapInfo* m_pEnemyBitmapInfo = nullptr;

};