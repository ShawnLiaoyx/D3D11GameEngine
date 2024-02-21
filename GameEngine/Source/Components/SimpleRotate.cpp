#include "stdafx.h"
#include "SimpleRotate.h"

SimpleRotate::SimpleRotate(RenderObj* pObj)
	:Component(pObj)
{

}

void SimpleRotate::LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager)
{
	GetFloatFromJSON(properties, "speed", mSpeed);
}

void SimpleRotate::Update(float deltaTime)
{
	mRotation += mSpeed * deltaTime;
	mRenderObj->mConstant.data = Matrix4::CreateScale(mRenderObj->mConstant.data.GetScale()) * Matrix4::CreateRotationZ(mRotation);
}