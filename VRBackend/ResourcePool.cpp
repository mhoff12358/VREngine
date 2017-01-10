#include "stdafx.h"
#include "ResourcePool.h"

#include "EntityHandler.h"
#include "Texture.h"

#include <fstream>

ResourcePool::ResourcePool(EntityHandler& entity_handler) :
	entity_handler_(entity_handler)
{
	lastest_model_number = 0;

	geometry_shaders.push_back(GeometryShader(nullptr));
	geometry_shader_lookup[ResourceIdentifier::GetConstantModelName("unset_geometry_shader")] = 0;
}


ResourcePool::~ResourcePool()
{
}

vector<char> ResourcePool::ReadFileToBytes(const string& file_name) {
	vector<char> data;
	std::ifstream file_stream(file_name.c_str(), std::ios::in | std::ios::binary);
	if (!file_stream.is_open()) {
		return data;
	}
	file_stream.seekg(0, file_stream.end);
	data.resize(file_stream.tellg());
	file_stream.seekg(0, file_stream.beg);
	file_stream.read(data.data(), data.size());
	return data;
}

void ResourcePool::Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con) {
	device_interface = dev;
	device_context = dev_con;
}

Model ResourcePool::LoadExistingModel(ModelIdentifier model_name) {
	auto existing_model_gen = model_lookup.find(model_name.GetFileName());
	if (existing_model_gen != model_lookup.end()) {
		return models_[existing_model_gen->second].generator_.GetModel(model_name.GetSubPart());
	}

	return Model();
}

map<string, Model> ResourcePool::LoadExistingModelAsParts(const string& file_name) {
	auto existing_model_gen = model_lookup.find(file_name);
	if (existing_model_gen != model_lookup.end()) {
		return models_[existing_model_gen->second].generator_.GetModels();
	}

	return map<string, Model>{};
}

Model ResourcePool::LoadModelFromFile(ModelIdentifier model_name, const ObjLoader::OutputFormat& model_output_format) {
	Model model = LoadExistingModel(model_name);
	if (!model.IsDummy()) {
		return model;
	}

	ModelGenerator generator = ObjLoader::CreateModelsFromFile(
		device_interface, model_name.GetFileName(), model_output_format);

	models_.emplace_back(std::move(generator), true);
	model_lookup[model_name.GetFileName()] = models_.size() - 1;

	return models_.back().generator_.GetModel(model_name.GetSubPart());
}

Model ResourcePool::LoadModelFromVertices(
	ModelIdentifier model_name, const Vertices& vertices,
	D3D_PRIMITIVE_TOPOLOGY primitive_type, ModelStorageDescription model_storage,
	map<string, ModelSlice> parts) {
	Model model = LoadExistingModel(model_name);
	if (!model.IsDummy()) {
		return model;
	}

	ModelGenerator generator(vertices.GetVertexType(), primitive_type);
	generator.AddVertexBatch(vertices);
	parts[""] = ModelSlice(generator.GetCurrentNumberOfVertices(), 0);
	generator.SetParts(parts);
	generator.Finalize(device_interface, entity_handler_, model_storage);

	models_.emplace_back(std::move(generator), false);
	model_lookup[model_name.GetFileName()] = models_.size() - 1;
	
	return models_.back().generator_.GetModel(model_name.GetSubPart());
}

Model ResourcePool::LoadModelFromGenerator(ModelIdentifier model_name, ModelGenerator&& generator) {
	Model model = LoadExistingModel(model_name);
	if (!model.IsDummy()) {
		return model;
	}

	models_.push_back(ModelValue(std::move(generator), false));
	model_lookup[model_name.GetFileName()] = models_.size() - 1;

	ModelGenerator& new_generator = models_.back().generator_;

	return new_generator.GetModel(model_name.GetSubPart());
}

void ResourcePool::UpdateModel(const string& file_name, const ModelMutation& mutation) {
	auto existing_model = model_lookup.find(file_name);
	if (existing_model == model_lookup.end()) {
		std::cout << "ATTEMPTING TO UPDATE NON-EXISTANT MODEL" << std::endl;
		return;
	}

	ModelGenerator& generator = models_[existing_model->second].generator_;
	mutation.ApplyToData(generator.GetWritableBuffer(device_context));
	generator.FinalizeWriteToBuffer(device_context);
}

