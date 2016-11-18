// WandDemo.cpp : Defines the entry point for the console application.
//

#include <memory>
using std::unique_ptr;
#include <chrono>

#include "stdafx.h"

#include "Kinect.h"

#include "VRBackend/BeginDirectx.h"
#include "VRBackend/TimeTracker.h"
#include "VRBackend/TextureView.h"
#include "VRBackend/ViewState.h"
#include "VRBackend/RenderGroup.h"
#include "VRBackend/RenderingPipeline.h"
#include "VRBackend/PipelineStageDesc.h"
#include "VRBackend/InputHandler.h"
#include "VRBackend/EntityHandler.h"
#include "VRBackend/Headset.h"

#include "VRBackend/gaussian.h"
#include "VRBackend/BlendDesc.h"

#include "SceneSystem/Component.h"
#include "SceneSystem/Scene.h"
#include "SceneSystem/GraphicsResources.h"
#include "SceneSystem/InputCommandArgs.h"
#include "SceneSystem/GraphicsObject.h"
#include "SceneSystem/HeadsetInterface.h"
#include "SceneSystem/KinectInterface.h"
#include "SceneSystem/IOInterface.h"
#include "NichijouGraph.h"
#include "SceneSystem/Sprite.h"
#include "SceneSystem/NewGraphicsObject.h"
#include "SceneSystem/Scene.h"
#include "SceneSystem/GrabbableObjectHandler.h"

#include "SceneSystem/BoostPythonWrapper.h"

#include "boost/numeric/ublas/matrix.hpp"

#include "openvr.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

using std::chrono::high_resolution_clock;

VRBackendBasics graphics_objects;
const float movement_scale = 0.0005f;
const float mouse_theta_scale = 0.001f;
const float mouse_phi_scale = 0.001f;

mutex device_context_access;

void GraphicsLoop() {
	int frame_index = 0;

	int prev_sec = timeGetTime();
	int fps = 0;

	float old_x = 1;
	bool is_decreasing = false;

	while (true) {
		graphics_objects.input_handler->UpdateHeadset(frame_index);
		unique_lock<mutex> device_context_lock(device_context_access);

		RenderGroup* drawing_groups = graphics_objects.entity_handler->GetRenderGroupForDrawing();
		for (unsigned int render_group_number = 0; render_group_number < graphics_objects.entity_handler->GetNumEntitySets(); render_group_number++) {
			drawing_groups[render_group_number].ApplyMutations(
				graphics_objects.view_state->device_interface, graphics_objects.view_state->device_context, *graphics_objects.resource_pool);
		}

		if (graphics_objects.oculus->IsHeadsetInitialized()) {
			for (vr::EVREye eye : Headset::eyes_) {
				Pose eye_pose = graphics_objects.oculus->GetEyePose(eye);
				PipelineCamera& camera = drawing_groups->cameras[graphics_objects.entity_handler->GetCameraIndex("player_head|" + std::to_string(eye))];
				camera.SetLocation(eye_pose.location_);
				camera.SetOrientation(eye_pose.orientation_);
				camera.BuildMatrices();
			}
		}

		graphics_objects.render_pipeline->Render(drawing_groups);
		frame_index++;
	}
}

