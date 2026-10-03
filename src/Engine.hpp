#pragma once

#include "constants.inc"

#include <cstring>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <wrl.h>

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4X4;
using DirectX::XMMATRIX;

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT2 texCoord;
};

struct MatrixBufferType
{
    XMFLOAT4X4 world;
    XMFLOAT4X4 projection;
};

struct Quad
{
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D11Buffer> vertexBuffer;

    HRESULT CreateVertexBuffer(ID3D11Device1 *device)
    {
        Vertex vertices[] = {{XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f)},
                             {XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f)},
                             {XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f)},
                             {XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f)},
                             {XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f)},
                             {XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f)}};

        D3D11_BUFFER_DESC vertexBufferDesc = {.ByteWidth = sizeof(vertices),
                                              .Usage = D3D11_USAGE_DEFAULT,
                                              .BindFlags = D3D11_BIND_VERTEX_BUFFER,
                                              .CPUAccessFlags = 0,
                                              .MiscFlags = 0};

        D3D11_SUBRESOURCE_DATA vertexData = {
            .pSysMem = vertices,
            .SysMemPitch = 0,
            .SysMemSlicePitch = 0,
        };

        return device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
    }
};

class Graphics2DEngine
{
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  public:
    // default init to nullptr all
    Graphics2DEngine() = default;

    Graphics2DEngine(Graphics2DEngine &) = delete;
    Graphics2DEngine operator=(Graphics2DEngine &) = delete;

    HRESULT EngineInit(HWND hwnd)
    {
        ComPtr<ID3D11Device> baseDevice;
        ComPtr<ID3D11DeviceContext> baseContext;

        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
        D3D_FEATURE_LEVEL chosenLevel;

        if (HRESULT hr =
                D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags,
                                  featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
                                  &baseDevice, &chosenLevel, &baseContext);
            FAILED(hr))
        {
            return hr;
        }

        if (HRESULT hr = baseDevice.As(&device); FAILED(hr))
        {
            return hr;
        }

        if (HRESULT hr = baseContext.As(&deviceContext); FAILED(hr))
        {
            return hr;
        }

        {
            ComPtr<IDXGIDevice2> dxgiDevice;
            if (HRESULT hr = device.As(&dxgiDevice); FAILED(hr))
            {
                return hr;
            }

            ComPtr<IDXGIAdapter> dxgiAdapter;
            if (HRESULT hr = dxgiDevice->GetAdapter(&dxgiAdapter); FAILED(hr))
            {
                return hr;
            }

            ComPtr<IDXGIFactory2> dxgiFactory;
            if (HRESULT hr = dxgiAdapter->GetParent(
                    __uuidof(IDXGIFactory2), reinterpret_cast<void **>(dxgiFactory.GetAddressOf()));
                FAILED(hr))
            {
                return hr;
            }
            DXGI_SWAP_CHAIN_DESC1 sDesc = {.Width = static_cast<UINT>(WINDOW_WIDTH),
                                           .Height = static_cast<UINT>(WINDOW_HEIGHT),
                                           .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
                                           .SampleDesc = {.Count = 1, .Quality = 0},
                                           .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                                           .BufferCount = 2,
                                           .Scaling = DXGI_SCALING_STRETCH,
                                           .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
                                           .Flags = 0};

            if (HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(device.Get(), hwnd, &sDesc,
                                                                 nullptr, nullptr, &swapChain);
                FAILED(hr))
            {
                return hr;
            }
        }

