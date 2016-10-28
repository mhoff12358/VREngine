#include "stdafx.h"
#include "Vertices.h"

#include "Logging.h"

const VertexType VertexType::vertex_type_all = VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC>({
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
}));

const VertexType VertexType::vertex_type_texture = VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC>({
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
}));

const VertexType VertexType::vertex_type_normal = VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC>({
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
}));

const VertexType VertexType::vertex_type_location = VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC>({
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
}));

const VertexType VertexType::xyuv = VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC>({
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
}));

VertexType::VertexType(D3D11_INPUT_ELEMENT_DESC* v_type, int num_elements_in_type) {
	for (int i = 0; i < num_elements_in_type; i++) {
		vertex_type.push_back(v_type[i]);
	}
	size_ = ComputeVertexSize();
}

VertexType::VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC> v_type) : vertex_type(v_type) {
	size_ = ComputeVertexSize();
}

VertexType::VertexType() {
	size_ = 0;
}

bool VertexType::operator==(const VertexType& that) const {
	if (vertex_type.size() != that.vertex_type.size()) {
		return false;
	}
	bool equal = true;
	for (unsigned int i = 0; equal && (i < vertex_type.size()); i++) {
		equal = vertex_type[i].Format == that.vertex_type[i].Format;
	}
	return equal;
}

const D3D11_INPUT_ELEMENT_DESC* VertexType::GetVertexType() const {
	return vertex_type.data();
}

int VertexType::GetSizeVertexType() const {
	return vertex_type.size();
}

unsigned int VertexType::GetVertexSize() const {
	return size_;
}

unsigned int VertexType::ComputeVertexSize() {
	unsigned int total_size = 0;
	for (const D3D11_INPUT_ELEMENT_DESC& ied : vertex_type) {
		if (ied.Format == DXGI_FORMAT_R32G32B32A32_FLOAT) {
			total_size += 4 * sizeof(float);
		} else if (ied.Format == DXGI_FORMAT_R32G32B32_FLOAT) {
			total_size += 3 * sizeof(float);
		} else if (ied.Format == DXGI_FORMAT_R32G32_FLOAT) {
			total_size += 2 * sizeof(float);
		} else if (ied.Format == DXGI_FORMAT_R32_FLOAT) {
			total_size += 1 * sizeof(float);
		} else {
			OutputFormatted("Invalid Input Element Description in vertex type");
		}
	}
	return total_size;
}

Vertex::Vertex(VertexType v_type, std::vector<float> in_data) :
	vertex_type(v_type), data(in_data) {

}

float* Vertex::GetData() {
	return data.data();
}

VertexType Vertex::GetVertexType() {
	return vertex_type;
}

vector<float>& Vertex::GetDataVec() {
	return data;
}

Vertices::Vertices(VertexType vertex_type, vector<float> data) : vertex_type_(vertex_type), data_(move(data)) {

}

const float* Vertices::GetData() const {
	return data_.data();
}

unsigned int Vertices::GetTotalByteSize() const {
	return data_.size() * sizeof(float);
}

Vertices::VertexIterator Vertices::GetVertexData(unsigned int vertex_index) {
	return VertexIterator(&data_[vertex_index * vertex_type_.GetVertexSize()], vertex_type_);
}

unsigned int Vertices::GetNumberOfVertices() const {
	return GetTotalByteSize() / vertex_type_.GetVertexSize();
}

const VertexType& Vertices::GetVertexType() const {
	return vertex_type_;
}