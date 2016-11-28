#pragma once
#include "stdafx.h"

#include "VRBackend/Vertices.h"
#include "VRBackend/ModelGenerator.h"
#include "VRBackend/ModelMutation.h"

template<size_t N>
void AddVerticesConstructor(class_<Vertices>& vertices);

template<>
void AddVerticesConstructor<0>(class_<Vertices>& vertices);

template<size_t N>
void AddVerticesConstructor(class_<Vertices>& vertices);

void ModelGeneratorFinalizeWithRef(ModelGenerator& generator, boost::reference_wrapper<ID3D11Device> device, EntityHandler& entity_handler, ModelStorageDescription storage_description);

void ModelMutationAddVertexBlock(ModelMutation& mutation, unsigned int vertex_index, Vertices& vertices);

void ModelCreationInterface();
