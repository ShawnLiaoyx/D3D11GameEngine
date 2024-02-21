#pragma once
#include "engineMath.h"
#include "Graphics.h"

class Shader;
class Texture;
class AssetManager;

class Material
{
public:
	struct MaterialConstant 
	{
		Vector3 diffuseColor;
		float padding;
		Vector3 specularColor;
		float specularPower;
	};
	struct MaterialConstant mMaterialConstant;

	Material();
	~Material();

	static Material* StaticLoad(const WCHAR* fileName, AssetManager* pManager);

	bool Load(const WCHAR* fileName, AssetManager* pManager);

	void SetActive();
	void SetShader(const Shader* shader) { mShader = shader; }
	void SetTexture(int slot, const Texture* texture) { mTextures[slot] = texture; }
	void SetDiffuseColor(const Vector3& color) { mMaterialConstant.diffuseColor = color; }
	void SetSpecularColor(const Vector3& color) { mMaterialConstant.specularColor = color; }
	void SetSpecularPower(float power) { mMaterialConstant.specularPower = power; }
private:
	ID3D11Buffer* mMaterialBuffer;
	const Shader* mShader;
	const Texture* mTextures[Graphics::TextureSlot::TEXTURE_SLOT_TOTAL];
};

