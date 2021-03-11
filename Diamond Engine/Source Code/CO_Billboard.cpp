#include "CO_Billboard.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

C_Billboard::C_Billboard(GameObject* _gm):Component(_gm), currentAlignment(BILLBOARD_ALIGNMENT::SCREEN_ALIGNED)
{
	name = "Billboard";
}

C_Billboard::~C_Billboard()
{
}

#ifndef STANDALONE
bool C_Billboard::OnEditor()
{
	if(Component::OnEditor() == false)
		return false;

	ImGui::Separator();

	std::string tempAlignment = "test";
	switch (currentAlignment) {
	case BILLBOARD_ALIGNMENT::AXIS_ALIGNED: tempAlignment = "Axis Aligned"; break;
	case BILLBOARD_ALIGNMENT::SCREEN_ALIGNED: tempAlignment = "Screen Aligned"; break;
	case BILLBOARD_ALIGNMENT::WORLD_ALIGNED: tempAlignment = "World Aligned"; break;
	}
	ImGui::Text("Current Billboard: %s", tempAlignment.c_str());

	if (ImGui::BeginMenu("Change billboard")) {
		if (ImGui::MenuItem("Screen Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::SCREEN_ALIGNED);
		if (ImGui::MenuItem("World Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::WORLD_ALIGNED);
		if (ImGui::MenuItem("Axis Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::AXIS_ALIGNED);

		ImGui::EndMenu();
	}

	return true;
}
#endif // !STANDALONE

void C_Billboard::SetAlignment(BILLBOARD_ALIGNMENT new_alignment)
{
	currentAlignment = new_alignment;
}

void C_Billboard::Draw()
{
	UseAlignment();

	//if the gameObject has particle system
	if (gameObject->GetComponent(Component::TYPE::PARTICLE_SYSTEM) != nullptr) {
		//TODO: Check if the particle system has material


	}
}

void C_Billboard::UseAlignment()
{
	switch (currentAlignment)
	{
	case BILLBOARD_ALIGNMENT::SCREEN_ALIGNED:
		ScreenAlign();
		break;
	case BILLBOARD_ALIGNMENT::WORLD_ALIGNED:
		WorldAlign();
		break;
	case BILLBOARD_ALIGNMENT::AXIS_ALIGNED:
		AxisAlign();
		break;
	default:
		break;
	}
}

void C_Billboard::ScreenAlign()
{
	//Option 1: Extract the rotation from the view matrix

}

void C_Billboard::WorldAlign()
{
	//The way is to generate a rotation matrix that +z looks at the camera
}

void C_Billboard::AxisAlign()
{
	//Use the global y-axis
}
