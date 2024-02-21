#include "stdafx.h"
#include "VertexBuffer.h"
#include "Graphics.h"
#include "VertexFormat.h"

VertexBuffer::VertexBuffer(const void* vertData, uint32_t vertCount, uint32_t vertStride,
						const void* indexData, uint32_t indexCount, uint32_t indexStride)
	:mVertexStride(vertStride)
	,mIndexNum(indexCount)
{
	Graphics* pGraphics = Graphics::Get();
	mVertexBuffer = pGraphics->CreateGraphicsBuffer(vertData, (int)vertCount * vertStride, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
	mIndexBuffer = pGraphics->CreateGraphicsBuffer(indexData, (int)indexCount * indexStride, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

	switch (indexStride)
	{
	case 4:
		indexBufferFormat = DXGI_FORMAT_R32_UINT;
		break;
	case 2:
		indexBufferFormat = DXGI_FORMAT_R16_UINT;
		break;
	case 1:
		indexBufferFormat = DXGI_FORMAT_R8_UINT;
		break;
	default:
		indexBufferFormat = DXGI_FORMAT_UNKNOWN;
		break;
	}
}

VertexBuffer::~VertexBuffer()
{
	mVertexBuffer->Release();
	mIndexBuffer->Release();
}

void VertexBuffer::Draw() const
{
	Graphics* pGraphics = Graphics::Get();
	ID3D11DeviceContext* devCon = pGraphics->GetDeviceContext();
	UINT stride = mVertexStride;
	UINT offset = 0;
	devCon->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	devCon->IASetIndexBuffer(mIndexBuffer, indexBufferFormat, 0);
	devCon->DrawIndexed(mIndexNum, 0, 0);
}