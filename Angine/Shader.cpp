#include "Shader.h"


Shader::Shader(void)
{
    m_vertexShader = 0;
    m_pixelShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;
}


Shader::~Shader(void)
{
}


bool Shader::initialize(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;


    // Initialize the pointers this function will use to null.
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    // Compile the vertex shader code.
    result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
                                   &vertexShaderBuffer, &errorMessage, NULL);
    if (FAILED(result)) {
        if (errorMessage)         // If the shader failed to compile it should have writen something to the error message.
            OutputDebugString(L"Shader failed to compile");
        else                      // If there was nothing in the error message then it simply could not find the shader file itself.
            OutputDebugString(L"Missing Shader File");

        return false;
    }

    // Compile the pixel shader code.
    result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
                                   &pixelShaderBuffer, &errorMessage, NULL);
    if (FAILED(result)) {
        if (errorMessage)         // If the shader failed to compile it should have writen something to the error message.
            OutputDebugString(L"Shader failed to compile");
        else                      // If there was nothing in the error message then it simply could not find the shader file itself.
            OutputDebugString(L"Missing Shader File");

        return false;
    }

    // Create the vertex shader from the buffer.
    LPVOID b = vertexShaderBuffer->GetBufferPointer();
    SIZE_T s = vertexShaderBuffer->GetBufferSize();
    result = device->CreateVertexShader(b, s, NULL, &m_vertexShader);
    if (FAILED(result))
        return false;

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result))
        return false;

    // Now setup the layout of the data that goes into the shader.
    // This setup needs to match the VertexType stucture in the ModelClass and in the shader.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
                                       vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result))
        return false;

    // Release the vertex shader buffer and pixel shader buffer
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result))
        return false;


    return true;
}


void Shader::shutdown()
{
    // Release the matrix constant buffer.
    if (m_matrixBuffer) {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    // Release the layout.
    if (m_layout) {
        m_layout->Release();
        m_layout = 0;
    }

    // Release the pixel shader.
    if (m_pixelShader) {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // Release the vertex shader.
    if (m_vertexShader) {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }
}


void Shader::render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
    if (!this->setParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
        return;

    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // Render the triangle.
    deviceContext->DrawIndexed(indexCount, 0, 0);

    return;
}


bool Shader::setParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    // Transpose the matrices to prepare them for the shader.
    D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
    D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
    D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

    // Lock the constant buffer so it can be written to.
    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
        return false;

    // Get a pointer to the data in the constant buffer.
    dataPtr = (MatrixBufferType*)mappedResource.pData;

    // Copy the matrices into the constant buffer.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    bufferNumber = 0;

    // Finanly set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    return true;
}
