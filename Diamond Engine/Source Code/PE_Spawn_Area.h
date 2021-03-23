#pragma once

#include "PE__Spawn_Shape_Base.h"

class PE_SpawnShapeArea : public PE_SpawnShapeBase
{
public:
	PE_SpawnShapeArea();
	~PE_SpawnShapeArea() override;

	void Spawn(Particle& particle, bool hasInitialSpeed,float speed) override; //Spawns in area

#ifndef STANDALONE
	void OnEditor(int emitterIndex) override;
#endif // !STANDALONE

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;
private:
	//Quad with a point and a radius the point is an offset from the transform
	float centerOfQuad[3]; //(x,y,z)
	float radius;
};