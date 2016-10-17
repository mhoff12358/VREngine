#pragma once

class_<game_scene::commands::HeadsetListenerRegistration, bases<game_scene::CommandArgs>, boost::noncopyable>("ListenerRegistration", no_init)
	.def_readonly("register_not_unregister", &game_scene::commands::HeadsetListenerRegistration::register_not_unregister_)
	.def_readonly("actor_to_register", &game_scene::commands::HeadsetListenerRegistration::actor_to_register_)
	.def_readonly("listener_id", &game_scene::commands::HeadsetListenerRegistration::listener_id_);

class_<game_scene::commands::ControllerMovement, bases<game_scene::CommandArgs>, boost::noncopyable>("ControllerMovement", no_init)
	.def_readonly("movement_vector", &game_scene::commands::ControllerMovement::movement_vector_);

class_<game_scene::commands::ControllerInformation, bases<game_scene::CommandArgs>, boost::noncopyable>("ControllerInformation", no_init)
	.def_readonly("controller_number", &game_scene::commands::ControllerInformation::controller_number_);

class_<game_scene::commands::TriggerStateChange, bases<game_scene::commands::ControllerInformation>, boost::noncopyable>("TriggerStateChange", no_init)
	.def_readonly("is_pulled", &game_scene::commands::TriggerStateChange::is_pulled_);

class_<game_scene::commands::TouchpadMotion, bases<game_scene::commands::ControllerInformation>, boost::noncopyable>("TouchpadMotion", no_init)
	.def_readonly("position", &game_scene::commands::TouchpadMotion::position_)
	.def_readonly("delta", &game_scene::commands::TouchpadMotion::delta_);