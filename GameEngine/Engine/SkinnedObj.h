#pragma once
#include "RenderObj.h"

#define MAX_BONES 80

class SkinnedObj : public RenderObj
{
public:
	struct SkinnedConstant {
		Matrix4 data[MAX_BONES];
	};
	struct SkinnedConstant mSkinnedConstant;

	SkinnedObj(const Mesh* mesh);
	~SkinnedObj();
	void Draw() override;
protected:
	ID3D11Buffer* mSkinnedConstantBuffer;
};

