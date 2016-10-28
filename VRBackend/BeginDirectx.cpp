#include "BeginDirectx.h"

VRBackendBasics BeginDirectx(vr::IVRSystem* headset_system, IKinectSensor* kinect_sensor, std::string resource_location) {
	VRBackendBasics graphics_objects;

	graphics_objects.oculus = new Headset;
	graphics_objects.input_handler = new InputHandler;
	if (headset_system != nullptr) {
		array<unsigned int, 2> viewport;
		headset_system->GetRecommendedRenderTargetSize(&viewport[0], &viewport[1]);
		graphics_objects.view_state = new ViewState({static_cast<int>(viewport[0]), static_cast<int>(viewport[1])});
	} else {
		graphics_objects.view_state = new ViewState;
	}
	graphics_objects.entity_handler = new EntityHandler;
	graphics_objects.resource_pool = new ResourcePool(*graphics_objects.entity_handler);
	graphics_objects.player_camera = new PerspectiveCamera;


	graphics_objects.input_handler->Initialize();
	graphics_objects.view_state->Initialize(graphics_objects.input_handler, GetModuleHandle(NULL), SW_SHOW);
	graphics_objects.resource_pool->Initialize(graphics_objects.view_state->device_interface, graphics_objects.view_state->device_context);
	graphics_objects.entity_handler->Initialize(graphics_objects.resource_pool);
	ProcessingEffect::CreateProcessingEffectResources(graphics_objects.resource_pool);

	// Initialize the oculus resources and inject them
	graphics_objects.oculus->Initialize(headset_system, kinect_sensor);
	graphics_objects.input_handler->SetHeadsetState(graphics_objects.oculus);
	if (headset_system != nullptr) {
		graphics_objects.render_pipeline = new ToHeadsetRenderingPipeline();
		dynamic_cast<ToHeadsetRenderingPipeline*>(graphics_objects.render_pipeline)->Initialize(graphics_objects.view_state, graphics_objects.input_handler, graphics_objects.entity_handler, resource_location, graphics_objects.oculus);
	}
	else {
		graphics_objects.render_pipeline = new ToScreenRenderingPipeline();
		dynamic_cast<ToScreenRenderingPipeline*>(graphics_objects.render_pipeline)->Initialize(graphics_objects.view_state, graphics_objects.input_handler, graphics_objects.entity_handler, resource_location);
	}

	graphics_objects.player_camera->vertical_fov = 120.0f / 180.0f * 3.1415f;
	graphics_objects.player_camera->aspect_ratio = ((float)graphics_objects.view_state->window_details.screen_size[0]) / graphics_objects.view_state->window_details.screen_size[1];
	
	return graphics_objects;
}
