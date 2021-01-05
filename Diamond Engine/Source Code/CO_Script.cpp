#include "CO_Script.h"
#include "ImGui/imgui.h"

#include "MO_Editor.h"
#include"MO_Scene.h"

#include "GameObject.h"
#include "Application.h"
#include "DETime.h"
#include "CO_Transform.h"

#include"DEJsonSupport.h"
#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>

C_Script* C_Script::runningScript = nullptr;
C_Script::C_Script(GameObject* _gm, const char* scriptName) : Component(_gm), noGCobject(0)
{
	name = scriptName;
	//strcpy(name, scriptName);

	//EngineExternal->moduleMono->DebugAllMethods(DE_SCRIPTS_NAMESPACE, "GameObject", methods);
	LoadScriptData(scriptName);

	for (unsigned int i = 0; i < fields.size(); i++)
	{
		const char* name = mono_field_get_name(fields[i].field);
		if (strcmp(mono_field_get_name(fields[i].field), "thisReference") == 0) 
		{
			fields[i].fiValue.goValue = _gm;
			SetField(fields[i].field, _gm);
		}
		else
		{
			if(fields[i].type == MonoTypeEnum::MONO_TYPE_CLASS)
				LOG(LogType::L_WARNING, "This reference not found");
		}
	}

}

C_Script::~C_Script()
{
	if (C_Script::runningScript == this)
		C_Script::runningScript = nullptr;

	mono_gchandle_free(noGCobject);

	methods.clear();
	fields.clear();
	name.clear();
}

void C_Script::Update()
{
	if (DETime::state == GameState::STOP || DETime::state == GameState::PAUSE)
		return;

	C_Script::runningScript = this; // I really think this is the peak of stupid code, but hey, it works, slow as hell but works.


	mono_runtime_invoke(updateMethod, mono_gchandle_get_target(noGCobject), NULL, NULL);
}

bool C_Script::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		//ImGui::Separator();

		for (int i = 0; i < fields.size(); i++)
		{
			DropField(fields[i], "_GAMEOBJECT");
		}
		ImGui::Separator();
		for (int i = 0; i < methods.size(); i++)
		{
			ImGui::Text(methods[i].c_str());
		}

		return true;
	}
	return false;
}

void C_Script::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);
	DEJson::WriteString(nObj, "ScriptName", name.c_str());

	for (int i = 0; i < fields.size(); i++)
	{
		switch (fields[i].type)
		{
		case MonoTypeEnum::MONO_TYPE_BOOLEAN:
			DEJson::WriteBool(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.bValue);
			break;

		case MonoTypeEnum::MONO_TYPE_I4:
			DEJson::WriteInt(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.iValue);
			break;

		case MonoTypeEnum::MONO_TYPE_CLASS:
			if(fields[i].fiValue.goValue != nullptr)
				DEJson::WriteInt(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.goValue->UID);
			break;

		case MonoTypeEnum::MONO_TYPE_R4:
			DEJson::WriteFloat(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.fValue);
			break;

		case MonoTypeEnum::MONO_TYPE_STRING:
			DEJson::WriteString(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.strValue);
			break;

		default:
			DEJson::WriteInt(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.iValue);
			break;
		}
	}
}
void C_Script::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	SerializedField* _field = nullptr;
	for (int i = 0; i < fields.size(); i++) //TODO IMPORTANT ASK: There must be a better way to do this... too much use of switches with this stuff, look at MONOMANAGER
	{
		_field = &fields[i];

		switch (_field->type)
		{
		case MonoTypeEnum::MONO_TYPE_BOOLEAN:
			_field->fiValue.bValue = nObj.ReadBool(mono_field_get_name(_field->field));
			break;

		case MonoTypeEnum::MONO_TYPE_I4:
			_field->fiValue.iValue = nObj.ReadInt(mono_field_get_name(_field->field));
			break;

		case MonoTypeEnum::MONO_TYPE_CLASS:
			//ERROR BUG IMPORTANT: Reference not working, if the object is a child, it wont be selected
			_field->fiValue.goValue = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, nObj.ReadInt(mono_field_get_name(_field->field)));

			if(_field->fiValue.goValue)
				SetField(_field->field, _field->fiValue.goValue);

			break;

		case MonoTypeEnum::MONO_TYPE_R4:
			_field->fiValue.fValue = nObj.ReadFloat(mono_field_get_name(_field->field));
			break;

		case MonoTypeEnum::MONO_TYPE_STRING: 
		{
			const char* ret = nObj.ReadString(mono_field_get_name(_field->field));

			if (ret == NULL)
				ret = "\0";

			strcpy(&_field->fiValue.strValue[0], ret);

			break;
		}

		default:
			_field->fiValue.iValue = nObj.ReadInt(mono_field_get_name(_field->field));
			break;
		}
	}
}

