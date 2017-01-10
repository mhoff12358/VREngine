// WandDemo.cpp : Defines the entry point for the console application.
//

#include <memory>
using std::unique_ptr;
#include <chrono>

#include "stdafx.h"

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
#include "VRBackend/Lights.h"
#include "VRBackend/PipelineCamera.h"
#include "VRBackend/gaussian.h"
#include "VRBackend/BlendDesc.h"
#include "VRBackend/ProcessingEffect.h"

#include "SceneSystem/Component.h"
#include "SceneSystem/Scene.h"
#include "SceneSystem/GraphicsResources.h"
#include "SceneSystem/InputCommandArgs.h"
#include "SceneSystem/GraphicsObject.h"
#include "SceneSystem/HeadsetInterface.h"
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

object main_namespace;
object loaded_module;

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

void PreUpdateInitialize() {
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

}

enum class UpdateLoopResult {
	CONTINUE,
	FINISH,
	RELOAD
};

UpdateLoopResult UpdateLoop() {
	int prev_time = timeGetTime();

	game_scene::Scene scene;
	game_scene::ActorId controls_registry = scene.RegisterByName("ControlsRegistry", scene.AddActorGroup());
	game_scene::ActorId tick_registry = scene.RegisterByName("TickRegistry", scene.AddActorGroup());;
	unique_ptr<game_scene::Actor> new_actor = 
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
	game_scene::ActorId grabbable_object_handler = scene.RegisterByName(
		"GrabbableObjectHandler",
		scene.AddActor(
			make_unique<game_scene::GrabbableObjectHandler>()
		)
	);

	//scene.PrefaceCommand();
	try {
		dict inputs;
		inputs["scene"] = boost::ref(scene);
		inputs["command_registry"] = boost::ref(game_scene::CommandRegistry::GetRegistry());
		inputs["query_registry"] = boost::ref(game_scene::QueryRegistry::GetRegistry());
		inputs["is_vr"] = graphics_objects.oculus->IsHeadsetInitialized();
		object result = loaded_module.attr("first_load")(inputs);
	} catch (error_already_set) {
		PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
		PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
		try {
			boost::python::handle<> h_tb(traceback_ptr);
			object pdb(import("pdb"));
			object pdb_fn = pdb.attr("post_mortem");
			pdb_fn(h_tb);
		} catch (error_already_set) {
			PyErr_Print();
		}
	}
	scene.FlushCommandQueue();

	MSG msg;
	vr::VREvent_t vr_msg;
	UpdateLoopResult response_code = UpdateLoopResult::CONTINUE;

	while (true)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				response_code = UpdateLoopResult::FINISH;
			} else if ((msg.message == WM_KEYDOWN) && (msg.wParam == VK_ESCAPE)) {
				response_code = UpdateLoopResult::RELOAD;
			}
		}
		if (response_code != UpdateLoopResult::CONTINUE) {
			break;
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

	// Push the updated state to the entity handler and wait for the new state
	// to begin being rendered.
	graphics_objects.entity_handler->FinishUpdate();
	graphics_objects.entity_handler->CycleGraphics();


	return response_code;
}

