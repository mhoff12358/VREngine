// WandDemo.cpp : Defines the entry point for the console application.
//

#include <memory>
using std::unique_ptr;
#include <chrono>

#include "stdafx.h"

#include "BeginDirectx.h"
#include "TimeTracker.h"
#include "TextureView.h"
#include "my_math.h"

#include "LightDetails.h"
#include "Component.h"
#include "Actor.h"
#include "LookInteractable.h"
#include "InteractableTriangle.h"
#include "InteractableQuad.h"
#include "InteractableCircle.h"
#include "Identifier.h"
#include "InteractableCollection.h"
#include "ActorHandler.h"

#include "gaussian.h"

#include "Scene.h"
#include "SpecializedActors.h"
#include "SpecializedCommandArgs.h"
#include "GraphicsObject.h"
#include "HeadsetInterface.h"
#include "NichijouGraph.h"

#include "BoostPythonWrapper.h"

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

mutex player_position_access;
array<float, 3> player_location = { { 0, 1.5, 0 } };
Quaternion player_orientation_quaternion;

mutex device_context_access;

template <>
class ConstantBufferTyped<array<float, 4>> : public ConstantBufferTypedTemp < array<float, 4> > {
public:
	ConstantBufferTyped(CB_PIPELINE_STAGES stages) : ConstantBufferTypedTemp(stages) {}
};

void GraphicsLoop() {
	int frame_index = 0;

	int prev_sec = timeGetTime();
	int fps = 0;


	while (true) {
		graphics_objects.input_handler->UpdateHeadset(frame_index);
		unique_lock<mutex> device_context_lock(device_context_access);

		unique_lock<mutex> player_position_lock(player_position_access);
		array<float, 3> player_location_copy = player_location;
		Quaternion player_orientation_quaternion_copy = player_orientation_quaternion;
		player_position_lock.unlock();

		RenderGroup* drawing_groups = graphics_objects.entity_handler->GetRenderGroupForDrawing();
		for (int render_group_number = 0; render_group_number < graphics_objects.entity_handler->GetNumEntitySets(); render_group_number++) {
			drawing_groups[render_group_number].ApplyMutations(*graphics_objects.resource_pool);
		}

		graphics_objects.render_pipeline->Render(Pose(Location(player_location), player_orientation_quaternion));
		frame_index++;
	}
}

