#include"AssetDir.h"
#include"Application.h"
#include"MO_ResourceManager.h"
#include"IM_FileSystem.h"
#include"IM_ModelImporter.h"
#include"MO_MonoManager.h"

AssetDir::AssetDir(const char* _dName, const char* _imPath, uint64 _lMod, bool _dir) : isDir(_dir), lastModTime(_lMod), resourceType(Resource::Type::UNKNOWN)
{
	dirName = _dName;
	dirName.push_back('\0');
	importPath = _imPath;
	importPath.push_back('\0');

	if (!isDir) 
	{
		GenerateMetaPath();
		if (HasMeta()) 
		{
			metaUID = EngineExternal->moduleResources->GetMetaUID(metaFileDir.c_str());
			resourceType = EngineExternal->moduleResources->GetMetaType(metaFileDir.c_str());
		}
		else if(!isDir)
		{
			resourceType = Resource::Type::MESH;
		}
	}
}

AssetDir::~AssetDir()
{
	dirName.clear();
	childDirs.clear();
	importPath.clear();
	metaFileDir.clear();
	lastModTime = 0;
}

void AssetDir::ClearData()
{
	dirName.clear();
	childDirs.clear();
	isDir = false;
	importPath.clear();
	metaFileDir.clear();
	lastModTime = 0;
}

bool AssetDir::HasMeta()
{
	return FileSystem::Exists(metaFileDir.c_str());
}

void AssetDir::GenerateMeta()
{
	GenerateMetaPath();

	if (!HasMeta())
	{
		resourceType = EngineExternal->moduleResources->GetTypeFromAssetExtension(importPath.c_str());
		uint resUID = EngineExternal->moduleResources->GenerateNewUID();
		metaUID = resUID;

		EngineExternal->moduleResources->GenerateMeta(importPath.c_str(), EngineExternal->moduleResources->GenLibraryPath(resUID, resourceType).c_str(), resUID, resourceType);

	}
}

void AssetDir::GenerateMetaRecursive()
{
	if (!isDir)
	{
		GenerateMeta();
	}

	for (size_t i = 0; i < childDirs.size(); i++)
	{
		childDirs[i].GenerateMetaRecursive();
	}
}

void AssetDir::CreateLibraryFileRecursive()
{
	//Create library file
	if (!isDir && EngineExternal->moduleResources->ExistsOnLibrary(importPath.c_str()) == 0) /*TODO: Or has meta but the mirror on library does not exist*/
	{
		//Create mirror
		uint id = EngineExternal->moduleResources->CreateLibraryFromAssets(importPath.c_str());
	}

	for (size_t i = 0; i < childDirs.size(); i++)
	{
		childDirs[i].CreateLibraryFileRecursive();
	}
}

void AssetDir::GenerateMetaPath()
{
	metaFileDir = importPath;
	metaFileDir.pop_back();

	metaFileDir += ".meta";
	metaFileDir.push_back('\0');
}

void AssetDir::DeletePermanent()
{
	//Directory can only be deleted if there is nothing inside
	if (isDir && childDirs.size() != 0) 
	{
		//Delete all files inside recursive
		for (size_t i = 0; i < childDirs.size(); i++)
		{
			childDirs[i].DeletePermanent();
		}
	}

	//Remove library
	if (!isDir) 
	{
		EngineExternal->moduleFileSystem->DeleteAssetFile(EngineExternal->moduleResources->LibraryFromMeta(this->metaFileDir.c_str()).c_str());

		if (resourceType == Resource::Type::MODEL) 
		{
			std::vector<uint> meshesID;
			//Get meshes from fbx meta
			if (FileSystem::Exists(metaFileDir.c_str())) 
			{
				ModelImporter::GetMeshesFromMeta(importPath.c_str(), meshesID);

				//Delete said meshes
				for (int i = 0; i < meshesID.size(); i++)
				{
					std::string meshes = MESHES_PATH;
					meshes += std::to_string(meshesID[i]);
					meshes += ".mmh";
					EngineExternal->moduleFileSystem->DeleteAssetFile(meshes.c_str());
				}

				EngineExternal->moduleResources->UpdateMeshesDisplay();
			}		
		}
	}

	EngineExternal->moduleFileSystem->DeleteAssetFile(importPath.c_str());

	if (!isDir)
	{
		if (resourceType == Resource::Type::SCRIPT)
		{
			EngineExternal->moduleMono->RemoveScriptFromSLN(this->importPath.c_str());
			EngineExternal->moduleMono->ReCompileCS();
		}
	}

	EngineExternal->moduleFileSystem->DeleteAssetFile(metaFileDir.c_str());

	ClearData();
}
