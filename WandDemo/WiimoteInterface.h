#pragma once

#include "stdafx.h"

#include "WiimoteHandler.h"

#include <thread>

class WiimoteInterface {
public:
	void Startup();
	WiimoteHandler* GetHandler();

private:
	std::thread wiimote_update_thread;
	bool ready = false;
	WiimoteHandler wiimote;
};