#pragma once
#include "engineMath.h"
#include "mesh.h"
class Component;

class RenderObj {
public:
	struct PerObjectConstant 
	{
		Matrix4 data;
	};
	struct PerObjectConstant mConstant;

	RenderObj(const Mesh* mesh);
	virtual ~RenderObj();
	virtual void Draw();
	virtual void Update(float deltaTime);
	void AddComponent(Component* pComp);
protected:
	const Mesh* mMesh;
	ID3D11Buffer* mObjectBuffer;
	std::vector<Component*> mComponents;
};

