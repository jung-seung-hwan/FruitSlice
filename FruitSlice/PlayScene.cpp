#include "PlayScene.h"
#include "GameObject.h"

void PlayScene::Initialize()
{
    // 리소스 로드 등 1회성 초기화
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
}