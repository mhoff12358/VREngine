#pragma once
#include "stdafx.h"

#include "SceneSystem/stdafx.h"

#include "SceneSystem/BoostPythonWrapper.h"
#include "SceneSystem/Actor.h"
#include "SceneSystem/Scene.h"

#include "PyCommandAndQuery.h"
#include "HandleError.h"

template<typename ActorBase>
struct PyActorImpl : public ActorBase, public boost::python::wrapper<ActorBase> {
public:
	object self_;
	// Wrap existing calls that work to be virtualizable through Python.
	void HandleCommand(const game_scene::CommandArgs& args) {
		if (boost::python::override fn_override = this->get_override("HandleCommand")) {
			try {
				const PyCommandArgs* downcast_args = dynamic_cast<const PyCommandArgs*>(&args);
				if (downcast_args) {
					fn_override(downcast_args->self_);
				} else {
					fn_override(boost::ref(args));
				}
			} catch (error_already_set) {
				HandleError();
			}
		}
		ActorBase::HandleCommand(args);
	}
	void default_HandleCommand(const game_scene::CommandArgs& args) {
		this->ActorBase::HandleCommand(args);
	}

	void AddedToScene() {
		if (boost::python::override fn_override = this->get_override("AddedToScene")) {
			try {
				fn_override();
			} catch (error_already_set) {
				HandleError();
			}
		}
		ActorBase::AddedToScene();
	}
	void default_AddedToScene() {
		this->ActorBase::AddedToScene();
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
	unique_ptr<game_scene::QueryResult> AnswerQuery(const game_scene::QueryArgs& args);

	void EmbedSelf(object self) {
		self_ = self;
	}

	string GetName() const {
		return "PyActor-" + game_scene::ActorImpl::GetName();
	}
};
typedef PyActorImpl<game_scene::ActorImpl> PyActor;