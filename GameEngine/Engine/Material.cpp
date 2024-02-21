#include "stdafx.h"
#include "Material.h"
#include "Shader.h"
#include "texture.h"
#include "assetManager.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include "jsonUtil.h"
#include "stringUtil.h"
#include <fstream>
#include <sstream>

Material::Material()
	:mShader(nullptr)
	,mTextures()
{
	Graphics* pGraphics = Graphics::Get();
	mMaterialBuffer = pGraphics->CreateGraphicsBuffer((const void*)&mMaterialConstant, sizeof(mMaterialConstant), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

Material::~Material()
{
	mMaterialBuffer->Release();
}

void Material::SetActive()
{
	mShader->SetActive();
	for (int i = 0; i < Graphics::TextureSlot::TEXTURE_SLOT_TOTAL; i++) {
		if (mTextures[i] == nullptr) {
			Graphics::Get()->SetActiveTexture(i, NULL);
		}
		else {
			mTextures[i]->SetActive(i);
		}
	}

	Graphics* pGraphics = Graphics::Get();
	pGraphics->UploadBuffer(mMaterialBuffer, (const void*)&mMaterialConstant, sizeof(mMaterialConstant));
	pGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_MATERIAL, 1, &mMaterialBuffer);
	pGraphics->GetDeviceContext()->PSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_MATERIAL, 1, &mMaterialBuffer);
}

bool Material::Load(const WCHAR* fileName, AssetManager* pManager)
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
		DbgAssert(false, "Unable to open Material file");
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "mat" ||
		ver != 1)
	{
		DbgAssert(false, "Material File Incorrect Version");
		return false;
	}

	//Set Shader
	std::wstring shaderName;
	GetWStringFromJSON(doc, "shader", shaderName);
	mShader = pManager->GetShader(shaderName);
	if (nullptr == mShader) {
		DbgAssert(false, "Unable to find shader");
		return false;
	}
	
	//Read TextureNames
	const rapidjson::Value& textureNames = doc["textures"];
	if (!textureNames.IsArray() || textureNames.Size() < 1) {
		DbgAssert(false, "Material File Invalid Texture Names");
		return false;
	}

	std::wstring tName;
	for (rapidjson::SizeType i = 0; i < textureNames.Size(); i++) {
		if (!textureNames[i].IsString()) {
			DbgAssert(false, "Invalid Texture name");
			return false;
		}
		if (i >= Graphics::TextureSlot::TEXTURE_SLOT_TOTAL) break;

		StringUtil::String2WString(tName, textureNames[i].GetString());
		mTextures[i] = pManager->LoadTexture(tName);
		if (nullptr == mTextures[i]) {
			DbgAssert(false, "Unable to Find Texture");
			return false;
		}
	}

	//Read Material Constants
	GetVectorFromJSON(doc, "diffuseColor", mMaterialConstant.diffuseColor);
	GetVectorFromJSON(doc, "specularColor", mMaterialConstant.specularColor);
	GetFloatFromJSON(doc, "specularPower", mMaterialConstant.specularPower);
	return true;
}

Material* Material::StaticLoad(const WCHAR* fileName, AssetManager* pManager)
{
	Material* pMaterial = new Material();
	if (false == pMaterial->Load(fileName, pManager)) {
		delete pMaterial;
		return nullptr;
	}
	return pMaterial;
}