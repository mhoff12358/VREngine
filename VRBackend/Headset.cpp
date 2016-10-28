#include "stdafx.h"
#include "Headset.h"

#include "Texture.h"

Headset::Headset() {
	for (IBody*& body : raw_bodies_) {
		body = nullptr;
	}
}

bool Headset::IsHeadsetInitialized() {
	return system_ != nullptr;
}

bool Headset::IsKinectInitialized() {
	return sensor_ != nullptr;
}

void Headset::Initialize(vr::IVRSystem* system, IKinectSensor* sensor) {
	if (system) {
		vr::EVRInitError error;
		system_ = system;
		compositor_ = static_cast<vr::IVRCompositor*>(vr::VR_GetGenericInterface(vr::IVRCompositor_Version, &error));

		for (auto& tracked_device_indexes : tracked_device_by_class_) {
			for (auto& index : tracked_device_indexes) {
				index = vr::k_unTrackedDeviceIndexInvalid;
			}
		}

		for (auto eye : eyes_) {
			vr::HmdMatrix34_t eye_transform = system_->GetEyeToHeadTransform(eye);
			array<float, 3> eye_offset;
			for (int i = 0; i < 3; i++) {
				eye_offset[i] = eye_transform.m[i][3];
			}
			eye_rendering_poses_[eye].location_ = Location(eye_offset);
		}

		for (unsigned int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			RegisterTrackedObject(i);
		}
	}
	if (sensor) {
		HRESULT hr;
		sensor_ = sensor;

		IBodyFrameSource* body_frame_source_ = nullptr;
		hr = sensor_->get_CoordinateMapper(&coordinate_mapper_);
		if (SUCCEEDED(hr)) {
			hr = sensor_->get_BodyFrameSource(&body_frame_source_);
		}
		if (SUCCEEDED(hr)) {
			hr = body_frame_source_->OpenReader(&body_frame_reader_);
		}

		if (FAILED(hr)) {
			sensor_ = nullptr;
		}
	}
}

void Headset::CreateEyeTextures(ID3D11Device* dev, ID3D11DeviceContext* dev_con, D3D11_TEXTURE2D_DESC texture_desc) {
	array<int, 2> viewport = GetEyeViewport();
	for (int i = 0; i < 2; i++) {
		Texture& eye_texture = eye_textures_[i];
		eye_texture = Texture(TextureUsage::RenderTarget());
		eye_texture.Initialize(dev, dev_con, viewport, &texture_desc);

		vr::Texture_t& eye_texture_submission = eye_texture_submissions_[i];
		eye_texture_submission.handle = eye_texture.GetTexture();
		eye_texture_submission.eType = vr::EGraphicsAPIConvention::API_DirectX;
		eye_texture_submission.eColorSpace = vr::EColorSpace::ColorSpace_Auto;
	}
}

void Headset::Cleanup() {

}

bool Headset::ProcessAndReturnLatestEvent(vr::VREvent_t* output_event) {
	return system_->PollNextEvent(output_event, sizeof(vr::VREvent_t));
}

const Texture& Headset::GetEyeTexture(vr::EVREye eye) {
	return eye_textures_[eye];
}

void Headset::SubmitForRendering() {
	for (const vr::EVREye& eye : eyes_) {
		compositor_->Submit(eye, &eye_texture_submissions_[eye]);
	}
}

void Headset::UpdateRenderingPoses() {
	compositor_->WaitGetPoses(rendering_trackings_.data(), rendering_poses_.size(), nullptr, 0);
	rendering_poses_[0] = DecomposePoseFromMatrix(rendering_trackings_[0].mDeviceToAbsoluteTracking);
}

