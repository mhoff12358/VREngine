#include "ResourcePool.h"


ResourcePool::ResourcePool()
{
	lastest_model_number = 0;

	geometry_shaders.push_back(GeometryShader(nullptr));
	geometry_shader_lookup[ResourceIdentifier::GetConstantModelName("unset_geometry_shader")] = 0;
}


ResourcePool::~ResourcePool()
{
}


void ResourcePool::Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con) {
	device_interface = dev;
	device_context = dev_con;
}

Model ResourcePool::LoadExistingModel(ModelIdentifier model_name) {
	auto existing_model_gen = model_lookup.find(model_name.GetFileName());
	if (existing_model_gen != model_lookup.end()) {
		return models_[existing_model_gen->second].GetModel(model_name.GetSubPart());
	}

	return Model();
}

map<string, Model> ResourcePool::LoadExistingModelAsParts(const string& file_name) {
	auto existing_model_gen = model_lookup.find(file_name);
	if (existing_model_gen != model_lookup.end()) {
		return models_[existing_model_gen->second].GetModels();
	}

	return map<string, Model>{};
}

Model ResourcePool::LoadModelFromFile(ModelIdentifier model_name, const ObjLoader::OutputFormat& model_output_format) {
	Model model = LoadExistingModel(model_name);
	if (!model.IsDummy()) {
		return model;
	}

	ModelGenerator generator = ObjLoader::CreateModelsFromFile(
		device_interface, device_context, model_name.GetFileName(), model_output_format);

	models_.push_back(generator);
	model_lookup[model_name.GetFileName()] = models_.size() - 1;

	return generator.GetModel(model_name.GetSubPart());
}

Model ResourcePool::LoadModelFromVertices(ModelIdentifier model_name, std::vector<Vertex> vertices, D3D_PRIMITIVE_TOPOLOGY primitive_type, ModelStorageDescription model_storage) {
	Model model = LoadExistingModel(model_name);
	if (!model.IsDummy()) {
		return model;
	}

	if (vertices.empty()) {
		return Model(); // Somehow return an empty model eventually
	}

	ModelGenerator generator(vertices[0].GetVertexType(), primitive_type);
	generator.AddVertexBatch(vertices);
	generator.Finalize(device_interface, device_context, model_storage);

	models_.push_back(generator);
	model_lookup[model_name.GetFileName()] = models_.size() - 1;
	
	return generator.GetModel(model_name.GetSubPart());
}

Model ResourcePool::LoadModelFromGenerator(ModelIdentifier model_name, ModelGenerator generator) {
	Model model = LoadExistingModel(model_name);
	if (!model.IsDummy()) {
		return model;
	}

	models_.push_back(generator);
	model_lookup[model_name.GetFileName()] = models_.size() - 1;

	return generator.GetModel(model_name.GetSubPart());
}

void ResourcePool::UpdateModel(const string& file_name, const ModelMutation& mutation) {
	auto existing_model = model_lookup.find(file_name);
	if (existing_model == model_lookup.end()) {
		std::cout << "ATTEMPTING TO UPDATE NON-EXISTANT MODEL" << std::endl;
		return;
	}

	ModelGenerator& generator = models_[existing_model->second];
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
	return Texture();
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

VertexShader ResourcePool::LoadVertexShader(std::string file_name, VertexType vertex_type) {
	return LoadVertexShader(file_name, vertex_type.GetVertexType(), vertex_type.GetSizeVertexType());
}

VertexShader ResourcePool::LoadVertexShader(std::string file_name, D3D11_INPUT_ELEMENT_DESC ied[], int ied_size) {
	return LoadVertexShader(file_name, "VShader", ied, ied_size);
}

VertexShader ResourcePool::LoadVertexShader(std::string file_name, std::string function_name, D3D11_INPUT_ELEMENT_DESC ied[], int ied_size) {
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
	device_interface->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), NULL, &new_dx_vertex_shader);
	HandleHResult(device_interface->CreateInputLayout(ied, ied_size, vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &new_dx_input_layout));

	if (new_dx_input_layout == nullptr) {
		std::cout << "Error creating input layout" << std::endl;
	}

	VertexShader new_vertex_shader(new_dx_input_layout, new_dx_vertex_shader);

	vertex_shaders.push_back(new_vertex_shader);
	vertex_shader_lookup[file_name] = vertex_shaders.size() - 1;

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

vector<float> ResourcePool::AccessDataFromResource(const ResourceIdent& resource_ident) {
	switch (resource_ident.type_) {
	case ResourceIdent::TEXTURE:
		return LoadExistingTexture(resource_ident.name_).CopyOutData();
	default:
		return vector<float>();
	}
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
			LoadModelFromGenerator(ModelIdentifier(resource_ident.name_), resource_ident.models_);
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