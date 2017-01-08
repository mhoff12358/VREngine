#include "stdafx.h"
#include "HeadsetInterface.h"

#include "PyScene.h"
#include "SceneSystem/HeadsetInterface.h"

BOOST_PTR_MAGIC(game_scene::commands::HeadsetListenerRegistration)
BOOST_PTR_MAGIC(game_scene::commands::HapticPulse)

void HeadsetInterface(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	enum_<game_scene::actors::HeadsetInterface::ListenerId>("HeadsetListenerId")
		.value("TRIGGER_STATE_CHANGE", game_scene::actors::HeadsetInterface::ListenerId::TRIGGER_STATE_CHANGE)
		.value("TOUCHPAD_SLIDE", game_scene::actors::HeadsetInterface::ListenerId::TOUCHPAD_SLIDE)
		.value("TOUCHPAD_DRAG", game_scene::actors::HeadsetInterface::ListenerId::TOUCHPAD_DRAG)
		.value("CONTROLLER_MOVEMENT", game_scene::actors::HeadsetInterface::ListenerId::CONTROLLER_MOVEMENT);

	class_<game_scene::HeadsetInterfaceCommand>("HeadsetInterfaceCommand")
		.def_readonly("REGISTER_LISTENER", &game_scene::HeadsetInterfaceCommand::REGISTER_LISTENER)
		.def_readonly("LISTEN_TRIGGER_STATE_CHANGE", &game_scene::HeadsetInterfaceCommand::LISTEN_TRIGGER_STATE_CHANGE)
		.def_readonly("LISTEN_TOUCHPAD_SLIDE", &game_scene::HeadsetInterfaceCommand::LISTEN_TOUCHPAD_SLIDE)
		.def_readonly("LISTEN_TOUCHPAD_DRAG", &game_scene::HeadsetInterfaceCommand::LISTEN_TOUCHPAD_DRAG)
		.def_readonly("LISTEN_CONTROLLER_MOVEMENT", &game_scene::HeadsetInterfaceCommand::LISTEN_CONTROLLER_MOVEMENT);

	class_<game_scene::commands::HeadsetListenerRegistration, bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::commands::HeadsetListenerRegistration>,
	 	boost::noncopyable>("HeadsetListenerRegistration",
		init<bool, game_scene::ActorId, game_scene::actors::HeadsetInterface::ListenerId, unsigned char>())
		.def(init<bool, game_scene::ActorId, game_scene::actors::HeadsetInterface::ListenerId>())
		.def_readonly("register_not_unregister", &game_scene::commands::HeadsetListenerRegistration::register_not_unregister_)
		.def_readonly("actor_to_register", &game_scene::commands::HeadsetListenerRegistration::actor_to_register_)
		.def_readonly("listener_id", &game_scene::commands::HeadsetListenerRegistration::listener_id_);
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::commands::HeadsetListenerRegistration>);

	class_<game_scene::commands::HapticPulse, bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::commands::HapticPulse>,
		boost::noncopyable>("HapticPulse", no_init)
		.def(init<unsigned char, uint32_t, uint32_t>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::commands::HapticPulse>);

	class_<game_scene::commands::ControllerInformation, bases<game_scene::CommandArgs>, boost::noncopyable>("ControllerInformation", no_init)
		.def_readonly("number", &game_scene::commands::ControllerInformation::controller_number_)
		.def_readonly("position", &game_scene::commands::ControllerInformation::controller_position_);

	class_<game_scene::commands::ControllerMovement, bases<game_scene::commands::ControllerInformation>, boost::noncopyable>("ControllerMovement", no_init)
		.def_readonly("movement", &game_scene::commands::ControllerMovement::movement_);

	class_<game_scene::commands::TriggerStateChange, bases<game_scene::commands::ControllerInformation>, boost::noncopyable>("TriggerStateChange", no_init)
		.def_readonly("is_pulled", &game_scene::commands::TriggerStateChange::is_pulled_);

	class_<vr::VRControllerAxis_t, boost::noncopyable>("ControllerAxis", no_init)
		.def_readonly("x", &vr::VRControllerAxis_t::x)
		.def_readonly("y", &vr::VRControllerAxis_t::y);

	class_<game_scene::commands::TouchpadMotion, bases<game_scene::commands::ControllerInformation>, boost::noncopyable>("TouchpadMotion", no_init)
		.def_readonly("position", &game_scene::commands::TouchpadMotion::position_)
		.def_readonly("delta", &game_scene::commands::TouchpadMotion::delta_);
}