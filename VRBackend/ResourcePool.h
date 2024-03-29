#pragma once

#include "stdafx.h"

#include "ModelGenerator.h"
#include "ObjLoader.h"
#include "ResourceIdentifier.h"
#include "Shaders.h"
#include "Texture.h"

class EntityHandler;
class ConstantBuffer;

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
	ResourceIdent(const ResourceIdent& other) = default;

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
	ResourcePool(EntityHandler& entity_handler);
	~ResourcePool();

	void Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con);
	void ClearImpermanentModels();

	vector<char> ReadFileToBytes(const string& file_name);

	Model LoadExistingModel(ModelIdentifier model_name);
	Model LoadModelFromFile(ModelIdentifier model_name, const ObjLoader::OutputFormat& model_output_format);
	Model LoadModelFromVertices(ModelIdentifier model_name, const Vertices& vertices, D3D_PRIMITIVE_TOPOLOGY primitive_type, ModelStorageDescription model_storage, map<string, ModelSlice> parts = {});
	Model LoadModelFromGenerator(ModelIdentifier model_name, ModelGenerator&& generator);
	map<string, Model> LoadExistingModelAsParts(const string& file_name);
	// Should only be called by the drawing thread when claiming ownership of the device contex.t
	void UpdateModel(const string& file_name, const ModelMutation& mutation);

	PixelShader LoadPixelShader(string file_name);
	PixelShader LoadPixelShader(string file_name, string function_name);
	PixelShader LoadPixelShader(const ShaderIdentifier& shader_identifier);
	PixelShader LoadExistingPixelShader(string file_name);

	VertexShader LoadVertexShader(string file_name, const D3D11_INPUT_ELEMENT_DESC ied[], int ied_size);
	VertexShader LoadVertexShader(string file_name, VertexType vertex_type);
	VertexShader LoadVertexShader(string file_name, string function_name, const D3D11_INPUT_ELEMENT_DESC ied[], int ied_size);
	VertexShader LoadVertexShader(string file_name, string function_name, VertexType vertex_type);
	VertexShader LoadVertexShader(const ShaderIdentifier& shader_identifier);
	VertexShader LoadExistingVertexShader(string file_name);

	GeometryShader LoadGeometryShader(string file_name);
	GeometryShader LoadGeometryShader(string file_name, string function_name);
	GeometryShader LoadGeometryShader(const ShaderIdentifier& shader_identifier);
	GeometryShader LoadExistingGeometryShader(string file_name);

	Texture& LoadTexture(string file_name);
	Texture& LoadExistingTexture(string file_name);

	vector<float> AccessDataFromResource(const ResourceIdent& resource_ident);
	void PreloadResource(const ResourceIdent& resource_ident);

	ID3D11Device* device_interface;
	ID3D11DeviceContext* device_context;
	EntityHandler& entity_handler_;

private:
	unsigned int lastest_model_number;

	typedef map<string, unsigned int> LookupType;

	struct ModelValue {
		ModelValue() : generator_(), permanent_(false) {}
		ModelValue(ModelGenerator generator, bool permanent) : generator_(std::move(generator)), permanent_(permanent) {}
		ModelGenerator generator_;
		bool permanent_;
	};

	vector<ModelValue> models_;
	LookupType model_lookup;
	vector<PixelShader> pixel_shaders;
	LookupType pixel_shader_lookup;
	vector<VertexShader> vertex_shaders;
	LookupType vertex_shader_lookup;
	vector<GeometryShader> geometry_shaders;
	LookupType geometry_shader_lookup;
	Texture dummy_texture;
	vector<Texture> textures;
	LookupType texture_lookup;

	template <typename ValueType> 
	void RemoveKeyFromLookup(string key, LookupType lookup, vector<ValueType>& values);

	template <typename ValueType> 
	void SwapLookupValues(LookupType& lookup, vector<ValueType>& values, unsigned int src, unsigned int dest);
	template <typename ValueType> 
	void SwapLookupValues(LookupType& lookup, map<unsigned int, string>& reverse_lookup, vector<ValueType>& values, unsigned int src, unsigned int dest);
};

template <typename ValueType>
void ResourcePool::SwapLookupValues(LookupType& lookup, map<unsigned int, string>& reverse_lookup, vector<ValueType>& values, unsigned int src, unsigned int dest) {
	std::swap(values[src], values[dest]);
	string src_key = reverse_lookup[src];
	string dest_key = reverse_lookup[dest];
	lookup[src_key] = dest;
	lookup[dest_key] = src;
	reverse_lookup[src] = dest_key;
	reverse_lookup[dest] = src_key;
}

template <typename ValueType>
void ResourcePool::SwapLookupValues(LookupType& lookup, vector<ValueType>& values, unsigned int src, unsigned int dest) {
	std::swap(values[src], values[dest]);
	string src_key = "";
	string dest_key = "";
	bool src_done = false;
	bool dest_found = false;
	for (auto lookup_iter = lookup.begin(); !((lookup_iter == lookup.end()) || (src_found && dest_found)); ++lookup_iter) {
		if (lookup_iter->second == src) {
			src_key = lookup_iter->first;
			src_found = true;
		}
		if (lookup_iter->second == dest) {
			dest_key = lookup_iter->first;
			dest_found = true;
		}
	}
	lookup[src_key] = dest;
	lookup[dest_key] = src;
}

template <typename ValueType> 
void ResourcePool::RemoveKeyFromLookup(string key, LookupType lookup, vector<ValueType>& values) {
	const auto& value_iter = lookup.find(key);
	if (value_iter == lookup.end()) {
		return;
	}
	unsigned int src_index = value_iter->second;

	if (values.size() == 1) {
		lookup.erase(value_iter);
		values.resize(0);
		return;
	}

	unsigned int dest_index = values.size() - 1;
	SwapLookupValues(lookup, values, src_index, dest_index);
	values.resize(dest_index);
}