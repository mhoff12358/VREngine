#include "ProcessingEffect.h"
#include "ObjLoader.h"

VertexType ProcessingEffect::squares_vertex_type = VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC>({
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
}));

Entity ProcessingEffect::default_screen_filling_entity;

ProcessingEffect::ProcessingEffect(ID3D11Device* dev, ID3D11DeviceContext* dev_con, const ProcessingEffectDesc& description)
	: PipelineStageWithRenderMode(description), input_textures_()
{
	assert(description.type_ == PST_PROCESSING_EFFECT);

	num_input_textures_ = description.input_textures_.size();
	input_textures_ = new TextureView[num_input_textures_];
	for (int i = 0; i < num_input_textures_; i++) {
		input_textures_[i] = TextureView(i, i, false, true, description.input_textures_[i]->GetShaderResourceView(), description.input_textures_[i]->GetSampler());
	}

	screen_filling_entity_ = default_screen_filling_entity;
	screen_filling_entity_.SetPixelShader(description.pixel_shader_);
	screen_filling_entity_.SetVertexShader(description.vertex_shader_);
	ConstantBuffer* settings_constant_buffer = NULL;
	if (description.settings_buffer_ != NULL) {
		settings_constant_buffer = new SizedConstantBuffer(CB_PS_PIXEL_SHADER, description.settings_buffer_size_);
		settings_constant_buffer->CreateBuffer(dev);
		memcpy(settings_constant_buffer->EditBufferData(), description.settings_buffer_, description.settings_buffer_size_);
	}
	screen_filling_entity_.MutableShaderSettings().constant_buffer = settings_constant_buffer;

	InitializeRenderMode(dev, dev_con, description.output_textures_, true, description.blend_desc_, NULL, NULL);
}

ProcessingEffect::~ProcessingEffect()
{
}

void ProcessingEffect::Apply(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw) {
	PreApply();
	render_mode_.Prepare();
	for (int i = 0; i < num_input_textures_; i++) {
		input_textures_[i].Prepare(dev, dev_con);
	}
	screen_filling_entity_.Draw(dev, dev_con);
	for (int i = 0; i < num_input_textures_; i++) {
		input_textures_[i].UnPrepare(dev, dev_con);
	}
	render_mode_.UnPrepare();
}

void ProcessingEffect::CreateProcessingEffectResources(ResourcePool* resource_pool) {
	default_screen_filling_entity = Entity(ES_NORMAL, Shaders(), ShaderSettings(), CreateScreenFillingModel(resource_pool), NULL);
}

Model ProcessingEffect::CreateScreenFillingModel(ResourcePool* resource_pool) {
	std::vector<Vertex> model_vertices;
	model_vertices.push_back(Vertex(ObjLoader::vertex_type_texture, { -1, -1, 0, 1 }));
	model_vertices.push_back(Vertex(ObjLoader::vertex_type_texture, { -1, 1, 0, 0 }));
	model_vertices.push_back(Vertex(ObjLoader::vertex_type_texture, { 1, -1, 1, 1 }));
	model_vertices.push_back(Vertex(ObjLoader::vertex_type_texture, { 1, -1, 1, 1 }));
	model_vertices.push_back(Vertex(ObjLoader::vertex_type_texture, { -1, 1, 0, 0 }));
	model_vertices.push_back(Vertex(ObjLoader::vertex_type_texture, { 1, 1, 1, 0 }));

	return resource_pool->LoadModel("ScreenFillingModel", model_vertices, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, {true, false, false});
}