int _tmain(int argc, _TCHAR* argv[])
{
	bool hmd_desired = false;

	Py_Initialize();

	try {
		main_namespace = import("__main__").attr("__dict__");
		exec("import first_load", main_namespace);
		loaded_module = main_namespace["first_load"];

		object result = loaded_module.attr("pre_load")();
		if (result["load_vr"]) {
			hmd_desired = true;
		}
	} catch (error_already_set) {
		PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
		PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
		try {
			boost::python::handle<> h_tb(traceback_ptr);
			object pdb(import("pdb"));
			object pdb_fn = pdb.attr("post_mortem");
			pdb_fn(h_tb);
		} catch (error_already_set) {
			PyErr_Print();
		}
	}

	bool hmd_active = false;
	bool hmd_found = vr::VR_IsHmdPresent();
	vr::IVRSystem* headset_system = nullptr;
	if (hmd_desired && hmd_found && vr::VR_IsRuntimeInstalled()) {
		vr::EVRInitError eError = vr::VRInitError_None;
		headset_system = vr::VR_Init( &eError, vr::VRApplication_Scene );
		hmd_active = true;
	}

	graphics_objects = BeginDirectx(headset_system, "");

	int stage_repetition = hmd_active ? 2 : 0;

	string single_camera_name = "player_head";
	if (hmd_active) {
		single_camera_name += "|0";
	}

	auto bloom_horiz_kernel = FillHLSLKernel<51>(Generate1DGausianWithPeak<51>(1, 0, 12));
	bloom_horiz_kernel[0] = graphics_objects.render_pipeline->GetStageBufferSize()[0];
	auto bloom_vert_kernel = FillHLSLKernel<51>(Generate1DGausianWithPeak<51>(1, 0, 12));
	bloom_vert_kernel[0] = graphics_objects.render_pipeline->GetStageBufferSize()[1];

	array<float, 4> clear_black = { 0, 0, 0, 0 };

	BlendDesc::Init();
	TextureSignature back_buffer_signature(*graphics_objects.render_pipeline->GetStagingBufferDesc());
	vector<unique_ptr<PipelineStageDesc>> pipeline_stages;
	pipeline_stages.emplace_back(new ProcessingEffectDesc("clear_bloom", { std::make_tuple("bloom", back_buffer_signature) }, { }, BlendDesc::keep_new_alpha_blend_state_desc, graphics_objects.resource_pool->LoadPixelShader("set_color.hlsl"), graphics_objects.resource_pool->LoadVertexShader("set_color.hlsl", ProcessingEffect::squares_vertex_type), (char*)&clear_black, sizeof(clear_black)));
	pipeline_stages.emplace_back(new RenderEntitiesDesc("skybox", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription::Empty(), {}, BlendDesc::no_alpha_blend_state_desc, PipelineCameraIdent(single_camera_name), LightingSystemIdent("cockpit_lights")));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("basic", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription("normal_depth", BlendDesc::keep_nearer_depth_test), {}, BlendDesc::no_alpha_blend_state_desc, PipelineCameraIdent("player_head"), LightingSystemIdent("cockpit_lights"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("terrain", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription("normal_depth", BlendDesc::keep_nearer_depth_test), {}, BlendDesc::no_alpha_blend_state_desc, PipelineCameraIdent("player_head"), LightingSystemIdent("cockpit_lights"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("bloom", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature), std::make_tuple("bloom", back_buffer_signature) }, DepthStencilDescription("normal_depth", BlendDesc::keep_nearer_depth_test), {}, BlendDesc::keep_new_alpha_blend_state_desc, PipelineCameraIdent("player_head"), LightingSystemIdent("cockpit_lights"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("alpha", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription("normal_depth", BlendDesc::keep_nearer_depth_test), {}, BlendDesc::keep_new_alpha_with_alpha_blend_state_desc, PipelineCameraIdent("player_head"), LightingSystemIdent("cockpit_lights"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("bloom_alpha", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature), std::make_tuple("bloom", back_buffer_signature) }, DepthStencilDescription("normal_depth", BlendDesc::keep_nearer_depth_test), {}, BlendDesc::keep_new_alpha_with_alpha_blend_state_desc, PipelineCameraIdent("player_head"), LightingSystemIdent("cockpit_lights"))));

	pipeline_stages.emplace_back(new ProcessingEffectDesc("horiz_bloom_applied", { std::make_tuple("horiz_bloom", back_buffer_signature) }, { "bloom" }, BlendDesc::keep_new_alpha_blend_state_desc, graphics_objects.resource_pool->LoadPixelShader("bloom_horiz.hlsl"), graphics_objects.resource_pool->LoadVertexShader("bloom_horiz.hlsl", ProcessingEffect::squares_vertex_type), (char*)&bloom_horiz_kernel, sizeof(bloom_horiz_kernel)));
	pipeline_stages.emplace_back(new ProcessingEffectDesc("vert_bloom_applied", { std::make_tuple("vert_bloom", back_buffer_signature) }, { "horiz_bloom" }, BlendDesc::keep_new_alpha_blend_state_desc, graphics_objects.resource_pool->LoadPixelShader("bloom_vert.hlsl"), graphics_objects.resource_pool->LoadVertexShader("bloom_vert.hlsl", ProcessingEffect::squares_vertex_type), (char*)&bloom_vert_kernel, sizeof(bloom_vert_kernel)));
	pipeline_stages.emplace_back(new ProcessingEffectDesc("blur_sum", { std::make_tuple("objects", back_buffer_signature) }, { "vert_bloom" }, BlendDesc::additative_for_all_blend_state_desc, graphics_objects.resource_pool->LoadPixelShader("passthrough.hlsl"), graphics_objects.resource_pool->LoadVertexShader("passthrough.hlsl", ProcessingEffect::squares_vertex_type), nullptr, 0));

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

	PreUpdateInitialize();
	while (true) {
		UpdateLoopResult update_result = UpdateLoop();
		if (update_result == UpdateLoopResult::FINISH) {
			break;
		} else {
			// Now that the references to the values are gone, recreate the 
			graphics_objects.resource_pool->ClearImpermanentModels();

			// Clear the python interpreter, reinitialize it, and then reload the first_load file.
			// However there is no need to redo the preload function.
			Py_Finalize();
			Py_Initialize();
			main_namespace = import("__main__").attr("__dict__");
			exec("import first_load", main_namespace);
			loaded_module = main_namespace["first_load"];

			continue;
		}
	}

	return 0;
}