Texture& ResourcePool::LoadTexture(std::string file_name) {
	auto existing_texture = texture_lookup.find(file_name);
	if (existing_texture != texture_lookup.end()) {
		return textures[existing_texture->second];
	}

	ID3D11Texture2D* loaded_texture_data;
	HRESULT load_result;
	D3DX11CreateTextureFromFile(
		device_interface,
		file_name.c_str(),
		NULL,
		NULL,
		(ID3D11Resource**)&loaded_texture_data,
		&load_result);
	if (load_result != S_OK) {
		std::cerr << "Error loading texture " << file_name << std::endl;
	}
	textures.emplace_back(TextureUsage::ShaderResource());
	unsigned int texture_id = textures.size() - 1;
	D3D11_TEXTURE2D_DESC texture_description;
	loaded_texture_data->GetDesc(&texture_description);
	textures[texture_id].Initialize(device_interface, device_context,
		{ { static_cast<int>(texture_description.Width), static_cast<int>(texture_description.Height) } },
		loaded_texture_data);
	texture_lookup[file_name] = texture_id;
	return textures[texture_id];
}

Texture& ResourcePool::LoadExistingTexture(std::string file_name) {
	auto existing_texture = texture_lookup.find(file_name);
	if (existing_texture != texture_lookup.end()) {
		return textures[existing_texture->second];
	}
	assert(false);
	return dummy_texture;
}

PixelShader ResourcePool::LoadExistingPixelShader(std::string file_name) {
	auto existing_pixel_shader = pixel_shader_lookup.find(file_name);
	if (existing_pixel_shader != pixel_shader_lookup.end()) {
		return pixel_shaders[existing_pixel_shader->second];
	}
	return PixelShader();
}

PixelShader ResourcePool::LoadPixelShader(std::string file_name) {
	return LoadPixelShader(file_name, "PShader");
}

PixelShader ResourcePool::LoadPixelShader(std::string file_name, std::string function_name) {
	auto existing_pixel_shader = pixel_shader_lookup.find(file_name);
	if (existing_pixel_shader != pixel_shader_lookup.end()) {
		return pixel_shaders[existing_pixel_shader->second];
	}

	ID3D11PixelShader* new_dx_pixel_shader;
	ID3D10Blob* pixel_shader_blob;
	ID3D10Blob* pixel_shader_error_blob;
	D3DX11CompileFromFile(file_name.c_str(), 0, 0, function_name.c_str(), "ps_5_0", 0, 0, 0, &pixel_shader_blob, &pixel_shader_error_blob, 0);
	if (pixel_shader_blob == NULL) {
		std::cout << std::string((const char*)pixel_shader_error_blob->GetBufferPointer()) << std::endl;
	}
	device_interface->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), NULL, &new_dx_pixel_shader);

	PixelShader new_pixel_shader(new_dx_pixel_shader);

	pixel_shaders.push_back(new_pixel_shader);
	pixel_shader_lookup[file_name] = pixel_shaders.size() - 1;

	return new_pixel_shader;
}

PixelShader ResourcePool::LoadPixelShader(const ShaderIdentifier& shader_identifier) {
	auto existing_pixel_shader = pixel_shader_lookup.find(shader_identifier.GetFileName());
	if (existing_pixel_shader != pixel_shader_lookup.end()) {
		return pixel_shaders[existing_pixel_shader->second];
	}

	if (!shader_identifier.GetPrecompiled()) {
		if (shader_identifier.GetFunctionName().empty()) {
			return LoadPixelShader(shader_identifier.GetFileName());
		} else {
			return LoadPixelShader(shader_identifier.GetFileName(), shader_identifier.GetFunctionName());
		}
	}

	vector<char> bytes = ReadFileToBytes(shader_identifier.GetFileName());
	if (bytes.empty()) {
		std::cout << "Attempting to load empty precompiled shader" << std::endl;
	}
	ID3D11PixelShader* new_dx_pixel_shader;
	HRESULT result = device_interface->CreatePixelShader(bytes.data(), bytes.size(), nullptr, &new_dx_pixel_shader);

	PixelShader new_pixel_shader(new_dx_pixel_shader);

	pixel_shaders.push_back(new_pixel_shader);
	pixel_shader_lookup[shader_identifier.GetFileName()] = pixel_shaders.size() - 1;

	return new_pixel_shader;
}

VertexShader ResourcePool::LoadVertexShader(std::string file_name, VertexType vertex_type) {
	return LoadVertexShader(file_name, vertex_type.GetVertexType(), vertex_type.GetSizeVertexType());
}

VertexShader ResourcePool::LoadVertexShader(std::string file_name, const D3D11_INPUT_ELEMENT_DESC ied[], int ied_size) {
	return LoadVertexShader(file_name, "VShader", ied, ied_size);
}

VertexShader ResourcePool::LoadVertexShader(std::string file_name, std::string function_name, VertexType vertex_type) {
	return LoadVertexShader(file_name, "VShader", vertex_type.GetVertexType(), vertex_type.GetSizeVertexType());
}

