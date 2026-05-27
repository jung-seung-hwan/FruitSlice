#include "MyFirstWndGame.h"
#include <iostream>


int main()
{
	// 윈도우 + 게임 로직을 가진 객체 생성
	MyFirstWndGame game;

	if (false == game.Initialize())
	{
		std::cout << "Failed to initialize the game. Error: " << GetLastError() << std::endl;
		return -1;
	}

	game.Run();

	game.Finalize();

	return 0;

}