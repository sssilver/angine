#pragma once
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")


#include <Windows.h>
#include <wingdi.h>

#include <DXGI.h>
#include <D3Dcommon.h>
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>


#include "itask.h"
#include "Shader.h"


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
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11RasterizerState* m_rasterState;
    D3DXMATRIX m_projectionMatrix;
    D3DXMATRIX m_worldMatrix;
    D3DXMATRIX m_orthoMatrix;

    int screenWidth, screenHeight;
    bool isFullscreen;
    float screenDepth, screenNear;

    Shader* m_shader;


    void beginScene(float red, float green, float blue, float alpha);
    void endScene();


public:
    Renderer(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool isFullscreen, float screenDepth, float screenNear);
    ~Renderer(void);

	bool start(void);
	void update(void);
	void stop(void);
};

