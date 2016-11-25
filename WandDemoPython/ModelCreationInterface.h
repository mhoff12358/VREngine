#pragma once
#include "stdafx.h"

#include "VRBackend/Vertices.h"
#include "VRBackend/ModelGenerator.h"

template<size_t N>
void AddVerticesConstructor(class_<Vertices>& vertices);

template<>
void AddVerticesConstructor<0>(class_<Vertices>& vertices) {}

template<size_t N>
void AddVerticesConstructor(class_<Vertices>& vertices) {
	vertices.def(init<VertexType, vector<array<float, N>>>());
	AddVerticesConstructor<N - 1>(vertices);
}

void ModelGeneratorFinalizeWithRef(ModelGenerator& generator, boost::reference_wrapper<ID3D11Device> device, EntityHandler& entity_handler, ModelStorageDescription storage_description) {
	generator.Finalize(device.get_pointer(), entity_handler, storage_description);
}

void ModelMutationAddVertexBlock(ModelMutation& mutation, unsigned int vertex_index, Vertices& vertices) {
	mutation.AddVertexBlock(vertex_index, std::move(vertices));
}

void ModelCreationInterface() {
	class_<VertexType>("VertexType", no_init)
		.def_readonly("location", &VertexType::vertex_type_location)
		.def_readonly("texture", &VertexType::vertex_type_texture)
		.def_readonly("texture_skip_normal", &VertexType::vertex_type_texture_skip_normal)
		.def_readonly("normal", &VertexType::vertex_type_normal)
		.def_readonly("all", &VertexType::vertex_type_all)
		.def_readonly("xyuv", &VertexType::xyuv);

	auto vertices_registration = class_<Vertices>("Vertices", init<VertexType, vector<float>>());
	AddVerticesConstructor<12>(vertices_registration);

	enum_<D3D_PRIMITIVE_TOPOLOGY>("D3DTopology")
		.value("POINTLIST", D3D_PRIMITIVE_TOPOLOGY_POINTLIST)
		.value("LINELIST", D3D_PRIMITIVE_TOPOLOGY_LINELIST)
		.value("LINESTRIP", D3D_PRIMITIVE_TOPOLOGY_LINESTRIP)
		.value("TRIANGLELIST", D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		.value("TRIANGLESTRIP", D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
		.value("LINELIST_ADJ", D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ)
		.value("LINESTRIP_ADJ", D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ)
		.value("TRIANGLELIST_ADJ", D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ)
		.value("TRIANGLESTRIP_ADJ", D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ);

	class_<ModelGenerator, std::shared_ptr<ModelGenerator>, boost::noncopyable>("ModelGenerator")
		.def(init<VertexType, D3D_PRIMITIVE_TOPOLOGY>())
		.def("AddVertexBatch", &ModelGenerator::AddVertexBatch)
		.def("SetParts", &ModelGenerator::SetParts)
		.def("Finalize", &ModelGeneratorFinalizeWithRef);

	class_<ModelMutation, boost::noncopyable>("ModelMutation")
		.def("AddVertexBlock", &ModelMutationAddVertexBlock);

	class_<ModelStorageDescription>("ModelStorageDescription", init<bool, bool, bool>());

	class_<ModelSlice>("ModelSlice")
		.def(init<unsigned int, unsigned int>())
		.add_property("start", boost::python::make_getter(&ModelSlice::start), boost::python::make_setter(&ModelSlice::start))
		.add_property("length", boost::python::make_getter(&ModelSlice::length), boost::python::make_setter(&ModelSlice::length));

	CreateMap<string, ModelSlice>("String", "ModelSlice");

	class_<ResourceIdentifier, boost::noncopyable>("ResourceIdentifier")
		.def("StripSubmodel", &ResourceIdentifier::StripSubmodel)
		.def("GetSubmodel", &ResourceIdentifier::GetSubmodel)
		.def("AddSubmodel", &ResourceIdentifier::AddSubmodel)
		.def("GetNewModelName", &ResourceIdentifier::GetNewModelName)
		.def("GetConstantModelName", &ResourceIdentifier::GetConstantModelName);
}
