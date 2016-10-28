#pragma once

#include "stdafx.h"
#include "PipelineStage.h"

class TextureCopyDesc;
class RenderGroup;
class Texture;

class TextureCopy : public PipelineStage
{
public:
	TextureCopy(ID3D11Device* dev, ID3D11DeviceContext* dev_con, const TextureCopyDesc& description);
	~TextureCopy();

	void Apply(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw) override;

private:
	Texture* input_texture_;
	Texture* output_texture_;
};

