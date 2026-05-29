#include "PlayScene.h"
#include "GameObject.h"
#include "Collider.h"

void PlayScene::Initialize()
{
    // 리소스 로드 (배경)
    m_pBackgroundBitmapInfo = renderHelp::CreateBitmapInfo(L"./Resource/background.png");
}

void PlayScene::Enter()
{
    // 씬 진입 시 초기 설정
}

void PlayScene::Update(float deltaTime)
{
    for (auto* pObj : m_GameObjects)
    {
        if (pObj && pObj->IsActive()) pObj->Update(deltaTime);
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
                    // 과일 객체가 자신의 ColliderCircle 정보를 반환
                    GameObject* pGameObj = static_cast<GameObject*>(pObj);

                    // 원형 충돌체 정보를 가져옴
                    const learning::ColliderCircle* pCircle = pGameObj->GetColliderCircle();

                    // 만들어둔 Intersect 함수로 충돌 검사
                    if (learning::Intersect(slashLine, *pCircle))
                    {
                        // 충돌 성공 처리
                        pObj->SetActive(false);

                        // TODO: 이곳에 과일이 반으로 갈라지는 이펙트나 점수 증가 로직을 추가
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

    //  배경 위에 과일, 파편, 이펙트 등 활성화된 객체 렌더링
    for (auto* pObj : m_GameObjects)
    {
        if (pObj && pObj->IsActive())
        {
            pObj->Render(hDC);
        }
    }
}

void PlayScene::Leave()
{
    // 씬 전환 시 객체들 비활성화 또는 초기화
    for (auto* pObj : m_GameObjects)
    {
        delete pObj;
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
}