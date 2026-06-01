#include "TitleScene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "RenderHelp.h"
#include <string>

void TitleScene::Initialize()
{
    // 배경
    m_pBackgroundBitmapInfo = renderHelp::CreateBitmapInfo(L"./Resource/Background.png");

    // 정중앙에 150x80 크기의 버튼 배치
    int btnWidth = 150;
    int btnHeight = 50;
    m_startButtonRect.left = (m_screenWidth - btnWidth) / 2;
    m_startButtonRect.top = (m_screenHeight - btnHeight) / 2;
    m_startButtonRect.right = m_startButtonRect.left + btnWidth;
    m_startButtonRect.bottom = m_startButtonRect.top + btnHeight;
}

void TitleScene::Enter()
{
}

void TitleScene::Update(float deltaTime)
{
    // UI 객체들 애니메이션(깜빡임 등) 업데이트
    for (auto* pObj : m_GameObjects)
    {
        if (pObj && pObj->IsActive()) pObj->Update(deltaTime);
    }

    // 현재 마우스 좌표 (모니터 기준 절대 좌표)
    POINT mousePt;
    GetCursorPos(&mousePt);

    // 게임 창을 찾아, 절대 좌표를 게임 창 내부 좌표(클라이언트 좌표)로 변환
    HWND hWnd = FindWindow(L"FruitSliceGame", L"FruitSliceGame");
    if (hWnd)
    {
        ScreenToClient(hWnd, &mousePt);
    }
    // 마우스 커서가 버튼 영역 안에 있는지 충돌 판정
    if (PtInRect(&m_startButtonRect, mousePt))
    {
        m_isButtonHovered = true; // 마우스 오버 상태

        // 영역 안에서 마우스 좌클릭이 발생했는지 확인
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            // 발생 시 Scene 전환
            SceneManager::GetInstance().ChangeScene(SceneType::PLAY);
        }
    }
    else
    {
        m_isButtonHovered = false; // 영역 밖
    }

}

void TitleScene::Render(HDC hDC)
{
    // 배경 렌더링
    if (m_pBackgroundBitmapInfo)
    {
        // 배경을 복사하기 위한 임시 메모리 공간 생성
        HDC hMemDC = CreateCompatibleDC(hDC);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_pBackgroundBitmapInfo->GetBitmapHandle());

        // 메모리에 올려둔 배경 비트맵을 실제 렌더링될 버퍼로 복사
        BitBlt(hDC, 0, 0, m_pBackgroundBitmapInfo->GetWidth(), m_pBackgroundBitmapInfo->GetHeight(), hMemDC, 0, 0, SRCCOPY);

        // 사용한 자원 반환
        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);
    }

    // 폰트 생성
    HFONT hFont = CreateFontW(
        72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS, L"Arial"
    );

    // 생성한 폰트를 DC에 적용
    HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

    // 텍스트 그리기 설정
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(255, 100, 0));   // 오렌지색
    SetTextAlign(hDC, TA_CENTER | TA_TOP); // 중앙 정렬

    // 타이틀 텍스트 출력
    std::wstring titleText = L"Fruits Slice";
    TextOutW(hDC, 512, 150, titleText.c_str(), titleText.length());

    // 사용이 끝난 폰트 리소스 반환 및 삭제
    SelectObject(hDC, hOldFont);
    DeleteObject(hFont);

    // 정렬 기준 원상 복구
    SetTextAlign(hDC, TA_LEFT | TA_TOP);

    // 시작 버튼 렌더링
    // 마우스 오버 상태에 따라 브러시 색상 변경
    HBRUSH hBrush = m_isButtonHovered ? CreateSolidBrush(RGB(200, 200, 200)) : CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    // 버튼 사각형 그리기
    Rectangle(hDC, m_startButtonRect.left, m_startButtonRect.top, m_startButtonRect.right, m_startButtonRect.bottom);

    // 버튼 텍스트 출력
    SetBkMode(hDC, TRANSPARENT); // 텍스트 배경을 투명하게 설정
    DrawTextW(hDC, L"GAME START", -1, &m_startButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 사용한 자원 반환
    SelectObject(hDC, hOldBrush);
    DeleteObject(hBrush);

    for (auto* pObj : m_GameObjects)
    {
        if (pObj && pObj->IsActive()) pObj->Render(hDC);
    }

}

void TitleScene::Finalize()
{
}

void TitleScene::Leave()
{
    // 다른 씬으로 전환될 때 호출되는 로직
    if (m_pBackgroundBitmapInfo)
    {
        delete m_pBackgroundBitmapInfo;
        m_pBackgroundBitmapInfo = nullptr;
    }
}