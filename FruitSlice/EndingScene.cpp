#include "EndingScene.h"
#include "MyFirstWndGame.h"

void EndingScene::Initialize()
{
}

void EndingScene::Update(float deltaTime)
{
}

void EndingScene::Render(HDC hDC)
{
    RECT rect;
    SetRect(&rect, 0, 0, 800, 600); // Adjust the rectangle size as needed
    DrawText(hDC, L"Ending Scene", -1, &rect, DT_CENTER | DT_VCENTER);
}

void EndingScene::Finalize()
{
}

void EndingScene::Enter()
{
}

void EndingScene::Leave()
{
}

