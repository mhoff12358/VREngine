#pragma once

#include "stdafx.h"

class TimeTracker {
public:
	enum TimeTrack : unsigned int {
		MAIN = 0,
		RENDERING = 1,
		NUM_TRACKS = 2
	};

	static void PreparePerformanceCounter();
	static void FrameStart();
	static void FrameEvent(const std::string& event_name, TimeTrack track = MAIN);
	static void ResetEvent(TimeTrack track = MAIN);

	static LARGE_INTEGER previous_frame;
	static std::array<LARGE_INTEGER, NUM_TRACKS> previous_events;
	static unsigned int frame_number;
	static float performance_freq_inv;
	static TimeTrack active_track;
	static bool track_time;

private:
	static void LogFrameDuration(LONGLONG num_ticks);
	static void LogEventDuration(const std::string& event_name, LONGLONG num_ticks);
};

template <unsigned int StepsPerFrame, unsigned int NumFrames>
class FramerateTracker {
public:
	FramerateTracker(const string& tracker_name, bool print);

	void StartFrame();
	void EndFrame();
	// Essentially ending and starting a frame at once
	void SwitchFrame();
	
	void RecordFrameDuration(float frame_duration);

private:
	string tracker_name_;
	array<float, NumFrames> times_;
	unsigned int num_frames_filled_;
	LARGE_INTEGER frame_start_;
	float performance_freq_inv_;
	bool print_;
};

template <unsigned int StepsPerFrame, unsigned int NumFrames>
FramerateTracker<StepsPerFrame, NumFrames>::FramerateTracker(const string& tracker_name, bool print)
	: tracker_name_(tracker_name), print_(print),
	num_frames_filled_(0) {
	LARGE_INTEGER perf_freq;
	QueryPerformanceFrequency(&perf_freq);
	performance_freq_inv_ = 1.0f / static_cast<float>(perf_freq.QuadPart);
}

template <unsigned int StepsPerFrame, unsigned int NumFrames>
void FramerateTracker<StepsPerFrame, NumFrames>::StartFrame() {
	QueryPerformanceCounter(&frame_start_);
}

template <unsigned int StepsPerFrame, unsigned int NumFrames>
void FramerateTracker<StepsPerFrame, NumFrames>::EndFrame() {
	LARGE_INTEGER frame_end;
	QueryPerformanceCounter(&frame_end);
	RecordFrameDuration((frame_end.QuadPart - frame_start_.QuadPart) * performance_freq_inv_);
}

template <unsigned int StepsPerFrame, unsigned int NumFrames>
void FramerateTracker<StepsPerFrame, NumFrames>::SwitchFrame() {
	LARGE_INTEGER frame_end;
	QueryPerformanceCounter(&frame_end);
	RecordFrameDuration((frame_end.QuadPart - frame_start_.QuadPart) * performance_freq_inv_);
	frame_start_ = frame_end;
}

template <unsigned int StepsPerFrame, unsigned int NumFrames>
void FramerateTracker<StepsPerFrame, NumFrames>::RecordFrameDuration(float frame_duration) {
	times_[num_frames_filled_] = frame_duration;
	num_frames_filled_++;
	if (num_frames_filled_ == NumFrames) {
		num_frames_filled_ = 0;
		if (print_) {
			float total_time = 0;
			for (unsigned int i = 0; i < NumFrames; i++) {
				total_time += times_[i];
			}
			std::cout << tracker_name_ << ": " << NumFrames / total_time << std::endl;
		}
	}
}