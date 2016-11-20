#pragma once
#include "stdafx.h"

#include "SceneSystem/BezierCurves.h"

void BezierCurvesInterface() {
	class_<BezierCurve>("BezierCurve", init<vector<Location>>)
		.def("At", &BezierCurve::At);
}
