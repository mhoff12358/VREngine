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
			object result;
			try {
				PyCommandArgs* downcast_args = dynamic_cast<PyCommandArgs*>(&args);
				if (downcast_args) {
					result = fn_override(downcast_args->self_);
				} else {
					result = fn_override(boost::ref(args));
				}
			} catch (error_already_set) {
				HandleError();
			}
		} else {
			ActorBase::HandleCommand(args);
		}
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
		if (boost::python::override fn_override = this->get_override("AnswerQuery")) {
			object raw_result;
			try {
				const PyQueryArgs* downcast_args = dynamic_cast<const PyQueryArgs*>(&args);
				if (downcast_args) {
					raw_result = fn_override(downcast_args->self_);
				}
				else {
					raw_result = fn_override(boost::ref(args));
				}
			}
			catch (error_already_set) {
				HandleError();
			}
			if (raw_result == object()) {
				return std::auto_ptr<game_scene::QueryResult>(nullptr);
			}
			extract<std::auto_ptr<game_scene::QueryResult>> extract_attempt(raw_result);
			if (extract_attempt.check()) {
				return static_cast<std::auto_ptr<game_scene::QueryResult>>(extract_attempt);
			}
			else {
				return std::auto_ptr<game_scene::QueryResult>(make_unique<PyQueryResult>(raw_result).release());
			}
		}
		else {
			return std::auto_ptr<game_scene::QueryResult>(nullptr);
		}
	}

	std::auto_ptr<game_scene::QueryResult> default_PyAnswerQuery(const game_scene::QueryArgs& args) {
		return std::auto_ptr<game_scene::QueryResult>(ActorBase::AnswerQuery(args).release());
	}
	unique_ptr<game_scene::QueryResult> AnswerQuery(const game_scene::QueryArgs& args) {
		std::auto_ptr<game_scene::QueryResult> result(PyAnswerQuery(args));
		if (result.get() != nullptr) {
			return unique_ptr<game_scene::QueryResult>(PyAnswerQuery(args).release());
		} else {
			return ActorBase::AnswerQuery(args);
		}
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

template<template<typename> typename ActorMixin>
struct WrapActorMixin {
  template<typename ActorImplChain>
  struct Level2 {
    typedef ActorImplChain SubChain;

    typedef ActorMixin<ActorImplChain> WholeChain;

    typedef PyActorImpl<ActorMixin<ActorImplChain>> WithPyActor;

    static void CreateChain(string name) {
      std::cout << "CREATING BASE CLASS: " << name << std::endl;
      class_<WholeChain, bases<SubChain>, boost::noncopyable>(name.c_str(), init<>());
    }
  };
};

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
			.def("HandleCommand", &ActorSpecial::HandleCommand, &ActorSpecial::default_HandleCommand)
			.def("ParentHandleCommand", &ActorSpecial::default_HandleCommand)
			.def("BaseHandleCommand", &ActorBase::HandleCommand)
			.def("AddedToScene", &ActorBase::AddedToScene)
			.def("AnswerQuery", &ActorSpecial::PyAnswerQuery, &ActorSpecial::default_PyAnswerQuery)
			.def("EmbedSelf", &EmbedSelfHack<ActorSpecial>)
			.def("GetScene", &ActorBase::GetScene, return_value_policy<reference_existing_object>())
			.add_property("id", &ActorBase::GetId)
			.def("SetScene", &ActorBase::SetScene);
	}
};

template<typename ActorBaseSource>
struct CreatePyActor2 {
  static void Create(class_<game_scene::Scene, boost::noncopyable>& scene) {
    string class_name = ActorBaseSource::WithPyActor::GetName();
		for (int i = 0; i < class_name.length(); i++) {
			if (class_name[i] == '-') {
				class_name[i] = '_';
			}
		}
		std::cout << "Creating: " << class_name << std::endl;
    string unwrapped_class_name = "unwrapped_" + class_name;
    ActorBaseSource::CreateChain(unwrapped_class_name);
    class_<ActorBaseSource::WithPyActor, bases<ActorBaseSource::WholeChain>, boost::noncopyable>(class_name.c_str(), init<>())
			.def("HandleCommand", &ActorBaseSource::WithPyActor::HandleCommand, &ActorBaseSource::WithPyActor::default_HandleCommand)
			.def("ParentHandleCommand", &ActorBaseSource::WithPyActor::default_HandleCommand)
			.def("BaseHandleCommand", &ActorBaseSource::WholeChain::HandleCommand)
			.def("AddedToScene", &ActorBaseSource::WholeChain::AddedToScene)
			.def("AnswerQuery", &ActorBaseSource::WithPyActor::PyAnswerQuery, &ActorBaseSource::WithPyActor::default_PyAnswerQuery)
			.def("EmbedSelf", &EmbedSelfHack<ActorBaseSource::WithPyActor>)
			.def("GetScene", &ActorBaseSource::WholeChain::GetScene, return_value_policy<reference_existing_object>())
			.add_property("id", &ActorBaseSource::WholeChain::GetId)
			.def("SetScene", &ActorBaseSource::WholeChain::SetScene);
	  //PyScene::AddActorSubclassCreation<ActorBaseSource::WithPyActor>(scene, unwrapped_class_name);
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

template<typename Preface>
void CreatePyActorsImpl(class_<game_scene::Scene, boost::noncopyable>& scene) {
}

template<typename Preface, typename PartialActorBaseSource, typename... FurtherBaseSources>
void CreatePyActorsImpl(class_<game_scene::Scene, boost::noncopyable>& scene) {
  typedef PartialActorBaseSource::Level2<Preface> ActorBaseSource;
  CreatePyActor2<ActorBaseSource>::Create(scene);
  CreatePyActorsImpl<Preface, FurtherBaseSources...>(scene);
  CreatePyActorsImpl<ActorBaseSource::WholeChain, FurtherBaseSources...>(scene);
}

template<typename... MixinSources>
void CreatePyActors2(class_<game_scene::Scene, boost::noncopyable>& scene) {
  /*class_<PyActorImpl<game_scene::ActorImpl>, bases<ActorBaseSource::WholeChain>, boost::noncopyable>(class_name.c_str(), init<>())
    .def("HandleCommand", &ActorBaseSource::WithPyActor::HandleCommand, &ActorBaseSource::WithPyActor::default_HandleCommand)
    .def("ParentHandleCommand", &ActorBaseSource::WithPyActor::default_HandleCommand)
    .def("BaseHandleCommand", &ActorBaseSource::WholeChain::HandleCommand)
    .def("AddedToScene", &ActorBaseSource::WholeChain::AddedToScene)
    .def("AnswerQuery", &ActorBaseSource::WithPyActor::PyAnswerQuery, &ActorBaseSource::WithPyActor::default_PyAnswerQuery)
    .def("EmbedSelf", &EmbedSelfHack<ActorBaseSource::WithPyActor>)
    .def("GetScene", &ActorBaseSource::WholeChain::GetScene, return_value_policy<reference_existing_object>())
    .add_property("id", &ActorBaseSource::WholeChain::GetId)
    .def("SetScene", &ActorBaseSource::WholeChain::SetScene);*/
  CreatePyActorsImpl<game_scene::ActorImpl, MixinSources...>(scene);
}