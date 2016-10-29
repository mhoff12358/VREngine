#pragma once
#include "stdafx.h"

#include "StlHelper.h"
#include "Kinect.h"

void KinectBackingClassesInterface() {
	class_<CameraSpacePoint, boost::noncopyable>("CameraSpacePoint", no_init)
		.def_readonly("X", &CameraSpacePoint::X)
		.def_readonly("Y", &CameraSpacePoint::Y)
		.def_readonly("Z", &CameraSpacePoint::Z);

	enum_<TrackingState>("TrackingState")
		.value("NotTracked", TrackingState_NotTracked)
		.value("Inferred", TrackingState_Inferred)
		.value("Tracked", TrackingState_Tracked);

	enum_<JointType>("JointType")
		.value("SpineBase", JointType_SpineBase)
		.value("SpineMid", JointType_SpineMid)
		.value("Neck", JointType_Neck)
		.value("Head", JointType_Head)
		.value("ShoulderLeft", JointType_ShoulderLeft)
		.value("ElbowLeft", JointType_ElbowLeft)
		.value("WristLeft", JointType_WristLeft)
		.value("HandLeft", JointType_HandLeft)
		.value("ShoulderRight", JointType_ShoulderRight)
		.value("ElbowRight", JointType_ElbowRight)
		.value("WristRight", JointType_WristRight)
		.value("HandRight", JointType_HandRight)
		.value("HipLeft", JointType_HipLeft)
		.value("KneeLeft", JointType_KneeLeft)
		.value("AnkleLeft", JointType_AnkleLeft)
		.value("FootLeft", JointType_FootLeft)
		.value("HipRight", JointType_HipRight)
		.value("KneeRight", JointType_KneeRight)
		.value("AnkleRight", JointType_AnkleRight)
		.value("FootRight", JointType_FootRight)
		.value("SpineShoulder", JointType_SpineShoulder)
		.value("HandTipLeft", JointType_HandTipLeft)
		.value("ThumbLeft", JointType_ThumbLeft)
		.value("HandTipRight", JointType_HandTipRight)
		.value("ThumbRight", JointType_ThumbRight);

	enum_<HandState>("HandState")
		.value("Unknown", HandState_Unknown)
		.value("NotTracked", HandState_NotTracked)
		.value("Open", HandState_Open)
		.value("Closed", HandState_Closed)
		.value("Lasso", HandState_Lasso);

	enum_<TrackingConfidence>("TrackingConfidence")
		.value("Low", TrackingConfidence_Low)
		.value("High", TrackingConfidence_High);

	class_<_Joint>("Joint")
		.def_readonly("position", &Joint::Position)
		.def_readonly("joint_type", &Joint::JointType)
		.def_readonly("tracking_state", &Joint::TrackingState);
	class_<JointOrientation>("JointOrientation")
		.def_readonly("joint_type", &JointOrientation::JointType)
		.def_readonly("orientation", &JointOrientation::Orientation);
	CreateArray<HandState, 2>("HandState");
	CreateArray<TrackingConfidence, 2>("TrackingConfidence");
	CreateArray<Joint, JointType_Count>("Joint");
	CreateArray<JointOrientation, JointType_Count>("JointOrientation");
}