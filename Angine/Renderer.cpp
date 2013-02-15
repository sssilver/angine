#include "Renderer.h"


Renderer::Renderer(int screenWidth, int screenHeight, bool isVsync, HWND hwnd, bool isFullscreen, float screenDepth, float screenNear) :
	hwnd_(hwnd),
	screenWidth_(screenWidth), screenHeight_(screenHeight),
	screenDepth_(screenDepth), screenNear_(screenNear),
	isFullscreen_(isFullscreen),
	isVsync_(isVsync),

    swapChain_(0),
    device_(0),
    deviceContext_(0),
    renderTargetView_(0),
    depthStencilBuffer_(0),
    depthStencilState_(0),
    depthStencilView_(0),
    rasterState_(0)
{
    shader_ = new Shader();
    mesh_ = new Mesh();
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
    for (i=0; i<numModes; ++i) {
        if (displayModeList[i].Width == (unsigned int)screenWidth_) {
            if (displayModeList[i].Height == (unsigned int)screenHeight_) {
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
    videoCardMemory_ = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // Convert the name of the video card to a character array and store it.
    error = wcstombs_s(&stringLength, videoCardDescription_, 128, adapterDesc.Description, 128);
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
    swapChainDesc.BufferDesc.Width = screenWidth_;
    swapChainDesc.BufferDesc.Height = screenHeight_;

    // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Set the refresh rate of the back buffer.
    if (isVsync_) {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    } else {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd_;

    // Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Set to full screen or windowed mode.
    swapChainDesc.Windowed = !isFullscreen_;

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
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, 
                                           D3D11_SDK_VERSION, &swapChainDesc, &swapChain_, &device_, NULL, &deviceContext_);
    if (FAILED(result))
        return false;

    // Get the pointer to the back buffer.
    result = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result))
        return false;

    // Create the render target view with the back buffer pointer.
    result = device_->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView_);
    if (FAILED(result))
        return false;

    // Release pointer to the back buffer as we no longer need it.
    backBufferPtr->Release();
    backBufferPtr = 0;

    // Initialize the description of the depth buffer.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = screenWidth_;
    depthBufferDesc.Height = screenHeight_;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    result = device_->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer_);
    if (FAILED(result))
        return false;

    // Initialize the description of the stencil state.
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state.
    result = device_->CreateDepthStencilState(&depthStencilDesc, &depthStencilState_);
    if (FAILED(result))
        return false;

    // Set the depth stencil state.
    deviceContext_->OMSetDepthStencilState(depthStencilState_, 1);

    	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    result = device_->CreateDepthStencilView(depthStencilBuffer_, &depthStencilViewDesc, &depthStencilView_);
    if (FAILED(result))
        return false;

    // Bind the render target view and depth stencil buffer to the output render pipeline.
    deviceContext_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);


    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;


    // Create the rasterizer state from the description we just filled out.
    result = device_->CreateRasterizerState(&rasterDesc, &rasterState_);
    if (FAILED(result))
        return false;

    // Now set the rasterizer state.
    deviceContext_->RSSetState(rasterState_);

    // Setup the viewport for rendering.
    viewport.Width = (float)screenWidth_;
    viewport.Height = (float)screenHeight_;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 0.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    deviceContext_->RSSetViewports(1, &viewport);

    // Initialize the world matrix to the identity matrix.
    D3DXMatrixIdentity(&worldMatrix_);

    // Create an orthographic projection matrix for 2D rendering.
	//D3DXMatrixOrthoOffCenterLH(&orthoMatrix_, 0, (float)screenWidth_, (float)screenHeight_, 0, screenNear_, screenDepth_);
	D3DXMatrixPerspectiveFovLH(&orthoMatrix_, D3DX_PI / 4.0f, (float)screenWidth_ / (float)screenHeight_, screenNear_, screenDepth_);

	// Initialize a minimal shader
    result = shader_->initialize(device_, hwnd_, L"color.vs", L"color.ps");
    if (!(result))
        return false;

    mesh_->initialize(device_);

    return true;
}


void Renderer::update(void)
{
    D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
    D3DXVECTOR3 up, position, lookAt;

    // Setup the vector that points upwards.
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    // Setup the position of the camera in the world.
    position.x = -5.0f;
    position.y = 5.0f;
    position.z = 5.0f;

    position.x = .0f;
    position.y = 5.0f;
    position.z = 3.0f;

    // Setup where the camera is looking by default.
    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 0.0f;

	D3DXMatrixIdentity(&viewMatrix);
	D3DXMatrixLookAtLH(&viewMatrix, &position, &lookAt, &up);
	D3DXMatrixIdentity(&worldMatrix_);

    beginScene(.3f, .0f, .0f, 1.0f);
    mesh_->render(deviceContext_);
    shader_->render(deviceContext_, 24, worldMatrix_, viewMatrix, orthoMatrix_);
    endScene();
}


void Renderer::stop(void)
{
    mesh_->shutdown();
    shader_->shutdown();

    // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
    if (swapChain_) {
        swapChain_->SetFullscreenState(false, NULL);
    }

    if (rasterState_) {
        rasterState_->Release();
        rasterState_ = 0;
    }

    if (depthStencilView_) {
        depthStencilView_->Release();
        depthStencilView_ = 0;
    }

    if (depthStencilState_) {
        depthStencilState_->Release();
        depthStencilState_ = 0;
    }

    if (depthStencilBuffer_) {
        depthStencilBuffer_->Release();
        depthStencilBuffer_ = 0;
    }

    if(renderTargetView_) {
        renderTargetView_->Release();
        renderTargetView_ = 0;
    }

    if(deviceContext_) {
        deviceContext_->Release();
        deviceContext_ = 0;
    }

    if(device_) {
        device_->Release();
        device_ = 0;
    }

    if(swapChain_) {
        swapChain_->Release();
        swapChain_ = 0;
    }
}


Renderer::~Renderer(void)
{
    delete mesh_;
    delete shader_;
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
    deviceContext_->ClearRenderTargetView(renderTargetView_, color);
    
    // Clear the depth buffer.
    deviceContext_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0);

    return;
}


void Renderer::endScene()
{
    // Present the back buffer to the screen since rendering is complete.
    if (isVsync_) {
        // Lock to screen refresh rate.
        swapChain_->Present(1, 0);
    } else {
        // Present as fast as possible.
        swapChain_->Present(0, 0);
    }

    return;
}