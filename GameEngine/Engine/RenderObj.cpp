#include "stdafx.h"
#include "RenderObj.h"
#include "Graphics.h"
#include "Component.h"

RenderObj::RenderObj(const Mesh* mesh)
	:mMesh(mesh)
{
	Graphics* pGraphics = Graphics::Get();
	mObjectBuffer = pGraphics->CreateGraphicsBuffer((const void*)&mConstant, sizeof(mConstant), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

RenderObj::~RenderObj()
{
	mObjectBuffer->Release();
	for (auto& c : mComponents) {
		delete c;
	}
}

void RenderObj::Draw()
{
	Graphics* pGraphics = Graphics::Get(); 
	pGraphics->UploadBuffer(mObjectBuffer, (const void*)mConstant.data.mat, sizeof(mConstant.data.mat));
	pGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_RENDEROBJ, 1, &mObjectBuffer);
	mMesh->Draw();
}

void RenderObj::Update(float deltaTime)
{
	for (auto& c : mComponents) {
		c->Update(deltaTime);
	}
}

void RenderObj::AddComponent(Component* pComp)
{
	mComponents.emplace_back(pComp);
}

