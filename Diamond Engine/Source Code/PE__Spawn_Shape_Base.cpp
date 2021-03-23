#include "PE__Spawn_Shape_Base.h"

PE_SpawnShapeBase::PE_SpawnShapeBase(PE_SPAWN_SHAPE_TYPE type):myType(type)
{
}

PE_SpawnShapeBase::~PE_SpawnShapeBase()
{
}

void PE_SpawnShapeBase::Spawn(Particle& particle, bool hasInitialSpeed, float speed)
{
}

void PE_SpawnShapeBase::OnEditor(int emitterIndex)
{
}

void PE_SpawnShapeBase::SaveData(JSON_Object* nObj)
{
}

void PE_SpawnShapeBase::LoadData(DEConfig& nObj)
{
}

PE_SPAWN_SHAPE_TYPE PE_SpawnShapeBase::GetType() const
{
	return myType;
}