void UpdateLoop() {
	int prev_time = timeGetTime();

	// Stored in theta, phi format, theta kept in [0, 2*pi], phi kept in [-pi, pi]
	array<float, 2> player_orientation_angles = { { 0, 0 } };

	Py_Initialize();

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = graphics_objects.view_state->window_handler;
	if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) {
		std::cout << "Error registering raw input device" << std::endl;
	}

	game_scene::actors::Sprite::Init();
	ModelGenerator point_gen(VertexType::vertex_type_location, D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	point_gen.AddVertex(Vertex(VertexType::vertex_type_location, { 0.0f, 0.0f, 0.0f }));
	point_gen.Finalize(graphics_objects.view_state->device_interface, optional<EntityHandler&>{}, ModelStorageDescription::Immutable());
	point_gen.parts_ = { { "Point", ModelSlice(point_gen.GetCurrentNumberOfVertices(), 0) } };
	graphics_objects.resource_pool->PreloadResource(ResourceIdent(ResourceIdent::MODEL, ResourceIdentifier::GetConstantModelName("point"), point_gen));

	game_scene::Scene scene;
	game_scene::ActorId controls_registry = scene.RegisterByName("ControlsRegistry", scene.AddActorGroup());
	game_scene::ActorId tick_registry = scene.RegisterByName("TickRegistry", scene.AddActorGroup());;
	unique_ptr<game_scene::Shmactor> new_actor = 
			make_unique<game_scene::actors::GraphicsResources>(
				*graphics_objects.resource_pool,
				*graphics_objects.entity_handler,
				graphics_objects.view_state->device_interface);
	game_scene::ActorId graphics_resources = scene.RegisterByName(
		"GraphicsResources",
		scene.AddActor(move(new_actor)));
	game_scene::ActorId io_interface = scene.RegisterByName(
		"IOInterface",
		scene.AddActor(
			make_unique<game_scene::actors::IOInterface>(*graphics_objects.input_handler)));
	scene.AddActorToGroup(io_interface, tick_registry);
	if (graphics_objects.oculus->IsHeadsetInitialized()) {
		game_scene::ActorId headset_interface = scene.RegisterByName(
			"HeadsetInterface",
			scene.AddActor(
				make_unique<game_scene::actors::HeadsetInterface>(*graphics_objects.oculus)));
		scene.AddActorToGroup(headset_interface, tick_registry);
	}
	if (graphics_objects.oculus->IsKinectInitialized()) {
		game_scene::ActorId kinect_interface = scene.RegisterByName(
			"KinectInterface",
			scene.AddActor(
				make_unique<game_scene::actors::KinectInterface>(*graphics_objects.oculus)));
		scene.AddActorToGroup(kinect_interface, tick_registry);
	}
	game_scene::ActorId grabbable_object_handler = scene.RegisterByName(
		"GrabbableObjectHandler",
		scene.AddActor(
			make_unique<game_scene::GrabbableObjectHandler>()
		)
	);

	scene.FlushCommandQueue();

	game_scene::ActorId added_actor;
	try {
		object main_namespace = import("__main__").attr("__dict__");
		exec("import scripts.first_load as first_load", main_namespace);
		object loaded_module = main_namespace["first_load"];

		dict inputs;
		inputs["scene"] = boost::ref(scene);
		inputs["command_registry"] = boost::ref(game_scene::CommandRegistry::GetRegistry());
		inputs["query_registry"] = boost::ref(game_scene::QueryRegistry::GetRegistry());
		object result = loaded_module.attr("first_load")(inputs);
	} catch (error_already_set) {
		PyErr_Print();
	}

	MSG msg;
	vr::VREvent_t vr_msg;

	bool body_found = false;

	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				break;
			}
		}

		if (graphics_objects.oculus->IsHeadsetInitialized()) {
			while (graphics_objects.oculus->ProcessAndReturnLatestEvent(&vr_msg)) {
				switch (vr_msg.eventType) {
				case vr::VREvent_InputFocusCaptured:
					std::cout << "Input focus captured" << std::endl;
					break;
				case vr::VREvent_TrackedDeviceActivated:
					graphics_objects.oculus->RegisterTrackedObject(vr_msg.trackedDeviceIndex);
					break;
				case vr::VREvent_TrackedDeviceDeactivated:
					graphics_objects.oculus->UnregisterTrackedObject(vr_msg.trackedDeviceIndex);
					break;
				default:
					break;
				}
			}
		}
		graphics_objects.oculus->UpdateGamePoses();

		int new_time = timeGetTime();
		int time_delta_ms = new_time - prev_time;
		prev_time = new_time;

		// Scene logic
		game_scene::CommandQueueLocation next_command = scene.MakeCommandAfter(
			scene.FrontOfCommands(),
			game_scene::Command(
				game_scene::Target(tick_registry),
				make_unique<game_scene::commands::TimeTick>(time_delta_ms)));
		next_command = scene.MakeCommandAfter(
			next_command,
			game_scene::Command(
				game_scene::Target(controls_registry),
				make_unique<game_scene::commands::InputUpdate>(
					graphics_objects.input_handler)));
		scene.FlushCommandQueue();

		graphics_objects.entity_handler->BuildCameras();

		graphics_objects.entity_handler->FinishUpdate();
	}

	// clean up DirectX and COM
	graphics_objects.view_state->Cleanup();
	if (graphics_objects.input_handler->IsHeadsetActive()) {
		graphics_objects.oculus->Cleanup();
	}
}

#include "VRBackend/PipelineCamera.h"
#include "VRBackend/PipelineStage.h"
#include "VRBackend/RenderEntities.h"

