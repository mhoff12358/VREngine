#pragma once
#include "stdafx.h"

#include "SceneSystem/stdafx.h"

#include "SceneSystem/BoostPythonWrapper.h"
#include "SceneSystem/Actor.h"
#include "SceneSystem/Scene.h"

#include "PyCommandAndQuery.h"
#include "HandleError.h"

template<typename ActorBase>
struct PyActorImpl : public ActorBase, public boost::python::wrapper<game_scene::IActor> {
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
	unique_ptr<game_scene::QueryResult> AnswerQuery(const game_scene::QueryArgs& args) {
		return unique_ptr<game_scene::QueryResult>(PyAnswerQuery(args).release());
	}

	void EmbedSelf(object self) {
		self_ = self;
	}

	static string GetName() {
		return "PyActor-" + game_scene::ActorImpl::GetName();
	}
};
typedef game_scene::ActorAdapter<PyActorImpl<game_scene::ActorImpl>> PyActor;



template<typename ActorBase>
struct CreatePyActor {
	static void Create() {
		typedef game_scene::ActorAdapter<PyActorImpl<ActorBase>> ActorSpecial;
		string class_name = ActorSpecial::GetName();
		for (int i = 0; i < class_name.length(); i++) {
			if (class_name[i] == '-') {
				class_name[i] = '_';
			}
		}
		class_<ActorSpecial, bases<game_scene::ActorImpl>, boost::noncopyable>(class_name.c_str(), init<>())
			.def("HandleCommand", &PyActor::HandleCommandVirt)
			.def("AddedToScene", &PyActor::AddedToSceneVirt)
			.def("AnswerQuery", &PyActor::PyAnswerQuery)
			.def("EmbedSelf", &EmbedSelfHack)
			.def("GetScene", &PyActor::GetScene, return_value_policy<reference_existing_object>())
			.add_property("id", &PyActor::GetId)
			.def("SetScene", &PyActor::SetScene);
	}
};

#define PYACT_FN_ARG template<typename> typename CreateFn, typename Preface
template<PYACT_FN_ARG>
void CreatePyActors() {
	CreatePyActor<Preface>::Create();
}

template<PYACT_FN_ARG, typename ActorBase, typename... FurtherBases>
void CreatePyActors() {
	CreatePyActors<CreateFn, Preface, FurtherBases>();
	CreatePyActors<CreateFn, ActorBase<Preface>, FurtherBases>();
}