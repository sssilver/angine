#pragma once

#include <d3d11.h>
#include <d3dx10math.h>



class Mesh
{
private:
    struct VertexType {
        D3DXVECTOR3 position;
        D3DXVECTOR4 color;
    };


    bool initializeBuffers(ID3D11Device*);
    void shutdownBuffers();
    void renderBuffers(ID3D11DeviceContext*);

    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_vertexCount, m_indexCount;



public:
    Mesh(void);
    Mesh(const Mesh&);
    ~Mesh(void);

    bool initialize(ID3D11Device*);
    void shutdown();
    void render(ID3D11DeviceContext*);

    const int getIndexCount();

};

