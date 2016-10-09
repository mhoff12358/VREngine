#pragma once

#include "stl.h"
#include "PipelineStageDesc.h"
#include "EntityHandler.h"

struct TextureBlock {
	int active_references_;
	int texture_;
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

	map<TextureSignature, int> group_number_lookup_;
	map<TextureIdent, tuple<int, int>> texture_block_lookup_;
	vector<vector<TextureBlock>> texture_blocks_;
	TextureBlock& GetTextureBlock(const tuple<int, int>& index);

	int RequestEntityGroup(string stage_name);

	ResourcePool& resource_pool_;

private:
	int CountDependanciesOnTexture(
		const TextureIdent& texture_ident,
		vector<unique_ptr<PipelineStageDesc>>::iterator first_stage,
		vector<unique_ptr<PipelineStageDesc>>::iterator end_stage,
		int initial_dependancies);
	int LookupBlockGroupNumber(const TextureSignature& signature);
	int AddTextureToStorage(const TextureSignature& signature, TextureUsage usage);
	int ReserveTexture(const TextureSignature& signature, const TextureIdent& ident, TextureUsage usage, int number_of_references);
	int ReserveDepthBufferTexture(const TextureSignature& signature_with_size, const TextureIdent& ident, int number_of_references);
	
	ID3D11Device* device_;
	ID3D11DeviceContext* device_context_;

	int number_of_entity_groups_;
};

