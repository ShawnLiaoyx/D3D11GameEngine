#pragma once
#include "Physics.h"
#include "Component.h"

class CollisionBox : public Component
{
public:
	CollisionBox(RenderObj* pObj, Physics* pPhysics);
	~CollisionBox();
	const Physics::AABB GetAABB() const;
	void LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager) override;
protected:
	Physics::AABB mAABB;
	Physics* mPhysics;
};

