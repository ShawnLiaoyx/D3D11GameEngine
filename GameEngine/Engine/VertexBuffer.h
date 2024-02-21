#pragma once

class VertexBuffer {
public:
	VertexBuffer(
		const void* vertData, uint32_t vertCount, uint32_t vertStride,
		const void* indexData, uint32_t indexCount, uint32_t indexStride);
	~VertexBuffer();
	void Draw() const;
private:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	uint32_t mVertexStride;
	uint32_t mIndexNum;
	DXGI_FORMAT indexBufferFormat;
};
