#pragma once

#include "Model.h"
#include "Shaders.h"
#include "Vertices.h"
#include "ModelGenerator.h"
#include "Texture.h"
#include "ObjLoader.h"
#include "ConstantBuffer.h"
#include "ResourceIdentifier.h"

#include "stl.h"

class ResourceIdent {
public:
	enum ResourceType {
		NONE = 0,
		MODEL = 1,
		PIXEL_SHADER = 2,
		VERTEX_SHADER = 3,
		GEOMETRY_SHADER = 4,
		SHADER_FILE = 5,
		TEXTURE = 6,
	};

	ResourceIdent() : type_(NONE) {}
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

	void Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con);

	Model LoadExistingModel(ModelIdentifier model_name);
	Model LoadModelFromFile(ModelIdentifier model_name, const ObjLoader::OutputFormat& model_output_format);
	Model LoadModelFromVertices(ModelIdentifier model_name, vector<Vertex> vertices, D3D_PRIMITIVE_TOPOLOGY primitive_type, ModelStorageDescription model_storage);
	Model LoadModelFromGenerator(ModelIdentifier model_name, ModelGenerator generator);
	map<string, Model> LoadExistingModelAsParts(const string& file_name);
	void UpdateModel(const string& file_name, const ModelMutation& mutation);

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
