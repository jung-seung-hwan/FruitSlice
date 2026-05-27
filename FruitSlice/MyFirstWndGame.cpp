// 가능하면 타이머 함수 개별로 만들어 보기
#include "MyFirstWndGame.h"
#include "GameTimer.h"
#include "Collider.h"
#include "GameObject.h"
#include <iostream>
#include <assert.h>
#include "RenderHelp.h"

using namespace learning;

constexpr int MAX_GAME_OBJECT_COUNT = 1000;

// 게임 시작 호출
bool MyFirstWndGame::Initialize()
{
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

    m_GameObjectPtrTable = new GameObjectBase * [MAX_GAME_OBJECT_COUNT];

    GameObject::InitializeResources();

    // memset or ZeroMemory로 for문으로 개별 배열에 넣는게 아닌 한번에 null로 초기화 가능
    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        m_GameObjectPtrTable[i] = nullptr;
    }

#pragma region resource
    // IDE에서 인지하는 현재 경로와 실제 실행 파일을 바로 실행했을 때의 경로 기준이 다름
    m_pPlayerBitmapInfo = renderHelp::CreateBitmapInfo(L"./Resource/redbird.png");
    m_pEnemyBitmapInfo = renderHelp::CreateBitmapInfo(L"./Resource/graybird.png");

#pragma endregion

    // [CHECK]. 첫 번째 게임 오브젝트는 플레이어 캐릭터로 고정!
    CreatePlayer();

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

    if (m_GameObjectPtrTable)
    {
        for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
        {
            if (m_GameObjectPtrTable[i])
            {
                delete m_GameObjectPtrTable[i];
                m_GameObjectPtrTable[i] = nullptr;
            }
        }
        delete m_GameObjectPtrTable;
    }

    GameObject::ReleaseResources();
    __super::Destroy();
}

void MyFirstWndGame::FixedUpdate()
{
    if (m_EnemySpawnPos.x != 0 && m_EnemySpawnPos.y != 0)
    {
        CreateEnemy();
    }
}

// 플레이어와 적의 충돌상태 점검
void MyFirstWndGame::HandlePlayerEnemyCollision()
{
    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        GameObject* pObj = static_cast<GameObject*>(m_GameObjectPtrTable[i]);
        if (pObj)
        {
            pObj->SetColliding(false);
        }
    }

    GameObject* pPlayer = GetPlayer();
    if (pPlayer == nullptr)
        return;

    for (int i = 1; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        GameObject* pOther = static_cast<GameObject*>(m_GameObjectPtrTable[i]);
        if (pOther == nullptr)
            continue;

        if (pOther->Type() != ObjectType::ENEMY)
            continue;

        if (pPlayer->IsCollidingWith(pOther))
        {
            pPlayer->SetColliding(true);
            pOther->SetColliding(true);
        }
    }

}

void MyFirstWndGame::EnemyCollision()
{
    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        GameObject* pObj = static_cast<GameObject*>(m_GameObjectPtrTable[i]);
        if (pObj == nullptr)
            continue;

        if (pObj->Type() != ObjectType::ENEMY)
            continue;

        for (int j = i + 1; j < MAX_GAME_OBJECT_COUNT; ++j)
        {
            GameObject* pOther = static_cast<GameObject*>(m_GameObjectPtrTable[j]);
            if (pOther == nullptr)
                continue;

            if (pOther->Type() != ObjectType::ENEMY)
                continue;

            Vector2f posA = pObj->GetPosition();
            Vector2f posB = pOther->GetPosition();

            Vector2f distance = posB - posA;
            float distanceAB = distance.Length();
            float minDis = 50.0f;
            if (distanceAB < minDis && distanceAB > 0.0f)
            {
                float overlap = minDis - distanceAB; // 겹친 깊이
                distance.Normalize();

                // 겹친 만큼 절반씩 양쪽으로 강제 이동시켜 밀어냄
                Vector2f pushOut = distance * (overlap * 0.5f);

                Vector2f newPosA = posA - pushOut;
                Vector2f newPosB = posB + pushOut;

                pObj->SetPosition(newPosA.x, newPosA.y);
                pOther->SetPosition(newPosB.x, newPosB.y);
            }

        }
    }
}


void MyFirstWndGame::LogicUpdate()
{

    UpdatePlayerInfo();
    UpdateEnemyInfo();

    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        if (m_GameObjectPtrTable[i])
        {
            m_GameObjectPtrTable[i]->Update(m_fDeltaTime);
        }
    }

    HandlePlayerEnemyCollision();
    EnemyCollision();

}

