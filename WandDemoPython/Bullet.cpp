#include "stdafx.h"
#include "Physics.h"
#include "StlHelper.h"

#include "SceneSystem/BulletInternals.h"

template <typename T>
struct PoseTypeTraits {
	static const char* class_name;
};

const char* PoseTypeTraits<Pose>::class_name = "Pose";
const char* PoseTypeTraits<Location>::class_name = "Location";
const char* PoseTypeTraits<Quaternion>::class_name = "Quaternion";
const char* PoseTypeTraits<Scale>::class_name = "Scale";

template<typename InType, typename OutType, OutType(*raw_construct)(const InType&)>
struct MakeFromConverter {
	MakeFromConverter() {
		boost::python::converter::registry::push_back(
			&Convertible, &Construct, boost::python::type_id<OutType>());
	}

	static void* Convertible(PyObject* obj) {
		if (obj->ob_type->tp_name == string(PoseTypeTraits<InType>::class_name)) {
			return obj;
		}
		return nullptr;
	}

	static void Construct(PyObject* obj, boost::python::converter::rvalue_from_python_stage1_data* data) {
		extract<InType*> extractor(object(boost::python::handle<>(boost::python::borrowed(obj))));
		InType* in = extractor;

		void* storage =
			((boost::python::converter::rvalue_from_python_storage<OutType>*)
				data)->storage.bytes;

		//OutType* out_ptr = new (storage) OutType(raw_construct(*in));
		//OutType temp = raw_construct(*in);
		//*reinterpret_cast<OutType*>(storage) = raw_construct(*in);
		new (storage) OutType(raw_construct(*in));
		data->convertible = storage;
	}
};

string DumpTypeString(boost::python::object obj) {
	PyObject* raw_obj = obj.ptr();
	PyTypeObject* type = raw_obj->ob_type;
	return string(type->tp_name);
}

std::auto_ptr<bullet::RigidBody> MakeRigidBody(std::auto_ptr<bullet::Shape> shape_ptr, Pose transform) {
	bullet::Shape shape(std::move(*shape_ptr));
	auto rigid_body = std::make_unique<bullet::RigidBody>(std::move(shape), bullet::poses::GetTransform(transform));
	return std::auto_ptr<bullet::RigidBody>(rigid_body.release());
}

std::auto_ptr<bullet::RigidBody> MakeRigidBody(std::auto_ptr<bullet::Shape> shape_ptr, Pose transform, btScalar mass) {
	bullet::Shape shape(std::move(*shape_ptr));
	auto rigid_body = std::make_unique<bullet::RigidBody>(std::move(shape), bullet::poses::GetTransform(transform), mass);
	return std::auto_ptr<bullet::RigidBody>(rigid_body.release());
}

std::auto_ptr<bullet::RigidBody> MakeRigidBody(std::auto_ptr<bullet::Shape> shape_ptr, Pose transform, btScalar mass, Location inertia) {
	bullet::Shape shape(std::move(*shape_ptr));
	auto rigid_body = std::make_unique<bullet::RigidBody>(std::move(shape), bullet::poses::GetTransform(transform), mass, bullet::poses::GetVector3(inertia));
	return std::auto_ptr<bullet::RigidBody>(rigid_body.release());
}

std::auto_ptr<bullet::Shape> MakeAutoSphere(btScalar radius) {
	return std::auto_ptr<bullet::Shape>(new bullet::Shape(std::move(bullet::Shape::MakeSphere(radius))));
}

std::auto_ptr<bullet::Shape> MakeAutoPlane(Location normal) {
	return std::auto_ptr<bullet::Shape>(new bullet::Shape(std::move(bullet::Shape::MakePlane(bullet::poses::GetVector3(normal)))));
}

std::auto_ptr<bullet::Shape> MakeAutoPlaneConstant(Location normal, btScalar plane_constant) {
	return std::auto_ptr<bullet::Shape>(new bullet::Shape(std::move(bullet::Shape::MakePlane(bullet::poses::GetVector3(normal), plane_constant))));
}

std::auto_ptr<bullet::Shape> MakeAutoPlanePoint(Location normal, Location point_in_plane) {
	return std::auto_ptr<bullet::Shape>(new bullet::Shape(std::move(bullet::Shape::MakePlane(bullet::poses::GetVector3(normal), bullet::poses::GetVector3(point_in_plane)))));
}

void Bullet(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	//MakeFromConverter < Pose, btTransform, bullet::poses::GetTransform > ();
	//MakeFromConverter < Location, btVector3, bullet::poses::GetVector3 > ();
	//MakeFromConverter < Quaternion, btQuaternion, bullet::poses::GetQuaternion > ();

	class_<bullet::Shape, std::auto_ptr<bullet::Shape>, boost::noncopyable>("Shape", no_init)
		.def("MakeSphere", MakeAutoSphere)
		.staticmethod("MakeSphere")
		.def("MakePlane", MakeAutoPlane)
		.def("MakePlane", MakeAutoPlaneConstant)
		.def("MakePlane", MakeAutoPlanePoint)
		.staticmethod("MakePlane");

	class_<bullet::RigidBody, std::auto_ptr<bullet::RigidBody>, boost::noncopyable>("RigidBody", no_init)
		.def("__init__", boost::python::make_constructor(
			static_cast<std::auto_ptr<bullet::RigidBody>(*)(std::auto_ptr<bullet::Shape>, Pose)>(MakeRigidBody)))
		.def("__init__", boost::python::make_constructor(
			static_cast<std::auto_ptr<bullet::RigidBody>(*)(std::auto_ptr<bullet::Shape>, Pose, btScalar)>(MakeRigidBody)))
		.def("__init__", boost::python::make_constructor(
			static_cast<std::auto_ptr<bullet::RigidBody>(*)(std::auto_ptr<bullet::Shape>, Pose, btScalar, Location)>(MakeRigidBody)));
}
