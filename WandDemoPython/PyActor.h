#pragma once
#include "stdafx.h"

#include "SceneSystem/stdafx.h"

#include "SceneSystem/BoostPythonWrapper.h"
#include "SceneSystem/Shmactor.h"
#include "SceneSystem/Scene.h"

struct PyActor : public game_scene::Shmactor, public boost::python::wrapper<game_scene::Shmactor> {
public:
	object self_;
	// Wrap existing calls that work to be virtualizable through Python.
	void HandleCommand(const game_scene::CommandArgs& args) {
		if (boost::python::override HandleCommand = this->get_override("HandleCommand")) {
			try {
				self_.attr("HandleCommand")(boost::ref(args));
				//HandleCommand(boost::ref(args));
			} catch (error_already_set) {
				PyErr_Print();
			}
		}
		Shmactor::HandleCommand(args);
	}
	void default_HandleCommand(const game_scene::CommandArgs& args) {
		this->Shmactor::HandleCommand(args);
	}

	void AddedToScene() {
		if (boost::python::override AddedToScene = this->get_override("AddedToScene")) {
			try {
				AddedToScene();
			} catch (error_already_set) {
				PyErr_Print();
			}
		}
		Shmactor::AddedToScene();
	}
	void default_AddedToScene() {
		this->Shmactor::AddedToScene();
	}

	// Creates a wrapper for the AnswerQuery call that passes around query results as auto_ptrs rather than unique_ptrs.
	//object PyAnswerQuery(const game_scene::QueryArgs& args) {
	std::auto_ptr<game_scene::QueryResult> PyAnswerQuery(const game_scene::QueryArgs& args) {
		object a;
		try {
			a = this->get_override("AnswerQuery")(boost::ref(args));
		} catch (error_already_set) {
			PyErr_Print();
		}
		return extract<std::auto_ptr<game_scene::QueryResult>>(a);
	}
	unique_ptr<game_scene::QueryResult> AnswerQuery(const game_scene::QueryArgs& args) override;

	static game_scene::Scene& GetScene() { return *scene_; }

	void EmbedSelf(object self) {
		self_ = self;
	}

	game_scene::ActorId GetId() {
		return id_;
	}
};