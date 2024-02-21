#pragma once
#include "engineMath.h"

class CollisionBox;

class Physics
{
public:
	class AABB 
	{
	public:
		AABB()
		{
		}
		AABB(Vector3 min, Vector3 max)
			:minCorner(min), maxCorner(max)
		{
		}
		Vector3 minCorner;
		Vector3 maxCorner;
	};

	class LineSegment
	{
	public:
		LineSegment()
		{
		}

		LineSegment(Vector3 start, Vector3 end)
			:startingPoint(start), endingPoint(end)
		{
		}
		Vector3 startingPoint;
		Vector3 endingPoint;
	};

	static bool Intersect(const AABB& a, const AABB& b, AABB* pOverlap = nullptr);
	static bool Intersect(const LineSegment& segment, const AABB& box,
		Vector3* pHitPoint = nullptr);
	static bool UnitTest();
	void AddObj(const CollisionBox* pObj);
	void RemoveObj(const CollisionBox* pObj);
	bool RayCast(const LineSegment& segment, Vector3*pHitPoint = nullptr);
protected:
	std::vector<const CollisionBox*> mCollisionBoxes;
};

