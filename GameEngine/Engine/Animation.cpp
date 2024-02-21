#include "stdafx.h"
#include "Animation.h"
#include "assetManager.h"
#include <fstream>
#include <sstream>
#include "jsonUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"

Animation* Animation::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
	Animation* pAnim = new Animation();
	if (false == pAnim->Load(fileName)) {
		delete pAnim;
		return nullptr;
	}
	return pAnim;
}

bool Animation::Load(const WCHAR* fileName)
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
		DbgAssert(false, "Unable to open Animation file");
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "anim" ||
		ver != 2)
	{
		DbgAssert(false, "Animation File Incorrect Version");
		return false;
	}

	//Get Sequence
	const rapidjson::Value& seq = doc["sequence"];
	if (!seq.IsObject())
	{
		return false;
	}
	mFrameNum = seq["frames"].GetUint();
	GetFloatFromJSON(seq, "length", mLen);
	mBoneNum = seq["bonecount"].GetUint();
	Quaternion rotation;
	Vector3 translation;
	const rapidjson::Value& tracks = seq["tracks"];
	if (!tracks.IsArray()) {
		return false;
	}
	mAnimData.reserve(mBoneNum);
	for (rapidjson::SizeType i = 0; i < tracks.Size(); i++) {
		if (!tracks[i].IsObject()) {
			return false;
		}
		mAnimData.emplace_back();
		const rapidjson::Value& transforms = tracks[i]["transforms"];
		if (!transforms.IsArray()) {
			return false;
		}
		for (rapidjson::SizeType j = 0; j < transforms.Size(); j++) {
			if (!transforms[j].IsObject()) {
				return false;
			}
			GetQuaternionFromJSON(transforms[j], "rot", rotation);
			GetVectorFromJSON(transforms[j], "trans", translation);
			BoneTransform boneTransf = { rotation, translation };
			mAnimData.back().emplace_back(boneTransf);
		}
	}
	return true;
}
