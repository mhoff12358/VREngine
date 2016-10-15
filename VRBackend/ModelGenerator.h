#pragma once

#include "Vertices.h"
#include "Model.h"

#include "stl.h"

struct ModelStorageDescription {
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
	virtual ~ModelGenerator() {};

	void AddVertex(Vertex new_vertex);
	void AddVertexBatch(const Vertices& new_vertices);
	void SetParts(map<string, ModelSlice> parts);

	Model GetModel(std::string part_name = "");
	std::map<std::string, Model> GetModels();
	void Finalize(ID3D11Device* device, ID3D11DeviceContext* device_context, ModelStorageDescription storage_desription);

	char* GetWritableBuffer(ID3D11DeviceContext* device_context);
	void FinalizeWriteToBuffer(ID3D11DeviceContext* device_context, bool apply_changes = true);

	unsigned int GetCurrentNumberOfVertices() const;

	std::map<std::string, ModelSlice> parts_;

private:
	unsigned int number_of_vertices = 0;
	bool cpu_side_data_kept;
	std::vector<char> cpu_side_data;
	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* staging_buffer;
	VertexType vertex_type;
	D3D_PRIMITIVE_TOPOLOGY primitive_type;

	void InitializeVertexBuffer(ID3D11Device* device, ID3D11DeviceContext* device_context, ModelStorageDescription storage_desription);
	void CopyFromStagingToVertexBuffer(ID3D11DeviceContext* device_context);

	char* MapEditableVertexBuffer(ID3D11DeviceContext* device_context);
};