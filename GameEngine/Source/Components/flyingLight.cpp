#include "stdafx.h"
#include "Components/flyingLight.h"
#include "game.h"
#include "jsonUtil.h"
#include "RenderObj.h"

FlyingLight::FlyingLight(RenderObj* pObj, Game* pGame)
	: Component(pObj)
    , mAngle(0.0f)
    , mCenter(0.0f, 0.0f, 0.0f)
    , mScale(0.0f)
    , mRadius(0.0f)
    , mRotSpeed(0.0f)
    , mZSpeed(0.0f)
    , mZHeight(0.0f)
{
    mCenter = mRenderObj->mConstant.data.GetTranslation();
    mScale = mRenderObj->mConstant.data.GetScale().x;
}

FlyingLight::~FlyingLight()
{
}

void FlyingLight::LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager)
{
    GetFloatFromJSON(properties, "startAngle", mAngle);
    GetFloatFromJSON(properties, "radius", mRadius);
    GetFloatFromJSON(properties, "rotSpeed", mRotSpeed);
    GetFloatFromJSON(properties, "zSpeed", mZSpeed);
    GetFloatFromJSON(properties, "zHeight", mZHeight);
}

void FlyingLight::Update(float deltaTime)
{
    mAngle += deltaTime;
    float time = mAngle * mRotSpeed;
    float zTime = mAngle * mZSpeed;
    Vector3 pos = Vector3(mRadius * cosf(time), mRadius * sinf(time), mZHeight * sinf(zTime));
    pos += mCenter;
	mRenderObj->mConstant.data = Matrix4::CreateScale(mScale) * Matrix4::CreateTranslation(pos);
}

