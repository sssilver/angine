#include "Renderer.h"


Renderer::Renderer(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool isFullscreen, float screenDepth, float screenNear)
{

    this->hwnd = hwnd;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->screenDepth = screenDepth;
    this->screenNear = screenNear;
    this->isFullscreen = isFullscreen;
    
    // Store the vsync setting.
    this->m_vsync_enabled = vsync;

    m_swapChain = 0;
    m_device = 0;
    m_deviceContext = 0;
    m_renderTargetView = 0;
    m_depthStencilBuffer = 0;
    m_depthStencilState = 0;
    m_depthStencilView = 0;
    m_rasterState = 0;

    m_shader = new Shader();
}


bool Renderer::start(void)
{
    HRESULT result;
    bool bResult;

    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes, i, numerator, denominator, stringLength;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;
    int error;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_RASTERIZER_DESC rasterDesc;
    D3D11_VIEWPORT viewport;
    float fieldOfView, screenAspect;


    // Create a DirectX graphics interface factory.
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result))
        return false;

    // Use the factory to create an adapter for the primary graphics interface (video card).
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result))
        return false;

    // Enumerate the primary adapter output (monitor).
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result))
        return false;

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result))
        return false;

    // Create a list to hold all the possible display modes for this monitor/video card combination.
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList)
        return false;

    // Now fill the display mode list structures.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result))
        return false;

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    for (i=0; i<numModes; i++) {
        if (displayModeList[i].Width == (unsigned int)screenWidth) {
            if (displayModeList[i].Height == (unsigned int)screenHeight) {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // Get the adapter (video card) description.
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result))
        return false;

    OutputDebugString(adapterDesc.Description);

    // Store the dedicated video card memory in megabytes.
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // Convert the name of the video card to a character array and store it.
    error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    if (error != 0)
        return false;

    // Release the display mode list.
    delete [] displayModeList;
    displayModeList = 0;

    // Release the adapter output.
    adapterOutput->Release();
    adapterOutput = 0;

    // Release the adapter.
    adapter->Release();
    adapter = 0;

    // Release the factory.
    factory->Release();
    factory = 0;

    // Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

    // Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Set the refresh rate of the back buffer.
    if (m_vsync_enabled) {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    } else {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd;

    // Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Set to full screen or windowed mode.
    swapChainDesc.Windowed = !isFullscreen;

    // Set the scan line ordering and scaling to unspecified.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard the back buffer contents after presenting.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // Don't set the advanced flags.
    swapChainDesc.Flags = 0;

    // Set the feature level to DirectX 11.
    featureLevel = D3D_FEATURE_LEVEL_10_0;

    // Create the swap chain, Direct3D device, and Direct3D device context.
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,  0, &featureLevel, 1, 
                                           D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if (FAILED(result))
        return false;

    // Get the pointer to the back buffer.
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result))
        return false;

    // Create the render target view with the back buffer pointer.
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if (FAILED(result))
        return false;

    // Release pointer to the back buffer as we no longer need it.
    backBufferPtr->Release();
    backBufferPtr = 0;

    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;


    // Create the rasterizer state from the description we just filled out.
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
    if (FAILED(result))
        return false;

    // Now set the rasterizer state.
    m_deviceContext->RSSetState(m_rasterState);

    // Setup the viewport for rendering.
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    m_deviceContext->RSSetViewports(1, &viewport);

    // Initialize the world matrix to the identity matrix.
    D3DXMatrixIdentity(&m_worldMatrix);

    // Create an orthographic projection matrix for 2D rendering.
    D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    result = this->m_shader->initialize(this->m_device, this->hwnd, L"color.vs", L"color.ps");
    if (!(result))
        return false;

    return true;
}


void Renderer::update(void)
{
    D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;

    this->beginScene(.3f, .0f, .0f, 1.0f);
    this->m_shader->render(this->m_deviceContext, 3, m_worldMatrix, m_orthoMatrix, m_projectionMatrix);
    this->endScene();
}


void Renderer::stop(void)
{
    this->m_shader->shutdown();

    // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
    if (m_swapChain) {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    if (m_rasterState) {
        m_rasterState->Release();
        m_rasterState = 0;
    }

    if (m_depthStencilView) {
        m_depthStencilView->Release();
        m_depthStencilView = 0;
    }

    if (m_depthStencilState) {
        m_depthStencilState->Release();
        m_depthStencilState = 0;
    }

    if (m_depthStencilBuffer) {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = 0;
    }

    if(m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = 0;
    }

    if(m_deviceContext) {
        m_deviceContext->Release();
        m_deviceContext = 0;
    }

    if(m_device) {
        m_device->Release();
        m_device = 0;
    }

    if(m_swapChain) {
        m_swapChain->Release();
        m_swapChain = 0;
    }
}


Renderer::~Renderer(void)
{
    delete this->m_shader;
}


void Renderer::beginScene(float red, float green, float blue, float alpha)
{
    float color[4];


    // Setup the color to clear the buffer to.
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // Clear the back buffer.
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    
    // Clear the depth buffer.
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    return;
}


void Renderer::endScene()
{
    // Present the back buffer to the screen since rendering is complete.
    if (m_vsync_enabled) {
        // Lock to screen refresh rate.
        m_swapChain->Present(1, 0);
    } else {
        // Present as fast as possible.
        m_swapChain->Present(0, 0);
    }

    return;
}