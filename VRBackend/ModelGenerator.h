#pragma once

#include "stdafx.h"
#include "Vertices.h"
#include "Model.h"

class ModelSlice;
class EntityHandler;

struct ModelStorageDescription {
	ModelStorageDescription() : ModelStorageDescription(true, false, false) {}
	ModelStorageDescription(bool immutable, bool store_staging_copy, bool retain_cpu_copy)
		: immutable(immutable), store_staging_copy(store_staging_copy), retain_cpu_copy(retain_cpu_copy) {
	}

	bool immutable;
	bool store_staging_copy;
	bool retain_cpu_copy;

	static ModelStorageDescription Immutable() {
		return {true, false, false};
	}
};

class ModelGenerator {
public:
	ModelGenerator() {}
	ModelGenerator(VertexType v_type, D3D_PRIMITIVE_TOPOLOGY p_type);
	ModelGenerator(const ModelGenerator& other) = default;
	ModelGenerator(ModelGenerator&& other);
	ModelGenerator operator=(ModelGenerator&& other);

	void AddVertex(Vertex new_vertex);
	void AddVertexBatch(const Vertices& new_vertices);
	void SetParts(map<string, ModelSlice> parts);

	Model GetModel(string part_name = "");
	map<string, Model> GetModels();
	void Finalize(ID3D11Device* device, optional<EntityHandler&> entity_handler, ModelStorageDescription storage_desription);

	char* GetWritableBuffer(ID3D11DeviceContext* device_context);
	void FinalizeWriteToBuffer(ID3D11DeviceContext* device_context, bool apply_changes = true);

	unsigned int GetCurrentNumberOfVertices() const;
	void Release();

	map<string, ModelSlice> parts_;

private:
	unsigned int number_of_vertices = 0;
	bool cpu_side_data_kept;
	vector<char> cpu_side_data;
	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* staging_buffer = nullptr;
	VertexType vertex_type;
	D3D_PRIMITIVE_TOPOLOGY primitive_type;
	bool finalized = false;

	void InitializeVertexBuffer(ID3D11Device* device, optional<EntityHandler&> entity_handler, ModelStorageDescription storage_desription);
	void CopyFromStagingToVertexBuffer(ID3D11DeviceContext* device_context);

	char* MapEditableVertexBuffer(ID3D11DeviceContext* device_context);
};