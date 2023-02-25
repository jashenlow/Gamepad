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
#include <unordered_map>
#include <future>
#include <Windows.h>	//This must be included BEFORE Xinput.h!
#include <Xinput.h>

#ifdef WIN32_LEAN_AND_MEAN
#include <mmsyscom.h>
#endif

#pragma comment (lib, "xinput.lib")
#pragma comment (lib, "winmm.lib")

#pragma warning(disable:26812)	//Disable warning for unscoped enums

constexpr float DEFAULT_DEADZONE	= 0.04f;
constexpr short GPID_DISCONNECTED	= -1;

namespace GpDef
{
	enum DeviceID : uint8_t
	{
		ID_0,
		ID_1,
		ID_2,
		ID_3
	};

	enum StreamType : uint8_t
	{
		STREAM_ALL,
		STREAM_DIGITAL,
		STREAM_ANALOG
	};

	struct AnalogStruct
	{
		float Trigger_L;   //Value Range: [0 to 1]	
		float Trigger_R;   //Value Range: [0 to 1]  
		float Thumb_L_X;   //Value Range: [-1 to 1]
		float Thumb_L_Y;   //Value Range: [-1 to 1]
		float Thumb_R_X;   //Value Range: [-1 to 1]
		float Thumb_R_Y;   //Value Range: [-1 to 1]
		float Vibration_L; //Value Range: [0 to 1]
		float Vibration_R; //Value Range: [0 to 1]

		AnalogStruct()
		{
			Reset();
		}

		inline void Reset()
		{
			memset(this, 0, sizeof(AnalogStruct));
		}

		bool operator==(const AnalogStruct& other)
		{
			return (
				Trigger_L == other.Trigger_L &&
				Trigger_R == other.Trigger_R &&
				Thumb_L_X == other.Thumb_L_X &&
				Thumb_L_Y == other.Thumb_L_Y &&
				Thumb_R_X == other.Thumb_R_X &&
				Thumb_R_Y == other.Thumb_R_Y &&
				Vibration_L == other.Vibration_L &&
				Vibration_R == other.Vibration_R
				);
		}

		bool operator!=(const AnalogStruct& other)
		{
			return (
				Trigger_L != other.Trigger_L ||
				Trigger_R != other.Trigger_R ||
				Thumb_L_X != other.Thumb_L_X ||
				Thumb_L_Y != other.Thumb_L_Y ||
				Thumb_R_X != other.Thumb_R_X ||
				Thumb_R_Y != other.Thumb_R_Y ||
				Vibration_L != other.Vibration_L ||
				Vibration_R != other.Vibration_R
				);
		}
	};

	struct DigitalStruct
	{
		bool Face_A;
		bool Face_B;
		bool Face_X;
		bool Face_Y;
		bool Dpad_Left;
		bool Dpad_Right;
		bool Dpad_Up;
		bool Dpad_Down;
		bool Shoulder_Left;
		bool Shoulder_Right;
		bool Thumb_Left;
		bool Thumb_Right;
		bool Back;
		bool Start;

		DigitalStruct()
		{
			Reset();
		}

		inline void Reset()
		{
			memset(this, 0, sizeof(DigitalStruct));
		}

		bool operator==(const DigitalStruct& other)
		{
			return (
				Face_A == other.Face_A &&
				Face_B == other.Face_B &&
				Face_X == other.Face_X &&
				Face_Y == other.Face_Y &&
				Dpad_Left == other.Dpad_Left &&
				Dpad_Right == other.Dpad_Right &&
				Dpad_Up == other.Dpad_Up &&
				Dpad_Down == other.Dpad_Down &&
				Shoulder_Left == other.Shoulder_Left &&
				Shoulder_Right == other.Shoulder_Right &&
				Thumb_Left == other.Thumb_Left &&
				Thumb_Right == other.Thumb_Right &&
				Back == other.Back &&
				Start == other.Start
				);
		}

		bool operator!=(const DigitalStruct& other)
		{
			return (
				Face_A != other.Face_A ||
				Face_B != other.Face_B ||
				Face_X != other.Face_X ||
				Face_Y != other.Face_Y ||
				Dpad_Left != other.Dpad_Left ||
				Dpad_Right != other.Dpad_Right ||
				Dpad_Up != other.Dpad_Up ||
				Dpad_Down != other.Dpad_Down ||
				Shoulder_Left != other.Shoulder_Left ||
				Shoulder_Right != other.Shoulder_Right ||
				Thumb_Left != other.Thumb_Left ||
				Thumb_Right != other.Thumb_Right ||
				Back != other.Back ||
				Start != other.Start
				);
		}
	};

