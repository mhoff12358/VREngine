
#pragma once

#include <string>
#include <iostream>

#include "openvr.h"
#include "Kinect.h"

#include "Headset.h"
#include "RenderingPipeline.h"
#include "InputHandler.h"
#include "ViewState.h"
#include "EntityHandler.h"
#include "PipelineStageDesc.h"

typedef struct {
	Headset* oculus;
	InputHandler* input_handler;
	RenderingPipeline* render_pipeline;
	ViewState* view_state;
	ResourcePool* resource_pool;
	EntityHandler* entity_handler;
	PerspectiveCamera* player_camera;
} VRBackendBasics;

VRBackendBasics BeginDirectx(vr::IVRSystem* headset_system, IKinectSensor* kinect_sensor, std::string resource_location);