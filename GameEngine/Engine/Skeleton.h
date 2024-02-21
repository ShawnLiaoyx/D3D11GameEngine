#pragma once
#include "BoneTransform.h"

class AssetManager;

class Skeleton
{
public:
	typedef struct Bone {
		BoneTransform mTransform;
		std::string mName;
		int mParent = -1;
	} Bone;

	size_t GetNumBones() const { return mBones.size(); }
	const Bone& GetBone(size_t idx) const { return mBones[idx]; }
	const std::vector<Bone>& GetBones() const { return mBones; }
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindMatrices; }

	static Skeleton* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);
	bool Load(const WCHAR* fileName);
private:
	std::vector<Bone> mBones;
	std::vector<Matrix4> mGlobalInvBindMatrices;

	void ComputeGlobalInvBindPose();
};

