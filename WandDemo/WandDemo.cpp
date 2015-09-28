// WandDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "WiimoteInterface.h"

#include "BeginDirectx.h"
#include "TimeTracker.h"

#include "TextureView.h"

WiimoteInterface wiimote_interface;
WiimoteHandler* wiimote;
VRBackendBasics graphics_objects;

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

Entity makeconsole(const VRBackendBasics& graphics_objects) {
	Model model = graphics_objects.resource_pool->LoadModel("console.obj", { { { 0, 1, 2 }, { 1.0f, 1.0f, 1.0f } }, ObjLoader::vertex_type_all });

	ConstantBufferTyped<TransformationMatrixAndInvTransData>* object_settings = new ConstantBufferTyped<TransformationMatrixAndInvTransData>(CB_PS_VERTEX_SHADER);
	object_settings->CreateBuffer(graphics_objects.view_state->device_interface);
	object_settings->SetBothTransformations(DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationZ(3.14f/2.0f), DirectX::XMMatrixTranslation(0, 0, -4)));
	object_settings->PushBuffer(graphics_objects.view_state->device_context);

	Texture texture = graphics_objects.resource_pool->LoadTexture("console.png");
	TextureView texture_view(0, 0, texture);

	return Entity(ES_NORMAL,
		graphics_objects.resource_pool->LoadPixelShader("texturednormaledshader.hlsl"),
		graphics_objects.resource_pool->LoadVertexShader("texturednormaledshader.hlsl", ObjLoader::vertex_type_all.GetVertexType(), ObjLoader::vertex_type_all.GetSizeVertexType()),
		ShaderSettings(NULL),
		model,
		object_settings,
		texture_view);
}

void GraphicsLoop() {
	MSG msg;
	int prev_time = timeGetTime();
	int frame_index = 0;

	unsigned int wiimote_entity_id = graphics_objects.entity_handler->AddEntity(makewiimote(graphics_objects));
	graphics_objects.entity_handler->DisableEntity(wiimote_entity_id);

	unsigned int console_entity_id = graphics_objects.entity_handler->AddEntity(makeconsole(graphics_objects));

	graphics_objects.entity_handler->FinishUpdate();

	Quaternion obj_orient;

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

		if (wiimote) {
			obj_orient = wiimote->GetCurrentState().orientation;
		}
		// Convert orientation from wiimote coord system to screen coord system
		std::swap(obj_orient.y, obj_orient.z);
		//float tmp = obj_orient.y;
		//obj_orient.y = obj_orient.z;
		//obj_orient.z = tmp;
		obj_orient.x = -obj_orient.x;
		ConstantBufferTyped<TransformationMatrixAndInvTransData>* wiimote_settings = graphics_objects.entity_handler->GetEntityObjectSettings<TransformationMatrixAndInvTransData>(console_entity_id);
		wiimote_settings->SetBothTransformations(
			DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationQuaternion(
			DirectX::XMVectorSet(
			obj_orient.x,
			obj_orient.y,
			obj_orient.z,
			obj_orient.w)),
			DirectX::XMMatrixTranslation(0, -1.75, -1)));

		TimeTracker::FrameEvent("Load Wiimote information");

		graphics_objects.entity_handler->FinishUpdate();
		
		TimeTracker::FrameEvent("Finalize object position stuff");

		int new_time = timeGetTime();
		int time_delta = new_time - prev_time;
		prev_time = new_time;

		graphics_objects.input_handler->UpdateStates(frame_index);
		graphics_objects.world->UpdateLogic(time_delta);

		TimeTracker::FrameEvent("Update states");

		graphics_objects.render_pipeline->Render();

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
	wiimote_interface.Startup();
	wiimote = wiimote_interface.GetHandler();

	graphics_objects = BeginDirectx(false, "");
	TimeTracker::PreparePerformanceCounter();
	TimeTracker::active_track = TimeTracker::NUM_TRACKS;
	TimeTracker::track_time = false;

	graphics_objects.input_handler->SetUsePredictiveTiming(true);
	graphics_objects.input_handler->SetPredictiveTiming(-5);

	GraphicsLoop();

	return 0;
}

