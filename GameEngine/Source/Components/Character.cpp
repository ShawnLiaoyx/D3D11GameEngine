#include "stdafx.h"
#include "Character.h"
#include "SkinnedObj.h"
#include "Animation.h"
#include "Skeleton.h"
#include "game.h"
#include "stringUtil.h"
#include "AnimJob.h"

Character::Character(SkinnedObj* pObj, Game* pGame)
	:Component(pObj)
	,mGame(pGame)
	,mSkinnedObj(pObj)
	,currentAnim(nullptr)
	,mSkeleton(nullptr)
{
}

void Character::LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager)
{
	std::wstring skeletonName;
	GetWStringFromJSON(properties, "skeleton", skeletonName);
	mSkeleton = pAssetManager->LoadSkeleton(skeletonName);
	const rapidjson::Value& anims = properties["animations"];
	if (!anims.IsArray()) {
		DbgAssert(false, "Character fails to load.");
		return;
	}
	std::string animFile;
	std::wstring animFileW;
	for (rapidjson::SizeType i = 0; i < anims.Size(); i++)
	{
		if (!anims[i].IsArray() || anims[i].Size() != 2) {
			DbgAssert(false, "Character fails to load.");
			return;
		}
		animFile = anims[i][1].GetString();
		StringUtil::String2WString(animFileW, animFile);
		Animation* newAnim = pAssetManager->LoadAnimation(animFileW);
		mAnims.emplace(anims[i][0].GetString(), newAnim);
	}
}

void Character::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const
{
	// Thread Safe
	if (currentAnim == nullptr) return;
	const Animation* cacheAnim = currentAnim;
	outPoses.clear();
	const std::vector<Skeleton::Bone>& bones = inSkeleton->GetBones();
	float timePerFrame = cacheAnim->GetLength() / (cacheAnim->GetNumFrames() - 1);
	size_t frameIndex = inTime / timePerFrame;
	float frameLerp = (inTime - timePerFrame * frameIndex) / timePerFrame;
	size_t frameNext = frameIndex + 1;
	if (frameNext >= cacheAnim->GetNumFrames()) frameNext -= cacheAnim->GetNumFrames();
	for (size_t i = 0; i < bones.size(); i++) {
		Matrix4 boneMatrix;
		const std::vector<std::vector<BoneTransform>>& animData = cacheAnim->GetAnimData();
		if (i >= animData.size() || animData[i].empty()) {
			boneMatrix = Matrix4::Identity;
		}
		else {
			boneMatrix = BoneTransform::Interpolate(animData[i][frameIndex], animData[i][frameNext], frameLerp).ToMatrix();
		}
		if (bones[i].mParent == -1) {
			outPoses.emplace_back(boneMatrix);
		}
		else {
			boneMatrix = boneMatrix * outPoses[bones[i].mParent];
			outPoses.emplace_back(boneMatrix);
		}
	}
}

bool Character::SetAnim(const std::string& animName)
{
	if (mAnims.find(animName) != mAnims.end()) {
		currentAnim = mAnims[animName];
		currentTime = 0.0f;
		return true;
	}
	else {
		return false;
	}
}

void Character::UpdateAnim(float deltaTime) 
{
	currentTime += deltaTime;
	while (currentTime > currentAnim->GetLength())
	{
		currentTime -= currentAnim->GetLength();
	}
	AnimJob* pJob = new AnimJob(this);
	mGame->GetJobmanager()->AddJob(pJob);
}

void Character::Update(float deltaTime)
{
	if (currentAnim == nullptr) SetAnim("run");
	UpdateAnim(deltaTime);
}