void UpdateLoop() {
	int prev_time = timeGetTime();
	ActorHandler actor_handler(graphics_objects);

	// Stored in theta, phi format, theta kept in [0, 2*pi], phi kept in [-pi, pi]
	array<float, 2> player_orientation_angles = { { 0, 0 } };

	LookState previous_look = { Identifier(""), NULL, 0, { { 0, 0 } } };

	//unique_lock<mutex> device_context_lock(device_context_access);
	//actor_handler.LoadSceneFromLuaScript("cockpit_scene.lua");
	//device_context_lock.unlock();

	Camera player_look_camera;
	player_look_camera.BuildViewMatrix();

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = graphics_objects.view_state->window_handler;
	if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) {
		std::cout << "Error registering raw input device" << std::endl;
	}

	Py_Initialize();

	game_scene::Scene scene;
	game_scene::Shmactor::SetScene(&scene);
	game_scene::ActorId controls_registry = scene.AddActorGroup();
	game_scene::ActorId tick_registry = scene.AddActorGroup();
	game_scene::ActorId camera_movement = scene.AddActor(
		make_unique<game_scene::actors::MovableCamera>(&player_look_camera));
	scene.AddActorToGroup(camera_movement, controls_registry);
	game_scene::ActorId graphics_resources = scene.AddActor(
		make_unique<game_scene::actors::GraphicsResources>(
			*graphics_objects.resource_pool,
			*graphics_objects.entity_handler,
			graphics_objects.view_state->device_interface));
	game_scene::ActorId cockpit = scene.AddActor(
		make_unique<game_scene::actors::GraphicsObject>());
	game_scene::ActorId floor = scene.AddActor(
		make_unique<game_scene::actors::GraphicsObject>());
	game_scene::ActorId weird_wall = scene.AddActor(
		make_unique<game_scene::actors::GraphicsObject>());
	game_scene::ActorId nichijou_graph = scene.AddActor(
		make_unique<game_scene::actors::NichijouGraph>());

	if (graphics_objects.oculus->IsInitialized()) {
		game_scene::ActorId headset_interface = scene.AddActor(
			make_unique<game_scene::actors::HeadsetInterface>(*graphics_objects.oculus));
		scene.AddActorToGroup(headset_interface, tick_registry);
	}

	game_scene::actors::GraphicsObjectDetails square_details;
	square_details.heirarchy_ = game_scene::actors::ComponentHeirarchy(
		"square",
		{
			{"square.obj|Square",
			ObjLoader::OutputFormat(
				ModelModifier(
					{0, 1, 2},
					{1, 1, 1},
					{false, true}),
				ObjLoader::vertex_type_all,
				false)}
		},
		{});
	square_details.heirarchy_.textures_.push_back(game_scene::actors::TextureDetails("terrain.png", false, true));
	square_details.heirarchy_.shader_name_ = "texturedspecularlightsource.hlsl";
	square_details.heirarchy_.entity_group_ = "basic";
	square_details.heirarchy_.shader_settings_ = {
		{0.0f, 0.5f, 0.0f},
		{0.2f}
	};
	game_scene::actors::GraphicsObjectDetails cockpit_details;
	cockpit_details.heirarchy_ = game_scene::actors::ComponentHeirarchy(
		"bars",
		{
			{"cockpit_bars.obj|MetalBars",
			ObjLoader::OutputFormat(
				ModelModifier(
					{0, 1, 2},
					{1, 1, 1},
					{false, true}),
				ObjLoader::vertex_type_all,
				false)}
		},
		{});
	cockpit_details.heirarchy_.textures_.push_back(game_scene::actors::TextureDetails("metal_bars.png", false, true));
	cockpit_details.heirarchy_.shader_name_ = "texturedspecularlightsource.hlsl";
	cockpit_details.heirarchy_.entity_group_ = "basic";
	cockpit_details.heirarchy_.shader_settings_ = {
		{0.0f, 0.5f, 0.0f},
		{0.2f}
	};

	scene.ExecuteCommand(game_scene::Command(
		game_scene::Target(floor),
		make_unique<game_scene::WrappedCommandArgs<game_scene::actors::GraphicsObjectDetails>>(
			game_scene::commands::GraphicsCommandType::CREATE_COMPONENTS,
			square_details)));
	scene.ExecuteCommand(game_scene::Command(
		game_scene::Target(floor),
		make_unique<game_scene::commands::ComponentPlacement>("Square", DirectX::XMMatrixIdentity())));

	scene.ExecuteCommand(game_scene::Command(
		game_scene::Target(weird_wall),
		make_unique<game_scene::WrappedCommandArgs<game_scene::actors::GraphicsObjectDetails>>(
			game_scene::commands::GraphicsCommandType::CREATE_COMPONENTS,
			square_details)));
	scene.ExecuteCommand(game_scene::Command(
		game_scene::Target(weird_wall),
		make_unique<game_scene::commands::ComponentPlacement>("Square", DirectX::XMMatrixTranslation(0, 0, -2))));

	scene.ExecuteCommand(game_scene::Command(
		game_scene::Target(cockpit),
		make_unique<game_scene::WrappedCommandArgs<game_scene::actors::GraphicsObjectDetails>>(
			game_scene::commands::GraphicsCommandType::CREATE_COMPONENTS,
			cockpit_details)));
	scene.ExecuteCommand(game_scene::Command(
		game_scene::Target(cockpit),
		make_unique<game_scene::commands::ComponentPlacement>("bars", DirectX::XMMatrixTranslation(0, 1.5, 0))));
	scene.FlushCommandQueue();

	MSG msg;
	vr::VREvent_t vr_msg;

	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				break;
			}
		}

		if (graphics_objects.oculus->IsInitialized()) {
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

			graphics_objects.oculus->UpdateGamePoses();
		}

		int new_time = timeGetTime();
		int time_delta_ms = new_time - prev_time;
		prev_time = new_time;

		graphics_objects.input_handler->UpdateKeyboardState();
		unique_lock<mutex> player_position_lock(player_position_access);
		if (graphics_objects.input_handler->GetKeyPressed('W')) {
			player_location[2] -= time_delta_ms * movement_scale;
		}
		if (graphics_objects.input_handler->GetKeyPressed('S')) {
			player_location[2] += time_delta_ms * movement_scale;
		}
		if (graphics_objects.input_handler->GetKeyPressed('A')) {
			player_location[0] -= time_delta_ms * movement_scale;
		}
		if (graphics_objects.input_handler->GetKeyPressed('D')) {
			player_location[0] += time_delta_ms * movement_scale;
		}
		if (graphics_objects.input_handler->GetKeyPressed('Q')) {
			player_location[1] -= time_delta_ms * movement_scale;
		}
		if (graphics_objects.input_handler->GetKeyPressed('E')) {
			player_location[1] += time_delta_ms * movement_scale;
		}

		array<float, 2> aim_movement = { 0, 0 };
		if (graphics_objects.input_handler->GetKeyPressed('J')) {
			aim_movement[1] += 1;
		}
		if (graphics_objects.input_handler->GetKeyPressed('L')) {
			aim_movement[1] -= 1;
		}
		if (graphics_objects.input_handler->GetKeyPressed('I')) {
			aim_movement[0] += 1;
		}
		if (graphics_objects.input_handler->GetKeyPressed('K')) {
			aim_movement[0] -= 1;
		}

		actor_handler.root_environment_.StoreToStack(tuple<>());
		actor_handler.root_environment_.StoreToTable(string("aim_movement"), aim_movement);
		actor_handler.root_environment_.SetGlobalFromStack(string("user_input"));

		std::array<int, 2> mouse_motion = graphics_objects.input_handler->ConsumeMouseMotion();
		player_orientation_angles[1] = min(pi / 2.0f, max(-pi / 2.0f, player_orientation_angles[1] - mouse_motion[1] * mouse_phi_scale));
		player_orientation_angles[0] = fmodf(player_orientation_angles[0] - mouse_motion[0] * mouse_phi_scale, pi*2.0f);
		player_orientation_quaternion = Quaternion::RotationAboutAxis(AID_Y, player_orientation_angles[0]) * Quaternion::RotationAboutAxis(AID_X, player_orientation_angles[1]);
		player_position_lock.unlock();

		player_look_camera.location = player_location;
		if (graphics_objects.input_handler->IsHeadsetActive()) {
			/*
			player_orientation_quaternion = graphics_objects.input_handler->GetHeadPose().orientation_ * player_orientation_quaternion;
			array<float, 3> oculus_offset = graphics_objects.input_handler->GetHeadOffset();
			array<float, 3> oculus_eye_offset = graphics_objects.input_handler->GetEyeOffset(0) + graphics_objects.input_handler->GetEyeOffset(1);
			for (int i = 0; i < 3; i++) {
				player_look_camera.location[i] += oculus_offset[i] + oculus_eye_offset[i] / 2.0f;
			}
			*/
		}
		player_look_camera.orientaiton = player_orientation_quaternion.GetArray();
		player_look_camera.InvalidateViewMatrices();
		LookState current_look = { Identifier(""), NULL, 0, { 0, 0 } };
		std::tie(current_look.id_of_object, current_look.actor, current_look.distance_to_object, current_look.where_on_object) = actor_handler.interactable_collection_.GetClosestLookedAtAndWhere(player_look_camera.GetViewMatrix());

		for (Lua::InteractableObject* update_tick_listener : actor_handler.LookupListeners("update_tick")) {
			assert(update_tick_listener->CallLuaFunc(string("update_tick"), time_delta_ms));
		}

		if ((mouse_motion[0] != 0) && (mouse_motion[1] != 0)) {
			for (Lua::InteractableObject* mouse_motion_listener : actor_handler.LookupListeners("mouse_motion")) {
				mouse_motion_listener->CallLuaFunc(string("mouse_motion"), mouse_motion);
			}
		}

		if (graphics_objects.input_handler->GetKeyToggled(' ')) {
			for (Lua::InteractableObject* keydown_listener : actor_handler.LookupListeners("space_down")) {
				assert(keydown_listener->CallLuaFunc(string("space_down")));
			}
		}

		if (previous_look.id_of_object != current_look.id_of_object) {
			if (previous_look.actor != NULL) {
				previous_look.actor->lua_interface_.CallLuaFunc(string("look_left"), current_look.id_of_object.GetId(), previous_look.id_of_object.GetId());
			}
			if (current_look.actor != NULL) {
				current_look.actor->lua_interface_.CallLuaFunc(string("look_entered"), current_look.id_of_object.GetId());
			}
		}
		if (current_look.actor != NULL) {
			current_look.actor->lua_interface_.CallLuaFunc(string("look_maintained"), current_look.id_of_object.GetId(), current_look.where_on_object[0], current_look.where_on_object[1]);
		}
		previous_look = current_look;

		// Scene logic
		game_scene::CommandQueueLocation tick_command = scene.MakeCommandAfter(
			scene.FrontOfCommands(),
			game_scene::Command(
				game_scene::Target(tick_registry),
				make_unique<game_scene::commands::TimeTick>(time_delta_ms)));
		scene.MakeCommandAfter(
			tick_command,
			game_scene::Command(
				game_scene::Target(controls_registry),
				make_unique<game_scene::commands::InputUpdate>(
					graphics_objects.input_handler)));
		scene.FlushCommandQueue();

		graphics_objects.entity_handler->FinishUpdate();
	}

	// clean up DirectX and COM
	graphics_objects.view_state->Cleanup();
	if (graphics_objects.input_handler->IsHeadsetActive()) {
		graphics_objects.oculus->Cleanup();
	}
}

