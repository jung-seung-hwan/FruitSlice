#pragma once
#include "GameObject.h"

class Fruit : public GameObject
{
public:
    Fruit();
    ~Fruit() override;

    // 객체 풀 생명주기 오버라이드
    void OnSpawn() override;
    void OnDespawn() override;

    void Update(float deltaTime) override;
    void Render(HDC hdc) override;

private:
    // 충돌체 및 렌더링을 위한 반지름
    float m_radius = 30.0f;

    // 화면 경계 이탈 검사를 위한 해상도 수치
    int m_screenWidth = 1024;
    int m_screenHeight = 720;

    int m_fruitType = 0; // 0:사과, 1:오렌지, 2:복숭아, 3:배, 4:키위레몬, 5:파인애플
};