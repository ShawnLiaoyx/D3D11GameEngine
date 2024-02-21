#include "stdafx.h"
#include "assetManager.h"

AssetManager::AssetManager()
	:mShaderCache(this)
	,mTextureCache(this)
	,mMaterialCache(this)
	,mMeshCache(this)
	,mSkeletonCache(this)
	,mAnimationCache(this)
{
}

AssetManager::~AssetManager()
{
}

void AssetManager::Clear()
{
	mShaderCache.Clear();
	mTextureCache.Clear();
	mMaterialCache.Clear();
	mMeshCache.Clear();
	mSkeletonCache.Clear();
	mAnimationCache.Clear();
}