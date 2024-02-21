#pragma once
#include "Graphics.h"
#include "Physics.h"
#include "Lights.h"
#include "assetManager.h"
#include "JobManager.h"

class RenderObj;
class Shader;
class Camera;
class Texture;
class Material;
class Mesh;
class GBuffer;

class Game
{
public:
    Game();
    ~Game();

    void Init(HWND hWnd, float width, float height);
    void Shutdown();
	void Update(float deltaTime);
    void RenderFrame();

	void OnKeyDown(uint32_t key);
	void OnKeyUp(uint32_t key);
	bool IsKeyHeld(uint32_t key) const;

	Lights::PointLightData* AllocateLight();
	void FreeLight(Lights::PointLightData* pLight);
	void SetAmbientLight(const Vector3& color) { mLightingConstants.ambient = color; }
	const Vector3& GetAmbientLight() const { return mLightingConstants.ambient; }
	Camera* GetCamera() { return mCamera; }
	Physics* GetPhysics() { return &mPhysics; }
	JobManager* GetJobmanager() { return &mJobManager; }
private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;
	Graphics mGraphics;
	AssetManager mAssetManager;
	Physics mPhysics;
	JobManager mJobManager;

	std::vector<RenderObj*> mRenderObjects;
	
	Lights::LightingConstants mLightingConstants;
	Lights::PointLightData mPointLightDatas[Lights::MAX_POINT_LIGHTS_CAPACITY];
	ID3D11Buffer* mLightingBuffer = nullptr;

	Camera* mCamera = nullptr;
	bool LoadLevel(const WCHAR* fileName);

	enum OffScreenIndex {
		DIFFUSE,
		NORMAL,
		WORLDPOS,
		OFF_SCREEN_TOTAL
	};
	ID3D11RenderTargetView* mOffScreenTargets[OFF_SCREEN_TOTAL] = { nullptr };
	GBuffer* mOffScreenDiffuse = nullptr;
	GBuffer* mOffScreenNormal = nullptr;
	GBuffer* mOffScreenWorldPos = nullptr;

	ID3D11BlendState* opaqueState = nullptr;
	ID3D11BlendState* additiveState = nullptr;
};