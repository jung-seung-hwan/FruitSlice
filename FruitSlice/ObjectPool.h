#pragma once
#include <vector>
#include <assert.h>

template <typename T>
class ObjectPool
{
public:
    ObjectPool(int capacity) : m_capacity(capacity)
    {
        // 초기화 시점에 지정된 용량만큼 메모리를 미리 할당
        m_pool.reserve(capacity);
        for (int i = 0; i < capacity; ++i)
        {
            T* newObj = new T();
            newObj->SetActive(false);
            m_pool.push_back(newObj);
        }
    }

    ~ObjectPool()
    {
        for (T* obj : m_pool)
        {
            delete obj;
        }
        m_pool.clear();
    }

    // 비활성화된 객체를 찾아 활성화하여 반환
    T* Get()
    {
        T* obj = m_pool[m_currentIndex];

        // 해당 인덱스의 객체가 이미 사용 중이라면, 가장 오래된 객체이므로 강제로 비활성화 처리
        if (obj->IsActive())
        {
            obj->OnDespawn();
            obj->SetActive(false);
        }

        // 객체 활성화 및 스폰 로직 실행
        obj->SetActive(true);
        obj->OnSpawn();

        // 다음 인덱스로 이동하며, 최대 용량 도달 시 0으로 순환
        m_currentIndex = (m_currentIndex + 1) % m_capacity;

        return obj;
    }

    // 객체 사용이 끝나면 상태를 비활성화하여 풀에 반환
    void Release(T* obj)
    {
        if (obj && obj->IsActive())
        {
            obj->OnDespawn();
            obj->SetActive(false);
        }
    }

    const std::vector<T*>& GetAllObjects() const { return m_pool; }

private:
    std::vector<T*> m_pool;
    int m_capacity;
};