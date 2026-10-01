#pragma once

#include <d3d11_1.h>
#include <wrl.h>

struct Quad
{
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D11Buffer> vertexBuffer;

    Quad(ID3D11Device *device, float halfW, float halfH)
    {
        float verts[] = {halfW, halfH, 0.f, -halfW, halfH,  0.f, -halfW, -halfH, 0.f,
                         halfW, halfH, 0.f, -halfW, -halfH, 0.f, halfW,  -halfH, 0.f};
        D3D11_BUFFER_DESC bd = {
            .ByteWidth = sizeof(verts),
            .Usage = D3D11_USAGE_IMMUTABLE,
        };
    }
};
