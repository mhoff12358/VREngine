#ifndef __VERTICES_H_INCLUDED__
#define __VERTICES_H_INCLUDED__

#include "D3D11.h"

#include "stl.h"
#include "Logging.h"

enum VERTEXID {
	TEXTUREVERTEX_ID = 0,
	COLORVERTEX_ID = 1
};

class VertexType {
public:
	VertexType(D3D11_INPUT_ELEMENT_DESC* v_type, int num_elements_in_type);
	VertexType(vector<D3D11_INPUT_ELEMENT_DESC> v_type);
	VertexType();

	const D3D11_INPUT_ELEMENT_DESC* GetVertexType() const;
	int GetSizeVertexType() const;

	// Returns the size of the vertex's data in bytes
	unsigned int GetVertexSize() const;

	bool operator==(const VertexType& that) const;
	//bool operator!=(const VertexType& that) const;

	static const VertexType vertex_type_location;
	static const VertexType vertex_type_texture;
	static const VertexType vertex_type_normal;
	static const VertexType vertex_type_all;

	static const VertexType xyuv;

private:
	unsigned int ComputeVertexSize();

	vector<D3D11_INPUT_ELEMENT_DESC> vertex_type;
	unsigned int size_;
};

class Vertex {
public:
	Vertex(VertexType v_type, vector<float> in_data);

	float* GetData();
	VertexType GetVertexType();
	vector<float>& GetDataVec();

private:
	vector<float> data;
	VertexType vertex_type;
};

class Vertices {
public:
	template<unsigned int N>
	Vertices(VertexType vertex_type, vector<array<float, N>> data_per_vertex);
	Vertices(VertexType vertex_type, vector<float> data);

	const float* GetData() const;
	unsigned int GetNumberOfVertices() const;
	const VertexType& GetVertexType() const;
	unsigned int GetTotalByteSize() const;

	class VertexIterator : public std::iterator<std::input_iterator_tag, float> {
		float* p_;
		VertexType& vertex_type_;
	public:
		VertexIterator(float* p, VertexType& vertex_type) : p_(p), vertex_type_(vertex_type) {}
		VertexIterator(const VertexIterator& other) : p_(other.p_), vertex_type_(other.vertex_type_) {}
		VertexIterator& operator++() { p_ += vertex_type_.GetVertexSize(); return *this; }
		VertexIterator& operator++(int) { VertexIterator tmp(*this); operator++(); return tmp; }
		bool operator==(const VertexIterator& other) { return p_ == other.p_; }
		bool operator!=(const VertexIterator& other) { return p_ != other.p_; }
		float& operator*() { return *p_; }
	};
	VertexIterator begin() { return VertexIterator(data_.begin()._Ptr, vertex_type_); }
	VertexIterator end() { return VertexIterator(data_.end()._Ptr, vertex_type_); }
	VertexIterator GetVertexData(unsigned int vertex_index);
	
private:
	VertexType vertex_type_;
	vector<float> data_;
};

template<unsigned int N>
Vertices::Vertices(VertexType vertex_type, vector<array<float, N>> data_per_vertex) : vertex_type_(vertex_type) {
	assert(vertex_type.GetVertexSize() == (sizeof(float) * N));
	data_.resize(data_per_vertex.size() * N);
	for (unsigned int array_index = 0; array_index < data_per_vertex.size(); array_index++) {
		memcpy(data_.data() + (N * array_index), data_per_vertex[array_index].data(), vertex_type.GetVertexSize());
	}
}

#endif