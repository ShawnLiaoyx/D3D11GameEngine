#include "stdafx.h"
#include "Graphics.h"
#include "Shader.h"
#include "engineMath.h"

#pragma comment (lib, "d3d11.lib") 

/*static*/ Graphics* Graphics::s_theGraphics = nullptr;


Graphics::Graphics()
    : mScreenWidth(0)
    , mScreenHeight(0)
    , mNumViews(0)
    , mSwapChain(nullptr)
    , mDev(nullptr)
    , mDevCon(nullptr)
    , mBackBuffer(nullptr)
    , mCurrentRenderTargets{nullptr}
    , mDepthTexture(nullptr)
    , mDepthStencilView(nullptr)
    , mSamplerState(nullptr)
{
    DbgAssert(nullptr == s_theGraphics, "You can only have 1 Graphics");
    s_theGraphics = this;
}

Graphics::~Graphics()
{
    DbgAssert(this == s_theGraphics, "There can only be 1 Graphics");
    s_theGraphics = nullptr;
}

void Graphics::InitD3D(HWND hWnd, float width, float height)
{
    mScreenWidth = width;
    mScreenHeight = height;

    HRESULT hr = S_OK;

    { // set up the swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = static_cast<UINT>(width);
        sd.BufferDesc.Height = static_cast<UINT>(height);
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
        UINT               numLevelsRequested = 1;
        D3D_FEATURE_LEVEL  FeatureLevelsSupported;
        hr = D3D11CreateDeviceAndSwapChain(NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
#ifdef _DEBUG
            D3D11_CREATE_DEVICE_DEBUG,
#else
            0,
#endif
            & FeatureLevelsRequested,
            numLevelsRequested,
            D3D11_SDK_VERSION,
            &sd,
            &mSwapChain,
            &mDev,
            &FeatureLevelsSupported,
            &mDevCon);
        DbgAssert(hr == S_OK, "Failed to create device");
    }

    { // set up the viewport (full screen)
        SetViewport(0.0f, 0.0f, width, height);
    }

    { // grab the back-buffer as our render target
        ID3D11Texture2D* pBackBuffer;
        hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        DbgAssert(hr == S_OK, "Something wrong with your back buffer");
        hr = mDev->CreateRenderTargetView(pBackBuffer, nullptr, &mBackBuffer);
        DbgAssert(hr == S_OK, "Something went wrong with your back buffer");
        pBackBuffer->Release();
    }

    // we'll be drwaing triangle lists
    mDevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Z-buffers
    ID3D11DepthStencilState* depthState = this->CreateDepthStencilState(D3D11_COMPARISON_LESS_EQUAL);
    mDevCon->OMSetDepthStencilState(depthState, 0);
    depthState->Release();
    this->CreateDepthStencil((int)width, (int)height, &mDepthTexture, &mDepthStencilView);

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    mDev->CreateSamplerState(&sampDesc, &mSamplerState);
    this->SetActiveSampler(0, mSamplerState);

    //Change from CW to CCW
    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(rasterDesc));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = true;
    ID3D11RasterizerState* rasterState = nullptr;
    HRESULT hrastResult = mDev->CreateRasterizerState(&rasterDesc, &rasterState);
    mDevCon->RSSetState(rasterState);
    rasterState->Release();
}

void Graphics::CleanD3D()
{
#ifdef _DEBUG
    ID3D11Debug* pDbg = nullptr;
    HRESULT hr = mDev->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDbg));
    DbgAssert(S_OK == hr, "Unable to create debug device");
#endif
    
    mSamplerState->Release();
    mDepthTexture->Release();
    mDepthStencilView->Release();
    mSwapChain->Release();
    mDev->Release();
    mDevCon->Release();

    mBackBuffer->Release();
    
    
#ifdef _DEBUG
    pDbg->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
    pDbg->Release();
#endif
}

void Graphics::SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* pDepthStencilView)
{
    SetRenderTargets(1, &renderTarget, pDepthStencilView);
}

void Graphics::ClearRenderTarget(const Color4 &clearColor)
{
    ClearRenderTargets(clearColor);
}

void Graphics::ClearRenderTarget(const Color4& clearColor, UINT viewIndex)
{
    mDevCon->ClearRenderTargetView(mCurrentRenderTargets[viewIndex], reinterpret_cast<const float*>(&clearColor));
}

