#include "stdafx.h"

#include "WiimoteInterface.h"

void WiimoteInterface::Startup() {
	std::vector<HANDLE> wiimote_handles = WiimoteHandler::GetWiimoteHandles();
	if (wiimote_handles.size() < 1) {
		//return 1;
	}
	else {
		wiimote.SetGravityCorrectionScale(0.1f);
		wiimote.SetPipe(wiimote_handles[0]);
		wiimote.ActivateIRCamera();
		wiimote.SendOutputReport(OutputReportTemplates::status_request);
		wiimote.SendOutputReport(OutputReportTemplates::set_leds);
		wiimote.SetDataReportingMethod(0x37, false);
		Sleep(1000);
		wiimote.CheckForMotionPlus();
		wiimote.SendOutputReport(OutputReportTemplates::request_calibration);
		wiimote_update_thread = std::thread(&WiimoteHandler::WatchForInputReports, &wiimote);
		wiimote_update_thread.detach();
		ready = true;
	}
}

WiimoteHandler* WiimoteInterface::GetHandler() {
	if (!ready) return NULL;
	return &wiimote;
}
