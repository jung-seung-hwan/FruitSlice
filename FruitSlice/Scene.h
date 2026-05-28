#pragma once
#include "INC_Windows.h"
#include <vector>

class GameObjectBase;

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    virtual void Initialize() = 0;
    virtual void Finalize() = 0;

    virtual void Enter() = 0;
    virtual void Leave() = 0;

    virtual void Update(float deltaTime) = 0;
    virtual void Render(HDC hDC) = 0;

protected:
    std::vector<GameObjectBase*> m_GameObjects;
};