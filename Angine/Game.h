#pragma once

#include <set>
#include <sstream>

#include "Kernel.h"
#include "Window.h"
#include "Renderer.h"



class Game
{
private:
	Kernel *kernel;
	Window *window;
    Renderer *renderer;

    bool isRunning;

    void processKeyboard();


public:
	Game(HINSTANCE hInstance);
	~Game(void);

	void init();
	bool running();
	void run();

    void quit();
};

