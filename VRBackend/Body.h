#pragma once

#include "stdafx.h"

#include "Kinect.h"

class Body {
public:
	Body() {}

	enum : unsigned char {
		LEFT = 0,
		RIGHT = 1
	};

	void Empty();
	void FillFromIBody(uint64_t tracking_id, IBody* body);

	bool filled_ = false;
	uint64_t tracking_id_;
	array<HandState, 2> hands_;
	array<TrackingConfidence, 2> hand_confidences_;
	array<Joint, JointType_Count> joints_;
	array<JointOrientation, JointType_Count> joint_orientations_;
};