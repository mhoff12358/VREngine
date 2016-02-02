// WandDemo.cpp : Defines the entry point for the console application.
//

#include <memory>
using std::unique_ptr;

#include "stdafx.h"

#include "WiimoteInterface.h"

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

#include "LuaGameScripting/Environment.h"
#include "LuaGameScripting/InteractableObject.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

WiimoteInterface wiimote_interface;
WiimoteHandler* wiimote;
ButtonHistory wiimote_buttons;
VRBackendBasics graphics_objects;
const float movement_scale = 0.0005f;
const float mouse_theta_scale = 0.001f;
const float mouse_phi_scale = 0.001f;

mutex player_position_access;
array<float, 3> player_location = { { 0, 0, 0 } };
Quaternion player_orientation_quaternion;

mutex device_context_access;

template <>
class ConstantBufferTyped<array<float, 4>> : public ConstantBufferTypedTemp<array<float, 4>>{
public:
	ConstantBufferTyped(CB_PIPELINE_STAGES stages) : ConstantBufferTypedTemp(stages) {}
};

void GraphicsLoop() {
	int frame_index = 0;

	int prev_sec = timeGetTime();
	int fps = 0;

	while (true) {
		graphics_objects.input_handler->UpdateOculus(frame_index);
		unique_lock<mutex> device_context_lock(device_context_access);

		unique_lock<mutex> player_position_lock(player_position_access);
		array<float, 3> player_location_copy = player_location;
		Quaternion player_orientation_quaternion_copy = player_orientation_quaternion;
		player_position_lock.unlock();

		RenderGroup* drawing_groups = graphics_objects.entity_handler->GetRenderGroupForDrawing();
		for (int render_group_number = 0; render_group_number < graphics_objects.entity_handler->GetNumEntitySets(); render_group_number++) {
			drawing_groups[render_group_number].ApplyMutations(*graphics_objects.resource_pool);
		}

		graphics_objects.render_pipeline->Render({ player_location, player_orientation_quaternion });
		frame_index++;
	}
}

