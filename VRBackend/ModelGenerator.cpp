#include "stdafx.h"
#include "ModelGenerator.h"

#include "EntityHandler.h"
#include "Model.h"
#include "ModelSlice.h"

ModelGenerator::ModelGenerator(VertexType v_type, D3D_PRIMITIVE_TOPOLOGY p_type)
	: vertex_type(v_type), primitive_type(p_type), number_of_vertices(0) {

}

ModelGenerator::ModelGenerator(ModelGenerator&& other) :
	parts_(move(other.parts_)),
	number_of_vertices(other.number_of_vertices),
	cpu_side_data_kept(other.cpu_side_data_kept),
	cpu_side_data(move(other.cpu_side_data)),
	vertex_buffer(other.vertex_buffer),
	staging_buffer(other.staging_buffer),
	vertex_type(other.vertex_type),
	primitive_type(other.primitive_type),
	finalized(other.finalized) {
}

ModelGenerator ModelGenerator::operator=(ModelGenerator&& other) {
	parts_ = move(other.parts_);
	number_of_vertices = other.number_of_vertices;
	cpu_side_data_kept = other.cpu_side_data_kept;
	cpu_side_data = move(other.cpu_side_data);
	vertex_buffer = other.vertex_buffer;
	staging_buffer = other.staging_buffer;
	vertex_type = other.vertex_type;
	primitive_type = other.primitive_type;
	finalized = other.finalized;
	return *this;
}

void ModelGenerator::AddVertex(Vertex new_vertex) {
	AddVertexBatch(Vertices(new_vertex.GetVertexType(), vector<float>{move(new_vertex.GetDataVec())}));
}

void ModelGenerator::AddVertexBatch(const Vertices& new_vertices) {
	unsigned int existing_data_size = cpu_side_data.size();
	cpu_side_data.resize(existing_data_size + new_vertices.GetTotalByteSize());
	char* new_data_location = cpu_side_data.data() + existing_data_size;
	memcpy(new_data_location, new_vertices.GetData(), new_vertices.GetTotalByteSize());
	number_of_vertices += new_vertices.GetNumberOfVertices();
}

void ModelGenerator::SetParts(map<string, ModelSlice> parts) {
	parts_ = std::move(parts);
}

void ModelGenerator::InitializeVertexBuffer(ID3D11Device* device, optional<EntityHandler&> entity_handler, ModelStorageDescription storage_description) {
	int vertex_data_size = vertex_type.GetVertexSize();

	if (storage_description.immutable || !storage_description.store_staging_copy) {
		assert(!storage_description.store_staging_copy);
		D3D11_BUFFER_DESC buffer_desc;
		ZeroMemory(&buffer_desc, sizeof(buffer_desc));

		if (storage_description.immutable) {
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.CPUAccessFlags = 0;
		} else {
			buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		buffer_desc.ByteWidth = cpu_side_data.size();
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertices_data;
		vertices_data.pSysMem = (void*)cpu_side_data.data();

		device->CreateBuffer(&buffer_desc, &vertices_data, &vertex_buffer);
		staging_buffer = nullptr;
	} else {
		D3D11_BUFFER_DESC buffer_desc;
		ZeroMemory(&buffer_desc, sizeof(buffer_desc));

		buffer_desc.Usage = D3D11_USAGE_STAGING;
		buffer_desc.ByteWidth = cpu_side_data.size();
		buffer_desc.BindFlags = 0;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA vertices_data;
		vertices_data.pSysMem = (void*)cpu_side_data.data();

		device->CreateBuffer(&buffer_desc, &vertices_data, &staging_buffer);

		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.ByteWidth = cpu_side_data.size();
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;

		device->CreateBuffer(&buffer_desc, NULL, &vertex_buffer);
		assert(entity_handler.is_initialized());
		entity_handler.get().AddBufferCopy(vertex_buffer, staging_buffer);
	}

	cpu_side_data_kept = true;
	if (!storage_description.retain_cpu_copy) {
		cpu_side_data.clear();
		cpu_side_data_kept = false;
	}
}

char* ModelGenerator::GetWritableBuffer(ID3D11DeviceContext* device_context) {
	// If the buffer is being kept CPU side, the client can just write to that.
	if (cpu_side_data_kept) {
		return cpu_side_data.data();
	}
	
	// If the data is kept GPU side a Map must be created.
	return MapEditableVertexBuffer(device_context);
}

void ModelGenerator::FinalizeWriteToBuffer(ID3D11DeviceContext* device_context, bool apply_changes) {
	// If the data is kept CPU side, that's where the edit was assumed to be.
	// Map the writable buffer and copy the CPU data there,
	// then continue as if the client had made the change to the map.
	if (cpu_side_data_kept) {
		char* gpu_side_data = MapEditableVertexBuffer(device_context);
		memcpy(gpu_side_data, cpu_side_data.data(), cpu_side_data.size());
	}

	// Unmap whatever was opened for writing.
	device_context->Unmap(staging_buffer, 0);
	if (staging_buffer != NULL) {
		// If the staging buffer was used for writing, also do a copy to the usable vertex buffer.
		CopyFromStagingToVertexBuffer(device_context);
	}
}

char* ModelGenerator::MapEditableVertexBuffer(ID3D11DeviceContext* device_context) {
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	if (staging_buffer != NULL) {
		device_context->Map(staging_buffer, 0, D3D11_MAP_WRITE, 0, &mapped_buffer);
	}
	else {
		device_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
	}

	return (char*)mapped_buffer.pData;
}

void ModelGenerator::CopyFromStagingToVertexBuffer(ID3D11DeviceContext* device_context) {
	device_context->CopyResource(vertex_buffer, staging_buffer);
}

void ModelGenerator::Finalize(ID3D11Device* device, optional<EntityHandler&> entity_handler, ModelStorageDescription storage_desription) {
	if (!finalized) {
		InitializeVertexBuffer(device, entity_handler, storage_desription);
		finalized = true;
	}
}

Model ModelGenerator::GetModel(std::string part_name) {
	ModelSlice slice;
	if (part_name.empty()) {
		slice = ModelSlice(GetCurrentNumberOfVertices(), 0);
	} else {
		if (parts_.count(part_name) == 0) {
			part_name = "";
			std::cout << "Attempting to load a model part that doesn't exist." << std::endl;
		}
		slice = parts_[part_name];
	}
	Model model(vertex_buffer, vertex_type.GetVertexSize(), 0, primitive_type, slice);
	return model;
}

std::map<std::string, Model> ModelGenerator::GetModels() {
	std::map<std::string, Model> model_map;
	for (const std::pair<std::string, ModelSlice>& part : parts_) {
		model_map[part.first] = Model(vertex_buffer, vertex_type.GetVertexSize(), 0, primitive_type, part.second);
	}
	return model_map;
}

unsigned int ModelGenerator::GetCurrentNumberOfVertices() const {
	return number_of_vertices;
}

void ModelGenerator::Release() {
	if (vertex_buffer) {
		vertex_buffer->Release();
	}
	if (staging_buffer) {
		staging_buffer->Release();
	}
}