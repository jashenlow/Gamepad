#include "GamepadLib.h"

GamepadLib::GamepadLib()
{
	
}

GamepadLib::~GamepadLib()
{

}

void GamepadLib::InitDevices()
{
	MMRESULT res;
	JOYCAPSA devInfo;
	ZeroMemory(&devInfo, sizeof(devInfo));
	memset(&devInfo.szPname, '\0', MAXPNAMELEN);
	
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		ZeroMemory(&gamepads[i].PadState, sizeof(XINPUT_STATE));

		if (gamepads[i].ID == GPID_DISCONNECTED)
		{
			if (XInputGetState(i, &gamepads[i].PadState) == ERROR_SUCCESS)
			{
				gamepads[i].ID = (short)i;
				
				//Get Gamepad Device Name
				res	= joyGetDevCapsA(i, &devInfo, sizeof(devInfo));
				memset(gamepads[i].ProductName, '\0', MAXPNAMELEN);
				snprintf(gamepads[i].ProductName, sizeof(gamepads[i].ProductName), "%s", devInfo.szPname);
			}
		}
	}
}

const char* GamepadLib::GetProductName(const GpDef::DeviceID& index)
{
	return gamepads[(uint16_t)index].ProductName;
}

bool GamepadLib::IsConnected(const GpDef::DeviceID& index)
{
	return (gamepads[(uint16_t)index].ID >= 0);
}

void GamepadLib::SetDeadZone(const GpDef::DeviceID& index, const float& deadX, const float& deadY)
{
	gamepads[(uint16_t)index].Controls.Deadzone.X = deadX;
	gamepads[(uint16_t)index].Controls.Deadzone.Y = deadY;
}

const GpDef::DeadzoneStruct& GamepadLib::GetDeadZone(const GpDef::DeviceID& index)
{
	return gamepads[(uint16_t)index].Controls.Deadzone;
}

void GamepadLib::Tick()
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
			UpdateAnalogInputs((GpDef::DeviceID)i);
			UpdateDigitalInputs((GpDef::DeviceID)i);
		}
		else
		{
			if (gamepads[i].ID > GPID_DISCONNECTED)
			{
				gamepads[i].Controls.Reset();
				memset(gamepads[i].ProductName, '\0', MAXPNAMELEN);
				gamepads[i].ID = GPID_DISCONNECTED;
			}
		}
	}
}

bool GamepadLib::IsJustConnected(const GpDef::DeviceID& index)
{
	return (gamepads[(uint16_t)index].ID > GPID_DISCONNECTED) && (gamepads[(uint16_t)index].PrevID == GPID_DISCONNECTED);
}

bool GamepadLib::IsJustDisconnected(const GpDef::DeviceID& index)
{
	return (gamepads[(uint16_t)index].ID == GPID_DISCONNECTED) && (gamepads[(uint16_t)index].PrevID > GPID_DISCONNECTED);
}

const GpDef::AnalogStruct& GamepadLib::GetAnalogStates(const GpDef::DeviceID& index)
{
	return gamepads[(uint16_t)index].Controls.Analog;
}

const GpDef::DigitalStruct& GamepadLib::GetDigitalStates(const GpDef::DeviceID& index)
{
	return gamepads[(uint16_t)index].Controls.Digital;
}

float GamepadLib::GetAnalogState(const GpDef::DeviceID& index, const GpDef::AnalogState& ctrl)
{
	if (gamepads[(uint16_t)index].ID > GPID_DISCONNECTED)
		return gamepads[(uint16_t)index].Controls.Analog.Data[(uint16_t)ctrl];
	else
		return 0.0f;
}

inline bool GamepadLib::IsButtonPressed(const GpDef::DeviceID& index, const GpDef::Buttons& button)
{
	if (gamepads[(uint16_t)index].ID > GPID_DISCONNECTED)
		return gamepads[(uint16_t)index].Controls.Digital.Data[(uint16_t)button];
	else
		return false;
}

