#ifndef __INPUT_HANDLER_H_INCLUDED__
#define __INPUT_HANDLER_H_INCLUDED__

#include "Windows.h"
#include "stl.h"

#include "Logging.h"
#include "Headset.h"

class InputHandler {
public:
	InputHandler(std::function<void(unsigned int)> kd_call, std::function<void(unsigned int)> ku_call) :
		head_pose_center({ 0.0f, 0.0f, 0.0f }), keydown_callback(kd_call), keyup_callback(ku_call) {
		mouse_motion[0] = 0;
		mouse_motion[1] = 0;
	}
	InputHandler() : head_pose_center({0.0f, 0.0f, 0.0f}), keydown_callback(), keyup_callback() {
		mouse_motion[0] = 0;
		mouse_motion[1] = 0;
	}

	void Initialize();

	void UpdateKeyboardState();
	void UpdateHeadset(unsigned int frame_index);

	void HandleKeydown(unsigned int key_code);

	bool GetKeyPressed(char key) const;
	bool GetKeyToggled(char key, bool now_pressed_down = true) const;
	Pose GetHeadPose() const;
	array<float, 3> GetHeadOffset() const;
	array<float, 3> GetEyeOffset(int eye_number) const;
	array<float, 3> GetActiveEyeOffset() const;
	unsigned int GetCurrentFrameIndex() const;

	void SetActiveEyeNumber(int eye_number);
	void SetEyeOffsets(array<float, 3> left_eye_offset, array<float, 3> right_eye_offset);

	void ResetHeadTracking();
	
	bool IsHeadsetActive() const;

	void SetHeadsetState(Headset* ocu);

	void SetUsePredictiveTiming(bool new_use_predictive);
	void SetPredictiveTiming(int new_predictive_offset_ms);

	std::array<std::atomic<int>, 2>& GetMutableMouseMotion();
	std::array<int, 2> ConsumeMouseMotion();

	int active_eye_number;
private:
	// State variables
	std::array<BYTE, 256> keyboard_state;
	std::array<BYTE, 256> prev_keyboard_state;
	std::array<std::atomic<int>, 2> mouse_motion;
	unsigned int current_frame_index;
	
	// State outputting variables
	std::array<BYTE, 256> keyboard_down_waiting;
	std::function<void(unsigned int)> keydown_callback;
	std::function<void(unsigned int)> keyup_callback;

	// Information gathering variables
	Headset* oculus = NULL;
	array<float, 3> eye_pose_offsets[2];
	array<float, 3> head_pose_center; // The location head offsets are taken from

	// Head tracking config variables
	bool use_predictive = false;
	int predictive_offset_ms = 0;
};

#endif