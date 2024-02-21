#include "stdafx.h"
#include "SkinnedObj.h"
#include <algorithm>

SkinnedObj::SkinnedObj(const Mesh* mesh)
	:RenderObj(mesh)
{
	for (int i = 0; i < MAX_BONES; i++) mSkinnedConstant.data[i] = Matrix4::Identity;
	Graphics* pGraphics = Graphics::Get();
	mSkinnedConstantBuffer = pGraphics->CreateGraphicsBuffer((const void*)&mSkinnedConstant, sizeof(mSkinnedConstant), 
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

void SkinnedObj::Draw()
{
	Graphics* pGraphics = Graphics::Get();
	pGraphics->UploadBuffer(mSkinnedConstantBuffer, (const void*)mSkinnedConstant.data, sizeof(mSkinnedConstant.data));
	pGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_SKINNED, 1, &mSkinnedConstantBuffer);
	RenderObj::Draw();
}

SkinnedObj::~SkinnedObj()
{
	mSkinnedConstantBuffer->Release();
}