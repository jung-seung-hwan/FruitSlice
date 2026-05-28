#include "FruitSpawner.h"
#include "ObjectPool.h"
#include "Fruit.h"
#include <stdlib.h> 

FruitSpawner::FruitSpawner(ObjectPool<Fruit>* pool)
    : m_pFruitPool(pool)
{
}

void FruitSpawner::Update(float deltaTime)
{
    m_spawnTimer += deltaTime;

    // 타이머가 설정된 주기를 넘으면 과일 스폰
    if (m_spawnTimer >= m_spawnInterval)
    {
        SpawnFruit();
        m_spawnTimer = 0.0f;
    }
}

void FruitSpawner::SpawnFruit()
{
    if (m_pFruitPool == nullptr) return;

    // 풀에서 비활성화된 과일 객체 반환
    Fruit* newFruit = m_pFruitPool->Get();
    if (newFruit == nullptr) return; // 가용 객체가 없을 경우 안전하게 종료

    // 초기 스폰 위치 설정 (화면 하단, X좌표는 무작위)
    float randomX = static_cast<float>(rand() % (m_screenWidth - 200) + 100);
    float startY = static_cast<float>(m_screenHeight + 50); // 화면 약간 아래에서 시작
    newFruit->SetPosition(randomX, startY);

    // 초기 속도 벡터 설정
    float randomVx = static_cast<float>((rand() % 100) - 50) * 0.05f; // 좌우 난수
    float startVy = -15.0f; // 윈도우 좌표계는 Y가 아래로 갈수록 증가하므로 음수 부여

     newFruit->SetVelocity(learning::Vector2f{randomVx, startVy});
}