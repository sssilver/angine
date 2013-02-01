#pragma once

#include <Windows.h>
#include <wingdi.h>
#include <gl\GL.h>


#include "itask.h"


class Renderer :
    public ITask
{
private:
    HDC hdc;
    int iPixelFormat; 
    HGLRC hglrc; 

public:
    Renderer(HWND hwnd);
    ~Renderer(void);

	bool start(void);
	void update(void);
	void stop(void);
};