void C_Script::DropField(SerializedField& field, const char* dropType)
{

	const char* fieldName = mono_field_get_name(field.field);
	ImGui::PushID(fieldName);

	ImGui::Text(fieldName);
	ImGui::SameLine();


	switch (field.type)
	{
	case MonoTypeEnum::MONO_TYPE_BOOLEAN:
		if(ImGui::Checkbox(mono_field_get_name(field.field), &field.fiValue.bValue))
			mono_field_set_value(mono_gchandle_get_target(noGCobject), field.field, &field.fiValue.bValue);
		break;

	case MonoTypeEnum::MONO_TYPE_I4:
		if(ImGui::InputInt(mono_field_get_name(field.field), &field.fiValue.iValue, 1, 10))
			mono_field_set_value(mono_gchandle_get_target(noGCobject), field.field, &field.fiValue.iValue);
		break;

	case MonoTypeEnum::MONO_TYPE_CLASS:
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), (field.fiValue.goValue != nullptr) ? field.fiValue.goValue->name.c_str() : "None");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dropType))
			{
				field.fiValue.goValue = EngineExternal->moduleEditor->GetDraggingGO();
				SetField(field.field, field.fiValue.goValue);
			}
			ImGui::EndDragDropTarget();
		}
		break;

	case MonoTypeEnum::MONO_TYPE_R4: {
		//float test = 0.f;
		//mono_field_get_value(coreObject, field.field, &test); //TODO: IMPORTANT THIS IS A TEST, REMOVE TEST
		if(ImGui::InputFloat(mono_field_get_name(field.field), &field.fiValue.fValue, 0.1f))
			mono_field_set_value(mono_gchandle_get_target(noGCobject), field.field, &field.fiValue.fValue);
		break;
	}

	case MonoTypeEnum::MONO_TYPE_STRING:
		if(ImGui::InputText(mono_field_get_name(field.field), &field.fiValue.strValue[0], 50))
			mono_field_set_value(mono_gchandle_get_target(noGCobject), field.field, &field.fiValue.strValue);
		break;

	default:
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), mono_type_get_name(mono_field_get_type(field.field)));
		break;
	}
	//TODO: Update C# field value

	ImGui::PopID();
}

void C_Script::LoadScriptData(const char* scriptName)
{
	methods.clear();
	fields.clear();

	//if(coreObject)
	//	mono_free(coreObject);


	EngineExternal->moduleMono->DebugAllMethods(USER_SCRIPTS_NAMESPACE, scriptName, methods);

	MonoClass* klass = mono_class_from_name(EngineExternal->moduleMono->image, USER_SCRIPTS_NAMESPACE, scriptName);


	noGCobject = mono_gchandle_new(mono_object_new(EngineExternal->moduleMono->domain, klass), false);
	mono_runtime_object_init(mono_gchandle_get_target(noGCobject));

	MonoMethodDesc* mdesc = mono_method_desc_new(":Update", false);
	updateMethod = mono_method_desc_search_in_class(mdesc, klass);
	mono_method_desc_free(mdesc);

	EngineExternal->moduleMono->DebugAllFields(scriptName, fields, mono_gchandle_get_target(noGCobject));
}

void C_Script::SetField(MonoClassField* field, GameObject* value)
{
	mono_field_set_value(mono_gchandle_get_target(noGCobject), field, EngineExternal->moduleMono->GoToCSGO(value));
}
