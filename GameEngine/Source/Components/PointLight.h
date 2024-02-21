#pragma once
#include "Component.h"
#include "Lights.h"
class Game;
class PointLight : public Component
{
public:
	PointLight(RenderObj* pObj, Game* pGame);
	~PointLight();
	void LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager) override;
	void Update(float deltaTime) override;
private:
	Lights::PointLightData* mPointLightData;
	Game* mGame;
};

