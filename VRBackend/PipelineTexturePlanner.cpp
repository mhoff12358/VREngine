#include "PipelineTexturePlanner.h"


PipelineTexturePlanner::PipelineTexturePlanner(
	ID3D11Device* dev, ID3D11DeviceContext* dev_con,
	vector<Texture>& texture_storage,
	map<string, int>& entity_group_associations,
	ResourcePool& resource_pool,
	EntityHandler& entity_handler)
	: device_(dev), device_context_(dev_con),
	texture_storage_(texture_storage),
	entity_group_associations_(entity_group_associations),
	resource_pool_(resource_pool),
	entity_handler_(entity_handler),
	number_of_entity_groups_(0),
	number_of_cameras_(0)
{
}


PipelineTexturePlanner::~PipelineTexturePlanner()
{
}

int PipelineTexturePlanner::RequestEntityGroup(string stage_name) {
	string base_name = PipelineStageDesc::GetBaseName(stage_name);
	auto existing_group_number = entity_group_associations_.find(base_name);
	if (existing_group_number != entity_group_associations_.end()) {
		return existing_group_number->second;
	}
	int new_group_number = number_of_entity_groups_;
	number_of_entity_groups_++;
	entity_group_associations_[base_name] = new_group_number;
	return new_group_number;
}

unsigned int PipelineTexturePlanner::RequestCameraIndex(PipelineCameraIdent camera_ident) {
	auto existing_camera_number = camera_map_.find(camera_ident);
	if (existing_camera_number != camera_map_.end()) {
		return existing_camera_number->second;
	}
	int new_camera_number = number_of_cameras_;
	number_of_cameras_++;
	camera_map_[camera_ident] = new_camera_number;
	return new_camera_number;
}

int PipelineTexturePlanner::CountDependanciesOnTexture(
	const TextureIdent& texture_ident,
	vector<unique_ptr<PipelineStageDesc>>::iterator first_stage,
	vector<unique_ptr<PipelineStageDesc>>::iterator end_stage,
	int initial_dependancies) {

	int number_of_dependancies = initial_dependancies;

	for (auto dependant_stage_iter = first_stage; dependant_stage_iter != end_stage; dependant_stage_iter++) {
		for (BasePipelineStageDesc* dependant_stage : (*dependant_stage_iter)->GetBaseStages()) {
			for (const TextureIdent& dependancy : dependant_stage->input_textures_idents_) {
				if (dependancy == texture_ident) {
					number_of_dependancies++;
				}
			}
			for (const tuple<TextureIdent, TextureSignature>& dependancy : dependant_stage->output_textures_descs_) {
				if (get<0>(dependancy) == texture_ident) {
					number_of_dependancies++;
				}
			}
			if (dependant_stage->depth_desc_.depth_texture_ident_ == texture_ident) {
				number_of_dependancies++;
			}
		}
	}

	return number_of_dependancies;
}

