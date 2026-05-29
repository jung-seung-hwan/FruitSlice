#include "PlayScene.h"
#include "GameObject.h"

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
    // 과일 스폰 로직 (Spawner)
    // 마우스 궤적 입력 처리 (InputManager)

    // 객체들 갱신
    for (auto* pObj : m_GameObjects)
    {
        if (pObj && pObj->IsActive())
        {
            pObj->Update(deltaTime);
        }
    }

    // 충돌 판정 (과일 vs 마우스 궤적)
}

void PlayScene::Render(HDC hDC)
{
    if (m_pBackgroundBitmapInfo)
    {
        // 배경을 복사하기 위한 임시 메모리 공간 생성
        HDC hMemDC = CreateCompatibleDC(hDC);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_pBackgroundBitmapInfo->GetBitmapHandle());

        // 메모리에 올려둔 배경 비트맵을 실제 렌더링될 버퍼로 복사
        BitBlt(hDC, 0, 0, m_pBackgroundBitmapInfo->GetWidth(), m_pBackgroundBitmapInfo->GetHeight(), hMemDC, 0, 0, SRCCOPY);

        // 사용이 끝난 임시 공간 정리
        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);
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