#include "stdafx.h"
#include "LightInterface.h"

#include "StlHelper.h"
#include "VRBackend/Lights.h"

void LightInterface() {
	CreateArray<PointLight, LightSystem::num_point_lights>("PointLight");
	CreateArray<DirectionalLight, LightSystem::num_directional_lights>("DirectionalLight");

	class_<Color>("Color", init<float, float, float, float>())
		.def(init<float, float, float>())
		.def(init<array<float, 3>>())
		.def(init<array<float, 4>>());

	class_<AmbientLight, boost::noncopyable>("AmbientLight", no_init)
		.def_readwrite("color", &AmbientLight::color_);

	class_<PointLight, boost::noncopyable>("PointLight", no_init)
		.def_readwrite("location", &PointLight::location_)
		.def_readwrite("color", &PointLight::color_);

	class_<DirectionalLight, boost::noncopyable>("DirectionalLight", no_init)
		.def_readwrite("direction", &DirectionalLight::direction_)
		.def_readwrite("color", &DirectionalLight::color_);

	class_<LightSystem, boost::noncopyable>("LightSystem", no_init)
		.def("MutableAmbientLight", &LightSystem::MutableAmbientLight, return_value_policy<reference_existing_object>())
		.def("MutablePointLights", &LightSystem::MutablePointLights, return_value_policy<reference_existing_object>())
		.def("MutableDirectionalLights", &LightSystem::MutableDirectionalLights, return_value_policy<reference_existing_object>());
}
