#include "stdafx.h"
#include "PoseInterface.h"

#include "StlHelper.h"
#include "VRBackend/Pose.h"

string PrintLocation(const Location& location) {
	return "Location: [" + std::to_string(location[0]) + ", " + std::to_string(location[1]) + ", " + std::to_string(location[2]) + "]";
}

void PoseInterface() {
	auto scale = class_<Scale>("Scale")
		.def(init<float>())
		.def(init<float, float, float>())
		.def("Inverse", &Scale::Inverse)
		.def(boost::python::self * boost::python::self)
		.def("__str__", (string(*)(const Scale&))&std::to_string)
		.def("__repr__", (string(*)(const Scale&))&std::to_string)
		.def("__eq__", &Scale::operator==)
		.def("get", &Scale::operator[])
		.def("__copy__", &CopyObject<Scale>);

	auto location =class_<Location>("Location")
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
		.def("Inverse", &Location::Inverse)
		.def("get", &Location::operator[])
		.def("__len__", +static_cast<size_t(*)(const Location&)>([](const Location&)->size_t {return 3; }))
		.def("__getitem__", +static_cast<float(*)(Location&, size_t)>([](Location& self, size_t index)->float {return self[index]; }))
		.def("__str__", (string(*)(const Location&))&std::to_string)
		.def("__repr__", (string(*)(const Location&))&std::to_string)
		.def("GetLength", &Location::GetLength)
		.def("GetLengthSquared", &Location::GetLengthSquared)
		.def("GetNormalized", &Location::GetNormalized)
		.def("Dot", &Location::Dot)
		.def("__copy__", &CopyObject<Location>);

	class_<Quaternion>("Quaternion")
		.def(init<float, float, float, float>())
		.def("dot", &Quaternion::dot)
		.def(boost::python::self * boost::python::self)
		.def(boost::python::self * float())
		.def("ToPower", &Quaternion::ToPower)
		.def("Inverse", &Quaternion::Inverse)
		.def("__eq__", &Quaternion::operator==)
		.def("StripAxis", &Quaternion::StripAxis)
		.def("Inverse", &Quaternion::Inverse)
		.def("__str__", (string(*)(const Quaternion&))&std::to_string)
		.def("__repr__", (string(*)(const Quaternion&))&std::to_string)
		.def("Slerp", &Quaternion::Slerp)
		.staticmethod("Slerp")
		.def("Identity", &Quaternion::Identity)
		.staticmethod("Identity")
		.def("RotationAboutAxis", &Quaternion::RotationAboutAxis)
		.staticmethod("RotationAboutAxis")
		.def("RotationBetweenLocations", (Quaternion(*)(const Location&, const Location&, float))&Quaternion::RotationBetweenLocations)
		.staticmethod("RotationBetweenLocations")
		.def("__copy__", &CopyObject<Quaternion>);

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
		.def("UnapplyAfter", &Pose::UnapplyAfter)
		.def("Inverse", &Pose::Inverse)
		.def("Delta", &Pose::Delta)
		.def("__str__", (string(*)(const Pose&))&std::to_string)
		.def("__repr__", (string(*)(const Pose&))&std::to_string)
		.def("__eq__", &Pose::operator==)
		.def("__copy__", &CopyObject<Pose>);

	CreateVector<Location>("Location");
	CreateVector<Scale>("Scale");
	CreateVector<Quaternion>("Quaternion");
	CreateVector<Pose>("Pose");
}