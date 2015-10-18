#include "stdafx.h"
#include "Actor.h"

#include "LuaGameScripting/Index.h"

Actor::Actor(ResourcePool& resource_pool)
	: resource_pool_(resource_pool), models_(), components_()
{
}


Actor::~Actor()
{
}


void Actor::InitializeFromLuaScript(const string& script_name, const VRBackendBasics& graphics_objects, InteractableCollection& interactable_collection, ConstantBuffer* shader_settings) {
	Lua::Environment lua_environment = Lua::Environment(true);
	lua_environment.RunFile(script_name);
	if (!lua_environment.CallGlobalFunction(string("create_actor"))) {
		lua_environment.PrintStack("Error loading lua script: " + script_name);
		return;
	}
	string model_file_name;
	lua_environment.LoadFromTable(string("model_file_name"), &model_file_name);
	lua_environment.GetFromTableToStack(string("output_format"));
	ObjLoader::OutputFormat output_format = ObjLoader::default_output_format;
	if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
		lua_environment.GetFromTableToStack(string("model_modifier"));
		if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
			if (lua_environment.LoadArrayFromTable(string("axis_swap"), output_format.model_modifier.axis_swap, Lua::Environment::stack_top, 3) &&
				lua_environment.LoadArrayFromTable(string("axis_scale"), output_format.model_modifier.axis_scale, Lua::Environment::stack_top, 3) &&
				lua_environment.LoadArrayFromTable(string("invert_texture_axis"), output_format.model_modifier.invert_texture_axis, Lua::Environment::stack_top, 2)) {
				output_format.vertex_type = ObjLoader::vertex_type_all;
				LoadModelsFromFile(model_file_name, output_format);
			}
		}
		lua_environment.RemoveFromStack();
	}
	LoadModelsFromFile(model_file_name, output_format);
	lua_environment.RemoveFromStack();

	string shader_file_name;
	lua_environment.LoadFromTable(string("shader_file_name"), &shader_file_name);
	string texture_file_name;
	lua_environment.LoadFromTable(string("texture_file_name"), &texture_file_name);
	Texture texture = graphics_objects.resource_pool->LoadTexture(texture_file_name);
	TextureView texture_view(0, 0, texture);
	shader_settings_entity_id_ = graphics_objects.entity_handler->AddEntity(Entity(
		ES_SETTINGS,
		graphics_objects.resource_pool->LoadPixelShader(shader_file_name),
		graphics_objects.resource_pool->LoadVertexShader(shader_file_name, output_format.vertex_type.GetVertexType(), output_format.vertex_type.GetSizeVertexType()),
		ShaderSettings(shader_settings),
		Model(),
		NULL,
		texture_view));

	// Expects a model_parentage value that determines the parentage for the visual components.
	// The format for this is a table mapping string keys to lists of lists of strings.
	// Each element in the value lists represents a vector of strings that should be added
	// to the multi-map as a vector of strings.
	multimap<string, vector<string>> model_parentage;
	lua_environment.GetFromTableToStack(string("model_parentage"));
	if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
		lua_environment.BeginToIterateOverTableLeaveValue();
		string parent_name;
		bool successful;
		while (lua_environment.IterateOverTableLeaveValue(&parent_name, &successful)) {
			// The list of string lists should now be on the top of the stack.
			lua_environment.BeginToIterateOverTableLeaveKey();
			while (lua_environment.IterateOverTableLeaveKey(NULL)) {
				vector<string> children_names(lua_environment.GetArrayLength());
				lua_environment.PeekArrayFromStack(children_names.data());
				model_parentage.insert(make_pair(parent_name, children_names));
			}
		}
		assert(successful);
		CreateComponents(*graphics_objects.entity_handler, graphics_objects.view_state->device_interface, model_parentage);
	}
	lua_environment.RemoveFromStack();

	lua_environment.GetFromTableToStack(string("interactable_objects"));
	if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
		lua_environment.BeginToIterateOverTableLeaveKey();
		while (lua_environment.IterateOverTableLeaveKey(NULL)) {
			LookInteractable* new_interactable = interactable_collection.CreateLookInteractableFromLua(lua_environment);
			string parent_component_name;
			if (new_interactable != NULL && lua_environment.LoadFromTable(string("parent_component"), &parent_component_name)) {
				new_interactable->SetModelTransformation(&components_[component_lookup_[parent_component_name]].GetTransformationData()->transformation);
			}
		}
	}
	lua_environment.RemoveFromStack();

	lua_environment.GetFromTableToStack(string("interaction_callbacks"));

	lua_environment.RemoveFromStack(Lua::Index(1));
	lua_environment.StoreToStack((void*)this, Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < Actor, &Actor::ClearComponentTransformation >, 1 }));
	lua_environment.StoreToTable(string("clear_component_transformation"), Lua::Index(2), Lua::Index(1));
	lua_environment.RemoveFromStack();
	lua_environment.StoreToStack((void*)this, Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < Actor, &Actor::ApplyToComponentTransformation >, 1 }));
	lua_environment.StoreToTable(string("apply_to_component_transformation"), Lua::Index(2), Lua::Index(1));
	lua_environment.RemoveFromStack();
	lua_environment.PrintStack("End");

	lua_interface_ = Lua::InteractableObject(lua_environment);
	// Ends with the lua_environment containing only the callbacks
}

