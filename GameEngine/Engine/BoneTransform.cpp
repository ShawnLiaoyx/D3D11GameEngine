#include "stdafx.h"
#include "BoneTransform.h"

Matrix4 BoneTransform::ToMatrix() const
{
	return Matrix4::CreateFromQuaternion(mRotation) * Matrix4::CreateTranslation(mTranslation);
}

BoneTransform BoneTransform::Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	return { Slerp(a.mRotation, b.mRotation, f), Lerp(a.mTranslation, b.mTranslation, f) };
}