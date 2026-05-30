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

    // 과일 베기 실행 함수와 상태 반환
    void Slice();
    bool IsSliced() const { return m_isSliced; }

private:
    // 충돌체 및 렌더링을 위한 반지름
    float m_radius = 30.0f;

    // 화면 경계 이탈 검사를 위한 해상도 수치
    int m_screenWidth = 1024;
    int m_screenHeight = 720;

    int m_fruitType = 0; // 0:사과, 1:오렌지, 2:복숭아, 3:배, 4:키위레몬, 5:파인애플

    // 파편화(Sliced) 상태 및 분리된 물리 변수
    bool m_isSliced = false;

    learning::Vector2f m_leftHalfPos = { 0.0f, 0.0f };
    learning::Vector2f m_rightHalfPos = { 0.0f, 0.0f };
    learning::Vector2f m_leftHalfVel = { 0.0f, 0.0f };
    learning::Vector2f m_rightHalfVel = { 0.0f, 0.0f };
};