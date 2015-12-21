#include "stdafx.h"
#include "Actor.h"

#include "LuaGameScripting/Index.h"
#include "ConstantBufferLua.h"
#include "ActorHandler.h"

Actor::Actor(const Identifier& ident, ActorHandler& actor_handler, ResourcePool& resource_pool, EntityHandler& entity_handler)
	: actor_handler_(actor_handler), resource_pool_(resource_pool), entity_handler_(entity_handler), models_(), components_(), ident_(ident)
{
}


Actor::~Actor()
{
}


void Actor::InitializeFromLuaScript(lua_State* L, const VRBackendBasics& graphics_objects, const string& script_name, const Identifier& ident) {
	Lua::Environment lua_environment;
	if (L == NULL) {
		lua_environment = Lua::Environment(true);
	}
	else {
		lua_environment = Lua::Environment(L);
	}
	lua_environment.RunFile(script_name);
	if (lua_environment.CheckSizeOfStack() != 0) {
		lua_environment.PrintStack("ERROR RUNNING FILE");
	}
	if (!lua_environment.CallGlobalFunction(string("create_actor"), ident.GetId())) {
		lua_environment.PrintStack("Error loading lua script: " + script_name);
		return;
	}

	std::cout << "Successfully ran script: " << script_name << std::endl;

	LoadModels(lua_environment, graphics_objects);
	LoadShaders(lua_environment, graphics_objects);
	map<string, vector<Model>> component_models = LoadComponentHeirarchy(lua_environment, graphics_objects);
	LoadShaderSettings(lua_environment, graphics_objects, component_models);
	LoadInteractableObjects(lua_environment, graphics_objects);
	HookInCallbacks(lua_environment, graphics_objects);
}

void Actor::LoadModels(Lua::Environment lua_environment, const VRBackendBasics& graphics_objects) {
	if (lua_environment.GetFromTableToStackFailOnNil(string("model_definition"))) {
		if (lua_environment.GetFromTableToStackFailOnNil(string("multi_model"))) {
			int model_index;
			lua_environment.BeginToIterateOverTableLeaveValue();
			while (lua_environment.IterateOverTableLeaveValue(&model_index, NULL)) {
				LoadModel(lua_environment);
			}
			lua_environment.RemoveFromStack();  // Remove multi_model
		} else {
			LoadModel(lua_environment);
		}
		lua_environment.RemoveFromStack();  // Remove model_definition
	}
}

