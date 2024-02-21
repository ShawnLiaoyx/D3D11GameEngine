#pragma once
#include "engineMath.h"
#include "Graphics.h"

struct VertexPosColor
{
    Vector3 pos;
    Graphics::Color4 color;
};

struct VertexPosColorUV
{
    Vector3 pos;
    Graphics::Color4 color;
    Vector2 uv;
};

struct VertexPosColorUVNormal
{
    Vector3 pos;
    Vector3 normal;
    Graphics::Color4 color;
    Vector2 uv;
};

struct VertexPosNormalUV
{
    Vector3 pos;
    Vector3 normal;
    Vector2 uv;
};

struct VertexPosNormalBoneWeightUV
{
    Vector3 pos;
    Vector3 normal;
    unsigned char bone[4];
    unsigned char weight[4];
    Vector2 uv;
};

struct VertexPosNormalTangentUV
{
    Vector3 pos;
    Vector3 normal;
    Vector3 tangent;
    Vector2 uv;
};

struct VertexPosUV
{
    Vector3 pos;
    Vector2 uv;
};