void Headset::UpdateGamePoses() {
	if (IsHeadsetInitialized()) {
		system_->GetDeviceToAbsoluteTrackingPose(
			vr::TrackingUniverseStanding, photon_prediction_time_,
			logic_trackings_.data(), logic_trackings_.size());
		// Updates the pose for all devices that aren't invalid.
		for (unsigned int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			if (tracked_device_classes_[i] != vr::TrackedDeviceClass_Invalid) {
				logic_poses_[i] = DecomposePoseFromMatrix(logic_trackings_[i].mDeviceToAbsoluteTracking);
			}
		}

		// Updates the controller state for all active controller devices.
		for (unsigned int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			if (tracked_device_classes_[i] == vr::TrackedDeviceClass_Controller) {
				uint32_t old_packet_stamp = logic_controller_states_[i].unPacketNum;
				system_->GetControllerState(i, &logic_controller_states_[i]);
				if (old_packet_stamp != logic_controller_states_[i].unPacketNum) {
					// Do any additional logic that only happens when the controller button state changes.
				}
			}
		}
	}
	if (IsKinectInitialized()) {
		// Release any old body frame and data if they still exist.
		if (body_frame_) {
			body_frame_->Release();
			body_frame_ = nullptr;
		}

		// Generate a new body frame and data.
		int64_t new_body_frame_time;
		HRESULT hr;
		hr = body_frame_reader_->AcquireLatestFrame(&body_frame_);
		if (SUCCEEDED(hr)) {
			hr = body_frame_->get_RelativeTime(&new_body_frame_time);
		}
		if (SUCCEEDED(hr)) {
			hr = body_frame_->GetAndRefreshBodyData(BODY_COUNT, raw_bodies_.data());
		}
		if (SUCCEEDED(hr)) {
			hr = body_frame_->get_FloorClipPlane(&floor_clip_plane_);
		}
		if (SUCCEEDED(hr)) {
			body_frame_time_delta_ = new_body_frame_time - body_frame_time_;
			body_frame_time_ = new_body_frame_time;
			for (size_t body_index = 0; body_index < BODY_COUNT; body_index++) {
				IBody* raw_body = raw_bodies_[body_index];
				bodies_[body_index].Empty();
				BOOLEAN is_tracked = FALSE;
				HRESULT tracking_hr;
				tracking_hr = raw_body->get_IsTracked(&is_tracked);
				if (SUCCEEDED(tracking_hr) && is_tracked) {
					uint64_t tracking_id;
					tracking_hr = raw_body->get_TrackingId(&tracking_id);
					if (SUCCEEDED(tracking_hr)) {
						bodies_[body_index].FillFromIBody(tracking_id, raw_body);
						if (tracking_ids_.count(tracking_id) == 0) {
							tracking_ids_.insert(tracking_id);
							new_tracked_ids_.push_back(tracking_id);
							std::cout << "New tracking ID: " << tracking_id << std::endl;
						}
					}
				}
			}
		}
		if (FAILED(hr) && (hr != E_PENDING)) {
			body_frame_time_delta_ = -1;
		}
	}
}

Body& Headset::GetBody(uint64_t tracking_id) {
	size_t body_index = 0;
	for (; body_index < BODY_COUNT; body_index++) {
		if (bodies_[body_index].tracking_id_ == tracking_id) {
			break;
		}
	}
	return bodies_[body_index];
}

vector<uint64_t> Headset::GetNewTrackedIds() {
	if (new_tracked_ids_.empty()) {
		return{};
	}
	return std::move(new_tracked_ids_);
}

Pose Headset::GetHeadPose() const {
	return rendering_poses_[0];
}

Pose Headset::GetEyePose(vr::EVREye eye_number) const {
	Pose head_pose = GetHeadPose();
	head_pose.location_ += eye_rendering_poses_[eye_number].location_.Rotate(head_pose.orientation_);
	return head_pose;
}

DirectX::XMMATRIX Headset::GetEyeProjectionMatrix(vr::EVREye eye_number, float near_dist, float far_dist) const {
	return ToDXMatrix(system_->GetProjectionMatrix(eye_number, near_dist, far_dist, vr::API_DirectX));
}

