#pragma once
#include "INC_Windows.h"

enum class SceneType
{
    TITLE,
    PLAY,
    MAX
};

class Scene;

// 메인 게임 루프(MyFirstWndGame)가 직접 씬을 관리하지 않고, SceneManager를 통해서만 화면을 갱신
// 메인 게임 루프에서 input, timer 정보만 받아와서 SceneManager에서 사용
class SceneManager
{
public:
    // 싱글톤(Singleton) 패턴 적용
    // SceneManager은 한번만 생성해서 해당 매니저 하나만 계속해서 사용
    static SceneManager& GetInstance()
    {
        static SceneManager instance;
        return instance;
    }

    void Initialize();
    void Finalize();

    void Update(float deltaTime);
    void Render(HDC hDC);

    void ChangeScene(SceneType type);

private:
    SceneManager() = default;
    ~SceneManager() = default;
    // 복사 생성자 및 대입 연산자 삭제 -> 싱글톤 유지를 위한 처리
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

private:
    Scene* m_pCurrentScene = nullptr;

    // 생성된 모든 씬을 보관하는 배열
    Scene* m_arrScenes[static_cast<int>(SceneType::MAX)] = { nullptr };
};