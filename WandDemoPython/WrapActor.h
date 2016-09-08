#pragma once

#include "boost/python.hpp"
#include "SceneSystem/Shmactor.h"
/*
struct WrapActor : game_scene::Shmactor, boost::python::wrapper<game_scene::Shmactor>
{
	void HandleCommand(const game_scene::CommandArgs& args) {
		if (boost::python::override HandleCommand = this->get_override("HandleCommand"))
			HandleCommand(args);
		WrapActor::HandleCommand(args);
	}
	void default_HandleCommand(const game_scene::CommandArgs& args) {
		this->WrapActor::HandleCommand(args);
	}

	unique_ptr<game_scene::QueryResult> AnswerQuery(const game_scene::QueryArgs& args) {
		if (boost::python::override AnswerQuery = this->get_override("AnswerQuery"))
			return move(AnswerQuery(args));
		return move(WrapActor::AnswerQuery(args));
	}
	unique_ptr<game_scene::QueryResult> default_AnswerQuery(const game_scene::QueryArgs& args) {
		return move(this->WrapActor::AnswerQuery(args));
	}

	void AddedToScene() {
		if (boost::python::override AddedToScene = this->get_override("AddedToScene"))
			AddedToScene();
		WrapActor::AddedToScene();
	}
	void default_AddedToScene() {
		this->WrapActor::AddedToScene();
	}
};*/