#pragma once
#include "BoneTransform.h"

class AssetManager;

class Animation
{
public:
	uint32_t GetNumBones() const { return mBoneNum; }
	uint32_t GetNumFrames() const { return mFrameNum; }
	float GetLength() const { return mLen; }
	const std::vector<std::vector<BoneTransform>>& GetAnimData() const { return mAnimData; }

	static Animation* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);
	bool Load(const WCHAR* fileName);
private:
	uint32_t mBoneNum = 0;
	uint32_t mFrameNum = 0;
	float mLen = 0.0f;
	std::vector<std::vector<BoneTransform>> mAnimData;
};

