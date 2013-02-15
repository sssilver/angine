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
#include "Mesh.h"


class Renderer :
    public ITask
{
private:
    HWND	hwnd_;
    HDC		hdc_;
    int		pixelFormat_; 
    HGLRC	hglrc_; 

    bool	isVsync_;
    int		videoCardMemory_;
    char	videoCardDescription_[128];

    IDXGISwapChain*				swapChain_;
    ID3D11Device*				device_;
    ID3D11DeviceContext*		deviceContext_;
    ID3D11RenderTargetView*		renderTargetView_;
    ID3D11Texture2D*			depthStencilBuffer_;
    ID3D11DepthStencilState*	depthStencilState_;
    ID3D11DepthStencilView*		depthStencilView_;
    ID3D11RasterizerState*		rasterState_;

    D3DXMATRIX projectionMatrix_;
    D3DXMATRIX worldMatrix_;
    D3DXMATRIX orthoMatrix_;

    int		screenWidth_, screenHeight_;
    bool	isFullscreen_;
    float	screenDepth_, screenNear_;

    Shader* shader_;
    Mesh*	mesh_;



    void beginScene(float red, float green, float blue, float alpha);
    void endScene();


public:
    Renderer(int screenWidth, int screenHeight, bool isVsync, HWND hwnd, bool isFullscreen, float screenDepth, float screenNear);
    ~Renderer(void);

	bool start(void);
	void update(void);
	void stop(void);
};