void Graphics::SetRenderTargets(UINT numViews, ID3D11RenderTargetView* const* renderTargets, ID3D11DepthStencilView* pDepthStencilView)
{
    mNumViews = numViews;
    CopyMemory(mCurrentRenderTargets, renderTargets, mNumViews * sizeof(ID3D11RenderTargetView*));
    mDevCon->OMSetRenderTargets(mNumViews, mCurrentRenderTargets, pDepthStencilView);
}

void Graphics::ClearRenderTargets(const Color4& clearColor)
{
    for (UINT i = 0; i < mNumViews; i++) {
        mDevCon->ClearRenderTargetView(mCurrentRenderTargets[i], reinterpret_cast<const float*>(&clearColor));
    }
}

void Graphics::EndFrame()
{
    mSwapChain->Present(1, 0);
}

ID3D11Buffer* Graphics::CreateGraphicsBuffer(const void* initialData, int inDataSize, D3D11_BIND_FLAG inBindFlags, D3D11_CPU_ACCESS_FLAG inCPUAccessFlags, D3D11_USAGE inUsage)
{
    ID3D11Buffer* buffer = nullptr;
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    desc.Usage = inUsage;
    desc.ByteWidth = inDataSize;
    desc.BindFlags = inBindFlags;
    desc.CPUAccessFlags = inCPUAccessFlags;
    HRESULT hr = GetDevice()->CreateBuffer(&desc, nullptr, &buffer);
    DbgAssert(hr == S_OK, "Unable to create graphics buffer");

    if (nullptr != initialData)
        UploadBuffer(buffer, initialData, inDataSize);
    return buffer;
}

void Graphics::UploadBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize)
{
    D3D11_MAPPED_SUBRESOURCE map;
    HRESULT hr = GetDeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
    DbgAssert(hr == S_OK, "Map failed");
    memcpy(map.pData, data, dataSize);
    GetDeviceContext()->Unmap(buffer, 0);
}

void Graphics::SetViewport(float x, float y, float width, float height)
{
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = x;
    viewport.TopLeftY = y;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    mDevCon->RSSetViewports(1, &viewport);
}

ID3D11DepthStencilState* Graphics::CreateDepthStencilState(D3D11_COMPARISON_FUNC inDepthComparisonFunction)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    // Depth test parameters
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = inDepthComparisonFunction;

    // Create depth stencil state
    ID3D11DepthStencilState* pDSState;
    mDev->CreateDepthStencilState(&dsDesc, &pDSState);

    return pDSState;
}

bool Graphics::CreateDepthStencil(int inWidth, int inHeight, ID3D11Texture2D** pDepthTexture, ID3D11DepthStencilView** pDepthView)
{
    D3D11_TEXTURE2D_DESC descDepth;
    descDepth.Width = inWidth;
    descDepth.Height = inHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    HRESULT hr = mDev->CreateTexture2D(&descDepth, NULL, pDepthTexture);
    if (hr != S_OK) {
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    hr = mDev->CreateDepthStencilView(*pDepthTexture, &descDSV, pDepthView);
    return hr == S_OK;
}

void Graphics::ClearDepthBuffer(ID3D11DepthStencilView* depthView, float depth)
{
    mDevCon->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH, depth, 0);
}

void Graphics::SetActiveTexture(int slot, ID3D11ShaderResourceView* pView)
{
    mDevCon->PSSetShaderResources(slot, 1, &pView);
}

void Graphics::SetActiveSampler(int slot, ID3D11SamplerState* pSampler)
{
    mDevCon->PSSetSamplers(slot, 1, &pSampler);
}

ID3D11BlendState* Graphics::CreateBlendState(
    bool enable, D3D11_BLEND srcBlend, D3D11_BLEND dstBlend)
{
    D3D11_BLEND_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.RenderTarget[0].BlendEnable = enable;
    desc.RenderTarget[0].SrcBlend = srcBlend;
    desc.RenderTarget[0].DestBlend = dstBlend;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    ID3D11BlendState* blendState;
    mDev->CreateBlendState(&desc, &blendState);

    return blendState;
}

void Graphics::SetBlendState(ID3D11BlendState* inBlendState)
{
    float blendFactor[4];
    // Setup the default blend factor if inBlendState is null
    blendFactor[0] = 1.0f;
    blendFactor[1] = 1.0f;
    blendFactor[2] = 1.0f;
    blendFactor[3] = 1.0f;
    mDevCon->OMSetBlendState(inBlendState, blendFactor, 0xffffffff);
}
