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
	void HandleCommand(game_scene::CommandArgs& args) {
		if (boost::python::override fn_override = this->get_override("HandleCommand")) {
			try {
				PyCommandArgs* downcast_args = dynamic_cast<PyCommandArgs*>(&args);
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
	void default_HandleCommand(game_scene::CommandArgs& args) {
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
		return "PyActor-" + ActorBase::GetName();
	}

	void HandleCommandVirt(game_scene::CommandArgs& args) override {
		HandleCommand(args);
    }
	unique_ptr<game_scene::QueryResult> AnswerQueryVirt(const game_scene::QueryArgs& args) override {
		return AnswerQuery(args);
    }
	void AddedToSceneVirt() override {
		AddedToScene();
    }
	void PrepareToDieVirt() override {
		PrepareToDie();
    }
	void DependencyDyingVirt(const game_scene::ActorId& dying_id) override {
		DependencyDying(dying_id);
    }
	string GetNameVirt() const override {
		return GetName();
    }
};
typedef game_scene::ActorAdapter<PyActorImpl<game_scene::ActorImpl>> PyActor;


template<typename ActorBase>
void EmbedSelfHack(ActorBase& actor, object self) {
	actor.EmbedSelf(self);
}

template<typename ActorBase>
struct CreatePyActor {
	static void Create() {
		typedef PyActorImpl<ActorBase> ActorSpecial;
		string class_name = ActorSpecial::GetName();
		for (int i = 0; i < class_name.length(); i++) {
			if (class_name[i] == '-') {
				class_name[i] = '_';
			}
		}
		std::cout << "Creating: " << class_name << std::endl;
		class_<ActorSpecial, bases<game_scene::ActorImpl>, boost::noncopyable>(class_name.c_str(), init<>())
			.def("HandleCommand", &ActorBase::HandleCommand)
			.def("AddedToScene", &ActorBase::AddedToScene)
			.def("AnswerQuery", &ActorSpecial::PyAnswerQuery)
			.def("EmbedSelf", &EmbedSelfHack<ActorSpecial>)
			.def("GetScene", &ActorBase::GetScene, return_value_policy<reference_existing_object>())
			.add_property("id", &ActorBase::GetId)
			.def("SetScene", &ActorBase::SetScene);
	}
};

#define PYACT_FN_ARG template<typename> typename CreateFn, typename Preface
template<PYACT_FN_ARG>
void CreatePyActors() {
	CreateFn<Preface>::Create();
}

template<PYACT_FN_ARG, template<typename> typename ActorBase, template<typename> typename... FurtherBases>
void CreatePyActors() {
	CreatePyActors<CreateFn, Preface, FurtherBases...>();
	CreatePyActors<CreateFn, ActorBase<Preface>, FurtherBases...>();
}