#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <wrl.h>

#define COLS 32
#define ROWS 32
#define CELL_PX 20
#define WIDTH (COLS * CELL_PX)
#define HEIGHT (ROWS * CELL_PX)

class Graphics2DEngine
{
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  private:
    // equivalent of ID2D1Factory, but coupled to GPU and owns allocations
    ComPtr<ID3D11Device1> device;
    // primary state machine
    ComPtr<ID3D11DeviceContext1> deviceContext;
    // owns back buf, front buf, state machine, and timing/sync info
    ComPtr<IDXGISwapChain1> swapChain;
    // part of the output group, last stage
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    // compiled shader bytes
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11VertexShader> vertexShader;

  public:
    // default init to nullptr all
    Graphics2DEngine() = default;

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

        {
            HRESULT hr =
                D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, featureLevels,
                                  ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &baseDevice, &chosenLevel, &baseContext);
            if (FAILED(hr))
                return E_FAIL;
        }
        if (FAILED(baseDevice.As(&device)))
            return E_FAIL;

        if (FAILED(baseContext.As(&deviceContext)))
            return E_FAIL;

        ComPtr<IDXGIDevice2> dxgiDevice;
        if (FAILED(device.As(&dxgiDevice)))
            return E_FAIL;

        ComPtr<IDXGIAdapter> dxgiAdapter;
        if (FAILED(dxgiDevice->GetAdapter(&dxgiAdapter)))
            return E_FAIL;

        ComPtr<IDXGIFactory2> dxgiFactory;
        if (FAILED(
                dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void **>(dxgiFactory.GetAddressOf()))))
            return E_FAIL;

        DXGI_SWAP_CHAIN_DESC1 sDesc = {.Width = WIDTH,
                                       .Height = HEIGHT,
                                       .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
                                       .SampleDesc = {.Count = 1, .Quality = 0},
                                       .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                                       .BufferCount = 2,
                                       .Scaling = DXGI_SCALING_STRETCH,
                                       .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
                                       .Flags = 0};

        if (FAILED(dxgiFactory->CreateSwapChainForHwnd(device.Get(), hwnd, &sDesc, nullptr, nullptr, &swapChain)))
            return E_FAIL;

        ComPtr<ID3D11Texture2D> backBuffer;
        if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                        reinterpret_cast<void **>(backBuffer.GetAddressOf()))))
            return E_FAIL;

        if (FAILED(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView)))
            return E_FAIL;

        ComPtr<ID3DBlob> vsBlob;
        ComPtr<ID3DBlob> psBlob;
        ComPtr<ID3DBlob> errorBlob;

        // use d3dcompiler.h to compile the vertex shader blob at runtime
        if (FAILED(D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vsBlob,
                                      &errorBlob)))
        {
#if defined(_DEBUG)
            if (errorBlob)
            {
                const char *compileErrors = static_cast<const char *>(errorBlob->GetBufferPointer());
                OutputDebugStringA("(Vertex) Shader Compilation Error:\n");
                OutputDebugStringA(compileErrors);
            }
#endif
            return E_FAIL;
        }
        // initialize vertexShader (opaque COM object)
        if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr,
                                              &vertexShader)))
            return E_FAIL;
        // compile pixel shader blob
        if (FAILED(D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &psBlob,
                                      &errorBlob)))
        {
#if defined(_DEBUG)
            if (errorBlob)
            {
                const char *compileErrors = static_cast<const char *>(errorBlob->GetBufferPointer());
                OutputDebugStringA("(Pixel) Shader Compilation Error:\n");
                OutputDebugStringA(compileErrors);
            }
#endif
            return E_FAIL;
        }
        // initialize pixelShader
        if (FAILED(
                device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader)))
            return E_FAIL;

        return S_OK;
    }
};
