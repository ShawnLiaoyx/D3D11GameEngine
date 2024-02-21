#include "stdafx.h"
#include "Component.h"

Component::Component(RenderObj* pObj)
	:mRenderObj(pObj)
{
	pObj->AddComponent(this);
}

Component::~Component()
{

}

void Component::LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager)
{

}

void Component::Update(float deltaTime)
{

}