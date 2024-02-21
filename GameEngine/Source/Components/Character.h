#pragma once
#include <unordered_map>
#include <Component.h>
class SkinnedObj;
class Skeleton;
class Animation;
class Game;

class Character : public Component
{
public:
	Character(SkinnedObj* pObj, Game* pGame);
	void LoadProperties(const rapidjson::Value& properties, AssetManager* pAssetManager) override;
	void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const;
	bool SetAnim(const std::string& animName);
	void UpdateAnim(float deltaTime);
	void Update(float deltaTime) override;
protected:
	Game* mGame;
	SkinnedObj* mSkinnedObj;
	Skeleton* mSkeleton;
	std::unordered_map<std::string, const Animation*> mAnims;
	const Animation* currentAnim;
	float currentTime = 0.0f;
	friend class AnimJob;
};