void MyFirstWndGame::CreatePlayer()
{
    // 디버깅 코드 -> 배포시에는 동작하지 않음
    assert(m_GameObjectPtrTable[0] == nullptr && "Player object already exists!");

    GameObject* pNewObject = new GameObject(ObjectType::PLAYER);

    pNewObject->SetName("Player");
    //pNewObject->SetPosition(0.0f, 0.0f); // 일단, 임의로 설정 

    //창의 중앙에 배치 -> NzWndBase를 상속받았기 때문에 GetWidth, GetHeight 사용 가능
    float centerX = static_cast<float>(GetWidth()) / 2.0f;
    float centerY = static_cast<float>(GetHeight()) / 2.0f;

    pNewObject->SetPosition(centerX, centerY);
    // 기존 초기화되어있는 플레이어의 기본위치가 0,0 이므로 중앙값으로 변경
    m_PlayerTargetPos.x = static_cast<int>(centerX);
    m_PlayerTargetPos.y = static_cast<int>(centerY);
    pNewObject->SetSpeed(1.0f); // 일단, 임의로 설정   

    pNewObject->SetColliderCircle(50.0f); // 일단, 임의로 설정. 오브젝트 설정할 거 다 하고 나서 하자.
    pNewObject->SetBitmapInfo(m_pPlayerBitmapInfo);

    pNewObject->SetWidth(100);
    pNewObject->SetHeight(100);

    m_GameObjectPtrTable[0] = pNewObject;
}

void MyFirstWndGame::CreateEnemy()
{
    GameObject* pNewObject = new GameObject(ObjectType::ENEMY);

    pNewObject->SetName("Enemy");

    float x = m_EnemySpawnPos.x;
    float y = m_EnemySpawnPos.y;

    m_EnemySpawnPos = { 0, 0 };

    pNewObject->SetPosition(x, y);
    pNewObject->SetSpeed(0.1f); // 일단, 임의로 설정   

    pNewObject->SetColliderCircle(50.0f); // 일단, 임의로 설정. 오브젝트 설정할 거 다 하고 나서 하자.
    pNewObject->SetBitmapInfo(m_pEnemyBitmapInfo);

    pNewObject->SetWidth(100);
    pNewObject->SetHeight(100);

    int i = 0;
    bool isOverlapping = false;

    // 충돌 검사 
    for (int j = 0; j < MAX_GAME_OBJECT_COUNT; ++j)
    {
        GameObject* pOther = static_cast<GameObject*>(m_GameObjectPtrTable[j]);
        if (pOther == nullptr)
            continue;

        if (pNewObject->IsCollidingWith(pOther))
        {
            isOverlapping = true;
            break;
        }
    }

    if (isOverlapping)
    {
        delete pNewObject;
        return;
    }

    while (++i < MAX_GAME_OBJECT_COUNT) //0번째는 언제나 플레이어!
    {
        if (nullptr == m_GameObjectPtrTable[i])
        {
            m_GameObjectPtrTable[i] = pNewObject;
            break;
        }
    }

    if (i == MAX_GAME_OBJECT_COUNT)
    {
        // 게임 오브젝트 테이블이 가득 찼습니다.
        delete pNewObject;
        pNewObject = nullptr;
    }
}

void MyFirstWndGame::UpdatePlayerInfo()
{
    static GameObject* pPlayer = GetPlayer();

    assert(pPlayer != nullptr);

    Vector2f mousePos(m_PlayerTargetPos.x, m_PlayerTargetPos.y);
    Vector2f playerPos = pPlayer->GetPosition();

    Vector2f playerDir = mousePos - playerPos;
    float distance = playerDir.Length(); // 거리 계산

    if (distance > 50.f) //임의로 설정한 거리
    {
        playerDir.Normalize(); // 정규화
        pPlayer->SetDirection(playerDir); // 플레이어 방향 설정
    }
    else
    {
        pPlayer->SetDirection(Vector2f(0, 0)); // 플레이어 정지
    }
}

