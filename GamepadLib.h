// MIT License

// Copyright (c) 2022 Jashen Low

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_

#include <iostream>
#include <sstream>
#include <Windows.h>	//This must be included BEFORE Xinput.h!
#include <Xinput.h>

#pragma comment (lib, "xinput.lib")
#pragma comment (lib, "winmm.lib")

constexpr float DEFAULT_DEADZONE	= 0.04f;
constexpr uint16_t GP_MAX_BUTTONS	= 14;
constexpr uint16_t GP_MAX_ANALOG	= 8;
constexpr short GPID_DISCONNECTED	= -1;

namespace GpDef
{
	enum class Buttons : uint16_t
	{
		FACE_A,
		FACE_B,
		FACE_X,
		FACE_Y,
		DPAD_LEFT,
		DPAD_RIGHT,
		DPAD_UP,
		DPAD_DOWN,
		SHOULDER_LEFT,
		SHOULDER_RIGHT,
		THUMB_LEFT,
		THUMB_RIGHT,
		BACK,
		START
	};

	enum class AnalogState : uint16_t
	{
		TRIGGER_L,
		TRIGGER_R,
		THUMB_L_X,
		THUMB_L_Y,
		THUMB_R_X,
		THUMB_R_Y,
		VIBRATION_L,
		VIBRATION_R
	};

	enum class DeviceID : uint16_t
	{
		ID_0,
		ID_1,
		ID_2,
		ID_3
	};

	enum class StreamType : uint16_t
	{
		STREAM_ALL,
		STREAM_DIGITAL,
		STREAM_ANALOG
	};

	struct AnalogStruct
	{
		/*
		* [Value Ranges]
		*   TRIGGER_L, TRIGGER_R, VIBRATION_L, VIBRATION_R: [0 to 1]
		*   THUMB_L_X, THUMB_L_Y, THUMB_R_X, THUMB_R_Y:     [-1 to 1]
		*/
		float Data[GP_MAX_ANALOG];

		AnalogStruct()
		{
			memset(Data, 0, sizeof(float) * GP_MAX_ANALOG);
		}
	};

	struct DigitalStruct
	{
		bool Data[GP_MAX_BUTTONS];

		DigitalStruct()
		{
			memset(Data, false, sizeof(bool) * GP_MAX_BUTTONS);
		}
	};

	struct DeadzoneStruct
	{
		float X;
		float Y;

		DeadzoneStruct()
		{
			X = DEFAULT_DEADZONE;
			Y = DEFAULT_DEADZONE;
		}
	};

	struct ControlsStruct
	{
		AnalogStruct	Analog;
		DigitalStruct	Digital;
		DeadzoneStruct	Deadzone;

		void Reset()
		{
			Analog		= AnalogStruct();
			Digital		= DigitalStruct();
			Deadzone	= DeadzoneStruct();
		}
	};

	struct GamepadState
	{
		ControlsStruct		Controls;
		ControlsStruct		PrevControls;
		char				ProductName[MAXPNAMELEN];
		XINPUT_STATE		PadState;
		XINPUT_VIBRATION	VibState;
		short				ID;
		short				PrevID;

		GamepadState()
		{
			Controls.Reset();
			memset(ProductName, '\0', MAXPNAMELEN);
			memset(&PadState, 0, sizeof(XINPUT_STATE));
			memset(&VibState, 0, sizeof(XINPUT_VIBRATION));
			ID		= GPID_DISCONNECTED;
			PrevID	= GPID_DISCONNECTED;
		}
	};
}

class GamepadLib
{
public:
	GamepadLib();
	~GamepadLib();
	
	const char* GetClassStr() { return "GamepadLib"; }

	/*
	* Description	 :	Checks for any connected controllers and initializes them.
	*                   NOTE: Calling this function is only necessary if controllers need to be initialized before calling Tick. 
	* Return		 :
	*/
	void InitDevices();

