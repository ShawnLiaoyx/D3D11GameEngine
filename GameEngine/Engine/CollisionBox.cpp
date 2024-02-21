#include "stdafx.h"
#include "CollisionBox.h"

CollisionBox::CollisionBox(RenderObj* pObj, Physics* pPhysics)
	:Component(pObj)
	,mPhysics(pPhysics)
{
	mPhysics->AddObj(this);
}

CollisionBox::~CollisionBox()
{
	mPhysics->RemoveObj(this);
}

void CollisionBox::LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager)
{
	Vector3 min, max;
	GetVectorFromJSON(properties, "min", min);
	GetVectorFromJSON(properties, "max", max);
	mAABB = Physics::AABB(min, max);
}

const Physics::AABB CollisionBox::GetAABB() const
{
	float scale = mRenderObj->mConstant.data.GetScale().x;
	Vector3 translate = mRenderObj->mConstant.data.GetTranslation();
	return Physics::AABB(mAABB.minCorner * scale + translate, mAABB.maxCorner * scale + translate);
}