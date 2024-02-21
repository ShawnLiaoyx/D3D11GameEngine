#pragma once
#include "engineMath.h"

namespace Lights
{
	constexpr auto MAX_POINT_LIGHTS_CAPACITY = 128;
	constexpr auto MAX_POINT_LIGHTS = 8;
	struct PointLightData
	{
		Vector3 lightColor;
		float paddingOne = 0.0f;
		Vector3 position;
		float innerRadius = 0.0f;

		float outerRadius = 0.0f;
		bool isEnabled = false;
		float paddingTwo = 0.0f;
		float paddingThree = 0.0f;
	};

	struct LightingConstants
	{
		Vector3 ambient;
		float padding = 0.0f;
		PointLightData mData[MAX_POINT_LIGHTS];
	};
}