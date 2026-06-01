#include "PlayScene.h"
#include "GameObject.h"
#include "Collider.h"
#include "Fruit.h"
#include "ObjectPool.h"
#include "FruitSpawner.h"
#include <string>

static int s_highScore = 0;

void PlayScene::Initialize()
{
    // 리소스 로드 (배경)
    m_pBackgroundBitmapInfo = renderHelp::CreateBitmapInfo(L"./Resource/background.png");

    // 과일 이미지 로드
    m_pAppleBitmapInfo = renderHelp::CreateBitmapInfo(L"./Resource/Fruits.png");

    // 오브젝트 풀 및 스포너 생성
    m_pFruitPool = new ObjectPool<Fruit>(15);
    m_pFruitSpawner = new FruitSpawner(m_pFruitPool);

    // 풀에 생성된 모든 과일을 씬의 렌더링 리스트에 미리 등록
    const std::vector<Fruit*>& poolItems = m_pFruitPool->GetAllObjects();

    for (Fruit* pFruit : poolItems)
    {
        pFruit->SetBitmapInfo(m_pAppleBitmapInfo);
        m_GameObjects.push_back(pFruit);
    }
    m_score = 0;
    m_missedCount = 0;
    m_bGameOver = false;

}

void PlayScene::Enter()
{
    // 씬 진입 시 초기 설정
    Fruit* pTestFruit = new Fruit();
    pTestFruit->OnSpawn();
    m_GameObjects.push_back(pTestFruit);
}

void PlayScene::Update(float deltaTime)
{
    if (m_bGameOver)
    {
        // R 키를 누르면 재시작
        if ((GetAsyncKeyState('R') & 0x8000) != 0)
        {
            // 상태 변수 초기화 (최고 점수 제외)
            m_score = 0;
            m_missedCount = 0;
            m_bGameOver = false;

            // 화면에 남아있는 모든 과일 객체 강제 회수
            for (auto* pObj : m_GameObjects)
            {
                if (pObj && pObj->IsActive())
                {
                    pObj->SetActive(false);
                    pObj->OnDespawn();
                }
            }
        }
        return; // 게임 오버 상태일 때는 아래 물리 연산을 진행하지 않음
    }

    if (m_pFruitSpawner)
    {
        m_pFruitSpawner->Update(deltaTime);
    }

    // 객체 업데이트 및 화면 이탈 관리
    for (auto* pObj : m_GameObjects)
    {
        if (pObj && pObj->IsActive())
        {
            pObj->Update(deltaTime);

            Fruit* pFruit = dynamic_cast<Fruit*>(pObj);
            if (pFruit && pFruit->IsOutOfBounds())
            {
                // 안 잘린 상태로 화면 아래로 떨어졌을 경우
                if (!pFruit->IsSliced() && pFruit->GetPosition().y > 720.0f)
                {
                    m_missedCount++;
                    if (m_missedCount >= 5)
                    {
                        m_bGameOver = true;
                    }
                }

                // 화면을 벗어난 객체 회수
                pFruit->SetActive(false);
                pFruit->OnDespawn();
            }
        }
    }

    POINT mousePt;
    GetCursorPos(&mousePt);
    HWND hWnd = FindWindow(L"FruitSliceGame", L"FruitSliceGame");
    if (hWnd)
    {
        ScreenToClient(hWnd, &mousePt);
    }

    bool bIsLButtonPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

    // 직선 그리기
    if (bIsLButtonPressed)
    {
        if (!m_bIsDragging)
        {
            // 마우스 클릭 시 시작점과 끝점을 일치시킴
            m_bIsDragging = true;
            m_ptDragStart = mousePt;
            m_ptDragEnd = mousePt;
        }
        else
        {
            // 끝점만 현재 마우스 좌표로 갱신
            m_ptDragEnd = mousePt;
        }
    }
    else
    {
        if (m_bIsDragging)
        {
            // 마우스 클릭을 뗀 순간
            // 선분과 과일들의 교차 판정을 수행
            learning::ColliderLine slashLine;
            slashLine.startPoint = { static_cast<float>(m_ptDragStart.x), static_cast<float>(m_ptDragStart.y) };
            slashLine.endPoint = { static_cast<float>(m_ptDragEnd.x), static_cast<float>(m_ptDragEnd.y) };

            // 씬에 존재하는 모든 과일 객체와 충돌 검사
            for (auto* pObj : m_GameObjects)
            {
                if (pObj && pObj->IsActive())
                {
                    Fruit* pFruit = dynamic_cast<Fruit*>(pObj);

                    if (pFruit != nullptr)
                    {
                        // 과일 객체에서 원형 충돌체 정보를 가져옴
                        const learning::ColliderCircle* pCircle = pFruit->GetColliderCircle();

                        // 충돌체가 존재하고, 아직 잘리지 않은 과일이며, 선분과 충돌했을 경우 
                        if (pCircle != nullptr && !pFruit->IsSliced() && learning::Intersect(slashLine, *pCircle))
                        {
                            // 과일 파편화
                            pFruit->Slice();

                            // 점수 10점 증가 및 최고 점수 갱신
                            m_score += 10;
                            if (m_score > s_highScore)
                            {
                                s_highScore = m_score;
                            }

                        }
                    }
                }
            }

            m_bIsDragging = false; // 드래그 상태 종료
        }
    }
}

