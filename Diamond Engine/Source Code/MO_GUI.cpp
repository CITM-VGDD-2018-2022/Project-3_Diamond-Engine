#include "MO_GUI.h"

#include "Application.h"
#include "MO_Scene.h"
#include "MO_Window.h"
#include "MO_Renderer3D.h"

#include "GameObject.h"
#include "CO_Transform2D.h"
#include "CO_Transform.h"
#include "CO_Material.h"
#include "CO_Camera.h"
#include "CO_Button.h"

#include "RE_Material.h"
#include "RE_Shader.h"

#include <stack>

#include "OpenGL.h"

M_Gui::M_Gui(Application* app, bool startEnabled) : Module(app, startEnabled),
	canvas(-1),
	VAO(0),
	index_font(-1)
{
}


M_Gui::~M_Gui()
{
	glDeleteBuffers(1, &VAO);
	VAO = 0;

	canvas = -1;
}


bool M_Gui::Start()
{
	glGenBuffers(1, &VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, uiVAO, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}


void M_Gui::RenderCanvas2D()
{
	GameObject* canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);

	if (canvasGO != nullptr)
	{
		std::stack<GameObject*> stack;
		GameObject* node = nullptr;

		int elementsCount = canvasGO->children.size();
		for (int i = 0; i < elementsCount; ++i)
		{
			stack.push(canvasGO->children[i]);

			while (stack.empty() == false)
			{
				node = stack.top();
				stack.pop();

				RenderUiElement(node);

				int childNumber = node->children.size();
				for (int i = 0; i < childNumber; ++i)
					stack.push(node->children[i]);
			}
		}
	}
}


void M_Gui::RenderCanvas3D()
{

}


void M_Gui::RenderUiElement(GameObject* uiElement)
{
	Component* mat = uiElement->GetComponent(Component::TYPE::MATERIAL);
	Component* trans2D = uiElement->GetComponent(Component::TYPE::TRANSFORM_2D);

	if (mat != nullptr && trans2D != nullptr)
	{
		C_Transform2D* transform = static_cast<C_Transform2D*>(trans2D);
		ResourceMaterial* material = static_cast<C_Material*>(mat)->material;

		if (material->shader)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			material->shader->Bind();
			material->PushUniforms();

			//TOD: Change this with the C_Image resource id
			glBindTexture(GL_TEXTURE_2D, App->moduleRenderer3D->checkersTexture);



			GLint modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "model_matrix");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform->GetGlobal2DTransform().ptr());
			glBindBuffer(GL_ARRAY_BUFFER, VAO);
			glVertexPointer(2, GL_FLOAT, 0, NULL);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);

			if (material->shader)
				material->shader->Unbind();

			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
}


void M_Gui::CreateCanvas()
{
	GameObject* canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	if (canvasGO == nullptr)
	{
		canvasGO = new GameObject("Canvas", App->moduleScene->root);
		canvasGO->AddComponent(Component::TYPE::CANVAS);
	}
}


void M_Gui::CreateImage()
{
	GameObject* canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	if (canvasGO == nullptr)
	{
		CreateCanvas();
		canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	}

	GameObject* image = new GameObject("Image", canvasGO);
	image->AddComponent(Component::TYPE::TRANSFORM_2D);
	image->AddComponent(Component::TYPE::MATERIAL);

}

void M_Gui::CreateButton()
{
	GameObject* canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	if (canvasGO == nullptr)
	{
		CreateCanvas();
		canvasGO = App->moduleScene->GetGOFromUID(App->moduleScene->root, canvas);
	}

	GameObject* button = new GameObject("Button", canvas);
	button->AddComponent(Component::TYPE::TRANSFORM_2D);
	button->AddComponent(Component::TYPE::MATERIAL);
	button->AddComponent(Component::TYPE::BUTTON);
}

void M_Gui::CreateText()
{
	if (canvas == nullptr)	//TODO Create a GO with a component canvas
		CreateCanvas();

	GameObject* text = new GameObject("Text", canvas);
	text->AddComponent(Component::TYPE::TRANSFORM_2D);
	text->AddComponent(Component::TYPE::MATERIAL);
	text->AddComponent(Component::TYPE::TEXT_UI);

}


void M_Gui::SetCanvas(int uid)
{
	canvas = uid;
}


void M_Gui::EraseCanvas()
{
	canvas = -1;
}