void Actor::LoadModel(Lua::Environment lua_environment) {
	output_formats_.push_back(ObjLoader::default_output_format);
	int model_num = output_formats_.size() - 1;
	if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
		lua_environment.GetFromTableToStack(string("output_format"));
		if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
			lua_environment.GetFromTableToStack(string("model_modifier"));
			if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
				if (lua_environment.LoadArrayFromTable(string("axis_swap"), output_formats_[model_num].model_modifier.axis_swap, Lua::Environment::stack_top, 3) &&
					lua_environment.LoadArrayFromTable(string("axis_scale"), output_formats_[model_num].model_modifier.axis_scale, Lua::Environment::stack_top, 3) &&
					lua_environment.LoadArrayFromTable(string("invert_texture_axis"), output_formats_[model_num].model_modifier.invert_texture_axis, Lua::Environment::stack_top, 2)) {
					output_formats_[model_num].vertex_type = ObjLoader::vertex_type_all;
				}
			}
			lua_environment.RemoveFromStack();
			string vertex_type_name;
			lua_environment.LoadFromTable(string("vertex_type"), &vertex_type_name);
			if (vertex_type_name == "location") {
				output_formats_[model_num].vertex_type = ObjLoader::vertex_type_location;
			}
			else if (vertex_type_name == "texture") {
				output_formats_[model_num].vertex_type = ObjLoader::vertex_type_texture;
			}
			else if (vertex_type_name == "normal") {
				output_formats_[model_num].vertex_type = ObjLoader::vertex_type_normal;
			}
			else if (vertex_type_name == "all") {
				output_formats_[model_num].vertex_type = ObjLoader::vertex_type_all;
			}
			bool load_as_dynamic;
			if (lua_environment.LoadFromTable(string("load_as_dynamic"), &load_as_dynamic)) {
				output_formats_[model_num].load_as_dynamic = load_as_dynamic;
			}
		}
		lua_environment.RemoveFromStack();

		string model_file_name;
		if (lua_environment.LoadFromTable(string("model_file_name"), &model_file_name)) {
			LoadModelsFromFile(model_file_name, output_formats_[model_num]);
			model_resource_id_ = model_file_name;
		}
		else {
			lua_environment.GetFromTableToStack(string("model_vertices"));
			if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
				string object_name;
				vector<vector<float>> vertices_in;
				lua_environment.BeginToIterateOverTable();
				while (lua_environment.IterateOverTable(&object_name, &vertices_in, NULL)) {
					vector<Vertex> vertices_for_model;
					for (const vector<float>& vertex_in : vertices_in) {
						vertices_for_model.push_back(Vertex(output_formats_[model_num].vertex_type, vertex_in));
					}
					ModelStorageDescription model_description;
					if (output_formats_[model_num].load_as_dynamic) {
						model_description = { false, true, false };
					}
					else {
						model_description = { true, false, false, };
					}
					string model_name = resource_pool_.GetNewModelName();
					models_[object_name] = resource_pool_.LoadModel(model_name, vertices_for_model, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, model_description);
					model_resource_id_ = model_name;
				}
			}
			lua_environment.RemoveFromStack();
		}
	}
}
	
void Actor::LoadShaders(Lua::Environment lua_environment, const VRBackendBasics& graphics_objects) {
	int model_num = 0;
	lua_environment.GetFromTableToStack(string("preload_shader_file_names"));
	if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
		lua_environment.BeginToIterateOverTable();
		int key;
		string preload_shader_name;
		while (lua_environment.IterateOverTable(&key, &preload_shader_name, NULL)) {
			graphics_objects.resource_pool->LoadPixelShader(preload_shader_name);
			graphics_objects.resource_pool->LoadVertexShader(preload_shader_name, output_formats_[model_num].vertex_type.GetVertexType(), output_formats_[model_num].vertex_type.GetSizeVertexType());
		}
	}
	lua_environment.RemoveFromStack();
}

map<string, vector<Model>> Actor::LoadComponentHeirarchy(Lua::Environment lua_environment, const VRBackendBasics& graphics_objects) {
	// Expects a model_parentage value that determines the parentage for the visual components.
	// The format for this is a table mapping string keys to lists of lists of strings.
	// Each element in the value lists represents a vector of strings that should be added
	// to the multi-map as a vector of strings.
	multimap<string, vector<string>> model_parentage;
	lua_environment.GetFromTableToStack(string("model_parentage"));
	map<string, vector<Model>> component_models;
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
		component_models = CreateComponents(graphics_objects.view_state->device_interface, model_parentage);
	}
	lua_environment.RemoveFromStack();
	return component_models;
}