// 플레이어 이동 시 적이 추적
void MyFirstWndGame::UpdateEnemyInfo()
{
    if (m_mouseCheck < 1)
    {
        return;
    }

    // 플레이어 객체를 가져와 현재 위치를 목표 지점으로 설정
    GameObject* pPlayer = GetPlayer();
    if (pPlayer == nullptr) return;

    Vector2f playerPos = pPlayer->GetPosition();

    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        GameObject* pOther = static_cast<GameObject*>(m_GameObjectPtrTable[i]);

        if (pOther == nullptr) continue;
        if (pOther->Type() != ObjectType::ENEMY) continue;

        Vector2f enemyPos = pOther->GetPosition();
        // 최종 이동 방향을 누적하기 위한 벡터 초기화
        Vector2f finalDir = { 0.0f, 0.0f };

        // 목표(플레이어)를 향하는 방향 벡터와 거리 계산
        Vector2f enemyDir = playerPos - enemyPos;
        float distanceToPlayer = enemyDir.Length();

        // 플레이어 방향으로 이동
        if (distanceToPlayer > 100.0f)
        {
            enemyDir.Normalize();
            // 정규화된 방향 벡터로 이동
            // 방향 벡터만 설정하고, 실제 이동 속도는 GameObject의 Update에서 적용된다고 가정
            finalDir = finalDir + enemyDir;
        }
        else
        {
            pOther->SetDirection(Vector2f(0, 0));
        }
        // 주변 적들과의 거리를 벌리기 위한 분리 방향 벡터 초기화
        Vector2f separationDir = { 0.0f, 0.0f };
        int enemyCount = 0;

        for (int j = 1; j < MAX_GAME_OBJECT_COUNT; ++j)
        {
            // 나 자신과의 연산은 제외
            if (i == j) continue;

            // 다른 적과의 충돌계산
            GameObject* pOtherEnemy = static_cast<GameObject*>(m_GameObjectPtrTable[j]);
            if (pOtherEnemy == nullptr || pOtherEnemy->Type() != ObjectType::ENEMY) continue;

            Vector2f otherPos = pOtherEnemy->GetPosition();

            // 적끼리의 방향 벡터와 거리 계산
            Vector2f dirEnemy = enemyPos - otherPos;
            float distanceToEnemy = dirEnemy.Length();

            // 적이 100거리 이내에 있을때 동작
            if (distanceToEnemy > 0.0f && distanceToEnemy < 100.0f)
            {
                dirEnemy.Normalize();
                separationDir = separationDir + dirEnemy;
                enemyCount++;
            }
        }

        // 적이 하나 이상일때 동작
        if (enemyCount > 0)
        {
            // 회피 가중치를 부여해 플레이어 추적보다 적끼리 겹치지 않는것을 우선 연산
            separationDir = separationDir * 1.5f;
            finalDir = finalDir + separationDir;
        }

        if (finalDir.Length() > 0.0f)
        {
            finalDir.Normalize();
            // 최종방향에 대한 위치 계산
            Vector2f nextPos = enemyPos + finalDir;
            Vector2f nextDirToPlayer = playerPos - nextPos;

            // 밀려난 거리가 99 이내면 정지
            if (nextDirToPlayer.Length() < 99.0f)
            {
                pOther->SetDirection(Vector2f(0.0f, 0.0f));
            }
            else
            {
                pOther->SetDirection(finalDir);
            }
        }
        else
        {
            pOther->SetDirection(Vector2f(0.0f, 0.0f));
        }
    }

}


void MyFirstWndGame::Update()
{
    m_pGameTimer->Tick();

    // 가변 프레임 처리 -> 화면 갱신되는 횟수와 동일
    LogicUpdate();

    m_fDeltaTime = m_pGameTimer->DeltaTimeMS();
    m_fFrameCount += m_fDeltaTime;

    //고정 프레임 처리 -> 매 프레임의 시간을 누적하고 누적한 시간에 따라 fixedupdate를 반복해서 실행 -> 시간이 200.0f만 지났으면 1회 600.0f 지났으면 3회 반복
    while (m_fFrameCount >= 200.0f)
    {
        FixedUpdate();
        m_fFrameCount -= 200.0f;
    }
}

void MyFirstWndGame::Render()
{
    //Clear the back buffer
    ::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);

    //메모리 DC에 그리기
    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        if (m_GameObjectPtrTable[i])
        {
            m_GameObjectPtrTable[i]->Render(m_hBackDC);
        }
    }

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

    m_PlayerTargetPos.x = x;
    m_PlayerTargetPos.y = y;
    m_mouseCheck++;
}

void MyFirstWndGame::OnRButtonDown(int x, int y)
{
    /*  std::cout << __FUNCTION__ << std::endl;
   std::cout << "x: " << x << ", y: " << y << std::endl;*/

    m_EnemySpawnPos.x = x;
    m_EnemySpawnPos.y = y;
}