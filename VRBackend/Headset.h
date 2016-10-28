#ifndef __OCULUS_H_INCLUDED__
#define __OCULUS_H_INCLUDED__

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include "openvr.h"
#include "Kinect.h"

#include <array>
#include <vector>

#include "Quaternion.h"
#include "Pose.h"
#include "Body.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#include "DirectXMath.h"

#include "Texture.h"
#include "ViewState.h"
#include "PerspectiveCamera.h"

class Headset {
public:
	Headset();

	void Initialize(vr::IVRSystem* system, IKinectSensor* sensor);

	void Cleanup();

	Pose GetHeadPose() const;
	Pose GetEyePose(vr::EVREye eye_number) const;
	DirectX::XMMATRIX GetEyeProjectionMatrix(vr::EVREye eye_number, float near_dist, float far_dist) const;

	array<int, 2> GetEyeViewport() const;
	array<float, 2> GetEyeFov(int eye_number) const;

	bool ProcessAndReturnLatestEvent(vr::VREvent_t* output_event);

	void CreateEyeTextures(ID3D11Device* dev, ID3D11DeviceContext* dev_con, D3D11_TEXTURE2D_DESC texture_desc);
	const Texture& GetEyeTexture(vr::EVREye eye);

	void SubmitForRendering();
	void UpdateRenderingPoses();

	bool IsHeadsetInitialized();
	bool IsKinectInitialized();

	void RegisterTrackedObject(vr::TrackedDeviceIndex_t index);
	void UnregisterTrackedObject(vr::TrackedDeviceIndex_t index);
	void UpdateGamePoses();
	vr::TrackedDeviceIndex_t GetDeviceIndex(vr::ETrackedDeviceClass device_class, unsigned int device_number);
	Pose GetGamePose(vr::TrackedDeviceIndex_t index);
	vr::VRControllerState_t GetGameControllerState(vr::TrackedDeviceIndex_t index);

	Body& GetBody(uint64_t tracking_id);
	vector<uint64_t> GetNewTrackedIds();

	static Pose DecomposePoseFromMatrix(const vr::HmdMatrix34_t& matrix);
	static DirectX::XMMATRIX ToDXMatrix(const vr::HmdMatrix34_t& matrix);
	static DirectX::XMMATRIX ToDXMatrix(const vr::HmdMatrix44_t& matrix);

	static constexpr array<vr::EVREye, 2> eyes_ =
		{vr::EVREye::Eye_Left, vr::EVREye::Eye_Right};
	static constexpr array<vr::ETrackedDeviceClass, 4> devices_ =
		{vr::TrackedDeviceClass_HMD, vr::TrackedDeviceClass_Controller, vr::TrackedDeviceClass_TrackingReference, vr::TrackedDeviceClass_Other};
	static constexpr unsigned int GetDeviceClassIndex(const vr::ETrackedDeviceClass class_id) {
		return (class_id == vr::TrackedDeviceClass_HMD) ? 0 : (
			(class_id == vr::TrackedDeviceClass_Controller) ? 1 : (
			(class_id == vr::TrackedDeviceClass_TrackingReference) ? 2 : (
			/*implidied vr::TrackedDeviceClass_Other*/ 3
			)));
	}

private:
	// Interfaces to openvr
	vr::IVRSystem* system_;
	vr::IVRCompositor* compositor_;

	// Interfaces to kinect
	IKinectSensor* sensor_;
	IBodyFrameReader* body_frame_reader_;
	ICoordinateMapper* coordinate_mapper_;

	// Internal resource storage
	array<Texture, 2> eye_textures_;
	array<vr::Texture_t, 2> eye_texture_submissions_;

	// Rendering state variables
	array<vr::TrackedDevicePose_t, 1> rendering_trackings_;
	array<Pose, 1> rendering_poses_;
	array<Pose, 2> eye_rendering_poses_;

	// Logic state variables
	array<vr::ETrackedDeviceClass, vr::k_unMaxTrackedDeviceCount> tracked_device_classes_;
	array<array<vr::TrackedDeviceIndex_t, vr::k_unMaxTrackedDeviceCount>, 2> tracked_device_by_class_;
	array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> logic_trackings_;
	array<Pose, vr::k_unMaxTrackedDeviceCount> logic_poses_;
	array<vr::VRControllerState_t, vr::k_unMaxTrackedDeviceCount> logic_controller_states_;

	// body_frame_time_delta_ is -1 if the current frame is invalid.
	int64_t body_frame_time_ = -1;
	int64_t body_frame_time_delta_ = -1;
	IBodyFrame* body_frame_ = nullptr;
	Vector4 floor_clip_plane_;
	array<IBody*, BODY_COUNT> raw_bodies_;
	array<Body, BODY_COUNT+1> bodies_;
	set<uint64_t> tracking_ids_;
	vector<uint64_t> new_tracked_ids_;

	float photon_prediction_time_ = 0.0f;
};

#endif