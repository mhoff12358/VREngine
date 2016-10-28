
#pragma once

#include "stdafx.h"

namespace vr {
	class IVRSystem;
}
class IKinectSensor;
class EntityHandler;
class Headset;
class InputHandler;
class RenderingPipeline;
class ResourcePool;
class ViewState;

typedef struct {
	Headset* oculus;
	InputHandler* input_handler;
	RenderingPipeline* render_pipeline;
	ViewState* view_state;
	ResourcePool* resource_pool;
	EntityHandler* entity_handler;
} VRBackendBasics;

VRBackendBasics BeginDirectx(vr::IVRSystem* headset_system, IKinectSensor* kinect_sensor, std::string resource_location);