#pragma once
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include "jsonUtil.h"
#include "RenderObj.h"

class Component
{
public:
	Component(RenderObj* pObj);
	virtual ~Component();
	virtual void LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager);
	virtual void Update(float deltaTime);
protected:
	RenderObj* mRenderObj;
};