        // Create the Render Target View and associate it with the Back Buffer of the Swap Chain
        {
            ComPtr<ID3D11Texture2D> backBuffer;
            if (HRESULT hr =
                    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                         reinterpret_cast<void **>(backBuffer.GetAddressOf()));
                FAILED(hr))
            {
                return hr;
            }
            if (HRESULT hr =
                    device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
                FAILED(hr))
            {
                return hr;
            }
        }

        {
            ComPtr<ID3DBlob> vsBlob;
            ComPtr<ID3DBlob> psBlob;
            ComPtr<ID3DBlob> errorBlob;

            // use d3dcompiler.h to compile the vertex shader blob at runtime
            if (HRESULT hr = D3DCompileFromFile(L"shaders/VertexShader.hlsl", nullptr, nullptr,
                                                "VS_Main", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
                FAILED(hr))
            {
#if defined(_DEBUG)
                if (errorBlob)
                {
                    const char *compileErrors =
                        static_cast<const char *>(errorBlob->GetBufferPointer());
                    OutputDebugStringA("(Vertex) Shader Compilation Error:\n");
                    OutputDebugStringA(compileErrors);
                }
#endif
                return hr;
            }
            // initialize vertexShader (opaque COM object)
            if (HRESULT hr = device->CreateVertexShader(
                    vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
                FAILED(hr))
                return hr;
            // compile pixel shader blob
            if (HRESULT hr = D3DCompileFromFile(L"shaders/PixelShader.hlsl", nullptr, nullptr,
                                                "PSMain", "ps_5_0", 0, 0, &psBlob, &errorBlob);
                FAILED(hr))
            {
#if defined(_DEBUG)
                if (errorBlob)
                {
                    const char *compileErrors =
                        static_cast<const char *>(errorBlob->GetBufferPointer());
                    OutputDebugStringA("(Pixel) Shader Compilation Error:\n");
                    OutputDebugStringA(compileErrors);
                }
#endif
                return hr;
            }
            // initialize pixelShader
            if (HRESULT hr = device->CreatePixelShader(
                    psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
                FAILED(hr))
            {
                return hr;
            }

            D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}};

            if (HRESULT hr =
                    device->CreateInputLayout(vertexLayoutDesc, 2, vsBlob->GetBufferPointer(),
                                              vsBlob->GetBufferSize(), &inputLayout);
                FAILED(hr))
            {
                return hr;
            }
        }
        D3D11_BUFFER_DESC constantBufferDesc = {.ByteWidth = sizeof(MatrixBufferType),
                                                .Usage = D3D11_USAGE_DYNAMIC,
                                                .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
                                                .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
                                                .MiscFlags = 0,
                                                .StructureByteStride = 0};

        if (HRESULT hr = device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
            FAILED(hr))
        {
            return hr;
        }

        return S_OK;
    }

    void DrawFrame(ID3D11Buffer *quadVertexBuffer, float screenX, float screenY, float width,
                   float height)
    {
        XMMATRIX liveProjection = DirectX::XMMatrixOrthographicOffCenterLH(
            0.0f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f, 0.0f,
            1.0f);

        auto liveWorld = DirectX::XMMatrixScaling(width, -height, 1.0f) *
                         DirectX::XMMatrixTranslation(screenX, screenY, 0.0f);

        MatrixBufferType cbData;

        DirectX::XMStoreFloat4x4(&cbData.projection, DirectX::XMMatrixTranspose(liveProjection));
        DirectX::XMStoreFloat4x4(&cbData.world, DirectX::XMMatrixTranspose(liveWorld));

        D3D11_MAPPED_SUBRESOURCE mappedResource;

        deviceContext->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        std::memcpy(mappedResource.pData, &cbData, sizeof(MatrixBufferType));
        deviceContext->Unmap(constantBuffer.Get(), 0);

        deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
    }

  private:
    // equivalent of ID2D1Factory, but coupled to GPU and owns allocations
    ComPtr<ID3D11Device1> device;
    // primary state machine
    ComPtr<ID3D11DeviceContext1> deviceContext;
    // owns back buf, front buf, state machine, and timing/sync info
    ComPtr<IDXGISwapChain1> swapChain;
    // part of the output group, last stage
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    // object buffer to perform Map/Unmap, writing to GPU memory
    ComPtr<ID3D11Buffer> constantBuffer;
    ComPtr<ID3D11InputLayout> inputLayout;
    // compiled shader bytes
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11VertexShader> vertexShader;
};
