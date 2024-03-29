#include "stdafx.h"
#include "camera.h"
#include "components\followCam.h"
#include "game.h"
#include "jsonUtil.h"
#include "renderObj.h"
#include "stringUtil.h"


FollowCam::FollowCam(RenderObj* pObj, Game* pGame)
    : Component(pObj)
    , mGame(pGame)
{
	Matrix4 mat = mRenderObj->mConstant.data;
	mat = Matrix4::CreateRotationX(Math::PiOver2) * mat;
}

void FollowCam::LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager)
{
    Component::LoadProperties(properties, pAssetManager);
    GetVectorFromJSON(properties, "offset", mOffset);
}

void FollowCam::Update(float deltaTime)
{
    Camera* pCamera = mGame->GetCamera();
    Matrix4 mat = Matrix4::CreateTranslation(mRenderObj->mConstant.data.GetTranslation() + mOffset);
    mat.Invert();
    pCamera->SetWorldToCamera(mat);
}