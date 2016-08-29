#include "InputHandler.h"

void InputHandler::Initialize() {
}

void InputHandler::HandleKeydown(unsigned int key_code) {
	OutputFormatted("Key code: %u", key_code);
	keyboard_down_waiting[key_code] = 1;
	//if (keydown_callback.target != NULL) {
	//	keydown_callback(key_code);
	//}
}

void InputHandler::UpdateKeyboardState() {
	prev_keyboard_state = keyboard_state;
	GetKeyboardState(keyboard_state.data());
}

void InputHandler::UpdateHeadset(unsigned int frame_index) {
	current_frame_index = frame_index;
	if (IsHeadsetActive()) {
		if (GetKeyPressed('R')) {
			ResetHeadTracking();
		}
		if (GetKeyPressed('L')) {
			//ovr_DismissHSWDisplay(oculus->head_mounted_display);
		}
		if (GetKeyToggled('1')) {
			use_predictive = !use_predictive;
			std::cout << "Using prediction: " << use_predictive << std::endl;
		}
		if (GetKeyToggled('9')) {
			predictive_offset_ms -= 1;
			std::cout << "Predictive offset: " << predictive_offset_ms << std::endl;
		}
		if (GetKeyToggled('0')) {
			predictive_offset_ms += 1;
			std::cout << "Predictive offset: " << predictive_offset_ms << std::endl;
		}

		/*ovrFrameTiming begin_timing = ovr_GetFrameTiming(oculus->head_mounted_display, frame_index);
		ovrVector3f hmd_to_eye_view_offset[2] = { oculus->eye_render_descs[0].HmdToEyeViewOffset, oculus->eye_render_descs[1].HmdToEyeViewOffset };
		SetEyeOffsets(hmd_to_eye_view_offset[0], hmd_to_eye_view_offset[1]);

		if (use_predictive) {
			oculus->tracking_state = ovr_GetTrackingState(oculus->head_mounted_display, begin_timing.FrameIntervalSeconds+static_cast<float>(predictive_offset_ms)/1000.0f);
		} else {
			oculus->tracking_state = ovr_GetTrackingState(oculus->head_mounted_display, ovr_GetTimeInSeconds());
		}
		ovr_CalcEyePoses(oculus->tracking_state.HeadPose.ThePose, hmd_to_eye_view_offset,
			oculus->eye_rendering_pose);*/
	}
}

void InputHandler::ResetHeadTracking() {
	head_pose_center = GetHeadPose().location_.location_;
}

bool InputHandler::GetKeyPressed(char key) const {
	return keyboard_state[key] & 0x80;
}

bool InputHandler::GetKeyToggled(char key, bool now_pressed_down) const {
	return (((keyboard_state[key] & 0x80) == 0x80) == now_pressed_down) && (((prev_keyboard_state[key] & 0x80) == 0x80) != now_pressed_down);
}

std::array<std::atomic<int>, 2>& InputHandler::GetMutableMouseMotion() {
	return mouse_motion;
}

std::array<int, 2> InputHandler::ConsumeMouseMotion() 
{
	return{ { mouse_motion[0].fetch_and(0), mouse_motion[1].fetch_and(0) } };
}

Pose InputHandler::GetHeadPose() const {
	return oculus->GetHeadPose();
}

array<float, 3> InputHandler::GetHeadOffset() const {
	array<float, 3> current_position = GetHeadPose().location_.location_;
	return{ current_position[0] - head_pose_center[0],
		current_position[1] - head_pose_center[1],
		current_position[2] - head_pose_center[2] };
}

array<float, 3> InputHandler::GetEyeOffset(int eye_number) const {
	//std::array<float, 4> head_orientation = HeadsetHelper::ConvertQuaternionToArray(GetHeadPose().ThePose.Orientation);
	
	//DirectX::XMVECTOR new_offset = DirectX::XMVector3Transform(
	//	DirectX::XMVectorSet(eye_pose_offsets[eye_number].x, eye_pose_offsets[eye_number].y, eye_pose_offsets[eye_number].z, 1.0f),
	//	DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(head_orientation[0], head_orientation[1], head_orientation[2], head_orientation[3])));
	//ovrVector3f rotated_eye_offset = { DirectX::XMVectorGetX(new_offset) / DirectX::XMVectorGetW(new_offset), DirectX::XMVectorGetY(new_offset) / DirectX::XMVectorGetW(new_offset), DirectX::XMVectorGetZ(new_offset) / DirectX::XMVectorGetW(new_offset) };
	//return oculus->GetEyePose(eye_number).location_.location_;
	return {0, 0, 0};
}

array<float, 3> InputHandler::GetActiveEyeOffset() const {
	return GetEyeOffset(active_eye_number);
}

unsigned int InputHandler::GetCurrentFrameIndex() const {
	return current_frame_index;
}

void InputHandler::SetActiveEyeNumber(int eye_number) {
	active_eye_number = eye_number;
}

void InputHandler::SetEyeOffsets(array<float, 3> left_eye_offset, array<float, 3> right_eye_offset) {
	eye_pose_offsets[0] = left_eye_offset;
	eye_pose_offsets[1] = right_eye_offset;
}

void InputHandler::SetHeadsetState(Headset* ocu) {
	oculus = ocu;
}

bool InputHandler::IsHeadsetActive() const {
	return (oculus != NULL);
}

void InputHandler::SetUsePredictiveTiming(bool new_use_predictive) {
	use_predictive = new_use_predictive;
}

void InputHandler::SetPredictiveTiming(int new_predictive_offset_ms) {
	predictive_offset_ms = new_predictive_offset_ms;
}