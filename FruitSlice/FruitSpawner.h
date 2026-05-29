#pragma once
#include "Utillity.h" 

class Fruit; // 과일 객체
template <typename T> class ObjectPool;

class FruitSpawner
{
public:
    FruitSpawner(ObjectPool<Fruit>* pool);
    ~FruitSpawner() = default;

    void Update(float deltaTime);

private:
    void SpawnFruit();

private:
    ObjectPool<Fruit>* m_pFruitPool = nullptr;

    float m_spawnTimer = 0.0f;
    float m_spawnInterval = 1.0f; // 2초마다 생성

    // 화면 하단 스폰을 위한 해상도 정보
    int m_screenWidth = 1024;
    int m_screenHeight = 720;
};