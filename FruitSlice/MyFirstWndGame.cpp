#include "MyFirstWndGame.h"
#include "GameTimer.h"
#include "Collider.h"
#include "GameObject.h"
#include <iostream>
#include <assert.h>
#include "RenderHelp.h"
#include "SceneManager.h"
#include <time.h>

using namespace learning;

// 게임 시작 호출
bool MyFirstWndGame::Initialize()
{
    srand(static_cast<unsigned int>(time(NULL)));

    m_pGameTimer = new GameTimer();
    m_pGameTimer->Reset();

    const wchar_t* className = L"FruitSliceGame";
    const wchar_t* windowName = L"FruitSliceGame";

    if (false == __super::Create(className, windowName, 1024, 720))
    {
        return false;
    }


    RECT rcClient = {};
    GetClientRect(m_hWnd, &rcClient);
    m_width = rcClient.right - rcClient.left;
    m_height = rcClient.bottom - rcClient.top;

    m_hFrontDC = GetDC(m_hWnd);
    m_hBackDC = CreateCompatibleDC(m_hFrontDC);
    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

    m_hDefaultBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

    // SceneManager 초기화 및 첫 씬 진입 설정
    SceneManager::GetInstance().Initialize();

    return true;

}

void MyFirstWndGame::Run()
{
    MSG msg = { 0 };
    //종료 메시지 오기 전까지 무한히 루프 반복
    // window  메세지는 메세지를 큐에다 넣고 가져오는 방식 -> 느림
    // 더 빠른 처리를 위해 장치에서 큐에다 넣지 않고 바로 가져와서 더 빠른 처리를 진행 
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                MyFirstWndGame::OnLButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_RBUTTONDOWN)
            {
                MyFirstWndGame::OnRButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_MOUSEMOVE)
            {
                MyFirstWndGame::OnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            Update();
            Render();
        }
    }
}

void MyFirstWndGame::Finalize()
{
    delete m_pGameTimer;
    m_pGameTimer = nullptr;

    SceneManager::GetInstance().Finalize();
    GameObject::ReleaseResources();
    __super::Destroy();
}

void MyFirstWndGame::FixedUpdate()
{
    if (m_EnemySpawnPos.x != 0 && m_EnemySpawnPos.y != 0)
    {
    }
}




void MyFirstWndGame::LogicUpdate()
{
    // 씬 매니저의 Update를 호출하여 현재 씬 내부의 객체들이 갱신되도록 위임
    SceneManager::GetInstance().Update(m_fDeltaTime);
}

void MyFirstWndGame::Update()
{
    m_pGameTimer->Tick();

    // 가변 프레임 처리 -> 화면 갱신되는 횟수와 동일
    LogicUpdate();

    m_fDeltaTime = m_pGameTimer->DeltaTime();
    LogicUpdate();
    m_fFrameCount += m_fDeltaTime;

    //고정 프레임 처리 -> 매 프레임의 시간을 누적하고 누적한 시간에 따라 fixedupdate를 반복해서 실행 -> 시간이 200.0f만 지났으면 1회 600.0f 지났으면 3회 반복
    while (m_fFrameCount >= 0.2f)
    {
        FixedUpdate();
        m_fFrameCount -= 0.2f;
    }
}

void MyFirstWndGame::Render()
{
    //Clear the back buffer
    ::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);

    // Scene에 그림을 그릴 공간 정보 전달
    SceneManager::GetInstance().Render(m_hBackDC);

    //메모리 DC에 그려진 결과를 실제 DC(m_hFrontDC)로 복사
    BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}

void MyFirstWndGame::OnResize(int width, int height)
{
    std::cout << __FUNCTION__ << std::endl;

    learning::SetScreenSize(width, height);

    __super::OnResize(width, height);

    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

    HANDLE hPrevBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

    DeleteObject(hPrevBitmap);
}

void MyFirstWndGame::OnClose()
{
    std::cout << __FUNCTION__ << std::endl;

    SelectObject(m_hBackDC, m_hDefaultBitmap);

    DeleteObject(m_hBackBitmap);
    DeleteDC(m_hBackDC);

    ReleaseDC(m_hWnd, m_hFrontDC);
}

void MyFirstWndGame::OnMouseMove(int x, int y)
{
    /*   std::cout << __FUNCTION__ << std::endl;
       std::cout << "x: " << x << ", y: " << y << std::endl;*/
    m_MousePosPrev = m_MousePos;
    m_MousePos = { x, y };
}

void MyFirstWndGame::OnLButtonDown(int x, int y)
{
    /*  std::cout << __FUNCTION__ << std::endl;
 std::cout << "x: " << x << ", y: " << y << std::endl;*/

    m_mouseCheck++;
}

void MyFirstWndGame::OnRButtonDown(int x, int y)
{
    /*  std::cout << __FUNCTION__ << std::endl;
   std::cout << "x: " << x << ", y: " << y << std::endl;*/

}