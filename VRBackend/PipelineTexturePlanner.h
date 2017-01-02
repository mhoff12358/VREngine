#pragma once

#include "stdafx.h"

#include "PipelineCamera.h"
#include "Lights.h"
#include "PipelineStageDesc.h"
#include "TextureUsage.h"

class EntityHandler;
class Texture;
class ResourcePool;

struct TextureBlock {
	int active_references_;
	int64_t texture_;
};

struct TextureReservation {
	TextureReservation(int group_number, int block_number)
		: group_number_(group_number), block_number_(block_number) {}

	int group_number_;
	int block_number_;
};

class PipelineTexturePlanner
{
public:
	PipelineTexturePlanner(
		ID3D11Device* dev,
		ID3D11DeviceContext* dev_con,
		vector<Texture>& texture_storage,
		map<string, int>& entity_group_associations,
		ResourcePool& resource_pool,
		EntityHandler& entity_handler);
	~PipelineTexturePlanner();

	void ParsePipelineStageDescs(const vector<TextureIdent>& existing_textures, vector<unique_ptr<PipelineStageDesc>>& stages);

	vector<Texture>& texture_storage_;
	map<TextureIdent, tuple<ID3D11DepthStencilView*, ID3D11DepthStencilState*>> depth_state_lookup_;
	map<string, int>& entity_group_associations_;
	EntityHandler& entity_handler_;
	ResourcePool& resource_pool_;

	map<TextureSignature, int> group_number_lookup_;
	map<TextureIdent, tuple<int64_t, size_t>> texture_block_lookup_;
	vector<vector<TextureBlock>> texture_blocks_;
	TextureBlock& GetTextureBlock(const tuple<int64_t, size_t>& index);

	int RequestEntityGroup(string stage_name);
	unsigned int RequestCameraIndex(PipelineCameraIdent camera_ident);
	unsigned int RequestLightingIndex(LightingSystemIdent light_ident);

private:
	int CountDependanciesOnTexture(
		const TextureIdent& texture_ident,
		vector<unique_ptr<PipelineStageDesc>>::iterator first_stage,
		vector<unique_ptr<PipelineStageDesc>>::iterator end_stage,
		int initial_dependancies);
	int64_t LookupBlockGroupNumber(const TextureSignature& signature);
	int64_t AddTextureToStorage(const TextureSignature& signature, TextureUsage usage);
	int64_t ReserveTexture(const TextureSignature& signature, const TextureIdent& ident, TextureUsage usage, int number_of_references);
	int64_t ReserveDepthBufferTexture(const TextureSignature& signature_with_size, const TextureIdent& ident, int number_of_references);
	
	map<PipelineCameraIdent, unsigned int> camera_map_;
	map<LightingSystemIdent, unsigned int> light_map_;

	ID3D11Device* device_;
	ID3D11DeviceContext* device_context_;

	int number_of_cameras_;
	int number_of_lights_;
	int number_of_entity_groups_;
};

