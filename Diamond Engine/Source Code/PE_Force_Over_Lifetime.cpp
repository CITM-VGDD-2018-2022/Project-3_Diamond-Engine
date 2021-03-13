#include "PE_Force_Over_Lifetime.h"
#include "Particle.h"

#include "ImGui/imgui.h"

PE_ForceOverLifetime::PE_ForceOverLifetime() : ParticleEffect(PARTICLE_EFFECT_TYPE::FORCE_OVER_LIFETIME)
{

	memset(acceleration, 0.f, sizeof(acceleration));

	myGravity=-9.8f;
	gravityModifier = 1.0f;

}


PE_ForceOverLifetime::~PE_ForceOverLifetime()
{
}


void PE_ForceOverLifetime::Update(Particle& particle, float dt)
{
	particle.accel.x = acceleration[0];
	particle.accel.y = acceleration[1]+ myGravity;
	particle.accel.z = acceleration[2];
}


#ifndef STANDALONE
void PE_ForceOverLifetime::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Force Over Lifetime Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{


	/*	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Particle movement: ");

		int offset = ImGui::CalcTextSize("Particle speed: ").x + 16;
		ImGui::Text("Particle speed: ");
		ImGui::SameLine();
		suffixLabel = "##lPaSpdMoveEffect";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), speed, 0.1f);

		ImGui::Text("Part acceleration: ");
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		suffixLabel = "##lPaAccMoveEffect";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), acceleration, 0.1f);*/

		suffixLabel = "Force Vector##DirectionalForce";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), acceleration, 0.1f); 
		
		suffixLabel = "Gravity Modifier##GravityForce";
		suffixLabel += emitterIndex;
		if (ImGui::DragFloat(suffixLabel.c_str(), &gravityModifier, 0.1f))
		{
			myGravity = -9.8f * gravityModifier;
		}

	}
}
#endif // !STANDALONE