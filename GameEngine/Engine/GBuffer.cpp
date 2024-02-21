#include "stdafx.h"
#include "GBuffer.h"
#include "texture.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "VertexFormat.h"

static const VertexPosUV offScreenVertex[] = {
	{ Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f) },
	{ Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) },
	{ Vector3(1.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f) },
	{ Vector3(-1.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f) }
};

static const uint16_t offScreenIndex[] = {
	0, 1, 2,
	0, 2, 3
};

GBuffer::GBuffer(float width, float height, Graphics::TextureSlot textSlot)
	:mWidth(width), mHeight(height), mTextSlot(textSlot)
{
	mOffScreenText = new Texture();
	mOffScreenTarget = mOffScreenText->CreateRenderTarget(static_cast<int>(width), static_cast<int>(height), DXGI_FORMAT_R32G32B32A32_FLOAT);
	mOffScreenBuffer = new VertexBuffer((const void*)&offScreenVertex, ARRAYSIZE(offScreenVertex), sizeof(VertexPosUV),
		(const void*)&offScreenIndex, ARRAYSIZE(offScreenIndex), sizeof(uint16_t));
}

GBuffer::~GBuffer()
{
	delete mOffScreenBuffer;
	mOffScreenText->Free();
	mOffScreenTarget->Release();
}

void GBuffer::Draw(const Shader* shader)
{
	DrawAs(shader, mOffScreenBuffer);
}

void GBuffer::DrawAs(const Shader* shader, VertexBuffer* pVertexBuffer)
{
	shader->SetActive();
	mOffScreenText->SetActive(mTextSlot);
	pVertexBuffer->Draw();
	Graphics::Get()->SetActiveTexture(mTextSlot, nullptr);
}

static inline Vector2 ClampUV(float x, float y) {
	return Vector2(
		Math::Clamp((x + 1.0f) / 2, 0.0f, 1.0f),
		Math::Clamp((-y + 1.0f) / 2, 0.0f, 1.0f)
	);
}

static inline Vector3 ClampScreen(float x, float y) {
	return Vector3(
		Math::Clamp(x, -1.0f, 1.0f),
		Math::Clamp(y, -1.0f, 1.0f),
		0.0f
	);
}

void GBuffer::DrawInRect(const Shader* shader, const Vector3& upRight, const Vector3& bottomLeft)
{
	VertexPosUV rect[] = {
		{ ClampScreen(bottomLeft.x, bottomLeft.y), ClampUV(bottomLeft.x, bottomLeft.y)},
		{ ClampScreen(upRight.x, bottomLeft.y), ClampUV(upRight.x, bottomLeft.y)},
		{ ClampScreen(upRight.x, upRight.y), ClampUV(upRight.x, upRight.y)},
		{ ClampScreen(bottomLeft.x, upRight.y), ClampUV(bottomLeft.x, upRight.y)}
	};

	VertexBuffer* pVertexBuffer = new VertexBuffer(rect, ARRAYSIZE(rect), sizeof(VertexPosUV),
		(const void*)&offScreenIndex, ARRAYSIZE(offScreenIndex), sizeof(uint16_t));
	DrawAs(shader, pVertexBuffer);
	delete pVertexBuffer;
}

void GBuffer::SetTextureActive()
{
	mOffScreenText->SetActive(mTextSlot);
}

void GBuffer::DisableTexture()
{
	Graphics::Get()->SetActiveTexture(mTextSlot, nullptr);
}

void GBuffer::SetAsRenderTarget(ID3D11DepthStencilView* pDepthStencilView)
{
	Graphics::Get()->SetViewport(0.0f, 0.0f, mWidth, mHeight);
	Graphics::Get()->SetRenderTarget(mOffScreenTarget, pDepthStencilView);
}

