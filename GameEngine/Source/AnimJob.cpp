#include "stdafx.h"
#include "AnimJob.h"
#include "Animation.h"
#include "Components/Character.h"
#include "SkinnedObj.h"
#include "Animation.h"
#include "Skeleton.h"

AnimJob::AnimJob(Character* pCh)
	:mCharacter(pCh)
{

}

void AnimJob::DoIt()
{
	std::vector<Matrix4> worldPoses;
	const std::vector<Matrix4>& bCs = mCharacter->mSkeleton->GetGlobalInvBindPoses();
	mCharacter->GetGlobalPoseAtTime(worldPoses, mCharacter->mSkeleton, mCharacter->currentTime);
	for (size_t i = 0; i < worldPoses.size(); i++) {
		worldPoses[i] = bCs[i] * worldPoses[i];
		mCharacter->mSkinnedObj->mSkinnedConstant.data[i] = worldPoses[i];
	}
}