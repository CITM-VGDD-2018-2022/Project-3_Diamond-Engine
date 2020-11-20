#pragma once
#include "Component.h"

#include"MathGeoLib/include/Geometry/Frustum.h"

class C_Camera : public Component
{
public:
	C_Camera(GameObject* _gm);
	virtual ~C_Camera();

	/*void Update() override;*/
	bool OnEditor() override;

	void Update() override;

	void SaveData(JSON_Object* nObj) override;
	void LoadData(JSON_Object* nObj) override;

	float4x4 GetOpenGLViewMatrix() const;
	float4x4 GetOpenGLProjectionMatrix() const;
	void SetAspectRatio(float aspectRatio);

	void StartDraw();
	void EndDraw();

	void ReGenerateBuffer(int w, int h);

	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int rbo;

	Frustum camFrustrum;
	float fov;

};