#pragma once
#include "assetCache.h"
#include "Shader.h"
#include "texture.h"
#include "Material.h"
#include "mesh.h"
#include "Skeleton.h"
#include "Animation.h"

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();

    void Clear();

    Shader* GetShader(const std::wstring& shaderName) { return mShaderCache.Get(shaderName); }
    void SetShader(const std::wstring& shaderName, Shader* pShader) { mShaderCache.Cache(shaderName, pShader); }

    Texture* LoadTexture(const std::wstring& fileName) { return mTextureCache.Load(fileName); }

    Material* LoadMaterial(const std::wstring& materialName) { return mMaterialCache.Load(materialName); }

    Mesh* LoadMesh(const std::wstring& fileName) { return mMeshCache.Load(fileName); }

    Skeleton* LoadSkeleton(const std::wstring& fileName) { return mSkeletonCache.Load(fileName); }

    Animation* LoadAnimation(const std::wstring& fileName) { return mAnimationCache.Load(fileName); }

private:
    AssetCache<Shader> mShaderCache;
    AssetCache<Texture> mTextureCache;
    AssetCache<Material> mMaterialCache;
    AssetCache<Mesh> mMeshCache;
    AssetCache<Skeleton> mSkeletonCache;
    AssetCache<Animation> mAnimationCache;
};
