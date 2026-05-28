#include "SceneManager.h"
#include "TitleScene.h"
#include "PlayScene.h"
#include "EndingScene.h"

void SceneManager::Initialize()
{
    // 각 씬 인스턴스화 및 초기화
    m_arrScenes[static_cast<int>(SceneType::TITLE)] = new TitleScene();
    m_arrScenes[static_cast<int>(SceneType::PLAY)] = new PlayScene();
    m_arrScenes[static_cast<int>(SceneType::ENDING)] = new EndingScene();

    for (int i = 0; i < static_cast<int>(SceneType::MAX); ++i)
    {
        if (m_arrScenes[i] != nullptr)
        {
            m_arrScenes[i]->Initialize();
        }
    }

    // 초기 씬 설정
    ChangeScene(SceneType::TITLE);
}

void SceneManager::Update(float deltaTime)
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->Update(deltaTime);
    }
}

void SceneManager::Render(HDC hDC)
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->Render(hDC);
    }
}

void SceneManager::ChangeScene(SceneType type)
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->Leave(); // 기존 씬 종료 처리
    }

    m_pCurrentScene = m_arrScenes[static_cast<int>(type)];

    if (m_pCurrentScene)
    {
        m_pCurrentScene->Enter(); // 새 씬 진입 처리
    }
}

// 전체 Scene 종료
void SceneManager::Finalize()
{
    for (int i = 0; i < static_cast<int>(SceneType::MAX); ++i)
    {
        if (m_arrScenes[i] != nullptr)
        {
            m_arrScenes[i]->Finalize();
            delete m_arrScenes[i];
            m_arrScenes[i] = nullptr;
        }
    }
}