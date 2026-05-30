#include "Fruit.h"
#include "Collider.h"
#include "GameObject.h"
#include "RenderHelp.h"
#pragma comment(lib, "msimg32.lib")

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
    SetActive(true);
    SetColliding(false);
    m_fruitType = rand() % 6;

    // 스폰 시 잘림 상태 초기화
    m_isSliced = false;
}

void Fruit::OnDespawn()
{
    // 풀로 반환될 때의 정리 로직
}

void Fruit::Slice()
{
    if (m_isSliced) return; // 이미 잘린 과일은 중복 판정 방지
    m_isSliced = true;

    // 파편의 시작 위치는 잘린 순간의 원본 과일 중심점
    m_leftHalfPos = m_pos;
    m_rightHalfPos = m_pos;

    // 왼쪽 파편은 좌측(-), 오른쪽 파편은 우측(+)으로 밀어내며 살짝 위로 튀어오르게 함
    m_leftHalfVel = { -150.0f, -200.0f };
    m_rightHalfVel = { 150.0f, -200.0f };
}

void Fruit::Update(float deltaTime)
{
    if (!IsActive()) return;

    // 해상도 스케일에 맞게 조절할 중력 가속도 상수
    constexpr float GRAVITY = 600.0f;

    if (m_isSliced)
    {
        // 양쪽 파편에 중력 적용
        m_leftHalfVel.y += GRAVITY * deltaTime;
        m_rightHalfVel.y += GRAVITY * deltaTime;

        m_leftHalfPos.x += m_leftHalfVel.x * deltaTime;
        m_leftHalfPos.y += m_leftHalfVel.y * deltaTime;

        m_rightHalfPos.x += m_rightHalfVel.x * deltaTime;
        m_rightHalfPos.y += m_rightHalfVel.y * deltaTime;

        // 두 파편이 모두 화면 아래로 떨어지면 객체 회수
        if (m_leftHalfPos.y > 850.0f && m_rightHalfPos.y > 850.0f)
        {
            OnDespawn();
            SetActive(false);
        }
        return; // 원본 과일의 물리 연산은 건너뜀
    }

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

    if (m_pBitmapInfo)
    {
        HDC hMemDC = CreateCompatibleDC(hdc);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_pBitmapInfo->GetBitmapHandle());

        int frameWidth = m_pBitmapInfo->GetWidth() / 3;
        int frameHeight = m_pBitmapInfo->GetHeight() / 2;
        float scale = 0.2f;

        int destWidth = static_cast<int>(frameWidth * scale);
        int destHeight = static_cast<int>(frameHeight * scale);

        int col = m_fruitType % 3;
        int row = m_fruitType / 3;

        int srcX = col * frameWidth;
        int srcY = row * frameHeight;

        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

        if (!m_isSliced)
        {
            // [정상 상태] 원본 과일 통째로 그리기 (기존 코드)
            int drawX = static_cast<int>(m_pos.x - (destWidth / 2.0f));
            int drawY = static_cast<int>(m_pos.y - (destHeight / 2.0f));

            AlphaBlend(hdc, drawX, drawY, destWidth, destHeight,
                hMemDC, srcX, srcY, frameWidth, frameHeight, bf);
        }
        else
        {
            // [파편 상태] 이미지를 가로로 절반 잘라서 각각 그리기
            int srcHalfW = frameWidth / 2;
            int destHalfW = destWidth / 2;

            // 1. 왼쪽 파편 그리기
            int leftDrawX = static_cast<int>(m_leftHalfPos.x - destHalfW); // 왼쪽 조각의 중심 정렬
            int leftDrawY = static_cast<int>(m_leftHalfPos.y - (destHeight / 2.0f));
            AlphaBlend(hdc,
                leftDrawX, leftDrawY, destHalfW, destHeight, // 화면 절반
                hMemDC,
                srcX, srcY, srcHalfW, frameHeight, // 원본 왼쪽 절반
                bf);

            // 2. 오른쪽 파편 그리기
            int rightDrawX = static_cast<int>(m_rightHalfPos.x); // 오른쪽 조각 시작점
            int rightDrawY = static_cast<int>(m_rightHalfPos.y - (destHeight / 2.0f));
            AlphaBlend(hdc,
                rightDrawX, rightDrawY, destHalfW, destHeight, // 화면 절반
                hMemDC,
                srcX + srcHalfW, srcY, srcHalfW, frameHeight, // 원본 오른쪽 절반 (srcX 위치 이동)
                bf);
        }

        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);
    }
}