#pragma once
#include "INC_Windows.h"

// 함수 선언
// 다른 파일에서도 해당 함수를 사용할 수 있도록 h 파일에서 함수 선언 진행 (함수 공유는 항상 h 파일에서 하므로)
LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// 윈도우 클래스 생성 및 등록을 클래스로 구현한 예시.
// * 상속을 받아 확장할 수 있도록 구현.

class NzWndBase
{
public:
    NzWndBase() = default;
    virtual ~NzWndBase() = default;

    bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height);
    void Destroy();

    void* GetHandle() const { return m_hWnd; }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

protected:

    // NzWndBase는 NzWndProc를 친구를 인정, NzWndProc는 NzWndBase의 멤버 변수나 함수를 protected나 private라도 사용가능, 다만 NzWndProc에서 NzWndBase를 friend로 선언하지 않으면 NzWndProc에서 NzWndBase의 변수 함수 사용 불가
    friend LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    // 메시지 프로시저를 가상함수로 구현하여 상속받은 클래스에서 재정의할 수 있도록 함.
    virtual void OnResize(int width, int height);
    virtual void OnClose() {}

    HWND m_hWnd = HWND();
    int m_width = 0;
    int m_height = 0;


    // 복사 생성자 막기, 이미 생성된 객체를 복사해서 늘리는 것을 막음
    NzWndBase(const NzWndBase&) = delete;
    NzWndBase& operator=(const NzWndBase&) = delete;
    NzWndBase(NzWndBase&&) = delete;
    NzWndBase& operator=(NzWndBase&&) = delete;
};