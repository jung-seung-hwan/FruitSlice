#include "Fruit.h"
#include "Collider.h"
#include "GameObject.h"

Fruit::Fruit()
{
    // 생성 시점에 단 한 번 충돌체 메모리 할당 (반지름 30.0f)
    SetColliderCircle(m_radius);
    SetName("TargetFruit");
}

Fruit::~Fruit()
{
    // 부모 클래스(GameObject) 소멸자에서 충돌체 동적 할당 해제 처리
}

void Fruit::OnSpawn()
{
    // 풀에서 꺼내져 활성화될 때의 초기화 상태
    // 위치와 속도는 FruitSpawner에서 덮어씌우므로 충돌 상태만 초기화
    SetColliding(false);
}

void Fruit::OnDespawn()
{
    // 풀로 반환될 때의 정리 로직
}

void Fruit::Update(float deltaTime)
{
    if (!IsActive()) return;

    // 해상도 스케일에 맞게 조절할 중력 가속도 상수
    constexpr float GRAVITY = 980.0f;

    // 부모 클래스의 Move를 호출하여 속도에 중력을 더하고 위치를 갱신
    Move(deltaTime, GRAVITY);

    // 이동된 물리 좌표를 충돌체 중심점에 동기화
    if (m_pColliderCircle)
    {
        m_pColliderCircle->center = m_pos;
    }

    // 화면 경계 이탈 검사
    // 화면 하단을 완전히 벗어나거나, 양옆으로 날아갔을 경우 풀로 반환
    const float padding = 100.0f;
    if (m_pos.y > m_screenHeight + padding ||
        m_pos.x < -padding ||
        m_pos.x > m_screenWidth + padding)
    {
        OnDespawn();
        SetActive(false);
    }
}

void Fruit::Render(HDC hdc)
{
    if (!IsActive()) return;

    int left = static_cast<int>(m_pos.x - m_radius);
    int top = static_cast<int>(m_pos.y - m_radius);
    int right = static_cast<int>(m_pos.x + m_radius);
    int bottom = static_cast<int>(m_pos.y + m_radius);

    Ellipse(hdc, left, top, right, bottom);

}