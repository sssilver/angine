#pragma once

#include <string>
#include <set>

#include <Windows.h>

#include "ITask.h"



class Window: public ITask
{
private:
	HINSTANCE hInstance;
	WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

	const std::wstring wndClassName;

    std::set<int> bufferKeys;


public:
	Window(HINSTANCE hInstance);
	~Window(void);

	bool start(void);
	void update(void);
	void stop(void);

    HWND getHWND();

    void keyDown(int key);
    void keyUp(int key);

    std::set<int> keys();

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

