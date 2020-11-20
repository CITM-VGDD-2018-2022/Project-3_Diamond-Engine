#include "W_Scene.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"

#include "OpenGL.h"
#include"Application.h"
#include"M_Editor.h"

#include"ImGuizmo/ImGuizmo.h"

#include"C_Transform.h"
#include"GameObject.h"
#include"ModuleCamera3D.h"

W_Scene::W_Scene(Application* _app) : Window() /*: texColorBuffer(-1)*/
{
	name = "Scene";
	App = _app;
}

W_Scene::~W_Scene()
{

}

void W_Scene::InitSceneView()
{

}

void W_Scene::Draw()
{

	//We are only rendering the part of the screen seen by scene window size
	//WARNING/TODO: This is temporal, we should render the full texture but modufy the camera fov to avoid deformations
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*//*, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse*/)) 
	{
		if (/*ImGui::IsWindowFocused() &&*/ ImGui::IsWindowHovered()) 
		{
			App->moduleCamera->ProcessSceneKeyboard();
		}

		//ImVec2 texOriginalSize = ImVec2(App->moduleWindow->s_width, App->moduleWindow->s_height);
		//ImVec2 e = ImGui::GetContentRegionAvail();

		//ImVec2 startPoint = ImVec2((texOriginalSize.x / 2) - (e.x / 2), (texOriginalSize.y / 2) + (e.y / 2));
		//ImVec2 endPoint = ImVec2((texOriginalSize.x / 2) + (e.x / 2), (texOriginalSize.y / 2) - (e.y / 2));

		//ImVec2 uv0 = ImVec2(startPoint.x / texOriginalSize.x, startPoint.y / texOriginalSize.y);
		//ImVec2 uv1 = ImVec2(endPoint.x / texOriginalSize.x, endPoint.y / texOriginalSize.y);

		//Display the scene
		//ImGui::Image((ImTextureID)App->moduleRenderer3D->texColorBuffer, e, uv0, uv1);
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImGui::Image((ImTextureID)App->moduleRenderer3D->texColorBuffer, size, ImVec2(0, 1), ImVec2(1, 0));

		//Draw gizmo
		if (EngineExternal->moduleEditor->GetSelectedGO())
		{
			ImVec2 cornerPos = ImGui::GetWindowPos();
			ImVec2 _size = ImGui::GetWindowSize();
			//The small offset is due to some error margin in the rect height
			//ERROR: ImGuizmo drawing on top of editor
			//ERROR: WE BROKE THE FOV STUFF, YAAY
			ImGuizmo::SetRect(cornerPos.x, cornerPos.y, _size.x, _size.y);

			ImGuizmo::SetDrawlist();
			//ImGui::PushClipRect(ImGui::GetWindowSize(), ImGui::GetWindowSize(), true);
			ImGuizmo::Manipulate(EngineExternal->moduleCamera->GetViewMatrix(), &EngineExternal->moduleRenderer3D->ProjectionMatrix, ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, EngineExternal->moduleEditor->GetSelectedGO()->transform->globalTransform.Transposed().ptr());
			//ImGui::PopClipRect();

			//if (ImGuizmo::IsUsing())
			//{
			//	float4x4 newMatrix;
			//	newMatrix.Set(modelPtr);
			//	modelProjection = newMatrix.Transposed();
			//	gameObject->GetComponent<C_Transform>()->SetGlobalTransform(modelProjection);
			//}
		}

	}
	ImGui::End();
	ImGui::PopStyleVar();

}