void PlayScene::Render(HDC hDC)
{
    // 배경
    if (m_pBackgroundBitmapInfo)
    {
        // 배경을 복사하기 위한 임시 메모리 공간 생성
        HDC hMemDC = CreateCompatibleDC(hDC);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_pBackgroundBitmapInfo->GetBitmapHandle());

        // 메모리에 올려둔 배경 비트맵을 실제 렌더링될 버퍼로 복사
        BitBlt(hDC, 0, 0, m_pBackgroundBitmapInfo->GetWidth(), m_pBackgroundBitmapInfo->GetHeight(), hMemDC, 0, 0, SRCCOPY);

        // 사용한 자원 반환
        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);
    }

    // 직선 렌더링
    if (m_bIsDragging)
    {
        HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

        // 시작점에서부터 현재 마우스 위치까지만 하나의 직선을 그림
        MoveToEx(hDC, m_ptDragStart.x, m_ptDragStart.y, nullptr);
        LineTo(hDC, m_ptDragEnd.x, m_ptDragEnd.y);

        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);
    }

    //  배경 위에 과일, 파편 등 활성화된 객체 렌더링
    for (auto* pObj : m_GameObjects)
    {
        if (pObj && pObj->IsActive())
        {
            pObj->Render(hDC);
        }
    }

    SetBkMode(hDC, TRANSPARENT);

    // 점수 및 최고 점수 출력
    SetTextColor(hDC, RGB(0, 0, 255));
    std::wstring scoreText = L"Score: " + std::to_wstring(m_score);
    std::wstring highText = L"High Score: " + std::to_wstring(s_highScore);
    TextOutW(hDC, 20, 20, scoreText.c_str(), scoreText.length());
    TextOutW(hDC, 20, 50, highText.c_str(), highText.length());

    // 놓친 횟수 출력
    SetTextColor(hDC, RGB(255, 0, 0));
    std::wstring missText = L"Missed: " + std::to_wstring(m_missedCount) + L" / 5";
    TextOutW(hDC, 20, 80, missText.c_str(), missText.length());

    // 게임 오버 상태일 경우 중앙에 텍스트 출력
    if (m_bGameOver)
    {
        std::wstring overText = L"GAME OVER";
        TextOutW(hDC, 512 - 40, 300, overText.c_str(), overText.length());
    }

    if (m_bGameOver)
    {
        std::wstring restartText = L"Press 'R' to Restart";

        // 텍스트 출력 기준점을 좌상단에서 중앙 상단으로 변경
        SetTextAlign(hDC, TA_CENTER | TA_TOP);

        // 화면 가로 해상도(1024)의 절반인 512를 X축 기준으로 출력
        TextOutW(hDC, 512, 350, restartText.c_str(), restartText.length());

        // 다른 UI가 깨지지 않도록 정렬 기준 초기화
        SetTextAlign(hDC, TA_LEFT | TA_TOP);
    }

}

void PlayScene::Leave()
{
    // 씬 전환 시 객체들 비활성화
    for (auto* pObj : m_GameObjects)
    {
        pObj->SetActive(false);
    }
    m_GameObjects.clear();
}

void PlayScene::Finalize()
{
    // 씬이 완전히 파괴될 때 메모리 해제
    if (m_pBackgroundBitmapInfo)
    {
        delete m_pBackgroundBitmapInfo;
        m_pBackgroundBitmapInfo = nullptr;
    }

    if (m_pAppleBitmapInfo)
    {
        delete m_pAppleBitmapInfo;
        m_pAppleBitmapInfo = nullptr;
    }

    delete m_pFruitSpawner;
    delete m_pFruitPool;
}