#include "PipelineCamera.h"
#include "PipelineStage.h"
#include "RenderEntities.h"

int _tmain(int argc, _TCHAR* argv[])
{
	bool hmd_active = false;
	bool hmd_desired = true;
	bool hmd_found = vr::VR_IsHmdPresent();
	vr::IVRSystem* headset_system = nullptr;
	if (hmd_desired && hmd_found && vr::VR_IsRuntimeInstalled()) {
		vr::EVRInitError eError = vr::VRInitError_None;
		headset_system = vr::VR_Init( &eError, vr::VRApplication_Scene );
		hmd_active = true;
	}
	D3D11_BLEND_DESC no_alpha_blend_state_desc;
	no_alpha_blend_state_desc.AlphaToCoverageEnable = false;
	no_alpha_blend_state_desc.IndependentBlendEnable = false;
	no_alpha_blend_state_desc.RenderTarget[0].BlendEnable = true;
	no_alpha_blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	no_alpha_blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	no_alpha_blend_state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	no_alpha_blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	no_alpha_blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	no_alpha_blend_state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	no_alpha_blend_state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC keep_new_alpha_blend_state_desc;
	keep_new_alpha_blend_state_desc.AlphaToCoverageEnable = false;
	keep_new_alpha_blend_state_desc.IndependentBlendEnable = false;
	keep_new_alpha_blend_state_desc.RenderTarget[0].BlendEnable = true;
	keep_new_alpha_blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	keep_new_alpha_blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	keep_new_alpha_blend_state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	keep_new_alpha_blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	keep_new_alpha_blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	keep_new_alpha_blend_state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	keep_new_alpha_blend_state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC drop_alpha_with_alpha_blend_state_desc;
	drop_alpha_with_alpha_blend_state_desc.AlphaToCoverageEnable = false;
	drop_alpha_with_alpha_blend_state_desc.IndependentBlendEnable = false;
	drop_alpha_with_alpha_blend_state_desc.RenderTarget[0].BlendEnable = true;
	drop_alpha_with_alpha_blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	drop_alpha_with_alpha_blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	drop_alpha_with_alpha_blend_state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	drop_alpha_with_alpha_blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	drop_alpha_with_alpha_blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	drop_alpha_with_alpha_blend_state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	drop_alpha_with_alpha_blend_state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC additative_for_all_blend_state_desc;
	additative_for_all_blend_state_desc.AlphaToCoverageEnable = false;
	additative_for_all_blend_state_desc.IndependentBlendEnable = false;
	additative_for_all_blend_state_desc.RenderTarget[0].BlendEnable = true;
	additative_for_all_blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	additative_for_all_blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	additative_for_all_blend_state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	additative_for_all_blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	additative_for_all_blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	additative_for_all_blend_state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	additative_for_all_blend_state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_DEPTH_STENCIL_DESC keep_nearer_depth_test;
	keep_nearer_depth_test.DepthEnable = true;
	keep_nearer_depth_test.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	keep_nearer_depth_test.DepthFunc = D3D11_COMPARISON_LESS;
	keep_nearer_depth_test.StencilEnable = true;
	keep_nearer_depth_test.StencilReadMask = 0xFF;
	keep_nearer_depth_test.StencilWriteMask = 0xFF;
	keep_nearer_depth_test.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	keep_nearer_depth_test.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	keep_nearer_depth_test.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	keep_nearer_depth_test.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	keep_nearer_depth_test.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	keep_nearer_depth_test.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	keep_nearer_depth_test.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	keep_nearer_depth_test.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	graphics_objects = BeginDirectx(headset_system, "");

	int stage_repetition = hmd_active ? 2 : 0;

	map<string, PipelineCamera> pipeline_cameras;
	if (hmd_active) {
		pipeline_cameras["player_head|0"].SetRawProjection(graphics_objects.oculus->GetEyeProjectionMatrix(Headset::eyes_[0], 0.001f, 500.0f));
		pipeline_cameras["player_head|0"].BuildMatrices();
		pipeline_cameras["player_head|1"].SetRawProjection(graphics_objects.oculus->GetEyeProjectionMatrix(Headset::eyes_[1], 0.001f, 500.0f));
		pipeline_cameras["player_head|1"].BuildMatrices();
	} else {
		pipeline_cameras["player_head"].SetPerspectiveProjection(
			60.0f / 180.0f*3.1415f,
			((float)graphics_objects.view_state->window_details.screen_size[0]) / graphics_objects.view_state->window_details.screen_size[1],
			0.001f,
			500.0f);
		pipeline_cameras["player_head"].BuildMatrices();
	}
	graphics_objects.render_pipeline->SetPipelineCameras(pipeline_cameras);

	string single_camera_name = "player_head";
	if (hmd_active) {
		single_camera_name += "|0";
	}

	TextureSignature back_buffer_signature(*graphics_objects.render_pipeline->GetStagingBufferDesc());
	//TextureSignature hmd_signature(graphics_objects.oculus->GetEyeTexture(Headset::eyes_[0]).GetDesc());
	vector<unique_ptr<PipelineStageDesc>> pipeline_stages;
	pipeline_stages.emplace_back(new RenderEntitiesDesc("skybox", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription::Empty(), {}, no_alpha_blend_state_desc, PipelineCameraIdent(single_camera_name)));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("basic", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription("normal_depth", keep_nearer_depth_test), {}, no_alpha_blend_state_desc, PipelineCameraIdent("player_head"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("terrain", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription("normal_depth", keep_nearer_depth_test), {}, no_alpha_blend_state_desc, PipelineCameraIdent("player_head"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("bloom", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature), std::make_tuple("bloom", back_buffer_signature) }, DepthStencilDescription("normal_depth", keep_nearer_depth_test), {}, keep_new_alpha_blend_state_desc, PipelineCameraIdent("player_head"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<RenderEntitiesDesc>(stage_repetition, RenderEntitiesDesc("alpha", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, DepthStencilDescription("normal_depth", keep_nearer_depth_test), {}, drop_alpha_with_alpha_blend_state_desc, PipelineCameraIdent("player_head"))));
	pipeline_stages.emplace_back(new RepeatedStageDesc<TextureCopyDesc>(stage_repetition, TextureCopyDesc("move_to_back", { std::make_tuple("back_buffer", back_buffer_signature) }, { "objects" })));
	if (hmd_active) {
		pipeline_stages.emplace_back(new TextureCopyDesc("move_to_back", { std::make_tuple("back_buffer|2", back_buffer_signature) }, { "objects|0" }));
	}

	graphics_objects.render_pipeline->SetPipelineStages(pipeline_stages);

	TimeTracker::PreparePerformanceCounter();
	TimeTracker::active_track = TimeTracker::NUM_TRACKS;
	TimeTracker::track_time = true;

	graphics_objects.input_handler->SetUsePredictiveTiming(true);
	graphics_objects.input_handler->SetPredictiveTiming(-5);

	thread graphics_thread(GraphicsLoop);
	UpdateLoop();

	return 0;
}

