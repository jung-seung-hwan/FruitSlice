#include "INC_Windows.h"
#include "Collider.h"
#include "GameObject.h"
#include <assert.h>
#include "RenderHelp.h"

// 펜 그리기 함수
HPEN GameObject::s_hPenColliding = nullptr;
HPEN GameObject::s_hPenNormal = nullptr;

void GameObject::InitializeResources() {
    s_hPenColliding = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    s_hPenNormal = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
}

// 과제: 해당 코드의 문제는 무엇일까요? 어떻게 개선하면 좋을까요?
// 개선 방향에 대해 서로 토론하고 비교해 보세요.
// 새 게임오브젝트가 생성될 때마다 펜과 브러시를 생성하고 삭제함 -> 펜과 브러시는 한번만 생성하고 공유하는 방식으로 변경
void GameObject::DrawCollider(HDC hdc)
{
    /*COLORREF color = m_isColliding ? RGB(0, 0, 255) : RGB(255, 0, 0);
    HPEN hPen = CreatePen(PS_SOLID, 2, color);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));*/

    // 주소로 펜과 브러시 선택
    HPEN hTargetPen = s_hPenNormal;

    //HPEN hTargetPen = m_isColliding ? s_hPenColliding : s_hPenNormal;
    HPEN hOldPen = (HPEN)SelectObject(hdc, hTargetPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, (HBRUSH)GetStockObject(NULL_BRUSH));

    if (m_pColliderCircle)
    {
        Ellipse(hdc, m_pColliderCircle->center.x - m_pColliderCircle->radius,
            m_pColliderCircle->center.y - m_pColliderCircle->radius,
            m_pColliderCircle->center.x + m_pColliderCircle->radius,
            m_pColliderCircle->center.y + m_pColliderCircle->radius);
    }

    if (m_pColliderBox)
    {
        Rectangle(hdc, m_pColliderBox->center.x - m_pColliderBox->halfSize.x,
            m_pColliderBox->center.y - m_pColliderBox->halfSize.y,
            m_pColliderBox->center.x + m_pColliderBox->halfSize.x,
            m_pColliderBox->center.y + m_pColliderBox->halfSize.y);
    }

    // 이전 객체 복원 및 펜 삭제
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    //DeleteObject(hPen);
}

void GameObject::ReleaseResources() {
    DeleteObject(s_hPenColliding);
    DeleteObject(s_hPenNormal);
}

GameObject::~GameObject()
{
    if (m_pColliderCircle)
    {
        delete m_pColliderCircle;
        m_pColliderCircle = nullptr;
    }

    if (m_pColliderBox)
    {
        delete m_pColliderBox;
        m_pColliderBox = nullptr;
    }
}

void GameObject::SetBitmapInfo(BitmapInfo* bitmapInfo)
{
    assert(m_pBitmapInfo == nullptr && "BitmapInfo must be null!");
    m_pBitmapInfo = bitmapInfo;
    // 스프라이트 정보는 일단은 하드코딩해요. 

}


void GameObject::Update(float deltaTime)
{
    // 이동한 중심좌표에 따라서 Collider 업데이트
    if (m_pColliderCircle)
    {
        m_pColliderCircle->center = m_pos;
    }
    if (m_pColliderBox)
    {
        m_pColliderBox->center = m_pos;
    }

}

void GameObject::Render(HDC hdc)
{
    DrawBitmap(hdc);
    DrawCollider(hdc);
}


void GameObject::SetColliderCircle(float radius)
{
    if (m_pColliderCircle)
    {
        delete m_pColliderCircle;
        m_pColliderCircle = nullptr;
    }

    m_pColliderCircle = new ColliderCircle;

    assert(m_pColliderCircle != nullptr && "Failed to create ColliderCircle!");

    m_pColliderCircle->radius = radius;
    m_pColliderCircle->center = m_pos;
}


void GameObject::SetColliderBox(float width, float height)
{
    if (m_pColliderBox)
    {
        delete m_pColliderBox;
        m_pColliderBox = nullptr;
    }

    m_pColliderBox = new ColliderBox;

    assert(m_pColliderBox != nullptr && "Failed to create ColliderBox!");

    m_pColliderBox->center = m_pos;
    m_pColliderBox->halfSize.x = width / 2.0f;
    m_pColliderBox->halfSize.y = height / 2.0f;
}

void GameObject::DrawBitmap(HDC hdc)
{
    if (m_pBitmapInfo == nullptr) return;
    if (m_pBitmapInfo->GetBitmapHandle() == nullptr) return;

    HDC hBitmapDC = CreateCompatibleDC(hdc);

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hBitmapDC, m_pBitmapInfo->GetBitmapHandle());
    // BLENDFUNCTION 설정 (알파 채널 처리)
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;  // 원본 알파 채널 그대로 사용
    blend.AlphaFormat = AC_SRC_ALPHA;

    const int x = m_pos.x - m_width / 2;
    const int y = m_pos.y - m_height / 2;
    AlphaBlend(hdc, x, y, m_width, m_height, hBitmapDC, 0, 0, m_pBitmapInfo->GetWidth(), m_pBitmapInfo->GetHeight(), blend);

    // 비트맵 핸들 복원
    SelectObject(hBitmapDC, hOldBitmap);
    DeleteDC(hBitmapDC);

}


void GameObjectBase::SetName(const char* name)
{
    if (name == nullptr) return;

    strncpy_s(m_name, name, OBJECT_NAME_LEN_MAX - 1);
    m_name[OBJECT_NAME_LEN_MAX - 1] = '\0';
}

bool GameObject::IsCollidingWith(const GameObject* pOther) const
{
    if (pOther == nullptr)
        return false;

    if (pOther == this)
        return false;

    // 원끼리 충돌
    if (m_pColliderCircle && pOther->m_pColliderCircle)
    {
        ColliderCircle lhs = *m_pColliderCircle;
        ColliderCircle rhs = *pOther->m_pColliderCircle;

        lhs.center = m_pos;
        rhs.center = pOther->m_pos;

        return Intersect(lhs, rhs);
    }

    // 박스끼리 충돌
    if (m_pColliderBox && pOther->m_pColliderBox)
    {
        ColliderBox lhs = *m_pColliderBox;
        ColliderBox rhs = *pOther->m_pColliderBox;

        lhs.center = m_pos;
        rhs.center = pOther->m_pos;

        return Intersect(lhs, rhs);
    }

    return false;
}