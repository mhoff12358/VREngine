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
VRBackendBasics graphics_objects;
const float movement_scale = 0.0005f;
const float mouse_theta_scale = 0.001f;
const float mouse_phi_scale = 0.001f;

InteractableCollection interactable_objects;

Entity makewiimote(const VRBackendBasics& graphics_objects) {
	std::vector<Vertex> vertices;
	VertexType vertex_type = VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC>({
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	}));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));

	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));

	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));

	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));

	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));

	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));

	Model model = graphics_objects.resource_pool->LoadModel("wiimote", vertices, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ConstantBufferTyped<TransformationMatrixAndInvTransData>* object_settings = new ConstantBufferTyped<TransformationMatrixAndInvTransData>(CB_PS_VERTEX_SHADER);
	object_settings->CreateBuffer(graphics_objects.view_state->device_interface);
	object_settings->SetBothTransformations(DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(3.14f), DirectX::XMMatrixTranslation(0, 0, -4)));
	object_settings->PushBuffer(graphics_objects.view_state->device_context);

	return Entity(
		ES_NORMAL,
		graphics_objects.resource_pool->LoadPixelShader("shaders.hlsl"),
		graphics_objects.resource_pool->LoadVertexShader("shaders.hlsl", vertex_type.GetVertexType(), vertex_type.GetSizeVertexType()),
		ShaderSettings(NULL),
		model,
		object_settings);
}

std::vector<Entity> makeconsole(const VRBackendBasics& graphics_objects) {
	std::map<std::string, Model> models = graphics_objects.resource_pool->LoadModelAsParts("console.obj", { { { 0, 1, 2 }, { 1.0f, 1.0f, 1.0f }, { false, true } }, ObjLoader::vertex_type_all });

	ConstantBufferTyped<TransformationMatrixAndInvTransData>* object_settings = new ConstantBufferTyped<TransformationMatrixAndInvTransData>(CB_PS_VERTEX_SHADER);
	object_settings->CreateBuffer(graphics_objects.view_state->device_interface);
	object_settings->SetBothTransformations(DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationZ(3.14f/2.0f), DirectX::XMMatrixTranslation(0, 0, -4)));
	object_settings->PushBuffer(graphics_objects.view_state->device_context);

	Texture texture = graphics_objects.resource_pool->LoadTexture("console.png");
	TextureView texture_view(0, 0, texture);

	std::vector<Entity> entities;

	for (const std::pair<const std::string, Model>& model : models) {
		Entity new_entity(ES_NORMAL, PixelShader(), VertexShader(), ShaderSettings(NULL), model.second, NULL, texture_view);
		entities.push_back(new_entity);
	}

	entities[0].object_settings = object_settings;

	return entities;
}

Entity makeshaderselection(const VRBackendBasics& graphics_objects) {
	ConstantBufferTyped<LightDetails>* shader_settings_buffer = new ConstantBufferTyped<LightDetails>(CB_PS_VERTEX_SHADER);
	shader_settings_buffer->CreateBuffer(graphics_objects.view_state->device_interface);
	LightDetails& light_details = shader_settings_buffer->EditBufferDataRef();
	light_details.SetLightSourceDirection({ { 1, -1, 0 } });
	light_details.ambient_light = 0.2f;
	shader_settings_buffer->PushBuffer(graphics_objects.view_state->device_context);

	Texture texture = graphics_objects.resource_pool->LoadTexture("console.png");
	TextureView texture_view(0, 0, texture);

	return Entity(
		ES_SETTINGS,
		graphics_objects.resource_pool->LoadPixelShader("texturedspecular.hlsl"),
		graphics_objects.resource_pool->LoadVertexShader("texturedspecular.hlsl", ObjLoader::vertex_type_all.GetVertexType(), ObjLoader::vertex_type_all.GetSizeVertexType()),
		ShaderSettings(shader_settings_buffer),
		Model(),
		NULL,
		texture_view);
}

