// WandDemo.cpp : Defines the entry point for the console application.
//

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
		for (int render_group_number = 0; render_group_number < 2; render_group_number++) {
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
	actor_handler.LoadSceneFromLuaScript("tower_scene.lua");
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

int _tmain(int argc, _TCHAR* argv[])
{
	//LuaTest();

	wiimote_interface.Startup();
	wiimote = wiimote_interface.GetHandler();
	
	graphics_objects = BeginDirectx(false, "");
	TimeTracker::PreparePerformanceCounter();
	TimeTracker::active_track = TimeTracker::NUM_TRACKS;
	TimeTracker::track_time = false;

	graphics_objects.input_handler->SetUsePredictiveTiming(true);
	graphics_objects.input_handler->SetPredictiveTiming(-5);

	thread graphics_thread(GraphicsLoop);
	UpdateLoop();

	return 0;
}