VertexShader ResourcePool::LoadVertexShader(std::string file_name, std::string function_name, const D3D11_INPUT_ELEMENT_DESC ied[], int ied_size) {
	auto existing_vertex_shader = vertex_shader_lookup.find(file_name);
	if (existing_vertex_shader != vertex_shader_lookup.end()) {
		return vertex_shaders[existing_vertex_shader->second];
	}

	ID3D11VertexShader* new_dx_vertex_shader;
	ID3D11InputLayout* new_dx_input_layout;
	ID3D10Blob* vertex_shader_blob;
	ID3D10Blob* vertex_shader_error_blob;
	D3DX11CompileFromFile(file_name.c_str(), 0, 0, function_name.c_str(), "vs_5_0", 0, 0, 0, &vertex_shader_blob, &vertex_shader_error_blob, 0);
	if (vertex_shader_blob == NULL) {
		std::cout << std::string((const char*)vertex_shader_error_blob->GetBufferPointer()) << std::endl;
	}
	device_interface->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), nullptr, &new_dx_vertex_shader);
	device_interface->CreateInputLayout(ied, ied_size, vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &new_dx_input_layout);

	if (new_dx_input_layout == nullptr) {
		std::cout << "Error creating input layout" << std::endl;
	}

	VertexShader new_vertex_shader(new_dx_input_layout, new_dx_vertex_shader);

	vertex_shaders.push_back(new_vertex_shader);
	vertex_shader_lookup[file_name] = vertex_shaders.size() - 1;

	return new_vertex_shader;
}

VertexShader ResourcePool::LoadVertexShader(const ShaderIdentifier& shader_identifier) {
	auto existing_vertex_shader = vertex_shader_lookup.find(shader_identifier.GetFileName());
	if (existing_vertex_shader != vertex_shader_lookup.end()) {
		return vertex_shaders[existing_vertex_shader->second];
	}

	if (!shader_identifier.GetPrecompiled()) {
		if (shader_identifier.GetFunctionName().empty()) {
			return LoadVertexShader(shader_identifier.GetFileName(), shader_identifier.GetVertexType());
		} else {
			return LoadVertexShader(shader_identifier.GetFileName(), shader_identifier.GetFunctionName(), shader_identifier.GetVertexType());
		}
	}

	vector<char> bytes = ReadFileToBytes(shader_identifier.GetFileName());
	if (bytes.empty()) {
		std::cout << "Attempting to load empty precompiled shader" << std::endl;
	}
	ID3D11VertexShader* new_dx_vertex_shader;
	ID3D11InputLayout* new_dx_input_layout;
	HRESULT result = device_interface->CreateVertexShader(bytes.data(), bytes.size(), nullptr, &new_dx_vertex_shader);
	HRESULT result2 = device_interface->CreateInputLayout(shader_identifier.GetVertexType().GetVertexType(), shader_identifier.GetVertexType().GetSizeVertexType(), bytes.data(), bytes.size(), &new_dx_input_layout);

	if (new_dx_input_layout == nullptr) {
		std::cout << "Error creating input layout" << std::endl;
	}

	VertexShader new_vertex_shader(new_dx_input_layout, new_dx_vertex_shader);

	vertex_shaders.push_back(new_vertex_shader);
	vertex_shader_lookup[shader_identifier.GetFileName()] = vertex_shaders.size() - 1;

	return new_vertex_shader;
}

VertexShader ResourcePool::LoadExistingVertexShader(std::string file_name) {
	auto existing_vertex_shader = vertex_shader_lookup.find(file_name);
	if (existing_vertex_shader != vertex_shader_lookup.end()) {
		return vertex_shaders[existing_vertex_shader->second];
	}
	return VertexShader();
}

GeometryShader ResourcePool::LoadExistingGeometryShader(std::string file_name) {
	auto existing_geometry_shader = geometry_shader_lookup.find(file_name);
	if (existing_geometry_shader != geometry_shader_lookup.end()) {
		return geometry_shaders[existing_geometry_shader->second];
	}
	return GeometryShader();
}

GeometryShader ResourcePool::LoadGeometryShader(std::string file_name) {
	return LoadGeometryShader(file_name, "GShader");
}

