#include "stdafx.h"
#include "Body.h"

void Body::Empty() {
	filled_ = false;
}

void Body::FillFromIBody(uint64_t tracking_id, IBody* body) {
	HRESULT hr;
	hr = body->get_HandLeftState(&hands_[LEFT]);
	if (SUCCEEDED(hr)) {
		hr = body->get_HandLeftConfidence(&hand_confidences_[LEFT]);
	}
	if (SUCCEEDED(hr)) {
		hr = body->get_HandRightState(&hands_[RIGHT]);
	}
	if (SUCCEEDED(hr)) {
		hr = body->get_HandRightConfidence(&hand_confidences_[RIGHT]);
	}
	if (SUCCEEDED(hr)) {
		hr = body->GetJoints(JointType_Count, joints_.data());
	}
	if (SUCCEEDED(hr)) {
		hr = body->GetJointOrientations(JointType_Count, joint_orientations_.data());
	}
	tracking_id_ = tracking_id;
	filled_ = true;
}

Joint& Body::GetJoint(JointType joint_type) {
	for (Joint& joint : joints_) {
		if (joint.JointType == joint_type) {
			return joint;
		}
	}
	return *(Joint*)nullptr;
}