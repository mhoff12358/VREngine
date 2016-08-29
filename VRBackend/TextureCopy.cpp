#include "TextureCopy.h"


TextureCopy::TextureCopy(ID3D11Device* dev, ID3D11DeviceContext* dev_con, const TextureCopyDesc& description)
	: PipelineStage(description)
{
	assert(description.type_ == PST_TEXTURE_COPY);
	assert(description.input_textures_.size() == 1);
	assert(description.output_textures_.size() == 1);
	input_texture_ = description.input_textures_.front();
	output_texture_ = description.output_textures_[0];
}


TextureCopy::~TextureCopy()
{
}


void TextureCopy::Apply(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw) {
	output_texture_->CopyFrom(*input_texture_);
}