	/*
	* Description	 :	Returns the product name of the gamepad specified by it's ID.
	* Return		 :  Product name in a C-String format.
	*/
	const char* GetProductName(const GpDef::DeviceID& index);
	
	/*
	* Description	 :	Sets the values of X and Y deadzones specifically for thumbsticks.
	* Return		 :  
	*/
	void SetDeadZone(const GpDef::DeviceID& index, const float& deadX, const float& deadY);
	
	/*
	* Description	 :	Returns a struct containing the current X and Y deadzone values specifically for thumbsticks.
	* Return		 :
	*/
	const GpDef::DeadzoneStruct& GetDeadZone(const GpDef::DeviceID& index);
	
	/*
	* Description	 :	Checks and updates connectivity status, updates all analog and digital states.
	*                   IMPORTANT: This has to be called before reading any states, or calling comparison functions such as IsTriggeredDown.
	* Return		 :
	*/
	void Tick();
	
	/*
	* Description	 :	Checks and returns a boolean value indicating the connectivity of a controller specified by it's ID.
	* Return		 :  true = connected, false = not connected.
	*/
	bool IsConnected(const GpDef::DeviceID& index);

	/*
	* Description	 :	Returns a boolean value indicating if the specified controller has just been connected.
	* Return		 :  true = connected(current frame), false = not connected(current frame).
	*/
	bool IsJustConnected(const GpDef::DeviceID& index);
	
	/*
	* Description	 :	Returns a boolean value indicating if the specified controller has just been disconnected.
	* Return		 :  true = disconnected(current frame), false = not disconnected(current frame).
	*/
	bool IsJustDisconnected(const GpDef::DeviceID& index);

	/*
	* Description	 :	Returns a struct containing all analog control states.
	* Return		 :  AnalogStruct.
	*/
	const GpDef::AnalogStruct& GetAnalogStates(const GpDef::DeviceID& index);
	
	/*
	* Description	 :	Returns a struct containing all digital control states.
	* Return		 :  DigitalStruct.
	*/
	const GpDef::DigitalStruct& GetDigitalStates(const GpDef::DeviceID& index);

	/*
	* Description	 :	Returns the state of a single analog control specified by the AnalogState enum.
	* Return		 :  float value(normalized).
	*/
	float GetAnalogState(const GpDef::DeviceID& index, const GpDef::AnalogState& ctrl);
	
	/*
	* Description	 :	Returns a boolean value indicating if a specified button is currently pressed down.
	* Return		 :  true = pressed, false = not pressed.
	*/
	bool IsButtonPressed(const GpDef::DeviceID& index, const GpDef::Buttons& button);
	
	/*
	* Description	 :	Returns a boolean value indicating if a specified button has just been toggled down.
	* Return		 :  true = pressed(current frame), false = not pressed(current frame).
	*/
	bool IsTriggeredDown(const GpDef::DeviceID& index, const GpDef::Buttons& button);
	
	/*
	* Description	 :	Returns a boolean value indicating if a specified button has just been toggled up.
	* Return		 :  true = released(current frame), false = not released(current frame).
	*/
	bool IsTriggeredUp(const GpDef::DeviceID& index, const GpDef::Buttons& button);

	/*
	* Description	 :	Sets the vibration levels for the left and right motors using the arguments "left" and "right".
	* Return		 :  
	*/
	void SetVibration(const GpDef::DeviceID& index, const float& left, const float& right);

	/*
	* Description	 :	Appends state data of all controls into an output stream. This is called mainly for debugging purposes.
	* Return		 :
	*/
	void DumpToStream(const GpDef::DeviceID& index, const GpDef::StreamType& type, std::ostringstream& stream);

private:
	GpDef::GamepadState gamepads[XUSER_MAX_COUNT];
	
	inline void UpdateDigitalInputs(const GpDef::DeviceID& index);
	inline void UpdateAnalogInputs(const GpDef::DeviceID& index);
};

#endif