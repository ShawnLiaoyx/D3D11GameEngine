#pragma once
#include "engineMath.h"

class Camera
{
public:
	struct PerCameraConstant 
	{
		Matrix4 viewProj;
		Vector3 cameraPosition;
		float padding;
	};
	struct PerCameraConstant mConstant;

	Camera();
	Camera(Vector3 position, Quaternion rotation);
	virtual ~Camera();
	Vector3 GetCameraUp();
	Vector3 GetCameraRight();
	Vector3 WorldToScreenSpace(const Vector3& input);
	void SetWorldToCamera(const Matrix4& mat) { mWorldToCameraMatrix = mat; }
	void SetActive();
protected:
	void Init();
	ID3D11Buffer* mCameraBuffer;
	Matrix4 mWorldToCameraMatrix;
	Matrix4 mProjectionMatrix;
};