array<int, 2> Headset::GetEyeViewport() const {
	array<unsigned int, 2> viewport;
	system_->GetRecommendedRenderTargetSize(&viewport[0], &viewport[1]);
	return {static_cast<int>(viewport[0]), static_cast<int>(viewport[1])};
}

array<float, 2> Headset::GetEyeFov(int eye_number) const {
	return{ 1, 1 };
}

void Headset::RegisterTrackedObject(vr::TrackedDeviceIndex_t index) {
	if (index >= vr::k_unMaxTrackedDeviceCount) {
		return;
	}
	vr::TrackedDeviceClass object_class = system_->GetTrackedDeviceClass(index);
	if (tracked_device_classes_[index] == object_class) {
		// Short circuit since the object is already registered.
		return;
	} else if (tracked_device_classes_[index] != vr::TrackedDeviceClass_Invalid) {
		// If there is already a device registered for this index, we've got a weird case.
		// Unregister the existing object so this one can be registered.
		std::cout << "Unregistering object due to weird condition" << std::endl;
		UnregisterTrackedObject(index);
	}
	tracked_device_classes_[index] = object_class;
	for (auto& object_index : tracked_device_by_class_[GetDeviceClassIndex(object_class)]) {
		if (object_index == vr::k_unTrackedDeviceIndexInvalid) {
			object_index = index;
			break;
		}
	}
}

void Headset::UnregisterTrackedObject(vr::TrackedDeviceIndex_t index) {
	if (index >= vr::k_unMaxTrackedDeviceCount) {
		return;
	}
	for (auto& object_index : tracked_device_by_class_[tracked_device_classes_[index]]) {
		if (object_index == index) {
			object_index = vr::k_unTrackedDeviceIndexInvalid;
		}
	}
	tracked_device_classes_[index] = vr::TrackedDeviceClass_Invalid;
}

vr::TrackedDeviceIndex_t Headset::GetDeviceIndex(vr::ETrackedDeviceClass device_class, unsigned int device_number) {
	return tracked_device_by_class_[GetDeviceClassIndex(device_class)][device_number];
}

Pose Headset::GetGamePose(vr::TrackedDeviceIndex_t index) {
	return logic_poses_[index];
}

vr::VRControllerState_t Headset::GetGameControllerState(vr::TrackedDeviceIndex_t index) {
	return logic_controller_states_[index];
}

DirectX::XMMATRIX Headset::ToDXMatrix(const vr::HmdMatrix34_t& matrix) {
	float head_matrix_floats[4][4];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			head_matrix_floats[j][i] = matrix.m[i][j];
		}
	}
	head_matrix_floats[0][3] = 0.0f;
	head_matrix_floats[1][3] = 0.0f;
	head_matrix_floats[2][3] = 0.0f;
	head_matrix_floats[3][3] = 1.0f;
	return DirectX::XMMATRIX((float*)head_matrix_floats);
}

DirectX::XMMATRIX Headset::ToDXMatrix(const vr::HmdMatrix44_t& matrix) {
	float head_matrix_floats[4][4];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			head_matrix_floats[j][i] = matrix.m[i][j];
		}
	}
	return DirectX::XMMATRIX((float*)head_matrix_floats);
}

Pose Headset::DecomposePoseFromMatrix(const vr::HmdMatrix34_t& matrix) {
	Pose result_pose;
	DirectX::XMVECTOR head_scale, head_rot, head_trans;
	DirectX::XMMatrixDecompose(&head_scale, &head_rot, &head_trans, ToDXMatrix(matrix));
	result_pose.location_ = {
		DirectX::XMVectorGetX(head_trans),
		DirectX::XMVectorGetY(head_trans),
		DirectX::XMVectorGetZ(head_trans),
	};
	result_pose.orientation_ = Quaternion(
		DirectX::XMVectorGetX(head_rot),
		DirectX::XMVectorGetY(head_rot),
		DirectX::XMVectorGetZ(head_rot),
		DirectX::XMVectorGetW(head_rot));
	return result_pose;
}
