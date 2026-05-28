#pragma once

#include "Utillity.h"
#include <algorithm>

// namespace 포함해서 전방 선언
namespace learning
{
    struct ColliderCircle;
    struct ColliderBox;
    struct ColliderLine;
}

// 스스로 활성화 상태 관리 및 초기화하는 공통 규격
class IPoolable
{
public:
    virtual ~IPoolable() = default;

    virtual void OnSpawn() = 0;
    virtual void OnDespawn() = 0;

    bool IsActive() const { return m_isActive; }
    void SetActive(bool active) { m_isActive = active; }

protected:
    bool m_isActive = false; // 객체의 활성/비활성 상태 (기본값: 비활성)
};

// 최대 생성 오브젝트 15개
constexpr int OBJECT_NAME_LEN_MAX = 15;

// GameObjectBase가 IPoolable을 상속
class GameObjectBase : public IPoolable
{
    using Vector2f = learning::Vector2f;
public:
    GameObjectBase() = default;
    GameObjectBase(const GameObjectBase&) = delete;

    virtual ~GameObjectBase() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Render(HDC hdc) = 0;

    void SetPosition(float x, float y) { m_pos = { x, y }; }
    void SetDirection(Vector2f dir) { m_dir = dir; }
    void SetSpeed(float speed) { m_speed = speed; }
    void SetName(const char* name);

    const char* GetName() const { return m_name; }

    Vector2f GetPosition() const { return m_pos; }
    Vector2f GetDirection() const { return m_dir; }

    float GetSpeed() const { return m_speed; }

    void SetWidth(int w) { m_width = w; };
    void SetHeight(int h) { m_height = h; };

protected:
    int m_width = 0;
    int m_height = 0;

    Vector2f m_pos = { 0.0f, 0.0f };
    Vector2f m_dir = { 0.0f, 0.0f }; // 방향 (단위 벡터)

    float m_speed = 0.0f; // 속력

    char m_name[OBJECT_NAME_LEN_MAX] = "";
};

namespace renderHelp
{
    class BitmapInfo;
}

class GameObject : public GameObjectBase
{
    using ColliderCircle = learning::ColliderCircle;
    using ColliderBox = learning::ColliderBox;

    using BitmapInfo = renderHelp::BitmapInfo;

public:
    GameObject(const GameObject&) = delete;
    ~GameObject() override;

    // 생명주기 함수 오버라이드 선언
    void OnSpawn() override;
    void OnDespawn() override;

    void Update(float deltaTime) override;
    void Render(HDC hdc) override;

    void SetColliderCircle(float radius);
    void SetColliderBox(float halfWidth, float halfHeight);
    // 충돌처리 -> 충돌 시 true 반환
    bool IsCollidingWith(const GameObject* pOther) const;
    void SetColliding(bool isColliding) { m_isColliding = isColliding; }
    bool IsColliding() const { return m_isColliding; }

    // 펜 생성 함수
    static void InitializeResources();
    // 펜 해제 함수
    static void ReleaseResources();

    // 비트맵 관련 함수
    void SetBitmapInfo(BitmapInfo* bitmapInfo);

protected:
    void DrawCollider(HDC hdc);

    void Move(float deltaTime);

    // Collider
    ColliderCircle* m_pColliderCircle = nullptr;
    ColliderBox* m_pColliderBox = nullptr;

    void DrawBitmap(HDC hdc);
    void UpdateFrame(float deltaTime);

    // Bitmap 정보
    BitmapInfo* m_pBitmapInfo = nullptr;
    struct FrameFPos
    {
        int x;
        int y;
    };

private:
    // 충돌 여부를 판단하는 변수
    bool m_isColliding = false;

    // 파란펜
    static HPEN s_hPenColliding;
    // 빨간 펜
    static HPEN s_hPenNormal;
    // 초록 펜
    static HPEN s_hPenEnemy;
};