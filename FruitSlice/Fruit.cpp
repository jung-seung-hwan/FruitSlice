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
}

void Fruit::OnDespawn()
{
    // 풀로 반환될 때의 정리 로직
}

void Fruit::Update(float deltaTime)
{
    if (!IsActive()) return;

    // 해상도 스케일에 맞게 조절할 중력 가속도 상수
    constexpr float GRAVITY = 600.0f;

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

        // 전체 이미지 크기를 기반으로 한 칸(단일 과일)의 크기 계산
        int totalWidth = m_pBitmapInfo->GetWidth();
        int totalHeight = m_pBitmapInfo->GetHeight();

        int frameWidth = totalWidth / 3;  // 가로 3칸
        int frameHeight = totalHeight / 2; // 세로 2칸
        float scale = 0.2f;
        // 화면에 실제로 그려질 축소된 크기
        int destWidth = static_cast<int>(frameWidth * scale);
        int destHeight = static_cast<int>(frameHeight * scale);

        // 화면 출력용 중심 보정 좌표 계산
        int drawX = static_cast<int>(m_pos.x - (destWidth / 2.0f));
        int drawY = static_cast<int>(m_pos.y - (destHeight / 2.0f));

        // [핵심] 과일 타입에 따른 원본 이미지 자르기 좌표 연산
        int col = m_fruitType % 3;
        int row = m_fruitType / 3;

        int srcX = col * frameWidth;
        int srcY = row * frameHeight;

        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 255; // 전체 이미지의 투명도
        bf.AlphaFormat = AC_SRC_ALPHA; 

        // AlphaBlend 함수를 사용하여 투명한 PNG 영역을 자연스럽게 렌더링
        AlphaBlend(hdc,
            drawX, drawY, destWidth, destHeight,
            hMemDC,
            srcX, srcY, frameWidth, frameHeight,
            bf);

        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);
    }
}