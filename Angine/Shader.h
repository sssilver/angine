#pragma once

#include <Windows.h>
#include <D3Dcommon.h>
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>



class Shader
{
private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11Buffer* m_matrixBuffer;

    struct MatrixBufferType {
        D3DXMATRIX world;
        D3DXMATRIX view;
        D3DXMATRIX projection;
    };

	float number;

public:
    Shader(void);
    ~Shader(void);

    bool initialize(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
    void shutdown();
    void render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

    bool setParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

};