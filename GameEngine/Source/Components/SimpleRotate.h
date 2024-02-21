#pragma once
#include "Component.h"
class SimpleRotate : public Component
{
public:
	SimpleRotate(RenderObj* pObj);
	void LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager) override;
	void Update(float deltaTime) override;
private:
	float mRotation = 0;
	float mSpeed = 0;
};

