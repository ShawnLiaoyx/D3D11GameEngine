#include "stdafx.h"
#include "PointLight.h"
#include "game.h"

PointLight::PointLight(RenderObj* pObj, Game* pGame)
	:Component(pObj)
	,mGame(pGame)
{
	mPointLightData = mGame->AllocateLight();
}

PointLight::~PointLight()
{
	if (mPointLightData != nullptr) {
		mGame->FreeLight(mPointLightData);
	}
}

void PointLight::LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager)
{
	if (mPointLightData != nullptr) {
		Vector3 lightColor;
		float innerRadius, outerRadius;
		GetVectorFromJSON(properties, "lightColor", lightColor);
		GetFloatFromJSON(properties, "innerRadius", innerRadius);
		GetFloatFromJSON(properties, "outerRadius", outerRadius);
		mPointLightData->lightColor = lightColor;
		mPointLightData->innerRadius = innerRadius;
		mPointLightData->outerRadius = outerRadius;
	}
}

void PointLight::Update(float deltaTime)
{
	if (mPointLightData != nullptr) {
		mPointLightData->position = mRenderObj->mConstant.data.GetTranslation();
	}
}