GeometryShader ResourcePool::LoadGeometryShader(std::string file_name, std::string function_name) {
	auto existing_geometry_shader = geometry_shader_lookup.find(file_name);
	if (existing_geometry_shader != geometry_shader_lookup.end()) {
		return geometry_shaders[existing_geometry_shader->second];
	}

	ID3D11GeometryShader* new_dx_geometry_shader;
	ID3D10Blob* geometry_shader_blob;
	ID3D10Blob* geometry_shader_error_blob;
	D3DX11CompileFromFile(file_name.c_str(), 0, 0, function_name.c_str(), "gs_5_0", 0, 0, 0, &geometry_shader_blob, &geometry_shader_error_blob, 0);
	if (geometry_shader_blob == NULL) {
		std::cout << std::string((const char*)geometry_shader_error_blob->GetBufferPointer()) << std::endl;
	}
	device_interface->CreateGeometryShader(geometry_shader_blob->GetBufferPointer(), geometry_shader_blob->GetBufferSize(), NULL, &new_dx_geometry_shader);

	GeometryShader new_geometry_shader(new_dx_geometry_shader);

	geometry_shaders.push_back(new_geometry_shader);
	geometry_shader_lookup[file_name] = geometry_shaders.size() - 1;

	return new_geometry_shader;
}

GeometryShader ResourcePool::LoadGeometryShader(const ShaderIdentifier& shader_identifier) {
	auto existing_geometry_shader = geometry_shader_lookup.find(shader_identifier.GetFileName());
	if (existing_geometry_shader != geometry_shader_lookup.end()) {
		return geometry_shaders[existing_geometry_shader->second];
	}

	if (!shader_identifier.GetPrecompiled()) {
		if (shader_identifier.GetFunctionName().empty()) {
			return LoadGeometryShader(shader_identifier.GetFileName());
		} else {
			return LoadGeometryShader(shader_identifier.GetFileName(), shader_identifier.GetFunctionName());
		}
	}

	vector<char> bytes = ReadFileToBytes(shader_identifier.GetFileName());
	if (bytes.empty()) {
		std::cout << "Attempting to load empty precompiled shader" << std::endl;
	}
	ID3D11GeometryShader* new_dx_geometry_shader;
	HRESULT result = device_interface->CreateGeometryShader(bytes.data(), bytes.size(), nullptr, &new_dx_geometry_shader);

	GeometryShader new_geometry_shader(new_dx_geometry_shader);

	geometry_shaders.push_back(new_geometry_shader);
	geometry_shader_lookup[shader_identifier.GetFileName()] = geometry_shaders.size() - 1;

	return new_geometry_shader;
}

vector<float> ResourcePool::AccessDataFromResource(const ResourceIdent& resource_ident) {
	switch (resource_ident.type_) {
	case ResourceIdent::TEXTURE:
		return LoadExistingTexture(resource_ident.name_).CopyOutData();
	default:
		return vector<float>();
	}
}

void ResourcePool::ClearImpermanentModels() {
	int kept_index = models_.size();
	int remove_index = -1;
	map<unsigned int, string> reverse_lookup;
	for (const pair<string, unsigned int> kv : model_lookup) {
		reverse_lookup[kv.second] = kv.first;
	}

	while (true) {
		remove_index++;
		while ((remove_index != kept_index) && models_[remove_index].permanent_) {
			remove_index++;
		}
		kept_index--;
		while ((remove_index != kept_index) && !models_[kept_index].permanent_) {
			kept_index--;
		}
		
		if (remove_index != kept_index) {
			break;
		}

		//swaps.push_back(std::make_pair(kept_index, remove_index));
		SwapLookupValues(model_lookup, reverse_lookup, models_, kept_index, remove_index);
	}

	int num_permanents = 0;
	while (num_permanents < models_.size() && models_[num_permanents].permanent_) {
		num_permanents++;
	}

	for (int i = num_permanents + 1; i < models_.size(); i++) {
		model_lookup.erase(reverse_lookup[i]);
		models_[i].generator_.Release();
	}
	models_.resize(num_permanents);

}

void ResourcePool::PreloadResource(const ResourceIdent& resource_ident) {
	switch (resource_ident.type_) {
	case ResourceIdent::NONE:
		break;
	case ResourceIdent::TEXTURE:
		LoadTexture(resource_ident.name_);
		break;
	case ResourceIdent::MODEL:
		if (resource_ident.models_.GetCurrentNumberOfVertices() == 0) {
			LoadModelFromFile(ModelIdentifier(resource_ident.name_), resource_ident.output_format_);
		} else {
			//LoadModelFromGenerator(ModelIdentifier(resource_ident.name_), std::move(resource_ident.models_));
		}
		break;
	case ResourceIdent::VERTEX_SHADER:
		LoadVertexShader(resource_ident.name_, resource_ident.vertex_type_);
		break;
	case ResourceIdent::PIXEL_SHADER:
		LoadPixelShader(resource_ident.name_);
		break;
	case ResourceIdent::GEOMETRY_SHADER:
		LoadGeometryShader(resource_ident.name_);
		break;
	case ResourceIdent::SHADER_FILE:
		LoadPixelShader(resource_ident.name_);
		LoadVertexShader(resource_ident.name_, resource_ident.vertex_type_);
		break;
	}
}