void Actor::LoadShaderSettings(Lua::Environment lua_environment, const VRBackendBasics& graphics_objects, map<string, vector<Model>> component_models) {
	int model_num = 0;
	lua_environment.GetFromTableToStack(string("settings_blocks"));
	if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
		lua_environment.BeginToIterateOverTableLeaveValue();
		int settings_num;
		while (lua_environment.IterateOverTableLeaveValue(&settings_num, NULL)) {
			string shader_file_name = "";
			string texture_file_name = "";
			array<bool, 2> texture_stage_usage = { false, true };
			unsigned int entity_group_number = 0;
			lua_environment.LoadFromTable(string("shader_file_name"), &shader_file_name);
			lua_environment.LoadFromTable(string("texture_file_name"), &texture_file_name);
			lua_environment.LoadFromTable(string("texture_stage_usage"), &texture_stage_usage);
			lua_environment.LoadFromTable(string("entity_group_number"), &entity_group_number);

			ConstantBuffer* specified_shader_settings = NULL;
			if (lua_environment.GetFromTableToStack(string("shader_settings_format")) && lua_environment.CheckTypeOfStack() != LUA_TNIL) {
				string pipeline_stage;
				LuaConstantBufferFormat data_format;
				lua_environment.LoadFromTable(string("pipeline_stage"), &pipeline_stage);
				lua_environment.LoadFromTable(string("data_format"), &data_format);
				specified_shader_settings = new ConstantBufferLua(ConstantBufferLua::GetStageFromString(pipeline_stage), data_format);
				specified_shader_settings->CreateBuffer(graphics_objects.view_state->device_interface);
			}
			lua_environment.RemoveFromStack();

			ShaderSettings used_settings;
			if (specified_shader_settings != NULL) {
				used_settings = ShaderSettings(specified_shader_settings);
			}
			if (shader_file_name != "") {
				if (texture_file_name != "") {
					if (texture_file_name == "|first_stage_buffer") {
						TextureView texture_view = TextureView(0, 0, texture_stage_usage[0], texture_stage_usage[1], graphics_objects.render_pipeline->GetFirstStageBuffer());
						shader_settings_entity_ids_.push_back(graphics_objects.entity_handler->AddEntity(Entity(
							ES_SETTINGS,
							graphics_objects.resource_pool->LoadPixelShader(shader_file_name),
							graphics_objects.resource_pool->LoadVertexShader(shader_file_name, output_formats_[model_num].vertex_type.GetVertexType(), output_formats_[model_num].vertex_type.GetSizeVertexType()),
							ShaderSettings(used_settings),
							Model(),
							NULL,
							texture_view), entity_group_number));
					}
					else {
						Texture texture = graphics_objects.resource_pool->LoadTexture(texture_file_name);
						TextureView texture_view = TextureView(0, 0, texture_stage_usage[0], texture_stage_usage[1], texture);
						shader_settings_entity_ids_.push_back(graphics_objects.entity_handler->AddEntity(Entity(
							ES_SETTINGS,
							graphics_objects.resource_pool->LoadPixelShader(shader_file_name),
							graphics_objects.resource_pool->LoadVertexShader(shader_file_name, output_formats_[model_num].vertex_type.GetVertexType(), output_formats_[model_num].vertex_type.GetSizeVertexType()),
							ShaderSettings(used_settings),
							Model(),
							NULL,
							texture_view), entity_group_number));
					}
				}
				else {
					shader_settings_entity_ids_.push_back(graphics_objects.entity_handler->AddEntity(Entity(
						ES_SETTINGS,
						graphics_objects.resource_pool->LoadPixelShader(shader_file_name),
						graphics_objects.resource_pool->LoadVertexShader(shader_file_name, output_formats_[model_num].vertex_type.GetVertexType(), output_formats_[model_num].vertex_type.GetSizeVertexType()),
						ShaderSettings(used_settings),
						Model(),
						NULL), entity_group_number));
				}
				lua_environment.GetFromTableToStack(string("components"));
				if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
					lua_environment.BeginToIterateOverTable();
					int components_key;
					string component_name;
					while (lua_environment.IterateOverTable(&components_key, &component_name, NULL)) {
						components_[component_lookup_[component_name]].AddEntitiesToHandler(entity_handler_, entity_group_number, component_models[component_name]);
					}
				}
				lua_environment.RemoveFromStack();
			}
		}
	}
	lua_environment.RemoveFromStack();
}

