#include "stdafx.h"
#include "BezierCurvesInterface.h"

#include "SceneSystem/BezierCurve.h"

void BezierCurvesInterface() {
	class_<BezierCurve>("BezierCurve", init<vector<Location>>())
		.def("At", &BezierCurve::At);
}