	struct DeadzoneStruct
	{
		float X;
		float Y;

		DeadzoneStruct()
		{
			Reset();
		}

		inline void Set(const float& setX, const float& setY)
		{
			X = std::abs(setX);
			Y = std::abs(setY);
		}

		inline void Reset()
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

		ControlsStruct()
		{
			Reset();
		}

		void Reset()
		{
			Analog.Reset();
			Digital.Reset();
			Deadzone.Reset();
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
			Reset();
		}

		inline void Reset()
		{
			Controls.Reset();
			memset(ProductName, '\0', MAXPNAMELEN);
			memset(&PadState, 0, sizeof(XINPUT_STATE));
			memset(&VibState, 0, sizeof(XINPUT_VIBRATION));
			ID      = GPID_DISCONNECTED;
			PrevID  = GPID_DISCONNECTED;
		}
	};
}

typedef void(*GpConnectCallback)(void* usr, GpDef::DeviceID gamepadID);

class Gamepad
{
public:
	Gamepad() {}
	~Gamepad() 
	{
		connectedCallbacks.clear();
		disconnectedCallbacks.clear();
	}
	
	const char* GetClassStr() { return "Gamepad"; }

	/*
	* Description	 :	Checks for any connected controllers and initializes them.
	*                   NOTE: Calling this function is only necessary if controllers need to be initialized before calling Tick. 
	* Return		 :
	*/
	void InitDevices()
	{
		MMRESULT res;
		JOYCAPSA devInfo;
		ZeroMemory(&devInfo, sizeof(devInfo));
		memset(&devInfo.szPname, '\0', MAXPNAMELEN);
		numConnected = 0;

		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			ZeroMemory(&gamepads[i].PadState, sizeof(XINPUT_STATE));

			if (gamepads[i].ID == GPID_DISCONNECTED)
			{
				if (XInputGetState(i, &gamepads[i].PadState) == ERROR_SUCCESS)
				{
					gamepads[i].ID = (short)i;

					//Get Gamepad Device Name
					res = joyGetDevCapsA(i, &devInfo, sizeof(devInfo));
					memset(gamepads[i].ProductName, '\0', MAXPNAMELEN);
					snprintf(gamepads[i].ProductName, sizeof(gamepads[i].ProductName), "%s", devInfo.szPname);
					
					numConnected++;

					CallConnectedCallbacks(connectedCallbacks, (GpDef::DeviceID)gamepads[i].ID);
				}
			}
		}
	}

	/*
	* Description	 :	Returns the number of connected controllers.
	* Return		 :  Number of connected controllers.
	*/
	const uint8_t& ConnectedCount() { return numConnected; }

	/*
	* Description	 :	Returns the product name of the gamepad specified by it's ID.
	* Return		 :  Product name in a C-String format.
	*/
	const char* GetProductName(const GpDef::DeviceID& index)
	{ 
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return "";
		}

		return gamepads[index].ProductName;
	}
	
	/*
	* Description	 :	Sets the values of X and Y deadzones specifically for thumbsticks.
	* Return		 :  
	*/
	void SetDeadZone(const GpDef::DeviceID& index, const float& deadX, const float& deadY)
	{
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return;
		}

		gamepads[index].Controls.Deadzone.Set(deadX, deadY);
	}
	
	/*
	* Description	 :	Returns a struct containing the current X and Y deadzone values specifically for thumbsticks.
	* Return		 :
	*/
	const GpDef::DeadzoneStruct& GetDeadZone(const GpDef::DeviceID& index)
	{
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return dummyControls.Deadzone;
		}

		return gamepads[index].Controls.Deadzone;
	}
	
	/*
	* Description	 :	Checks and updates connectivity status, updates all analog and digital states.
	*                   IMPORTANT: This has to be called before reading any states, or calling comparison functions such as IsTriggeredDown.
	* Return		 :
	*/
	void Tick()
	{
		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			gamepads[i].PrevControls	= gamepads[i].Controls;
			gamepads[i].PrevID			= gamepads[i].ID;

			//Check for connectivity
			ZeroMemory(&gamepads[i].PadState, sizeof(XINPUT_STATE));
			if (XInputGetState(i, &gamepads[i].PadState) == ERROR_SUCCESS)
			{
				gamepads[i].ID = (short)i;
				
				if (gamepads[i].PrevID == GPID_DISCONNECTED)
				{
					JOYCAPSA devInfo;
					//Get Gamepad Device Name
					MMRESULT res = joyGetDevCapsA(i, &devInfo, sizeof(devInfo));
					memset(gamepads[i].ProductName, '\0', MAXPNAMELEN);
					snprintf(gamepads[i].ProductName, sizeof(gamepads[i].ProductName), "%s", devInfo.szPname);

					numConnected++;

					CallConnectedCallbacks(connectedCallbacks, (GpDef::DeviceID)gamepads[i].ID);
				}

				UpdateAnalogInputs((GpDef::DeviceID)i);
				UpdateDigitalInputs((GpDef::DeviceID)i);
			}
			else
			{
				if (gamepads[i].ID > GPID_DISCONNECTED)
				{
					GpDef::DeviceID disconnectedID = (GpDef::DeviceID)gamepads[i].ID;
					gamepads[i].Reset();

					if (numConnected > 0)
						numConnected--;

					CallConnectedCallbacks(disconnectedCallbacks, disconnectedID);
				}
			}
		}
	}
	
	/*
	* Description	 :	Checks and returns a boolean value indicating the connectivity of a controller specified by it's ID.
	* Return		 :  true = connected, false = not connected.
	*/
	bool IsConnected(const GpDef::DeviceID& index)
	{ 
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return false;
		}

		return (gamepads[index].ID > GPID_DISCONNECTED);
	}

	/*
	* Description	 :	If set to true, all registered callbacks will be called asynchronously.
	* Return		 :
	*/
	void SetAsyncCallbacks(bool isAsync)
	{
		asyncCallbacks = isAsync;
	}

	/*
	* Description	 :	Maps a user-defined function to be called when a controller is connected.
	* Return		 :  
	*/
	void AddGamepadConnectedCallback(GpConnectCallback fcn, void* usr)
	{
		if (fcn == nullptr)
			std::cerr << __FUNCTION__ << ": " << "Invalid input for argument fcn" << std::endl;

		connectedCallbacks[fcn] = usr;
	}
	
	/*
	* Description	 :	Removes mapping of a user-defined function to be called when a controller is connected.
	* Return		 :  
	*/
	void RemoveGamepadConnectedCallback(GpConnectCallback fcn)
	{
		if (fcn == nullptr)
			std::cerr << __FUNCTION__ << ": " << "Invalid input for argument fcn" << std::endl;

		connectedCallbacks.erase(fcn);
	}

	/*
	* Description	 :	Maps a user-defined function to be called when a controller is disconnected.
	* Return		 :
	*/
	void AddGamepadDisconnectedCallback(GpConnectCallback fcn, void* usr)
	{
		if (fcn == nullptr)
			std::cerr << __FUNCTION__ << ':' << "Invalid input for argument fcn" << std::endl;
		if (usr == nullptr)
			std::cerr << __FUNCTION__ << ':' << "Invalid input for argument usr" << std::endl;

		disconnectedCallbacks[fcn] = usr;
	}

	/*
	* Description	 :	Removes mapping of a user-defined function to be called when a controller is disconnected.
	* Return		 :
	*/
	void RemoveGamepadDisconnectedCallback(GpConnectCallback fcn)
	{
		if (fcn == nullptr)
			std::cerr << __FUNCTION__ << ':' << "Invalid input for argument fcn" << std::endl;

		disconnectedCallbacks.erase(fcn);
	}

	/*
	* Description	 :	Returns a struct containing all analog control states.
	* Return		 :  AnalogStruct.
	*/
	const GpDef::AnalogStruct& GetAnalogStates(const GpDef::DeviceID& index)
	{
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return dummyControls.Analog;
		}

		return gamepads[index].Controls.Analog;
	}
	
	/*
	* Description	 :	Returns a struct containing all digital control states.
	* Return		 :  DigitalStruct.
	*/
	const GpDef::DigitalStruct& GetDigitalStates(const GpDef::DeviceID& index)
	{
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return dummyControls.Digital;
		}

		return gamepads[index].Controls.Digital;
	}
	
	/*
	* Description	 :	Returns a struct containing all previous analog control states 1 tick earlier.
	* Return		 :  AnalogStruct.
	*/
	const GpDef::AnalogStruct& GetPrevAnalogStates(const GpDef::DeviceID& index)
	{
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return dummyControls.Analog;
		}

		return gamepads[index].PrevControls.Analog;
	}

	/*
	* Description	 :	Returns a struct containing all previous digital control states 1 tick earlier.
	* Return		 :  DigitalStruct.
	*/
	const GpDef::DigitalStruct& GetPrevDigitalStates(const GpDef::DeviceID& index)
	{
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return dummyControls.Digital;
		}

		return gamepads[index].PrevControls.Digital;
	}

	/*
	* Description	 :	Sets the vibration levels for the left and right motors using the arguments "left" and "right".
	* Return		 :  
	*/
	void SetVibration(const GpDef::DeviceID& index, const float& left, const float& right)
	{
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return;
		}

		float leftVal  = left;
		float rightVal = right;
		BoundValueRange(leftVal, 0.0f, 1.0f);
		BoundValueRange(rightVal, 0.0f, 1.0f);

		gamepads[index].Controls.Analog.Vibration_L = leftVal;
		gamepads[index].Controls.Analog.Vibration_R = rightVal;

		gamepads[index].VibState.wLeftMotorSpeed    = (WORD)(65535.0f * leftVal);
		gamepads[index].VibState.wRightMotorSpeed   = (WORD)(65535.0f * rightVal);

		XInputSetState((DWORD)index, &gamepads[index].VibState);
	}

	/*
	* Description	 :	Appends state data of all controls into an output stream. This is called mainly for debugging purposes.
	* Return		 :
	*/
	void DumpToStream(const GpDef::DeviceID& index, const GpDef::StreamType& type, std::ostringstream& stream)
	{
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return;
		}

		stream.str("");

		if ((type == GpDef::StreamType::STREAM_ALL) || (type == GpDef::StreamType::STREAM_ANALOG))
		{
			const GpDef::AnalogStruct& analog = GetAnalogStates(index);

			stream << GetClassStr() << ": " << "--------ANALOG----------" << std::endl;
			stream << GetClassStr() << ": " << "Trigger_L   = " << analog.Trigger_L << std::endl;
			stream << GetClassStr() << ": " << "Trigger_R   = " << analog.Trigger_R << std::endl;
			stream << GetClassStr() << ": " << "Thumb_L_X   = " << analog.Thumb_L_X << std::endl;
			stream << GetClassStr() << ": " << "Thumb_L_Y   = " << analog.Thumb_L_Y << std::endl;
			stream << GetClassStr() << ": " << "Thumb_R_X   = " << analog.Thumb_R_X << std::endl;
			stream << GetClassStr() << ": " << "Thumb_R_Y   = " << analog.Thumb_R_Y << std::endl;
			stream << GetClassStr() << ": " << "Vibration_L = " << analog.Vibration_L << std::endl;
			stream << GetClassStr() << ": " << "Vibration_R = " << analog.Vibration_R << std::endl;
		}

		if ((type == GpDef::StreamType::STREAM_ALL) || (type == GpDef::StreamType::STREAM_DIGITAL))
		{
			const GpDef::DigitalStruct& digital = GetDigitalStates(index);

			stream << GetClassStr() << ": " << "--------DIGITAL----------" << std::endl;
			stream << GetClassStr() << ": " << "Face_A         = " << (digital.Face_A ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Face_B         = " << (digital.Face_B ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Face_X         = " << (digital.Face_X ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Face_Y         = " << (digital.Face_Y ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Dpad_Left      = " << (digital.Dpad_Left ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Dpad_Right     = " << (digital.Dpad_Right ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Dpad_Up        = " << (digital.Dpad_Up ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Dpad_Down      = " << (digital.Dpad_Down ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Shoulder_Left  = " << (digital.Shoulder_Left ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Shoulder_Right = " << (digital.Shoulder_Right ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Thumb_Left     = " << (digital.Thumb_Left ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Thumb_Right    = " << (digital.Thumb_Right ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Back           = " << (digital.Back ? "TRUE" : "FALSE") << std::endl;
			stream << GetClassStr() << ": " << "Start          = " << (digital.Start ? "TRUE" : "FALSE") << std::endl;
		}

		stream << std::endl;
	}

private:
	Gamepad(const Gamepad& other) = delete;
	Gamepad& operator=(const Gamepad& other) = delete;

	GpDef::GamepadState gamepads[XUSER_MAX_COUNT];
	GpDef::ControlsStruct dummyControls;	//For error handling
	uint8_t numConnected = 0;
	std::unordered_map<GpConnectCallback, void*> connectedCallbacks;
	std::unordered_map<GpConnectCallback, void*> disconnectedCallbacks;
	bool asyncCallbacks = false;

	inline void UpdateDigitalInputs(const GpDef::DeviceID& index)
	{
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return;
		}

		GpDef::DigitalStruct& digital = gamepads[index].Controls.Digital;
		const DWORD& psButtons        = gamepads[index].PadState.Gamepad.wButtons;

		//Face Buttons
		digital.Face_A = (bool)(psButtons & XINPUT_GAMEPAD_A);
		digital.Face_B = (bool)(psButtons & XINPUT_GAMEPAD_B);
		digital.Face_X = (bool)(psButtons & XINPUT_GAMEPAD_X);
		digital.Face_Y = (bool)(psButtons & XINPUT_GAMEPAD_Y);
		//Direction Pad Buttons
		digital.Dpad_Left  = (bool)(psButtons & XINPUT_GAMEPAD_DPAD_LEFT);
		digital.Dpad_Right = (bool)(psButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
		digital.Dpad_Up    = (bool)(psButtons & XINPUT_GAMEPAD_DPAD_UP);
		digital.Dpad_Down  = (bool)(psButtons & XINPUT_GAMEPAD_DPAD_DOWN);
		//Shoulder Buttons
		digital.Shoulder_Left  = (bool)(psButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
		digital.Shoulder_Right = (bool)(psButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
		//Thumb Sticks (Press)
		digital.Thumb_Left  = (bool)(psButtons & XINPUT_GAMEPAD_LEFT_THUMB);
		digital.Thumb_Right = (bool)(psButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
		//Center Buttons
		digital.Back  = (bool)(psButtons & XINPUT_GAMEPAD_BACK);
		digital.Start = (bool)(psButtons & XINPUT_GAMEPAD_START);
	}

	inline void UpdateAnalogInputs(const GpDef::DeviceID& index)
	{
		if (index >= XUSER_MAX_COUNT)
		{
			std::cerr << "Invalid input for argument \"index\". (See definition for GpDef::DeviceID)" << std::endl;
			return;
		}

		GpDef::AnalogStruct& analog           = gamepads[index].Controls.Analog;
		const XINPUT_STATE& padState          = gamepads[index].PadState;
		const GpDef::DeadzoneStruct& deadZone = gamepads[index].Controls.Deadzone;

		//Left & Right Triggers (Normalized to [0 to 1] range)
		analog.Trigger_L = padState.Gamepad.bLeftTrigger / 255.0f;
		analog.Trigger_R = padState.Gamepad.bRightTrigger / 255.0f;
		//Left & Right Thumbsticks (Normalized to [-1 to 1] range)
		analog.Thumb_L_X = MaxVal(-1.0f, padState.Gamepad.sThumbLX / 32767.0f);
		analog.Thumb_L_Y = MaxVal(-1.0f, padState.Gamepad.sThumbLY / 32767.0f);
		analog.Thumb_R_X = MaxVal(-1.0f, padState.Gamepad.sThumbRX / 32767.0f);
		analog.Thumb_R_Y = MaxVal(-1.0f, padState.Gamepad.sThumbRY / 32767.0f);
		//Account for deadzones
		analog.Thumb_L_X = (std::abs(analog.Thumb_L_X) < deadZone.X) ? 0.0f : analog.Thumb_L_X;
		analog.Thumb_L_Y = (std::abs(analog.Thumb_L_Y) < deadZone.Y) ? 0.0f : analog.Thumb_L_Y;
		analog.Thumb_R_X = (std::abs(analog.Thumb_R_X) < deadZone.X) ? 0.0f : analog.Thumb_R_X;
		analog.Thumb_R_Y = (std::abs(analog.Thumb_R_Y) < deadZone.Y) ? 0.0f : analog.Thumb_R_Y;
	}

	template<typename T>
	inline T MaxVal(const T& a, const T& b)	//Declared this in case NOMINMAX was defined
	{
		return (a > b) ? a : b;
	}

	template<typename T1, typename T2>
	inline void BoundValueRange(T1& value, const T2& minVal, const T2& maxVal)
	{
		value = (value < minVal) ? minVal : value;
		value = (value > maxVal) ? maxVal : value;
	}

	inline void CallConnectedCallbacks(const std::unordered_map<GpConnectCallback, void*>& cbMap, const GpDef::DeviceID& gamepadID)
	{
		for (auto& it : cbMap)
		{
			GpConnectCallback fcn = it.first;
			
			if (asyncCallbacks)
				std::future<void> fut = std::async(std::launch::async, fcn, it.second, gamepadID);
			else	
				fcn(it.second, gamepadID);
		}
	}
};

#endif