void Actor::LoadInteractableObjects(Lua::Environment lua_environment, const VRBackendBasics& graphics_objects) {
	lua_environment.GetFromTableToStack(string("interactable_objects"));
	if (lua_environment.CheckTypeOfStack() != LUA_TNIL) {
		lua_environment.BeginToIterateOverTableLeaveKey();
		while (lua_environment.IterateOverTableLeaveKey(NULL)) {
			LookInteractable* new_interactable = actor_handler_.interactable_collection_.CreateLookInteractableFromLua(this, lua_environment);
			string parent_component_name;
			if (new_interactable != NULL && lua_environment.LoadFromTable(string("parent_component"), &parent_component_name)) {
				new_interactable->SetModelTransformation(&components_[component_lookup_[parent_component_name]].GetTransformationData()->transformation);
			}
		}
	}
	lua_environment.RemoveFromStack();
}

void Actor::HookInCallbacks(Lua::Environment lua_environment, const VRBackendBasics& graphics_objects) {
	lua_environment.GetFromTableToStack(string("interaction_callbacks"));

	lua_environment.RemoveFromStack(Lua::Index(1));
	lua_interface_ = Lua::InteractableObject(lua_environment);

	lua_interface_.env_.StoreToTable(string("cpp_interface"), (void*)&lua_interface_);
	lua_interface_.AddCObjectMember("clear_component_transformation", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < Actor, &Actor::ClearComponentTransformation >, 1 }));
	lua_interface_.AddCObjectMember("apply_to_component_transformation", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < Actor, &Actor::ApplyToComponentTransformation >, 1 }));
	lua_interface_.AddCObjectMember("set_component_transformation", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < Actor, &Actor::SetComponentTransformation >, 1 }));
	lua_interface_.AddCObjectMember("set_shader", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < Actor, &Actor::SetShader >, 1 }));
	lua_interface_.AddCObjectMember("set_constant_buffer", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < Actor, &Actor::SetConstantBuffer >, 1 }));
	lua_interface_.AddCObjectMember("set_enabled", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < Actor, &Actor::SetEnabled>, 1 }));
	lua_interface_.AddCObjectMember("set_vertices", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < Actor, &Actor::SetVertices>, 1 }));
	lua_interface_.AddCObjectMember("raycast", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < Actor, &Actor::Raycast>, 1 }));
}

void Actor::LoadModelsFromFile(string file_name, const ObjLoader::OutputFormat& output_format) {
	for (const pair<string, Model>& kv : resource_pool_.LoadModelAsParts(file_name, output_format)) {
		models_[kv.first] = kv.second;
	}
}

map<string, vector<Model>> Actor::CreateComponents(ID3D11Device* device_interface, const multimap<string, vector<string>>& parentages) {
	int current_parent_index = -1;
	int current_children_index = 0;
	int current_num_children = 0;
	vector<string> component_names;
	map<string, vector<Model>> component_models;
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
					if (child_model != models_.end()) {
						current_children.push_back(child_model->second);
					} else {
						std::cout << "Child model not found" << std::endl;
					}
				}
				components_.emplace_back(device_interface);
				component_models.insert(std::make_pair(parentage.second.front(), current_children));
				components_.back().SetLocalTransformation(DirectX::XMMatrixIdentity());
				component_names.push_back(parentage.second.front());
				component_lookup_[parentage.second.front()] = components_.size() - 1;
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

	return component_models;
}

void XM_CALLCONV Actor::SetComponentTransformation(const string& component_name, DirectX::FXMMATRIX new_transformation) {
	components_[component_lookup_[component_name]].SetLocalTransformation(new_transformation);
}

