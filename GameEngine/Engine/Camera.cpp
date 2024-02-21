#include "stdafx.h"
#include "Camera.h"
#include "Graphics.h"

Camera::Camera()
	:mWorldToCameraMatrix(Matrix4::CreateTranslation(Vector3(500.0f, 0.0f, 0.0f)))
{
	Init();
}

Camera::Camera(Vector3 position, Quaternion rotation)
{
	position *= -1.0f;
	rotation.Conjugate();
	mWorldToCameraMatrix = Matrix4::CreateTranslation(position) * Matrix4::CreateFromQuaternion(rotation);
	Init();
}


Camera::~Camera()
{
	mCameraBuffer->Release();
}

void Camera::Init()
{
	Graphics* pGraphics = Graphics::Get();
	mProjectionMatrix = Matrix4::CreateRotationY(-Math::PiOver2)
		* Matrix4::CreateRotationZ(-Math::PiOver2)
		* Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
			pGraphics->GetScreenWidth(), pGraphics->GetScreenHeight(),
			25.0f, 10000.0f);
	mCameraBuffer = pGraphics->CreateGraphicsBuffer((const void*)&mConstant, sizeof(mConstant), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

void Camera::SetActive()
{
	mConstant.viewProj = mWorldToCameraMatrix * mProjectionMatrix;
	Vector3 xAxis = mWorldToCameraMatrix.GetXAxis();
	Vector3 yAxis = mWorldToCameraMatrix.GetYAxis();
	Vector3 zAxis = mWorldToCameraMatrix.GetZAxis();
	Vector3 translation = mWorldToCameraMatrix.GetTranslation();
	xAxis /= xAxis.Length();
	yAxis /= yAxis.Length();
	zAxis /= zAxis.Length();
	translation /= -1;
	mConstant.cameraPosition = Vector3(Dot(xAxis, translation), Dot(yAxis, translation), Dot(zAxis, translation));

	Graphics* pGraphics = Graphics::Get();
	pGraphics->UploadBuffer(mCameraBuffer, (const void*)&mConstant, sizeof(mConstant));
	pGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_CAMERA, 1, &mCameraBuffer);
	pGraphics->GetDeviceContext()->PSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_CAMERA, 1, &mCameraBuffer);
}

Vector3 Camera::GetCameraUp()
{
	Vector3 xAxis = mWorldToCameraMatrix.GetXAxis();
	Vector3 yAxis = mWorldToCameraMatrix.GetYAxis();
	Vector3 zAxis = mWorldToCameraMatrix.GetZAxis();
	xAxis /= xAxis.Length();
	yAxis /= yAxis.Length();
	zAxis /= zAxis.Length();
	return Vector3(xAxis.z, yAxis.z, zAxis.z);
}

Vector3 Camera::GetCameraRight()
{
	Vector3 xAxis = mWorldToCameraMatrix.GetXAxis();
	Vector3 yAxis = mWorldToCameraMatrix.GetYAxis();
	Vector3 zAxis = mWorldToCameraMatrix.GetZAxis();
	xAxis /= xAxis.Length();
	yAxis /= yAxis.Length();
	zAxis /= zAxis.Length();
	return Vector3(xAxis.y, yAxis.y, zAxis.y);
}

Vector3 Camera::WorldToScreenSpace(const Vector3& input)
{
	Vector4 screenPos = Transform(Vector4(input.x, input.y, input.z, 1.0f), mConstant.viewProj);
	return Vector3(screenPos.x / screenPos.w, screenPos.y / screenPos.w, 0.0f);
}