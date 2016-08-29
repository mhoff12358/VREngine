#include "TimeTracker.h"


float TimeTracker::performance_freq_inv = 0.0f;
unsigned int TimeTracker::frame_number = 0;
LARGE_INTEGER TimeTracker::previous_frame;
std::array<LARGE_INTEGER, TimeTracker::NUM_TRACKS> TimeTracker::previous_events;
TimeTracker::TimeTrack TimeTracker::active_track = TimeTracker::MAIN;
bool TimeTracker::track_time = true;

void TimeTracker::PreparePerformanceCounter() {
	LARGE_INTEGER perf_freq;
	QueryPerformanceFrequency(&perf_freq);
	performance_freq_inv = 1.0f / static_cast<float>(perf_freq.QuadPart);
}

void TimeTracker::FrameStart() {
	if (track_time) {
		LARGE_INTEGER new_frame;
		QueryPerformanceCounter(&new_frame);
		LogFrameDuration(new_frame.QuadPart - previous_frame.QuadPart);

		previous_frame.QuadPart = new_frame.QuadPart;
		for (auto& previous_event : previous_events) {
			previous_event.QuadPart = new_frame.QuadPart;
		}
	}
}

void TimeTracker::FrameEvent(const std::string& event_name, TimeTracker::TimeTrack track) {
	if (track_time && (track == active_track)) {
		LARGE_INTEGER current_time;
		QueryPerformanceCounter(&current_time);
		LogEventDuration(event_name, current_time.QuadPart - previous_events[track].QuadPart);
		previous_events[track].QuadPart = current_time.QuadPart;
	}
}

void TimeTracker::ResetEvent(TimeTracker::TimeTrack track) {
	if (track_time && (track == active_track)) {
		LARGE_INTEGER current_time;
		QueryPerformanceCounter(&current_time);
		previous_events[track].QuadPart = current_time.QuadPart;
	}
}

void TimeTracker::LogFrameDuration(LONGLONG num_ticks) {
	float duration = num_ticks * performance_freq_inv;
	std::cout << "Frame duration: " << duration << "\t" << 1.0f/duration << std::endl;
}

void TimeTracker::LogEventDuration(const std::string& event_name, LONGLONG num_ticks) {
	float duration = num_ticks * performance_freq_inv;
	std::cout << "Event " << event_name << " duration: " << duration << "\t" << 1.0f/duration<< std::endl;
}