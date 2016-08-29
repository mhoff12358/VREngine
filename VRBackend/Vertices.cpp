#include "Vertices.h"

VertexType common_vertex_types[2] = {
	VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC>({
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		})),
	VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC>({
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		})),
};

VertexType::VertexType(D3D11_INPUT_ELEMENT_DESC* v_type, int num_elements_in_type) {
	for (int i = 0; i < num_elements_in_type; i++) {
		vertex_type.push_back(v_type[i]);
	}
}

VertexType::VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC> v_type) : vertex_type(v_type) {

}

VertexType::VertexType() {

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

D3D11_INPUT_ELEMENT_DESC* VertexType::GetVertexType() {
	return vertex_type.data();
}

int VertexType::GetSizeVertexType() {
	return vertex_type.size();
}

int VertexType::GetVertexSize() {
	int total_size = 0;
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

D3D11_INPUT_ELEMENT_DESC TEXTUREVERTEX::input_element_desc[2] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
int TEXTUREVERTEX::input_element_desc_size = 2;
int TEXTUREVERTEX::number_of_elements = 5;
VERTEXID TEXTUREVERTEX::vertex_type = TEXTUREVERTEX_ID;
int TEXTUREVERTEX::vertex_size = 5 * sizeof(float);


D3D11_INPUT_ELEMENT_DESC COLORVERTEX::input_element_desc[2] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

int COLORVERTEX::input_element_desc_size = 2;
int COLORVERTEX::number_of_elements = 7;
VERTEXID COLORVERTEX::vertex_type = COLORVERTEX_ID;
int COLORVERTEX::vertex_size = 7 * sizeof(float);

int VERTEX_SIZE_LOOKUP[2] = { TEXTUREVERTEX::number_of_elements,
COLORVERTEX::number_of_elements };