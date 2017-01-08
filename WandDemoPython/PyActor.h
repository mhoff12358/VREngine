#pragma once
#include "stdafx.h"

#include "SceneSystem/stdafx.h"

#include "SceneSystem/BoostPythonWrapper.h"
#include "SceneSystem/Actor.h"
#include "SceneSystem/Scene.h"

#include "PyCommandAndQuery.h"
#include "HandleError.h"

struct PyActor : public game_scene::Actor, public boost::python::wrapper<game_scene::Actor> {
public:
	object self_;
	// Wrap existing calls that work to be virtualizable through Python.
	void HandleCommand(const game_scene::CommandArgs& args) {
		if (boost::python::override HandleCommand = this->get_override("HandleCommand")) {
			try {
				const PyCommandArgs* downcast_args = dynamic_cast<const PyCommandArgs*>(&args);
				if (downcast_args) {
					HandleCommand(downcast_args->self_);
				} else {
					HandleCommand(boost::ref(args));
				}
			} catch (error_already_set) {
				HandleError();
			}
		}
		Actor::HandleCommand(args);
	}
	void default_HandleCommand(const game_scene::CommandArgs& args) {
		this->Actor::HandleCommand(args);
	}

	void AddedToScene() {
		if (boost::python::override AddedToScene = this->get_override("AddedToScene")) {
			try {
				AddedToScene();
			} catch (error_already_set) {
				HandleError();
			}
		}
		Actor::AddedToScene();
	}
	void default_AddedToScene() {
		this->Actor::AddedToScene();
	}

	// Creates a wrapper for the AnswerQuery call that passes around query results as auto_ptrs rather than unique_ptrs.
	//object PyAnswerQuery(const game_scene::QueryArgs& args) {
	std::auto_ptr<game_scene::QueryResult> PyAnswerQuery(const game_scene::QueryArgs& args) {
		object raw_result;
		try {
			const PyQueryArgs* downcast_args = dynamic_cast<const PyQueryArgs*>(&args);
			if (downcast_args) {
				raw_result = this->get_override("AnswerQuery")(downcast_args->self_);
			} else {
				raw_result = this->get_override("AnswerQuery")(boost::ref(args));
			}
		} catch (error_already_set) {
			HandleError();
		}
		extract<std::auto_ptr<game_scene::QueryResult>> extract_attempt(raw_result);
		if (extract_attempt.check()) {
			return static_cast<std::auto_ptr<game_scene::QueryResult>>(extract_attempt);
		} else {
			return std::auto_ptr<game_scene::QueryResult>(make_unique<PyQueryResult>(raw_result).release());
		}
	}
	unique_ptr<game_scene::QueryResult> AnswerQuery(const game_scene::QueryArgs& args) override;

	void EmbedSelf(object self) {
		self_ = self;
	}

	game_scene::ActorId GetId() {
		return id_;
	}
};