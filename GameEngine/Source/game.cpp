#include "stdafx.h"
#include "Game.h"
#include "stringUtil.h"
#include "engineMath.h"
#include "Graphics.h"
#include "Shader.h"
#include "VertexFormat.h"
#include "RenderObj.h"
#include "VertexBuffer.h"
#include "Camera.h"
#include "texture.h"
#include "Material.h"
#include "mesh.h"
#include "Lights.h"
#include "Components/PointLight.h"
#include "Components/flyingLight.h"
#include "Components/player.h"
#include "Components/followCam.h"
#include "Components/Character.h"
#include "Components/SimpleRotate.h"
#include "Skeleton.h"
#include "Animation.h"
#include "SkinnedObj.h"
#include "CollisionBox.h"
#include "Profiler.h"
#include "GBuffer.h"
#include "jsonUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hWnd, float width, float height)
{
	mGraphics.InitD3D(hWnd, width, height);

	//Init Shaders
	D3D11_INPUT_ELEMENT_DESC phongInputElem[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormalUV, pos),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormalUV, normal),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormalUV, uv),
	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Shader* mPhongShader = new Shader();
	mPhongShader->Load(L"Shaders/Phong.hlsl", phongInputElem, ARRAYSIZE(phongInputElem));
	mAssetManager.SetShader(L"Phong", mPhongShader);

	D3D11_INPUT_ELEMENT_DESC unlitInputElem[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormalUV, pos),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormalUV, normal),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormalUV, uv),
	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Shader* mUnlitShader = new Shader();
	mUnlitShader->Load(L"Shaders/Unlit.hlsl", unlitInputElem, ARRAYSIZE(unlitInputElem));
	mAssetManager.SetShader(L"Unlit", mUnlitShader);

	D3D11_INPUT_ELEMENT_DESC skinnedInputElem[] = 
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormalBoneWeightUV, pos),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormalBoneWeightUV, normal),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BONE", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, offsetof(VertexPosNormalBoneWeightUV, bone),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WEIGHT", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(VertexPosNormalBoneWeightUV, weight),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormalBoneWeightUV, uv),
	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Shader* mSkinnedShader = new Shader();
	mSkinnedShader->Load(L"Shaders/Skinned.hlsl", skinnedInputElem, ARRAYSIZE(skinnedInputElem));
	mAssetManager.SetShader(L"Skinned", mSkinnedShader);

	D3D11_INPUT_ELEMENT_DESC normalInputElem[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormalTangentUV, pos),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormalTangentUV, normal),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormalTangentUV, tangent),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormalTangentUV, uv),
	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Shader* mNormalShader = new Shader();
	mNormalShader->Load(L"Shaders/Normal.hlsl", normalInputElem, ARRAYSIZE(normalInputElem));
	mAssetManager.SetShader(L"Normal", mNormalShader);


	D3D11_INPUT_ELEMENT_DESC ambientInputElem[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosUV, pos),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosUV, uv),
	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Shader* mAmbientShader = new Shader();
	mAmbientShader->Load(L"Shaders/Ambient.hlsl", ambientInputElem, ARRAYSIZE(ambientInputElem));
	mAssetManager.SetShader(L"Ambient", mAmbientShader);

	D3D11_INPUT_ELEMENT_DESC pointLightInputElem[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosUV, pos),
	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosUV, uv),
	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Shader* mPointLightShader = new Shader();
	mPointLightShader->Load(L"Shaders/PointLight.hlsl", pointLightInputElem, ARRAYSIZE(pointLightInputElem));
	mAssetManager.SetShader(L"PointLight", mPointLightShader);

	//Load levels
	bool result = LoadLevel(L"Assets/Levels/Final.level");

	
	//Set Up Lights
	//Currently Has Only Ambient Light
	mLightingBuffer = mGraphics.CreateGraphicsBuffer((const void*)&mLightingConstants, sizeof(mLightingConstants),
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

	//Set Offscreen
	mOffScreenDiffuse = new GBuffer(width, height, Graphics::TextureSlot::TEXTURE_SLOT_DIFFUSE);
	mOffScreenNormal = new GBuffer(width, height, Graphics::TextureSlot::TEXTURE_SLOT_NORMAL);
	mOffScreenWorldPos = new GBuffer(width, height, Graphics::TextureSlot::TEXTURE_SLOT_WORLDPOS);
	mOffScreenTargets[DIFFUSE] = mOffScreenDiffuse->GetRenderTarget();
	mOffScreenTargets[NORMAL] = mOffScreenNormal->GetRenderTarget();
	mOffScreenTargets[WORLDPOS] = mOffScreenWorldPos->GetRenderTarget();

	//Set BlendStates
	opaqueState = mGraphics.CreateBlendState(false, D3D11_BLEND_ONE, D3D11_BLEND_ZERO);
	additiveState = mGraphics.CreateBlendState(true, D3D11_BLEND_ONE, D3D11_BLEND_ONE);

	//Begin Job Managers
	mJobManager.Begin();
}

