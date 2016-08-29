#pragma once

#include "Model.h"
#include "Shaders.h"
#include "Vertices.h"
#include "ModelGenerator.h"
#include "Texture.h"
#include "ObjLoader.h"
#include "ConstantBuffer.h"

#include "stl.h"

class ResourceIdent {
public:
	enum ResourceType {
		MODEL = 1,
		PIXEL_SHADER = 2,
		VERTEX_SHADER = 3,
		GEOMETRY_SHADER = 4,
		SHADER_FILE = 5,
		TEXTURE = 6,
	};

	ResourceIdent(ResourceType type, const string& name) : type_(type), name_(name) {}
	ResourceIdent(ResourceType type, const string& name, ObjLoader::OutputFormat output_format)
		: type_(type), name_(name), output_format_(output_format) {}
	ResourceIdent(ResourceType type, const string& name, VertexType vertex_type)
		: type_(type), name_(name), vertex_type_(vertex_type) {}
	ResourceIdent(ResourceType type, const string& name, ModelGenerator models)
		: type_(type), name_(name), models_(models) {}

	ResourceType type_;
	string name_;
	
	// Values that are needed for some of the resource types but not included for all.
	VertexType vertex_type_;

	ObjLoader::OutputFormat output_format_;

	ModelGenerator models_;
};

class ResourcePool
{
public:
	ResourcePool();
	~ResourcePool();

	static string object_delimiter;
	static string unique_identifier;
	static int unique_number;
	static string StripSubmodel(string name) {
		return name.substr(0, name.find(object_delimiter));
	};
	static string GetSubmodel(string name) {
		return name.substr(max(name.find(object_delimiter) + 1, name.length()));
	}
	static string AddSubmodel(string name, string submodel) {
		return name + object_delimiter + submodel;
	}
	static string GetNewModelName(string description = "") {
		return unique_identifier + description + unique_identifier + std::to_string(unique_number++);
	}
	static string GetConstantModelName(string description) {
		return unique_identifier + description;
	}

	void Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con);

	Model LoadModel(string file_name);
	Model LoadModel(string file_name, const ObjLoader::OutputFormat& model_output_format);
	map<string, Model> LoadModelAsParts(string file_name, const ObjLoader::OutputFormat& model_output_format);
	Model LoadModel(string model_name, vector<Vertex> vertices, D3D_PRIMITIVE_TOPOLOGY primitive_type, ModelStorageDescription model_storage);
	void UpdateModel(string model_name, const ModelMutation& mutation);
	void InsertExternallyCreatedModelParts(string model_name, ModelGenerator models);
	Model LoadExistingModel(string model_name);

	PixelShader LoadPixelShader(string file_name);
	PixelShader LoadPixelShader(string file_name, string function_name);
	PixelShader LoadExistingPixelShader(string file_name);

	VertexShader LoadVertexShader(string file_name, D3D11_INPUT_ELEMENT_DESC ied[], int ied_size);
	VertexShader LoadVertexShader(string file_name, VertexType vertex_type);
	VertexShader LoadVertexShader(string file_name, string function_name, D3D11_INPUT_ELEMENT_DESC ied[], int ied_size);
	VertexShader LoadExistingVertexShader(string file_name);

	GeometryShader LoadGeometryShader(string file_name);
	GeometryShader LoadGeometryShader(string file_name, string function_name);
	GeometryShader LoadExistingGeometryShader(string file_name);

	//template <typename T>
	//void CreateNamedConstantBuffer(string name, CB_PIPELINE_STAGES stage);
	//ConstantBuffer* GetNamedConstantBuffer(string name);

	Texture& LoadTexture(string file_name);
	Texture& LoadExistingTexture(string file_name);

	vector<float> AccessDataFromResource(const ResourceIdent& resource_ident);
	void PreloadResource(const ResourceIdent& resource_ident);

	ID3D11Device* device_interface;
	ID3D11DeviceContext* device_context;

private:
	unsigned int lastest_model_number;

	vector<ModelGenerator> models_;
	map<string, unsigned int> model_lookup;
	vector<PixelShader> pixel_shaders;
	map<string, unsigned int> pixel_shader_lookup;
	vector<VertexShader> vertex_shaders;
	map<string, unsigned int> vertex_shader_lookup;
	vector<GeometryShader> geometry_shaders;
	map<string, unsigned int> geometry_shader_lookup;
	vector<Texture> textures;
	map<string, unsigned int> texture_lookup;
	map<string, unique_ptr<ConstantBuffer>> named_constant_buffers_;
};
/*
template <typename T>
void ResourcePool::CreateNamedConstantBuffer<T>(string name, CB_PIPELINE_STAGES stage) {
	unique_ptr<T> new_buffer(new ConstantBufferTypedTemp<T>(stage));
	new_buffer->CreateBuffer(device_interface);
	named_constant_buffers_.emplace(name, move(new_buffer));
}*/