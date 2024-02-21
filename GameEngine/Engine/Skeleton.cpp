#include "stdafx.h"
#include "Skeleton.h"
#include "assetManager.h"
#include <fstream>
#include <sstream>
#include "jsonUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
void Skeleton::ComputeGlobalInvBindPose()
{
	for (auto& b : mBones) {
		if (b.mParent == -1) {
			mGlobalInvBindMatrices.emplace_back(b.mTransform.ToMatrix());
		}
		else {
			mGlobalInvBindMatrices.emplace_back(b.mTransform.ToMatrix() *
				mGlobalInvBindMatrices[b.mParent]);

		}
	}
	for (auto& m : mGlobalInvBindMatrices) {
		m.Invert();
	}
}

Skeleton* Skeleton::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
	Skeleton* pSkeleton = new Skeleton();
	if (false == pSkeleton->Load(fileName)) {
		delete pSkeleton;
		return nullptr;
	}
	return pSkeleton;
}

bool Skeleton::Load(const WCHAR* fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		DbgAssert(false, "Unable to open Skeleton file");
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "skel" ||
		ver != 1)
	{
		DbgAssert(false, "Skeleton File Incorrect Version");
		return false;
	}

	// Get bonecount;
	int bonecount = doc["bonecount"].GetInt();

	// Get bones
	const rapidjson::Value& bones = doc["bones"];
	if (!bones.IsArray())
	{
		return false;
	}
	std::string name;
	int parent;
	Quaternion boneRot;
	Vector3 boneTranslat;
	for (rapidjson::SizeType i = 0; i < bones.Size(); i++) {
		if (!bones[i].IsObject()) {
			return false;
		}
		name = bones[i]["name"].GetString();
		parent = bones[i]["parent"].GetInt();
		GetQuaternionFromJSON(bones[i]["bindpose"], "rot", boneRot);
		GetVectorFromJSON(bones[i]["bindpose"], "trans", boneTranslat);
		BoneTransform boneTransf = { boneRot, boneTranslat };
		Bone bone = { boneTransf, name, parent };
		mBones.emplace_back(bone);
	}
	ComputeGlobalInvBindPose();
	return true;
}