void Game::Shutdown()
{
	opaqueState->Release();
	additiveState->Release();

	delete mOffScreenDiffuse;
	delete mOffScreenNormal;
	delete mOffScreenWorldPos;

	mJobManager.End();
	mLightingBuffer->Release();
	for (auto& p : mRenderObjects) {
		delete p;
	}
	delete mCamera;
	mAssetManager.Clear();
	mGraphics.CleanD3D();
}

void Game::Update(float deltaTime)
{
	PROFILE_SCOPE(GameUpdate);
	for (auto& r : mRenderObjects) {
		r->Update(deltaTime);
	}
	mJobManager.WaitForJobs();
}

void Game::RenderFrame()
{
	PROFILE_SCOPE(GameRender);

	//Draw Into GBuffers
	mGraphics.SetBlendState(opaqueState);
	mGraphics.SetRenderTargets(OFF_SCREEN_TOTAL, mOffScreenTargets, mGraphics.GetStencilView());
	{	
		PROFILE_SCOPE(RenderClear);
		Graphics::Color4 blueClearColor(0.0f, 0.2f, 0.4f, 0.0f);
		Graphics::Color4 blackClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		mGraphics.ClearRenderTarget(blueClearColor, DIFFUSE);
		mGraphics.ClearRenderTarget(blackClearColor, NORMAL);
		mGraphics.ClearRenderTarget(blackClearColor, WORLDPOS);
		mGraphics.ClearDepthBuffer(mGraphics.GetStencilView(), 1.0f);
	}
	mCamera->SetActive();
	{
		PROFILE_SCOPE(RenderDraw);
		for (auto& p : mRenderObjects) {
			p->Draw();
		}
	}

	//Apply Lighting
	{
		PROFILE_SCOPE(RenderLighting);
		mGraphics.SetViewport(0.0f, 0.0f, mGraphics.GetScreenWidth(), mGraphics.GetScreenHeight());
		mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), nullptr);
		mOffScreenNormal->SetTextureActive();
		mOffScreenWorldPos->SetTextureActive();

		// Apply Ambient Light
		mGraphics.UploadBuffer(mLightingBuffer, (const void*)&mLightingConstants, sizeof(mLightingConstants));
		mGraphics.GetDeviceContext()->VSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_LIGHTING, 1, &mLightingBuffer);
		mGraphics.GetDeviceContext()->PSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_LIGHTING, 1, &mLightingBuffer);
		mOffScreenDiffuse->Draw(mAssetManager.GetShader(L"Ambient"));
		

		// Apply PointLight Data
		mGraphics.SetBlendState(additiveState);
		Vector3 up = mCamera->GetCameraUp();
		Vector3 right = mCamera->GetCameraRight();
		for (auto i = 0; i < Lights::MAX_POINT_LIGHTS_CAPACITY; i += Lights::MAX_POINT_LIGHTS) {
			Vector3 upRight = Vector3(FLT_MIN, FLT_MIN, 0.0f);
			Vector3 bottemLeft = Vector3(FLT_MAX, FLT_MAX, 0.0f);
			bool hasLight = false;
			ZeroMemory(mLightingConstants.mData, sizeof(mLightingConstants.mData));
			for (auto j = 0; j < Lights::MAX_POINT_LIGHTS; j++) {
				if (i + j >= Lights::MAX_POINT_LIGHTS_CAPACITY || !mPointLightDatas[i + j].isEnabled) {
					continue;
				}
				mLightingConstants.mData[j] = mPointLightDatas[i + j];
				upRight = Vector3::Max(mCamera->WorldToScreenSpace(mLightingConstants.mData[j].position + up * mLightingConstants.mData[j].outerRadius
					+ right * mLightingConstants.mData[j].outerRadius), upRight);
				bottemLeft = Vector3::Min(mCamera->WorldToScreenSpace(mLightingConstants.mData[j].position - up * mLightingConstants.mData[j].outerRadius
					- right * mLightingConstants.mData[j].outerRadius), bottemLeft);
				hasLight = true;
			}
			if (!hasLight) {
				continue;
			}
			mGraphics.UploadBuffer(mLightingBuffer, (const void*)&mLightingConstants, sizeof(mLightingConstants));
			mGraphics.GetDeviceContext()->VSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_LIGHTING, 1, &mLightingBuffer);
			mGraphics.GetDeviceContext()->PSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_LIGHTING, 1, &mLightingBuffer);

			mOffScreenDiffuse->DrawInRect(mAssetManager.GetShader(L"PointLight"), upRight, bottemLeft);
		}
		mOffScreenNormal->DisableTexture();
		mOffScreenWorldPos->DisableTexture();
	}

	{
		PROFILE_SCOPE(RenderEnd);
		mGraphics.EndFrame();
	}
}

