#include "main.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Game *ttt = new Game(hInstance);
	ttt->init();

    while (ttt->running()) {
	    ttt->run();
    }

	delete ttt;

    _CrtDumpMemoryLeaks();

    return 0;
}