void GraphicsLoop() {
	MSG msg;
	int prev_time = timeGetTime();
	int frame_index = 0;

	unsigned int wiimote_entity_id = graphics_objects.entity_handler->AddEntity(makewiimote(graphics_objects));
	graphics_objects.entity_handler->DisableEntity(wiimote_entity_id);

	unsigned int shader_selection_entity_id = graphics_objects.entity_handler->AddEntity(makeshaderselection(graphics_objects));

	Actor console_actor = (*graphics_objects.resource_pool);
	console_actor.LoadModelsFromFile("console.obj", { { { 0, 1, 2 }, { 1.0f, 1.0f, 1.0f }, { false, true } }, ObjLoader::vertex_type_all });
	multimap<string, vector<string>> console_parentage = {
			{ "", { "terminal_Plane.001", "grate_Plane" } },
			{ "terminal_Plane.001", { "button2_Circle.001" } },
			{ "terminal_Plane.001", { "button1_Circle" } },
	};
	map<string, unsigned int> console_component_locations = console_actor.CreateComponents(*graphics_objects.entity_handler, graphics_objects.view_state->device_interface, console_parentage);

	Camera player_look_camera;
	InteractableQuad terminal_interact =
		InteractableQuad("terminal",
		{ { -0.75, 1.5, 0 } },
		{ { .75, 1.5, 0 } },
		{ { -0.75, 2.207107, -0.707107 } });
	InteractableCircle red_button_interact =
		InteractableCircle("red_button",
		0.188f,
		{ { -0.3125, 1.9243, -0.2828 } },
		{ { 0, 1, 1 } },
		{ { 1, 0, 0 } });
	InteractableCircle green_button_interact =
		InteractableCircle("green_button",
		0.188f,
		{ { 0.3125, 1.9243, -0.2828 } },
		{ { 0, 1, 1 } },
		{ { 1, 0, 0 } });
	interactable_objects.AddObject(&terminal_interact);
	interactable_objects.AddObject(&red_button_interact);
	interactable_objects.AddObject(&green_button_interact);

	player_look_camera.BuildViewMatrix();
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = graphics_objects.view_state->window_handler;
	if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) {
		std::cout << "Error registering raw input device" << std::endl;
	}

	Quaternion obj_orient;

	std::array<float, 3> player_location = { { 0, 0, 0 } };
	// Stored in theta, phi format, theta kept in [0, 2*pi], phi kept in [-pi, pi]
	std::array<float, 2> player_orientation_angles = { { 0, 0 } };

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
			else if (msg.message == WM_KEYDOWN) {
				graphics_objects.input_handler->HandleKeydown(msg.wParam);

				switch (msg.wParam) {
				case 'F':
					wiimote->RequestCalibrateMotionPlus();
					break;
				case 'C':
					wiimote->SendOutputReport(OutputReportTemplates::request_calibration);
					break;
				case 'V':
					wiimote->Rezero();
					break;
				}
			}
		}
		TimeTracker::FrameEvent("Windows Message Handling");

		int new_time = timeGetTime();
		int time_delta_ms = new_time - prev_time;
		prev_time = new_time;

		graphics_objects.input_handler->UpdateStates(frame_index);

		TimeTracker::FrameEvent("Update states");

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

		std::array<int, 2> mouse_motion = graphics_objects.input_handler->ConsumeMouseMotion();
		player_orientation_angles[1] = min(pi / 2.0f, max(-pi / 2.0f, player_orientation_angles[1] - mouse_motion[1] * mouse_phi_scale));
		player_orientation_angles[0] = fmodf(player_orientation_angles[0] - mouse_motion[0] * mouse_phi_scale, pi*2.0f);

		infinite_light_direction = Quaternion::RotationAboutAxis(AID_Y, time_delta_ms * movement_scale).ApplyToVector(infinite_light_direction);
		ConstantBufferTyped<LightDetails>* infinite_light_buffer = graphics_objects.entity_handler->GetShaderSettings<LightDetails>(shader_selection_entity_id);
		infinite_light_buffer->EditBufferDataRef().SetLightSourceDirection(infinite_light_direction);

		TimeTracker::FrameEvent("Update Game Objects");
		DirectX::XMMATRIX terminal_transformation = DirectX::XMMatrixTranslation(0, -1.75, -1);
		console_actor.SetComponentTransformation(console_component_locations["terminal_Plane.001"],
			terminal_transformation);
		terminal_interact.SetModelTransformation(terminal_transformation);
		red_button_interact.SetModelTransformation(terminal_transformation);
		green_button_interact.SetModelTransformation(terminal_transformation);

		Quaternion player_orientation_quaternion = Quaternion::RotationAboutAxis(AID_Y, player_orientation_angles[0]) * Quaternion::RotationAboutAxis(AID_X, player_orientation_angles[1]);
		player_look_camera.location = player_location;
		player_look_camera.orientaiton = player_orientation_quaternion.GetArray();
		player_look_camera.InvalidateViewMatrices();
		Identifier id_of_object;
		float distance_to_object;
		array<float, 2> where_on_object;
		std::tie(id_of_object, distance_to_object, where_on_object) = interactable_objects.GetClosestLookedAtAndWhere(player_look_camera.GetViewMatrix());
		std::cout << id_of_object << ",\t" << distance_to_object << ",\t" << where_on_object[0] << ",\t" << where_on_object[1] << std::endl;

		if (graphics_objects.input_handler->GetKeyToggled('B') && id_of_object == "red_button") {
			console_actor.SetComponentTransformation(console_component_locations["button2_Circle.001"],
				ComposeMatrices<3>({ {
				DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(1, 0, 0, 0), pi / 4.0f),
				DirectX::XMMatrixTranslation(0, 0, -0.099f),
				DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(1, 0, 0, 0), -pi / 4.0f) } }));
		}
		if (graphics_objects.input_handler->GetKeyToggled('B') && id_of_object == "green_button") {
			console_actor.SetComponentTransformation(console_component_locations["button1_Circle"],
				ComposeMatrices<3>({ {
				DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(1, 0, 0, 0), pi / 4.0f),
				DirectX::XMMatrixTranslation(0, 0, -0.099f),
				DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(1, 0, 0, 0), -pi / 4.0f) } }));
		}
		if (graphics_objects.input_handler->GetKeyToggled('B', false)) {
			console_actor.SetComponentTransformation(console_component_locations["button2_Circle.001"],
				DirectX::XMMatrixIdentity());
			console_actor.SetComponentTransformation(console_component_locations["button1_Circle"],
				DirectX::XMMatrixIdentity());
		}
		
		graphics_objects.entity_handler->FinishUpdate();
		
		TimeTracker::FrameEvent("Finalize object position stuff");

		if (graphics_objects.input_handler->IsOculusActive()) {
			Quaternion combined_orientation = OculusHelper::ConvertOculusQuaternionToQuaternion(graphics_objects.input_handler->GetHeadPose().ThePose.Orientation)*Quaternion();
			graphics_objects.render_pipeline->Render({ player_location, combined_orientation });
		} else {
			graphics_objects.render_pipeline->Render({ player_location,  player_orientation_quaternion});
		}

		TimeTracker::FrameEvent("Actual rendering");

		++frame_index;
	}

	// clean up DirectX and COM
	graphics_objects.view_state->Cleanup();
	if (graphics_objects.input_handler->IsOculusActive()) {
		graphics_objects.oculus->Cleanup();
	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	//wiimote_interface.Startup();
	//wiimote = wiimote_interface.GetHandler();
	wiimote = NULL;

	graphics_objects = BeginDirectx(false, "");
	TimeTracker::PreparePerformanceCounter();
	TimeTracker::active_track = TimeTracker::NUM_TRACKS;
	TimeTracker::track_time = false;

	graphics_objects.input_handler->SetUsePredictiveTiming(true);
	graphics_objects.input_handler->SetPredictiveTiming(-5);

	GraphicsLoop();

	return 0;
}

