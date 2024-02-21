#pragma once
#include "component.h"
#include "engineMath.h"

class Game;

class FlyingLight : public Component
{
public:
	FlyingLight(RenderObj* pObj, Game* pGame);
	~FlyingLight();

	void Update(float deltaTime) override;
    void LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager) override;

private:
    float mAngle;
    Vector3 mCenter;
    float mScale;
    float mRadius;
    float mRotSpeed;
    float mZSpeed;
    float mZHeight;
};