#pragma once
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3dx10.lib")


#include <Windows.h>
#include <wingdi.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>




#include "itask.h"


class Renderer :
    public ITask
{
private:
    HWND hwnd;
    HDC hdc;
    int iPixelFormat; 
    HGLRC hglrc; 

	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D10Device* m_device;
	ID3D10RenderTargetView* m_renderTargetView;
	ID3D10Texture2D* m_depthStencilBuffer;
	ID3D10DepthStencilState* m_depthStencilState;
	ID3D10DepthStencilView* m_depthStencilView;
	ID3D10RasterizerState* m_rasterState;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;


public:
    Renderer(HWND hwnd);
    ~Renderer(void);

	bool start(void);
	void update(void);
	void stop(void);
};

