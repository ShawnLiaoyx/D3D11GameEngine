#include "stdafx.h"
#include "RenderCube.h"
#include "Graphics.h"
#include "VertexBuffer.h"
#include "VertexFormat.h"
#include "material.h"

static VertexPosColorUVNormal cubeVertex[] =
{
{ Vector3(-0.5f, 0.5f, -0.5f), Vector3(0, 0, -1.0f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },
{ Vector3(0.5f, 0.5f, -0.5f), Vector3(0, 0, -1.0f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },
{ Vector3(0.5f, -0.5f, -0.5f), Vector3(0, 0, -1.0f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
{ Vector3(-0.5f, -0.5f, -0.5f), Vector3(0, 0, -1.0f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },

{ Vector3(-0.5f, -0.5f, 0.5f), Vector3(0, -1.0f, 0), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },
{ Vector3(0.5f, -0.5f, 0.5f), Vector3(0, -1.0f, 0), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },
{ Vector3(0.5f, -0.5f, -0.5f), Vector3(0, -1.0f, 0), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
{ Vector3(-0.5f, -0.5f, -0.5f), Vector3(0, -1.0f, 0), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },

{ Vector3(-0.5f, 0.5f, -0.5f), Vector3(-1.0f, 0, 0), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f) },
{ Vector3(-0.5f, 0.5f, 0.5f), Vector3(-1.0f, 0, 0), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f) },
{ Vector3(-0.5f, -0.5f, 0.5f), Vector3(-1.0f, 0, 0), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f) },
{ Vector3(-0.5f, -0.5f, -0.5f), Vector3(-1.0f, 0, 0), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f) },

{ Vector3(-0.5f, 0.5f, 0.5f), Vector3(0, 0, 1.0f), Graphics::Color4(1.0f, 0.7f, 0.7f, 1.0f), Vector2(0.0f, 0.0f) },
{ Vector3(0.5f, 0.5f, 0.5f), Vector3(0, 0, 1.0f), Graphics::Color4(1.0f, 0.7f, 0.7f, 1.0f), Vector2(0.0f, 1.0f) },
{ Vector3(0.5f, -0.5f, 0.5f), Vector3(0, 0, 1.0f), Graphics::Color4(1.0f, 0.7f, 0.7f, 1.0f), Vector2(1.0f, 1.0f) },
{ Vector3(-0.5f, -0.5f, 0.5f), Vector3(0, 0, 1.0f), Graphics::Color4(1.0f, 0.7f, 0.7f, 1.0f), Vector2(1.0f, 0.0f) },

{ Vector3(-0.5f, 0.5f, 0.5f), Vector3(0, 1.0f, 0), Graphics::Color4(0.7f, 1.0f, 0.7f, 1.0f), Vector2(0.0f, 0.0f) },
{ Vector3(0.5f, 0.5f, 0.5f), Vector3(0, 1.0f, 0), Graphics::Color4(0.7f, 1.0f, 0.7f, 1.0f), Vector2(0.0f, 1.0f) },
{ Vector3(0.5f, 0.5f, -0.5f), Vector3(0, 1.0f, 0), Graphics::Color4(0.7f, 1.0f, 0.7f, 1.0f), Vector2(1.0f, 1.0f) },
{ Vector3(-0.5f, 0.5f, -0.5f), Vector3(0, 1.0f, 0), Graphics::Color4(0.7f, 1.0f, 0.7f, 1.0f), Vector2(1.0f, 0.0f) },

{ Vector3(0.5f, 0.5f, -0.5f), Vector3(1.0f, 0, 0), Graphics::Color4(0.7f, 0.7f, 1.0f, 1.0f), Vector2(0.0f, 0.0f) },
{ Vector3(0.5f, 0.5f, 0.5f), Vector3(1.0f, 0, 0), Graphics::Color4(0.7f, 0.7f, 1.0f, 1.0f), Vector2(0.0f, 1.0f) },
{ Vector3(0.5f, -0.5f, 0.5f), Vector3(1.0f, 0, 0), Graphics::Color4(0.7f, 0.7f, 1.0f, 1.0f), Vector2(1.0f, 1.0f) },
{ Vector3(0.5f, -0.5f, -0.5f), Vector3(1.0f, 0, 0), Graphics::Color4(0.7f, 0.7f, 1.0f, 1.0f), Vector2(1.0f, 0.0f) },

};

static uint16_t cubeIndex[] =
{
2, 1, 0,
3, 2, 0,

4, 5, 6,
4, 6, 7,

8, 9, 10,
8, 10, 11,

12, 13, 14,
12, 14, 15,

18, 17, 16,
19, 18, 16,

22, 21, 20,
23, 22, 20,
};

RenderCube::RenderCube(Material* material)
	:RenderObj(new Mesh(new VertexBuffer((const void*)cubeVertex, sizeof(cubeVertex) / sizeof(VertexPosColorUVNormal), sizeof(VertexPosColorUVNormal),
				(const void*)cubeIndex, sizeof(cubeIndex) / sizeof(uint16_t), sizeof(uint16_t)), material))
{
	Graphics* pGraphics = Graphics::Get();
	mObjectBuffer->Release();
	mObjectBuffer = pGraphics->CreateGraphicsBuffer((const void*)&mConstant, sizeof(mConstant), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

RenderCube::~RenderCube() {
	delete mMesh;
}