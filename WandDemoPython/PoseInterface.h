#pragma once
#include "stdafx.h"
#include "StlHelper.h"

#include "VRBackend/Pose.h"

#include <string>

string PrintLocation(const Location& location) {
	return "Location: [" + std::to_string(location[0]) + ", " + std::to_string(location[1]) + ", " + std::to_string(location[2]) + "]";
}

void PoseInterface() {
	class_<Scale>("Scale")
		.def(init<float>())
		.def(init<float, float, float>())
		.def(boost::python::self * boost::python::self)
		.def("__eq__", &Scale::operator==)
		.def("__repr__", static_cast<string(*)(const Scale&)>(&std::to_string))
		.def("__str__", static_cast<string(*)(const Scale&)>(&std::to_string))
		.def("get", &Scale::operator[]);

	class_<Location>("Location")
		.def(init<float, float, float>())
		.def(boost::python::self + boost::python::self)
		.def(boost::python::self - boost::python::self)
		.def(boost::python::self * float())
		.def(float() * boost::python::self)
		.def(boost::python::self * Scale())
		.def(boost::python::self / float())
		.def(boost::python::self += boost::python::self)
		.def(boost::python::self -= boost::python::self)
		.def(boost::python::self *= float())
		.def(boost::python::self /= float())
		.def("__eq__", &Location::operator==)
		.def("Rotate", &Location::Rotate)
		.def("__repr__", static_cast<string(*)(const Location&)>(&std::to_string))
		.def("__str__", static_cast<string(*)(const Location&)>(&std::to_string))
		.def("get", &Location::operator[])
		.def("__len__", +static_cast<size_t(*)(const Location&)>([](const Location&)->size_t {return 3; }))
		.def("__getitem__", +static_cast<float(*)(Location&, size_t)>([](Location& self, size_t index)->float {return self[index]; }))
		.def("GetLength", &Location::GetLength)
		.def("GetLengthSquared", &Location::GetLengthSquared)
		.def("GetNormalized", &Location::GetNormalized)
		.def("Dot", &Location::Dot);

	class_<Quaternion>("Quaternion")
		.def(init<float, float, float, float>())
		.def("dot", &Quaternion::dot)
		.def(boost::python::self * boost::python::self)
		.def(boost::python::self * float())
		.def("ToPower", &Quaternion::ToPower)
		.def("Inverse", &Quaternion::Inverse)
		.def("__eq__", &Quaternion::operator==)
		.def("StripAxis", &Quaternion::StripAxis)
		.def("__repr__", static_cast<string(*)(const Quaternion&)>(&std::to_string))
		.def("__str__", static_cast<string(*)(const Quaternion&)>(&std::to_string))
		.def("Slerp", &Quaternion::Slerp)
		.staticmethod("Slerp")
		.def("Identity", &Quaternion::Identity)
		.staticmethod("Identity")
		.def("RotationAboutAxis", &Quaternion::RotationAboutAxis)
		.staticmethod("RotationAboutAxis");

	enum_<AxisID>("AxisID")
		.value("x", AID_X)
		.value("y", AID_Y)
		.value("z", AID_Z)
		.value("xy", AID_XY)
		.value("xz", AID_XZ)
		.value("yz", AID_YZ)
		.value("xyz", AID_XYZ);

	class_<Pose>("Pose")
		.def(init<Location, Quaternion, Scale>())
		.def(init<Location, Quaternion>())
		.def(init<Location, Scale>())
		.def(init<Quaternion, Scale>())
		.def(init<Location>())
		.def(init<Quaternion>())
		.def(init<Scale>())
		.def_readwrite("location", &Pose::location_)
		.def_readwrite("scale", &Pose::scale_)
		.def_readwrite("orientation", &Pose::orientation_)
		.def("ApplyAfter", &Pose::ApplyAfter)
		.def("Inverse", &Pose::Inverse)
		.def("Delta", &Pose::Delta)
		.def("__eq__", &Pose::operator==)
		.def("__repr__", static_cast<string(*)(const Pose&)>(&std::to_string))
		.def("__str__", static_cast<string(*)(const Pose&)>(&std::to_string));

	CreateVector<Location>("Location");
	CreateVector<Scale>("Scale");
	CreateVector<Quaternion>("Quaternion");
	CreateVector<Pose>("Pose");
}