void PipelineTexturePlanner::ParsePipelineStageDescs(const vector<TextureIdent>& existing_textures, vector<unique_ptr<PipelineStageDesc>>& stages) {
	// Add the existing textures to the references kept by the planner
	assert(existing_textures.size() == texture_storage_.size());
	for (int i = 0; i < existing_textures.size(); i++) {
		D3D11_TEXTURE2D_DESC existing_texture_desc;
		texture_storage_[i].GetTexture()->GetDesc(&existing_texture_desc);
		TextureSignature existing_signature(existing_texture_desc);
		int group_number = LookupBlockGroupNumber(existing_signature);
		texture_blocks_[group_number].push_back({ -1, i });
		texture_block_lookup_[existing_textures[i]] = make_tuple(group_number, texture_blocks_[group_number].size() - 1);
	}

	for (auto stage_iter = stages.begin(); stage_iter != stages.end(); stage_iter++) {
		for (BasePipelineStageDesc* stage : (*stage_iter)->GetBaseStages()) {
			for (const tuple<TextureIdent, TextureSignature>& output_texture_desc : stage->output_textures_descs_) {
				auto existing_texture = texture_block_lookup_.find(get<0>(output_texture_desc));
				if (existing_texture != texture_block_lookup_.end()) {
					stage->output_textures_.push_back((Texture*)GetTextureBlock(existing_texture->second).texture_);
				}
				else {
					int number_of_dependancies = CountDependanciesOnTexture(get<0>(output_texture_desc), stage_iter+1, stages.end(), 1);

					stage->output_textures_.push_back((Texture*)ReserveTexture(get<1>(output_texture_desc), get<0>(output_texture_desc), TextureUsage::Rendering(), number_of_dependancies));
				}
			}

			for (const tuple<TextureIdent, TextureSignature>& output_texture_desc : stage->output_textures_descs_) {
				TextureBlock& output_block = GetTextureBlock(texture_block_lookup_[get<0>(output_texture_desc)]);
				output_block.active_references_--;
			}

			stage->input_textures_.resize(stage->input_textures_idents_.size());
			for (int i = 0; i < stage->input_textures_idents_.size(); i++) {
				TextureBlock& input_block = GetTextureBlock(texture_block_lookup_[stage->input_textures_idents_[i]]);
				input_block.active_references_--;
				stage->input_textures_[i] = (Texture*)input_block.texture_;
			}

			if (stage->depth_desc_.depth_texture_ident_ != "") {
				auto existing_texture = texture_block_lookup_.find(stage->depth_desc_.depth_texture_ident_);
				int texture_index;
				if (existing_texture != texture_block_lookup_.end()) {
					texture_index = GetTextureBlock(existing_texture->second).texture_;
				}
				else {
					int number_of_dependancies = CountDependanciesOnTexture(stage->depth_desc_.depth_texture_ident_, stage_iter + 1, stages.end(), 1);
					D3D11_TEXTURE2D_DESC depth_stencil_desc;
					depth_stencil_desc.Width = get<1>(stage->output_textures_descs_[0]).texture_desc_.Width;
					depth_stencil_desc.Height = get<1>(stage->output_textures_descs_[0]).texture_desc_.Height;
					depth_stencil_desc.MipLevels = 1;
					depth_stencil_desc.ArraySize = 1;
					depth_stencil_desc.Format = DXGI_FORMAT_R32_TYPELESS;
					depth_stencil_desc.SampleDesc = { 1, 0 };
					depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
					depth_stencil_desc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
					depth_stencil_desc.CPUAccessFlags = 0;
					depth_stencil_desc.MiscFlags = 0;
					texture_index = ReserveDepthBufferTexture(depth_stencil_desc, stage->depth_desc_.depth_texture_ident_, number_of_dependancies);
				}

				Texture& depth_stencil_texture = texture_storage_[texture_index];
				stage->depth_stencil_view_ = depth_stencil_texture.GetDepthStencilView();
				device_->CreateDepthStencilState(&stage->depth_desc_.depth_stencil_state_desc_, &stage->depth_stencil_state_);

				TextureBlock& input_block = GetTextureBlock(texture_block_lookup_[stage->depth_desc_.depth_texture_ident_]);
				input_block.active_references_--;
			}
		}
	}

	// Turn the Texture* value that were an offset into the texture storage into actual texture pointer values
	for (const unique_ptr<PipelineStageDesc>& stage : stages) {
		for (BasePipelineStageDesc* base_stage : stage->GetBaseStages()) {
			for (int i = 0; i < base_stage->output_textures_.size(); i++) {
				base_stage->output_textures_[i] = &texture_storage_[(int)base_stage->output_textures_[i]];
			}
			for (int i = 0; i < base_stage->input_textures_idents_.size(); i++) {
				base_stage->input_textures_[i] = &texture_storage_[(int)base_stage->input_textures_[i]];
				base_stage->input_texture_lookup_[base_stage->input_textures_idents_[i]] = i;
			}
		}
	}

	// Allow stages to allocate any additional resources they may need
	for (const unique_ptr<PipelineStageDesc>& stage : stages) {
		stage->AllocateAdditionalResources(*this);
	}

	entity_handler_.SetEntitySets(entity_group_associations_);
	entity_handler_.SetCameras(camera_map_);
}

int PipelineTexturePlanner::LookupBlockGroupNumber(const TextureSignature& signature) {
	map<TextureSignature, int>::iterator existing_group = group_number_lookup_.find(signature);
	int group_number;
	if (existing_group == group_number_lookup_.end()) {
		group_number = texture_blocks_.size();
		group_number_lookup_.emplace(signature, group_number);
		texture_blocks_.emplace_back();
	}
	else {
		group_number = existing_group->second;
	}

	return group_number;
}

int PipelineTexturePlanner::ReserveTexture(const TextureSignature& signature, const TextureIdent& ident, TextureUsage usage, int number_of_references) {
	int group_number = LookupBlockGroupNumber(signature);
	vector<TextureBlock>& block_group = texture_blocks_[group_number];

	int block_number;
	for (block_number = 0; block_number < block_group.size(); block_number++) {
		TextureBlock* current_block = &block_group[block_number];
		if (current_block->active_references_ == 0) {
			break;
		}
	}
	if (block_number == block_group.size()) {
		block_group.emplace_back();
		block_group[block_number].texture_ = AddTextureToStorage(signature, usage);
	}

	block_group[block_number].active_references_ = number_of_references;
	texture_block_lookup_[ident] = make_tuple(group_number, block_number);
	
	return block_group[block_number].texture_;
}

int PipelineTexturePlanner::AddTextureToStorage(const TextureSignature& signature, TextureUsage usage) {
	texture_storage_.emplace_back(usage);
	texture_storage_.back().Initialize(device_, device_context_,
		{ { static_cast<int>(signature.texture_desc_.Width), static_cast<int>(signature.texture_desc_.Height) } },
		&signature.texture_desc_);
	return texture_storage_.size() - 1;
}

int PipelineTexturePlanner::ReserveDepthBufferTexture(const TextureSignature& signature_with_size, const TextureIdent& ident, int number_of_references) {
	TextureSignature signature = signature_with_size;
	//TextureSignature signature = DepthTexture::GetDefaultDepthTextureDesc();
	//signature.texture_desc_.Width = signature_with_size.texture_desc_.Width;
	//signature.texture_desc_.Height = signature_with_size.texture_desc_.Height;
	return ReserveTexture(signature, ident, TextureUsage::DepthStencil(), number_of_references);
}

TextureBlock& PipelineTexturePlanner::GetTextureBlock(const tuple<int, int>& index) {
	return texture_blocks_[get<0>(index)][get<1>(index)];
}