int _tmain(int argc, _TCHAR* argv[])
{
	bool hmd_active = false;
	bool hmd_desired = false;
	bool hmd_found = vr::VR_IsHmdPresent();
	vr::IVRSystem* headset_system = nullptr;
	if (hmd_desired && hmd_found && vr::VR_IsRuntimeInstalled()) {
		vr::EVRInitError eError = vr::VRInitError_None;
		headset_system = vr::VR_Init( &eError, vr::VRApplication_Scene );
		hmd_active = true;
	}

	bool kinect_desired = false;
	IKinectSensor* kinect_sensor = nullptr;
	if (kinect_desired) {
		HRESULT hr;
		hr = GetDefaultKinectSensor(&kinect_sensor);
		if (SUCCEEDED(hr)) {
			hr = kinect_sensor->Open();
		}
		if (FAILED(hr)) {
			kinect_sensor = nullptr;
		}
	}

	graphics_objects = BeginDirectx(headset_system, kinect_sensor, "");

	int stage_repetition = hmd_active ? 2 : 0;

	string single_camera_name = "player_head";
	if (hmd_active) {
		single_camera_name += "|0";
	}

	BlendDesc::Init();
	TextureSignature back_buffer_signature(*graphics_objects.render_pipeline->GetStagingBufferDesc());
	vector<unique_ptr<PipelineStageDesc>> pipeline_stages;
	pipeline_stages.emplace_back(new RenderEntitiesDesc("skybox", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription::Empty(), {}, BlendDesc::no_alpha_blend_state_desc, PipelineCameraIdent(single_camera_name)));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("basic", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription("normal_depth", BlendDesc::keep_nearer_depth_test), {}, BlendDesc::no_alpha_blend_state_desc, PipelineCameraIdent("player_head"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("terrain", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription("normal_depth", BlendDesc::keep_nearer_depth_test), {}, BlendDesc::no_alpha_blend_state_desc, PipelineCameraIdent("player_head"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("bloom", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature), std::make_tuple("bloom", back_buffer_signature) }, DepthStencilDescription("normal_depth", BlendDesc::keep_nearer_depth_test), {}, BlendDesc::keep_new_alpha_blend_state_desc, PipelineCameraIdent("player_head"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("alpha", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription("normal_depth", BlendDesc::keep_nearer_depth_test), {}, BlendDesc::drop_alpha_with_alpha_blend_state_desc, PipelineCameraIdent("player_head"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<TextureCopyDesc>(stage_repetition, TextureCopyDesc("move_to_back", { std::make_tuple("back_buffer", back_buffer_signature) }, { "objects" })));
	if (hmd_active) {
		pipeline_stages.emplace_back(new TextureCopyDesc("move_to_back", { std::make_tuple("back_buffer|2", back_buffer_signature) }, { "objects|0" }));
	}

	graphics_objects.render_pipeline->SetPipelineStages(*graphics_objects.resource_pool, pipeline_stages);

	if (hmd_active) {
		PipelineCamera& camera0 = graphics_objects.entity_handler->MutableCamera("player_head|0");
		PipelineCamera& camera1 = graphics_objects.entity_handler->MutableCamera("player_head|1");
		camera0.SetRawProjection(graphics_objects.oculus->GetEyeProjectionMatrix(Headset::eyes_[0], 0.001f, 500.0f));
		camera0.SetPose(Pose(Location(), Quaternion::Identity()));
		camera0.BuildMatrices();
		camera1.SetRawProjection(graphics_objects.oculus->GetEyeProjectionMatrix(Headset::eyes_[1], 0.001f, 500.0f));
		camera1.SetPose(Pose(Location(), Quaternion::Identity()));
		camera1.BuildMatrices();
	}
	else {
		PipelineCamera& camera = graphics_objects.entity_handler->MutableCamera("player_head");
		camera.SetPerspectiveProjection(
			60.0f / 180.0f*3.1415f,
			((float)graphics_objects.view_state->window_details.screen_size[0]) / graphics_objects.view_state->window_details.screen_size[1],
			0.001f,
			500.0f);
		camera.SetPose(Pose(Location(), Quaternion::Identity()));
		camera.BuildMatrices();
	}
	graphics_objects.entity_handler->FinishUpdate();

	TimeTracker::PreparePerformanceCounter();
	TimeTracker::active_track = TimeTracker::NUM_TRACKS;
	TimeTracker::track_time = true;

	graphics_objects.input_handler->SetUsePredictiveTiming(true);
	graphics_objects.input_handler->SetPredictiveTiming(-5);

	thread graphics_thread(GraphicsLoop);
	UpdateLoop();

	return 0;
}

