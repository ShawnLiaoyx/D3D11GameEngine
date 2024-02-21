#include "stdafx.h"
#include "Physics.h"
#include "CollisionBox.h"
using namespace Math;

bool Physics::Intersect(const AABB& a, const AABB& b, AABB* pOverlap)
{
	// does not intersect if any component of the min > max or max < min
	if (a.minCorner.x > b.maxCorner.x || a.maxCorner.x < b.minCorner.x ||
		a.minCorner.y > b.maxCorner.y || a.maxCorner.y < b.minCorner.y ||
		a.minCorner.z > b.maxCorner.z || a.maxCorner.z < b.minCorner.z)
	{
		return false;
	}

	// get the overlap by finding the min of maxs, and the max of mins
	pOverlap->minCorner = Vector3(Max(a.minCorner.x, b.minCorner.x),
		Max(a.minCorner.y, b.minCorner.y), Max(a.minCorner.z, b.minCorner.z));
	pOverlap->maxCorner = Vector3(Min(a.maxCorner.x, b.maxCorner.x),
		Min(a.maxCorner.y, b.maxCorner.y), Min(a.maxCorner.z, b.maxCorner.z));
	return true;
}

bool Physics::Intersect(const LineSegment& segment, const AABB& box, Vector3* pHitPoint)
{
	//get the segment as a vector
	Vector3 d = segment.endingPoint - segment.startingPoint;

	//cast Vector3 into float ptrs
	const float* pD = reinterpret_cast<const float*>(&d);
	const float* pStart = reinterpret_cast<const float*>(&segment.startingPoint);
	const float* pMin = reinterpret_cast<const float*>(&(box.minCorner));
	const float* pMax = reinterpret_cast<const float*>(&(box.maxCorner));

	//only solution between 0 and d.Length() intersects within the segment
	float tmin = 0.0f;
	float tmax = d.Length();
	d.Normalize();
	for (int i = 0; i < 3; i++) {
		//test if d is parallel to any axis
		if (IsCloseEnuf(fabsf(pD[i]), 0.0f)) {
			//no intesection if origin is not within the slab
			if (pStart[i] < pMin[i] || pStart[i] > pMax[i]) return false;
		}
		else
		{
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / pD[i];
			float t1 = (pMin[i] - pStart[i]) * ood;
			float t2 = (pMax[i] - pStart[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) {
				float temp = t2;
				t2 = t1;
				t1 = temp;
			}
			// Compute the intersection of slab intersection intervals
			tmin = Max(tmin, t1);
			tmax = Min(tmax, t2);
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return false;
		}
	}

	//set pHitPoint by the solution (tmin) and the starting point.
	*pHitPoint = segment.startingPoint + d * tmin;
	return true;
}

bool Physics::UnitTest()
{
	bool isOK = true;
	// Test AABB Intersect
	struct TestAABB
	{
		AABB a;
		AABB b;
		AABB overlap;
	};
	const TestAABB testAABB[] =
	{
	{
	AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f)),
	AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f))
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(-110.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f)),
	AABB(Vector3(-100.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f))
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(110.0f, 10.0f, 10.0f)),
	AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(100.0f, 10.0f, 10.0f))
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(-10.0f, -110.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f)),
	AABB(Vector3(-10.0f, -100.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f))
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 110.0f, 10.0f)),
	AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 100.0f, 10.0f))
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(-10.0f, -10.0f, -110.0f), Vector3(10.0f, 10.0f, -90.0f)),
	AABB(Vector3(-10.0f, -10.0f, -100.0f), Vector3(10.0f, 10.0f, -90.0f))
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 110.0f)),
	AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 100.0f))
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(-120.0f, -10.0f, -10.0f), Vector3(-110.0f, 10.0f, 10.0f)),
	AABB(Vector3::One, Vector3::Zero)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(110.0f, -10.0f, -10.0f), Vector3(120.0f, 10.0f, 10.0f)),
	AABB(Vector3::One, Vector3::Zero)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(-10.0f, -120.0f, -10.0f), Vector3(10.0f, -110.0f, 10.0f)),
	AABB(Vector3::One, Vector3::Zero)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(-10.0f, 110.0f, -10.0f), Vector3(10.0f, 120.0f, 10.0f)),
	AABB(Vector3::One, Vector3::Zero)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(-10.0f, -10.0f, -120.0f), Vector3(10.0f, 10.0f, -110.0f)),
	AABB(Vector3::One, Vector3::Zero)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	AABB(Vector3(-10.0f, -10.0f, 110.0f), Vector3(10.0f, 10.0f, 120.0f)),
	AABB(Vector3::One, Vector3::Zero)
	},
	};

	AABB overlapTest;
	for (auto& t : testAABB) {
		if (Vector3::IsCloseEnuf(t.overlap.minCorner, Vector3::One) && Vector3::IsCloseEnuf(t.overlap.maxCorner, Vector3::Zero)) {
			isOK &= !Intersect(t.a, t.b);
		}
		else
		{
			isOK &= Intersect(t.a, t.b, &overlapTest);
			isOK &= Vector3::IsCloseEnuf(t.overlap.minCorner, overlapTest.minCorner) && Vector3::IsCloseEnuf(t.overlap.maxCorner, overlapTest.maxCorner);
		}
	}

	//Test LineSegment Intersect
	struct TestSegment
	{
		AABB box;
		LineSegment segment;
		bool hit;
		Vector3 point;
	};
	const TestSegment testSegment[] =
	{
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(-110.0f, 0.0f, 0.0f), Vector3(-90.0f, 0.0f, 0.0f)),
	true, Vector3(-100.0f, 0.0f, 0.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(0.0f, -110.0f, 0.0f), Vector3(0.0f, -90.0f, 0.0f)),
	true, Vector3(0.0f, -100.0f, 0.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(0.0f, 0.0f, -110.0f), Vector3(0.0f, 0.0f, -90.0f)),
	true, Vector3(0.0f, 0.0f, -100.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(110.0f, 0.0f, 0.0f), Vector3(90.0f, 0.0f, 0.0f)),
	true, Vector3(100.0f, 0.0f, 0.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(0.0f, 110.0f, 0.0f), Vector3(0.0f, 90.0f, 0.0f)),
	true, Vector3(0.0f, 100.0f, 0.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(0.0f, 0.0f, 110.0f), Vector3(0.0f, 0.0f, 90.0f)),
	true, Vector3(0.0f, 0.0f, 100.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(-120.0f, 0.0f, 0.0f), Vector3(-110.0f, 0.0f, 0.0f)),
	false, Vector3::Zero
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(0.0f, -120.0f, 0.0f), Vector3(0.0f, -110.0f, 0.0f)),
	false, Vector3::Zero
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(0.0f, 0.0f, -120.0f), Vector3(0.0f, 0.0f, -110.0f)),
	false, Vector3::Zero
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(120.0f, 0.0f, 0.0f), Vector3(110.0f, 0.0f, 0.0f)),
	false, Vector3::Zero
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(0.0f, 120.0f, 0.0f), Vector3(0.0f, 110.0f, 0.0f)),
	false, Vector3::Zero
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	LineSegment(Vector3(0.0f, 0.0f, 120.0f), Vector3(0.0f, 0.0f, 110.0f)),
	false, Vector3::Zero
	},
	};

	Vector3 hitPointTest;
	for (auto& t : testSegment) {
		if (t.hit) {
			isOK &= Intersect(t.segment, t.box, &hitPointTest);
			isOK &= Vector3::IsCloseEnuf(hitPointTest, t.point);
		}
		else
		{
			isOK &= !Intersect(t.segment, t.box);
		}
	}
	return isOK;
}

void Physics::AddObj(const CollisionBox* pObj)
{
	mCollisionBoxes.emplace_back(pObj);
}

void Physics::RemoveObj(const CollisionBox* pObj)
{
	auto itr = std::find(mCollisionBoxes.begin(), mCollisionBoxes.end(), pObj);
	if (itr != mCollisionBoxes.end()) {
		mCollisionBoxes.erase(itr);
	}
}

bool Physics::RayCast(const LineSegment& segment, Vector3* pHitPoint)
{
	Vector3 hitPoint;
	//init the closest point to the length of the ray
	float minDis = (segment.endingPoint - segment.startingPoint).Length();
	float dis;
	bool hit = false;
	for (auto& p : mCollisionBoxes) {
		const AABB aabb = p->GetAABB();
		if (Intersect(segment, aabb, &hitPoint)) {
			//if intersects, updated the pHitPoint and minDis
			dis = (hitPoint - segment.startingPoint).Length();
			if (dis < minDis) {
				minDis = dis;
				*pHitPoint = hitPoint;
			}
			hit = true;
		}
	}
	return hit;
}
