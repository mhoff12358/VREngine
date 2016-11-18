#include "stdafx.h"
#include "ResourceDefinitions.h"

namespace game_scene {

NewShaderDetails::NewShaderDetails() {
	for (size_t i = 0; i < idents_.size(); i++) {
		idents_[i] = ShaderIdentifier();
	}
}

NewShaderDetails::NewShaderDetails(const vector<ShaderIdentifier>& idents) : NewShaderDetails() {
	for (const ShaderIdentifier& ident : idents) {
		size_t stage_number = ident.GetShaderStage().ToStageNumber();
		if (!idents_[stage_number].IsEmpty()) {
			std::cout << "Doubly defining a shader stage" << std::endl;
		}
		idents_[stage_number] = ident;
	}
}

ShaderStages NewShaderDetails::GetShaderStages() const {
	vector<ShaderStage> stages;
	for (const ShaderIdentifier& ident : idents_) {
		stages.push_back(ident.GetShaderStage());
	}
	return ShaderStages(stages);
}

}  // game_scene