#include "TitleScene.h"
#include "SceneManager.h"
#include "GameObject.h"

void TitleScene::Initialize()
{
    // 배경, 로고, 버튼 객체 생성 및 m_GameObjects 배열에 추가
}

void TitleScene::Enter()
{
    // 타이틀 BGM 재생 등
}

void TitleScene::Update(float deltaTime)
{
    // UI 객체들 애니메이션(깜빡임 등) 업데이트
    for (auto* pObj : m_GameObjects)
    {
        if (pObj && pObj->IsActive()) pObj->Update(deltaTime);
    }

    // 마우스 클릭 시 화면 전환 로직

}

void TitleScene::Render(HDC hDC)
{
    for (auto* pObj : m_GameObjects)
    {
        if (pObj && pObj->IsActive()) pObj->Render(hDC);
    }

}