void UpdateLoop() {
	MSG msg;
	int prev_time = timeGetTime();
	ActorHandler actor_handler(graphics_objects);

	// Stored in theta, phi format, theta kept in [0, 2*pi], phi kept in [-pi, pi]
	array<float, 2> player_orientation_angles = { { 0, 0 } };

	LookState previous_look = { Identifier(""), NULL, 0, array<float, 2>({ 0, 0 }) };

	unique_lock<mutex> device_context_lock(device_context_access);
	actor_handler.LoadSceneFromLuaScript("cockpit_scene.lua");
	device_context_lock.unlock();

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

	std::array<float, 3> infinite_light_direction = { { 1, 1, 0 } };

	while (TRUE)
	{
		TimeTracker::FrameStart();
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				break;
			}
		}
		TimeTracker::FrameEvent("Windows Message Handling");

		int new_time = timeGetTime();
		int time_delta_ms = new_time - prev_time;
		prev_time = new_time;

		TimeTracker::FrameEvent("Update states");

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

		if (wiimote != NULL) {
			if (graphics_objects.input_handler->GetKeyToggled('F')) {
				wiimote->RequestCalibrateMotionPlus();
				wiimote->Rezero();
			}
		}

		std::array<int, 2> mouse_motion = graphics_objects.input_handler->ConsumeMouseMotion();
		player_orientation_angles[1] = min(pi / 2.0f, max(-pi / 2.0f, player_orientation_angles[1] - mouse_motion[1] * mouse_phi_scale));
		player_orientation_angles[0] = fmodf(player_orientation_angles[0] - mouse_motion[0] * mouse_phi_scale, pi*2.0f);
		player_orientation_quaternion = Quaternion::RotationAboutAxis(AID_Y, player_orientation_angles[0]) * Quaternion::RotationAboutAxis(AID_X, player_orientation_angles[1]);
		player_position_lock.unlock();

		infinite_light_direction = Quaternion::RotationAboutAxis(AID_Y, time_delta_ms * movement_scale).ApplyToVector(infinite_light_direction);
		//shader_settings_buffer->EditBufferDataRef().SetLightSourceDirection(infinite_light_direction);

		TimeTracker::FrameEvent("Update Game Objects");

		player_look_camera.location = player_location;
		if (graphics_objects.input_handler->IsOculusActive()) {
			player_orientation_quaternion = OculusHelper::ConvertOculusQuaternionToQuaternion(graphics_objects.input_handler->GetHeadPose().ThePose.Orientation) * player_orientation_quaternion;
			array<float, 3> oculus_offset = OculusHelper::ConvertVector3fToArray(graphics_objects.input_handler->GetHeadOffset());
			array<float, 3> oculus_eye_offset = OculusHelper::ConvertVector3fToArray(graphics_objects.input_handler->GetEyeOffset(0) + graphics_objects.input_handler->GetEyeOffset(1));
			for (int i = 0; i < 3; i++) {
				player_look_camera.location[i] += oculus_offset[i] + oculus_eye_offset[i] / 2.0f;
			}
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

		if (wiimote != NULL) {
			WiimoteState wiimote_state = wiimote->GetCurrentState();
			wiimote_buttons.previous_button_state = wiimote_buttons.current_button_state;
			wiimote_buttons.current_button_state = wiimote_state.current_button_state;

			array<bool, 16> buttons_toggle_down = wiimote_buttons.GetAllButtonsToggled();
			array<bool, 16> buttons_toggle_up = wiimote_buttons.GetAllButtonsToggled(false);
			
			if (wiimote_buttons.GetButtonToggled(HOME_MASK)) {
				wiimote->RequestCalibrateMotionPlus();
				wiimote->Rezero();
			}

			for (Lua::InteractableObject* wiimote_button_listener : actor_handler.LookupListeners("wiimote_button")) {
				wiimote_button_listener->CallLuaFunc(string("wiimote_button_down"), buttons_toggle_down);
				wiimote_button_listener->CallLuaFunc(string("wiimote_button_up"), buttons_toggle_up);
			}

			for (Lua::InteractableObject* wiimote_position_listener : actor_handler.LookupListeners("wiimote_position")) {
				array<float, 4> raw_wiimote_orientation = (wiimote_state.orientation * Quaternion::RotationAboutAxis(AID_X, 3.14 / 2)).GetArray();
				std::swap(raw_wiimote_orientation[1], raw_wiimote_orientation[2]);
				raw_wiimote_orientation[0] *= -1;
				wiimote_position_listener->CallLuaFunc(string("wiimote_position"), raw_wiimote_orientation);
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
		
		graphics_objects.entity_handler->FinishUpdate();
		
		TimeTracker::FrameEvent("Finalize object position stuff");
	}

	// clean up DirectX and COM
	graphics_objects.view_state->Cleanup();
	if (graphics_objects.input_handler->IsOculusActive()) {
		graphics_objects.oculus->Cleanup();
	}

}

#include "LuaGameScripting/Environment.h"
#include "LuaGameScripting/MemberFunctionWrapper.h"
void LuaTest() {

	Lua::Environment env(true);
	env.RunFile("lua_test.lua");

	//tuple<float, float, int, string> vals;
	//env.LoadTupleElement<1, 4, float, float, int, string>(vals, Lua::Index(-1));
	//vector<vector<string>> vals;
	//bool success = env.LoadGlobal(string("bah"), &vals);

	env.PrintStack();

	std::cout << "Finish lua test" << std::endl;
}

#include "PipelineStage.h"
#include "RenderEntities.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//LuaTest();

	wiimote_interface.Startup();
	wiimote = wiimote_interface.GetHandler();

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

	graphics_objects = BeginDirectx(false, "");

	TextureSignature back_buffer_signature(graphics_objects.render_pipeline->GetStagingBufferDesc());
	vector<unique_ptr<PipelineStageDesc>> pipeline_stages;
	pipeline_stages.emplace_back(new RenderEntitiesDesc("basic", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, "normal_depth", {}, no_alpha_blend_state_desc));
	pipeline_stages.emplace_back(new RenderEntitiesDesc("bloom", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature), std::make_tuple("bloom", back_buffer_signature) }, "normal_depth", {}, keep_new_alpha_blend_state_desc));
	pipeline_stages.emplace_back(new RenderEntitiesDesc("alpha", PST_RENDER_ENTITIES, { std::make_tuple("objects", back_buffer_signature) }, "normal_depth", {}, drop_alpha_with_alpha_blend_state_desc));
	pipeline_stages.emplace_back(new TextureCopyDesc("move_to_back", { std::make_tuple("back_buffer", back_buffer_signature) }, { "objects" }));
	auto bloom_horiz_kernel = FillHLSLKernel<51>(Generate1DGausian<51>(1, 0, 6));
	bloom_horiz_kernel[0] = graphics_objects.render_pipeline->GetStageBufferSize()[0];
	auto bloom_vert_kernel = FillHLSLKernel<51>(Generate1DGausian<51>(1, 0, 6));
	bloom_vert_kernel[0] = graphics_objects.render_pipeline->GetStageBufferSize()[1];
	pipeline_stages.emplace_back(new ProcessingEffectDesc("horiz_bloom_applied", { std::make_tuple("horiz_bloom", back_buffer_signature) }, { "objects", "bloom" }, keep_new_alpha_blend_state_desc, graphics_objects.resource_pool->LoadPixelShader("bloom_horiz.hlsl"), graphics_objects.resource_pool->LoadVertexShader("bloom_horiz.hlsl", ProcessingEffect::squares_vertex_type), (char*)&bloom_horiz_kernel, sizeof(bloom_horiz_kernel)));
	pipeline_stages.emplace_back(new ProcessingEffectDesc("vert_bloom_applied", { std::make_tuple("back_buffer", back_buffer_signature) }, { "horiz_bloom", "bloom" }, additative_for_all_blend_state_desc, graphics_objects.resource_pool->LoadPixelShader("bloom_vert.hlsl"), graphics_objects.resource_pool->LoadVertexShader("bloom_vert.hlsl", ProcessingEffect::squares_vertex_type), (char*)&bloom_vert_kernel, sizeof(bloom_vert_kernel)));
	
	graphics_objects.render_pipeline->SetPipelineStages(pipeline_stages);
	for (const unique_ptr<PipelineStageDesc>& desc : pipeline_stages) {
		if (desc->entity_handler_set_ != -1) {
			graphics_objects.entity_handler_set_lookup[desc->name_] = desc->entity_handler_set_;
		}
	}

	TimeTracker::PreparePerformanceCounter();
	TimeTracker::active_track = TimeTracker::NUM_TRACKS;
	TimeTracker::track_time = false;

	graphics_objects.input_handler->SetUsePredictiveTiming(true);
	graphics_objects.input_handler->SetPredictiveTiming(-5);

	thread graphics_thread(GraphicsLoop);
	UpdateLoop();

	return 0;
}