inline void GamepadLib::UpdateDigitalInputs(const GpDef::DeviceID& index)
{
	GpDef::DigitalStruct& digital	= gamepads[(uint16_t)index].Controls.Digital;
	const DWORD& psButtons			= gamepads[(uint16_t)index].PadState.Gamepad.wButtons;

	//Face Buttons
	digital.Data[(uint16_t)GpDef::Buttons::FACE_A] = (bool)(psButtons & XINPUT_GAMEPAD_A);
	digital.Data[(uint16_t)GpDef::Buttons::FACE_B] = (bool)(psButtons & XINPUT_GAMEPAD_B);
	digital.Data[(uint16_t)GpDef::Buttons::FACE_X] = (bool)(psButtons & XINPUT_GAMEPAD_X);
	digital.Data[(uint16_t)GpDef::Buttons::FACE_Y] = (bool)(psButtons & XINPUT_GAMEPAD_Y);

	//Direction Pad Buttons
	digital.Data[(uint16_t)GpDef::Buttons::DPAD_LEFT]	= (bool)(psButtons & XINPUT_GAMEPAD_DPAD_LEFT);
	digital.Data[(uint16_t)GpDef::Buttons::DPAD_RIGHT]	= (bool)(psButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
	digital.Data[(uint16_t)GpDef::Buttons::DPAD_UP]		= (bool)(psButtons & XINPUT_GAMEPAD_DPAD_UP);
	digital.Data[(uint16_t)GpDef::Buttons::DPAD_DOWN]	= (bool)(psButtons & XINPUT_GAMEPAD_DPAD_DOWN);
	
	//Shoulder Buttons
	digital.Data[(uint16_t)GpDef::Buttons::SHOULDER_LEFT]	= (bool)(psButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
	digital.Data[(uint16_t)GpDef::Buttons::SHOULDER_RIGHT]	= (bool)(psButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
	
	//Thumb Sticks (Press)
	digital.Data[(uint16_t)GpDef::Buttons::THUMB_LEFT]	= (bool)(psButtons & XINPUT_GAMEPAD_LEFT_THUMB);
	digital.Data[(uint16_t)GpDef::Buttons::THUMB_RIGHT]	= (bool)(psButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
	
	//Center Buttons
	digital.Data[(uint16_t)GpDef::Buttons::BACK]	= (bool)(psButtons & XINPUT_GAMEPAD_BACK);
	digital.Data[(uint16_t)GpDef::Buttons::START]	= (bool)(psButtons & XINPUT_GAMEPAD_START);
}

inline void GamepadLib::UpdateAnalogInputs(const GpDef::DeviceID& index)
{
	GpDef::AnalogStruct& analog				= gamepads[(uint16_t)index].Controls.Analog;
	const XINPUT_STATE& padState			= gamepads[(uint16_t)index].PadState;
	const GpDef::DeadzoneStruct& deadZone	= gamepads[(uint16_t)index].Controls.Deadzone;

	//Left & Right Triggers (Normalized to [0 to 1] range)
	analog.Data[(uint16_t)GpDef::AnalogState::TRIGGER_L] = (padState.Gamepad.bLeftTrigger) / 255.0f;
	analog.Data[(uint16_t)GpDef::AnalogState::TRIGGER_R] = (padState.Gamepad.bRightTrigger) / 255.0f;

	//Left & Right Thumbsticks (Normalized to [-1 to 1] range)
	analog.Data[(uint16_t)GpDef::AnalogState::THUMB_L_X] = max(-1.0f, (padState.Gamepad.sThumbLX) / 32767.0f);
	analog.Data[(uint16_t)GpDef::AnalogState::THUMB_L_Y] = max(-1.0f, (padState.Gamepad.sThumbLY) / 32767.0f);
	analog.Data[(uint16_t)GpDef::AnalogState::THUMB_R_X] = max(-1.0f, (padState.Gamepad.sThumbRX) / 32767.0f);
	analog.Data[(uint16_t)GpDef::AnalogState::THUMB_R_Y] = max(-1.0f, (padState.Gamepad.sThumbRY) / 32767.0f);

	//Account for deadzones
	analog.Data[(uint16_t)GpDef::AnalogState::THUMB_L_X] = (abs(analog.Data[(uint16_t)GpDef::AnalogState::THUMB_L_X]) < deadZone.X) ? 0.0f : analog.Data[(uint16_t)GpDef::AnalogState::THUMB_L_X];
	analog.Data[(uint16_t)GpDef::AnalogState::THUMB_L_Y] = (abs(analog.Data[(uint16_t)GpDef::AnalogState::THUMB_L_Y]) < deadZone.Y) ? 0.0f : analog.Data[(uint16_t)GpDef::AnalogState::THUMB_L_Y];
	analog.Data[(uint16_t)GpDef::AnalogState::THUMB_R_X] = (abs(analog.Data[(uint16_t)GpDef::AnalogState::THUMB_R_X]) < deadZone.X) ? 0.0f : analog.Data[(uint16_t)GpDef::AnalogState::THUMB_R_X];
	analog.Data[(uint16_t)GpDef::AnalogState::THUMB_R_Y] = (abs(analog.Data[(uint16_t)GpDef::AnalogState::THUMB_R_Y]) < deadZone.Y) ? 0.0f : analog.Data[(uint16_t)GpDef::AnalogState::THUMB_R_Y];
}

bool GamepadLib::IsTriggeredDown(const GpDef::DeviceID& index, const GpDef::Buttons& button)
{
	if (gamepads[(uint16_t)index].ID > GPID_DISCONNECTED)
	{
		const bool& current		= gamepads[(uint16_t)index].Controls.Digital.Data[(uint16_t)button];
		const bool& previous	= gamepads[(uint16_t)index].PrevControls.Digital.Data[(uint16_t)button];

		return (current && !previous);
	}
	else
		return false;
}

bool GamepadLib::IsTriggeredUp(const GpDef::DeviceID& index, const GpDef::Buttons& button)
{
	if (gamepads[(uint16_t)index].ID > GPID_DISCONNECTED)
	{
		const bool& current		= gamepads[(uint16_t)index].Controls.Digital.Data[(uint16_t)button];
		const bool& previous	= gamepads[(uint16_t)index].PrevControls.Digital.Data[(uint16_t)button];

		return (!current && previous);
	}
	else
		return false;
}

void GamepadLib::SetVibration(const GpDef::DeviceID& index, const float& left, const float& right)
{
	float cappedLeft	= max(left, 0.0f);
	float cappedRight	= max(right, 0.0f);
	cappedLeft	= min(left, 1.0f);
	cappedRight = min(right, 1.0f);

	if (gamepads[(uint16_t)index].ID > GPID_DISCONNECTED)
	{
		gamepads[(uint16_t)index].Controls.Analog.Data[(uint16_t)GpDef::AnalogState::VIBRATION_L] = cappedLeft;
		gamepads[(uint16_t)index].Controls.Analog.Data[(uint16_t)GpDef::AnalogState::VIBRATION_R] = cappedRight;

		gamepads[(uint16_t)index].VibState.wLeftMotorSpeed = (WORD)(65535.0f * cappedLeft);
		gamepads[(uint16_t)index].VibState.wRightMotorSpeed = (WORD)(65535.0f * cappedRight);

		XInputSetState((DWORD)index, &gamepads[(uint16_t)index].VibState);
	}
}

void GamepadLib::DumpToStream(const GpDef::DeviceID& index, const GpDef::StreamType& type, std::ostringstream& stream)
{
	stream.clear();

	if ((type == GpDef::StreamType::STREAM_ALL) || (type == GpDef::StreamType::STREAM_ANALOG))
	{
		const GpDef::AnalogStruct& analog = gamepads[(uint16_t)index].Controls.Analog;

		stream << GetClassStr() << ": " << "--------ANALOG----------" << std::endl;
		stream << GetClassStr() << ": " << "Trigger_L = "	<< analog.Data[(uint16_t)GpDef::AnalogState::TRIGGER_L] << std::endl;
		stream << GetClassStr() << ": " << "Trigger_R = "	<< analog.Data[(uint16_t)GpDef::AnalogState::TRIGGER_R] << std::endl;
		stream << GetClassStr() << ": " << "Thumb_L_X = "	<< analog.Data[(uint16_t)GpDef::AnalogState::THUMB_L_X] << std::endl;
		stream << GetClassStr() << ": " << "Thumb_L_Y = "	<< analog.Data[(uint16_t)GpDef::AnalogState::THUMB_L_Y] << std::endl;
		stream << GetClassStr() << ": " << "Thumb_R_X = "	<< analog.Data[(uint16_t)GpDef::AnalogState::THUMB_R_X] << std::endl;
		stream << GetClassStr() << ": " << "Thumb_R_Y = "	<< analog.Data[(uint16_t)GpDef::AnalogState::THUMB_R_Y] << std::endl;
		stream << GetClassStr() << ": " << "Vibration_L = " << analog.Data[(uint16_t)GpDef::AnalogState::VIBRATION_L] << std::endl;
		stream << GetClassStr() << ": " << "Vibration_R = " << analog.Data[(uint16_t)GpDef::AnalogState::VIBRATION_R] << std::endl;
	}
	
	if ((type == GpDef::StreamType::STREAM_ALL) || (type == GpDef::StreamType::STREAM_DIGITAL))
	{
		const GpDef::DigitalStruct& digital = gamepads[(uint16_t)index].Controls.Digital;

		stream << GetClassStr() << ": " << "--------DIGITAL----------" << std::endl;
		stream << GetClassStr() << ": " << "Face_A = "			<< (digital.Data[(uint16_t)GpDef::Buttons::FACE_A] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "Face_B = "			<< (digital.Data[(uint16_t)GpDef::Buttons::FACE_B] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "Face_X = "			<< (digital.Data[(uint16_t)GpDef::Buttons::FACE_X] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "Face_Y = "			<< (digital.Data[(uint16_t)GpDef::Buttons::FACE_Y] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "DPAD_LEFT = "		<< (digital.Data[(uint16_t)GpDef::Buttons::DPAD_LEFT] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "DPAD_RIGHT = "		<< (digital.Data[(uint16_t)GpDef::Buttons::DPAD_RIGHT] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "DPAD_UP = "			<< (digital.Data[(uint16_t)GpDef::Buttons::DPAD_UP] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "DPAD_DOWN = "		<< (digital.Data[(uint16_t)GpDef::Buttons::DPAD_DOWN] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "SHOULDER_LEFT = "	<< (digital.Data[(uint16_t)GpDef::Buttons::SHOULDER_LEFT] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "SHOULDER_RIGHT = "	<< (digital.Data[(uint16_t)GpDef::Buttons::SHOULDER_RIGHT] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "THUMB_LEFT = "		<< (digital.Data[(uint16_t)GpDef::Buttons::THUMB_LEFT] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "THUMB_RIGHT = "		<< (digital.Data[(uint16_t)GpDef::Buttons::THUMB_RIGHT] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "Back = "			<< (digital.Data[(uint16_t)GpDef::Buttons::BACK] ? "TRUE" : "FALSE") << std::endl;
		stream << GetClassStr() << ": " << "Start = "			<< (digital.Data[(uint16_t)GpDef::Buttons::START] ? "TRUE" : "FALSE") << std::endl;
	}

	stream << std::endl;
}