void Game::OnKeyDown(uint32_t key)
{
	m_keyIsHeld[key] = true;
}

void Game::OnKeyUp(uint32_t key)
{
	m_keyIsHeld[key] = false;
}

bool Game::IsKeyHeld(uint32_t key) const
{
	const auto find = m_keyIsHeld.find(key);
	if (find != m_keyIsHeld.end())
		return find->second;
	return false;
}

bool Game::LoadLevel(const WCHAR* fileName)
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
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "level" ||
		ver != 2)
	{
		return false;
	}

	//Read the Camera
	if (!doc["camera"].IsObject()) {
		return false;
	}
	Vector3 cameraPos;
	Quaternion cameraRot;
	GetVectorFromJSON(doc["camera"], "position", cameraPos);
	GetQuaternionFromJSON(doc["camera"], "rotation", cameraRot);
	mCamera = new Camera(cameraPos, cameraRot);

	//Read Lighting info
	if (!doc["lightingData"].IsObject()) {
		return false;
	}
	Vector3 ambientL;
	GetVectorFromJSON(doc["lightingData"], "ambient", ambientL);
	SetAmbientLight(ambientL);

	//Read renderObjects
	const rapidjson::Value& in_renderObjects = doc["renderObjects"];
	if (!in_renderObjects.IsArray())
	{
		return false;
	}
	std::wstring meshName;
	float scale;
	Quaternion rotation;
	Vector3 translation;
	std::string typeName;
	for (rapidjson::SizeType i = 0; i < in_renderObjects.Size(); i++) {
		if (!in_renderObjects[i].IsObject()) {
			return false;
		}
		GetWStringFromJSON(in_renderObjects[i], "mesh", meshName);
		Mesh* newMesh = mAssetManager.LoadMesh(meshName);
		RenderObj* obj = newMesh->IsSkinned() ? new SkinnedObj(newMesh) : new RenderObj(newMesh);
		GetFloatFromJSON(in_renderObjects[i], "scale", scale);
		GetQuaternionFromJSON(in_renderObjects[i], "rotation", rotation);
		GetVectorFromJSON(in_renderObjects[i], "position", translation);
		obj->mConstant.data = Matrix4::CreateScale(scale) * Matrix4::CreateFromQuaternion(rotation) * Matrix4::CreateTranslation(translation);
		mRenderObjects.push_back(obj);

		
		const rapidjson::Value& components = in_renderObjects[i]["components"];
		if (!components.IsArray()) {
			return false;
		}
		for (rapidjson::SizeType j = 0; j < components.Size(); j++) {
			if (!components[j].IsObject()) {
				return false;
			}
			typeName = components[j]["type"].GetString();
			if (typeName == "PointLight") {
				PointLight* light = new PointLight(obj, this);
				light->LoadProperties(components[j], &mAssetManager);
			}
			else if (typeName == "Character" && newMesh->IsSkinned()) {
				Character* character = new Character(reinterpret_cast<SkinnedObj*>(obj), this);
				character->LoadProperties(components[j], &mAssetManager);
			}
			else if (typeName == "Player") {
				Player* player = new Player(reinterpret_cast<SkinnedObj*>(obj), this);
				player->LoadProperties(components[j], &mAssetManager);
			}
			else if (typeName == "FollowCam") {
				FollowCam* followCam = new FollowCam(obj, this);
				followCam->LoadProperties(components[j], &mAssetManager);
			}
			else if (typeName == "CollisionBox") {
				CollisionBox* collisionBox = new CollisionBox(obj, &mPhysics);
				collisionBox->LoadProperties(components[j], &mAssetManager);
			}
			else if (typeName == "SimpleRotate") {
				SimpleRotate* simpleRotate = new SimpleRotate(obj);
				simpleRotate->LoadProperties(components[j], &mAssetManager);
			}
			else if (typeName == "FlyingLight") {
				FlyingLight* flyingLight = new FlyingLight(obj, this);
				flyingLight->LoadProperties(components[j], &mAssetManager);
			}
		}
	}
	return true;
}

Lights::PointLightData* Game::AllocateLight()
{
	for (int i = 0; i < Lights::MAX_POINT_LIGHTS_CAPACITY; i++) {
		if (!mPointLightDatas[i].isEnabled) {
			mPointLightDatas[i].isEnabled = true;
			return &mPointLightDatas[i];
		}
	}
	return nullptr;
}

void Game::FreeLight(Lights::PointLightData* pLight)
{
	for (int i = 0; i < Lights::MAX_POINT_LIGHTS_CAPACITY; i++) {
		if (&mPointLightDatas[i] == pLight) {
			mPointLightDatas[i].isEnabled = false;
		}
	}
}