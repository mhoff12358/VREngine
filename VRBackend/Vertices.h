#ifndef __VERTICES_H_INCLUDED__
#define __VERTICES_H_INCLUDED__

#include <vector>

#include "D3D11.h"

#include "Logging.h"

enum VERTEXID {
	TEXTUREVERTEX_ID = 0,
	COLORVERTEX_ID = 1
};

class VertexType {
public:
	VertexType(D3D11_INPUT_ELEMENT_DESC* v_type, int num_elements_in_type);
	VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC> v_type);
	VertexType();

	D3D11_INPUT_ELEMENT_DESC* GetVertexType();
	int GetSizeVertexType();

	// Returns the size of the vertex's data in bytes
	int GetVertexSize();

	bool operator==(const VertexType& that) const;
	//bool operator!=(const VertexType& that) const;

private:
	std::vector<D3D11_INPUT_ELEMENT_DESC> vertex_type;
};

class Vertex {
public:
	Vertex(VertexType v_type, std::vector<float> in_data);

	float* GetData();
	VertexType GetVertexType();
private:
	std::vector<float> data;
	VertexType vertex_type;
};

extern VertexType common_vertex_types[];

struct TEXTUREVERTEX {
	float x, y, z;
	float u, v;

	static D3D11_INPUT_ELEMENT_DESC input_element_desc[2];
	static int input_element_desc_size;
	static int number_of_elements;
	static VERTEXID vertex_type;
	static int vertex_size;
};

struct COLORVERTEX {
	float x, y, z;
	float r, g, b, a;

	static D3D11_INPUT_ELEMENT_DESC input_element_desc[2];
	static int input_element_desc_size;
	static int number_of_elements;
	static VERTEXID vertex_type;
	static int vertex_size;
};

int VERTEX_SIZE_LOOKUP[];

#endif