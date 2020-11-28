#include"W_Assets.h"

#include"IM_FileSystem.h"
#include"MO_ResourceManager.h"
#include"MO_Input.h"

W_Assets::W_Assets() : Window(), selectedFile(nullptr)
{
	name = "Assets";
}

W_Assets::~W_Assets()
{
}

void W_Assets::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/))
	{
		DrawFileTree(EngineExternal->moduleResources->assetsRoot);
		DrawFileTree(EngineExternal->moduleResources->meshesLibraryRoot);

		if (selectedFile != nullptr && ImGui::IsWindowHovered() && EngineExternal->moduleInput->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) {
			selectedFile->DeletePermanent();
			selectedFile = nullptr;
			EngineExternal->moduleResources->NeedsDirsUpdate(EngineExternal->moduleResources->assetsRoot);
		}
	}


	ImGui::End();
}

void W_Assets::DrawFileTree(AssetDir& file)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	if (!file.isDir || file.childDirs.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	if (selectedFile == &file) 
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	//if (node == EngineExternal->moduleEditor->GetSelectedGO())
	//	flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;
	bool nodeOpen = ImGui::TreeNodeEx(&file, flags, file.dirName.c_str());

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
		selectedFile = &file;
	}

	if (!file.isDir) 
	{
		if (ImGui::BeginDragDropSource(/*ImGuiDragDropFlags_SourceNoDisableHover*/))
		{
			if (EngineExternal->moduleResources->GetTypeFromAssetExtension(file.importPath.c_str()) == Resource::Type::MODEL)
				ImGui::SetDragDropPayload("_MODEL", &file.metaFileDir, file.metaFileDir.length());
			else
				ImGui::SetDragDropPayload("_TEXTURE", &file.metaFileDir, file.metaFileDir.length());


			ImGui::Text("Import asset: %s", file.metaFileDir.c_str());
			ImGui::EndDragDropSource();
		}
	}

	bool showChildren = (!file.isDir || file.childDirs.size() == 0) ? false : nodeOpen;

	if (showChildren == true)
	{
		for (unsigned int i = 0; i < file.childDirs.size(); i++)
		{
			DrawFileTree(file.childDirs[i]);
		}
		ImGui::TreePop();
	}
}