unsigned int Actor::GetShaderSettingsId() {
	return shader_settings_entity_id_;
}

void Actor::LoadModelsFromFile(string file_name, const ObjLoader::OutputFormat& output_format) {
	models_ = resource_pool_.LoadModelAsParts(file_name, output_format);
}

void Actor::CreateComponents(EntityHandler& entity_handler, ID3D11Device* device_interface, const multimap<string, vector<string>>& parentages) {
	int current_parent_index = -1;
	int current_children_index = 0;
	int current_num_children = 0;
	vector<string> component_names;
	components_.reserve(parentages.size());

	// This while loop check should be redundant as both of these conditions should become false at the same time.
	// However, checking both ensures that components_ isn't underfilled for what is referenced and that
	// it doesn't grow infinitely.
	while ((current_parent_index != components_.size()) && (components_.size() <= models_.size())) {
		string next_parent = "";
		for (const pair<string, vector<string>>& parentage : parentages) {
			if ((current_parent_index != -1 && parentage.first == component_names[current_parent_index]) || (current_parent_index == -1 && parentage.first == "")) {
				vector<Model> current_children;
				for (const string& child_model_name : parentage.second) {
					auto child_model = models_.find(child_model_name);
					assert(child_model != models_.end());
					current_children.push_back(child_model->second);
				}
				components_.emplace_back(entity_handler, device_interface, current_children);
				components_.back().SetLocalTransformation(DirectX::XMMatrixIdentity());
				component_names.push_back(parentage.second.front());
				component_lookup_[parentage.second.front()] = components_.size() - 1;
				std::cerr << "Adding component with parent: " << (current_parent_index == -1 ? "" : component_names[current_parent_index]) << " and name: " << parentage.second.front() << std::endl;
				current_num_children++;
			}
		}

		if (current_parent_index != -1) {
			components_[current_parent_index].SetChildren(components_.data() + current_children_index, current_num_children);
		}

		current_children_index += current_num_children;
		current_num_children = 0;
		current_parent_index++;
	}
}

void XM_CALLCONV Actor::SetComponentTransformation(const string& component_name, DirectX::FXMMATRIX new_transformation) {
	components_[component_lookup_[component_name]].SetLocalTransformation(new_transformation);
}

int Actor::ClearComponentTransformation(lua_State* L) {
	Lua::Environment env(L);
	string component_name;
	env.LoadFromStack(&component_name, Lua::Index(1));
	SetComponentTransformation(component_name, DirectX::XMMatrixIdentity());
	env.RemoveFromStack(Lua::Index(1));
	return 0;
}

int Actor::ApplyToComponentTransformation(lua_State* L) {
	Lua::Environment env(L);
	string component_name;
	env.LoadFromStack(&component_name, Lua::Index(1));
	components_[component_lookup_[component_name]].LeftComposeLocalTransformation(LuaTableToMatrix(env));
	env.RemoveFromStack(Lua::Index(1));
	return 0;
}
