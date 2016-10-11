#pragma once

enum_<game_scene::actors::IOInterface::ListenerId>("ListenerId")
	.value("MOUSE_MOTION", game_scene::actors::IOInterface::ListenerId::MOUSE_MOTION)
	.value("KEY_PRESS", game_scene::actors::IOInterface::ListenerId::KEY_PRESS)
	.value("KEY_RELEASE", game_scene::actors::IOInterface::ListenerId::KEY_RELEASE)
	.value("KEY_TOGGLE", game_scene::actors::IOInterface::ListenerId::KEY_TOGGLE);

class_<game_scene::IOInterfaceCommand>("IOInterfaceCommand")
	.def_readonly("REGISTER_LISTENER", &game_scene::IOInterfaceCommand::REGISTER_LISTENER)
	.def_readonly("LISTEN_MOUSE_MOTION", &game_scene::IOInterfaceCommand::LISTEN_MOUSE_MOTION)
	.def_readonly("LISTEN_KEY_PRESS", &game_scene::IOInterfaceCommand::LISTEN_KEY_PRESS)
	.def_readonly("LISTEN_KEY_RELEASE", &game_scene::IOInterfaceCommand::LISTEN_KEY_RELEASE)
	.def_readonly("LISTEN_KEY_TOGGLE", &game_scene::IOInterfaceCommand::LISTEN_KEY_TOGGLE);

class_<game_scene::commands::IOListenerRegistration, bases<game_scene::CommandArgs>, std::auto_ptr<game_scene::commands::IOListenerRegistration>, boost::noncopyable>("IOListenerRegistration",
	init<bool, game_scene::ActorId, game_scene::actors::IOInterface::ListenerId>())
	//.def(init<bool, game_scene::ActorId, game_scene::actors::IOInterface::ListenerId, vector<unsigned char>>())
	.def(init<bool, game_scene::ActorId, game_scene::actors::IOInterface::ListenerId, unsigned char>());
scene_registration.def("MakeCommandAfter", &PyScene::MakeCommandAfter<game_scene::commands::IOListenerRegistration>);

class_<game_scene::commands::MouseMotion, bases<game_scene::CommandArgs>, boost::noncopyable>("MouseMotion", no_init)
	.def_readonly("motion", &game_scene::commands::MouseMotion::motion_);

class_<game_scene::commands::KeyToggle, bases<game_scene::CommandArgs>, boost::noncopyable>("KeyToggle", no_init)
	.def_readonly("key", &game_scene::commands::KeyToggle::key_);