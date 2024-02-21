#pragma once
#include "Graphics.h"
#include "engineMath.h"
class Texture;
class VertexBuffer;
class Shader;
class GBuffer
{
public:
	GBuffer(float width, float height, Graphics::TextureSlot textSlot);
	~GBuffer();
	ID3D11RenderTargetView* GetRenderTarget() { return mOffScreenTarget; }
	void Draw(const Shader* shader);
	void DrawAs(const Shader* shader, VertexBuffer* pVertexBuffer);
	void DrawInRect(const Shader* shader, const Vector3& upRight, const Vector3& bottomLeft);
	void SetTextureActive();
	void DisableTexture();
	void SetAsRenderTarget(ID3D11DepthStencilView* pDepthStencilView);
private:
	Texture* mOffScreenText;
	ID3D11RenderTargetView* mOffScreenTarget;
	VertexBuffer* mOffScreenBuffer;
	float mWidth;
	float mHeight;
	Graphics::TextureSlot mTextSlot;
};

