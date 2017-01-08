#pragma once
#include "stdafx.h"

namespace {
	struct general_ {};
	struct less_special_ : general_ {};
	struct special_ : less_special_ {};
}