const DirectX::XMMATRIX& Actor::GetComponentTransformation(const string& component_name) {
	return components_[component_lookup_[component_name]].GetCombinedTransformation();
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

int Actor::SetComponentTransformation(lua_State* L) {
	Lua::Environment env(L);
	string component_name;
	env.LoadFromStack(&component_name, Lua::Index(1));
	SetComponentTransformation(component_name, LuaTableToMatrix(env));
	env.RemoveFromStack(Lua::Index(1));
	return 0;
}

int Actor::SetShader(lua_State* L) {
	Lua::Environment env(L);
	size_t shader_number;
	env.LoadFromStack(&shader_number, Lua::Index(1));
	string shader_name;
	env.LoadFromStack(&shader_name, Lua::Index(1));
	entity_handler_.SetEntityPixelShader(shader_settings_entity_ids_[shader_number], resource_pool_.LoadExistingPixelShader(shader_name));
	entity_handler_.SetEntityVertexShader(shader_settings_entity_ids_[shader_number], resource_pool_.LoadExistingVertexShader(shader_name));
	env.RemoveFromStack();
	env.RemoveFromStack();
	return 0;
}

int Actor::SetConstantBuffer(lua_State* L) {
	Lua::Environment env(L);
	size_t shader_number;
	env.LoadFromStack(&shader_number, Lua::Index(1));
	vector<vector<float>> new_buffer_data;
	env.LoadFromStack(&new_buffer_data, Lua::Index(1));
	ConstantBuffer* buffer = entity_handler_.GetShaderSettings(shader_settings_entity_ids_[shader_number]);
	float* buffer_data = static_cast<float*>(buffer->EditBufferData());
	size_t current_buffer_data_index = 0;
	for (const vector<float>& element : new_buffer_data) {
		for (float value : element) {
			buffer_data[current_buffer_data_index] = value;
			current_buffer_data_index++;
		}
	}
	return 0;
}

int Actor::SetEnabled(lua_State* L) {
	Lua::Environment env(L);
	string component_name;
	env.LoadFromStack(&component_name, Lua::Index(1));
	bool is_enabled;
	env.LoadFromStack(&is_enabled, Lua::Index(1));
	Component& new_component = components_[component_lookup_[component_name]];
	for (unsigned int i = new_component.first_entity_; i < new_component.first_entity_ + new_component.number_of_entities_; ++i) {
		if (is_enabled) {
			entity_handler_.EnableEntity(i);
		} else {
			entity_handler_.DisableEntity(i);
		}
	}
	return 0;
}

int Actor::SetVertices(lua_State* L) {
	Lua::Environment env(L);
	int model_number;
	env.LoadFromStack(&model_number, Lua::Index(1));
	if (env.CheckTypeOfStack() != LUA_TTABLE) {
		std::cout << "DID NOT FIND TABLE FOR SETTING VERTICES" << std::endl;
	}
	env.BeginToIterateOverTable();
	int first_vertex_index;
	vector<vector<float>> new_vertices;
	ModelMutation new_mutation;
	while (env.IterateOverTable(&first_vertex_index, &new_vertices, NULL)) {
		vector<Vertex> vertices;
		for (const vector<float>& new_vertex : new_vertices) {
			vertices.push_back(Vertex(output_formats_[model_number].vertex_type, new_vertex));
		}
		new_mutation.AddVertexBlock(first_vertex_index, vertices.data(), vertices.size());
	}
	entity_handler_.AddModelMutation(model_resource_id_, new_mutation);
	return 0;
}

int Actor::Raycast(lua_State* L) {
	Lua::Environment env(L);
	string component_name;
	env.LoadFromStack(&component_name, Lua::Index(1));

	const DirectX::XMMATRIX& component_trans = GetComponentTransformation(component_name);
	DirectX::XMMATRIX ray_trans;
	DirectX::XMVECTOR scale;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR translation;
	DirectX::XMMatrixDecompose(&scale, &rotation, &translation, component_trans);
	rotation = DirectX::XMQuaternionInverse(rotation);
	translation = DirectX::XMVectorNegate(translation);
	ray_trans = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationQuaternion(rotation), DirectX::XMMatrixTranslationFromVector(translation));
	tuple<Identifier, Actor*, float> result = actor_handler_.interactable_collection_.GetClosestLookedAt(ray_trans);
	env.StoreToStack(std::get<0>(result).GetId());
	env.StoreToStack(std::get<2>(result));

	return 2;
}