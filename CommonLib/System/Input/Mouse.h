#pragma once

#include "ADevice.h"
#include "System/Functors/MultiFunctor.h"

struct Button {
	Byte	State;

	void Reset() {
		State = 0;
	}

	void	Set(Bool down, Bool changed) {
		State = (Byte)((down ? 0x80 : 0) | (Int32)changed);
	}

	Bool	IsSet() const {
		return State & 0x80;
	}

	Bool	Changed() const {
		return (State & 0x1) != 0;
	}

	void	ToString(String& outString) const {
		outString += String::Format("IsSet: %s, Changed: %s", IsSet() ? "True" : "False", Changed() ? "True" : "False");
	}
};

class Mouse : ADevice {
public:
	struct State {
		Int64	X;
		Int64	DeltaX;

		Int64	Y;
		Int64	DeltaY;

		Int64	DeltaZ;

		Button	Btns[4];

		void	ToString(String& outString) const {
			outString += String::Format("DeltaX: %d\n", DeltaX);
			outString += String::Format("DeltaY: %d\n", DeltaY);
			outString += String::Format("DeltaZ: %d\n", DeltaZ);

			for (int ix = 0; ix < STATIC_ARRAY_LENGTH(Btns); ++ix) {
				Btns[ix].ToString(outString);
				outString += "\n";
			}
		}
	};

protected:
	State		m_DeviceState;

public:
	Delegate<TYPELIST_1(State)>	OnChanged;

public:
	virtual Bool	Init(HWND hWnd, LPDIRECTINPUT8 system) {
		HRESULT hr = S_OK;

		hr = system->CreateDevice(GUID_SysMouse, &m_pDevice, NULL);
		if (FAILED(hr)) {
			return false;
		}

		m_pDevice->SetDataFormat(&c_dfDIMouse);
		m_pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		m_pDevice->Acquire();
		return true;
	}

	virtual Bool	Acquire() const {
		return SUCCEEDED(m_pDevice->Acquire());
	}

	virtual Bool	Unacquire() const {
		return SUCCEEDED(m_pDevice->Unacquire());
	}

	void			UpdatePosition(Int64 xPos, Int64 yPos) {
		m_DeviceState.X = xPos;
		m_DeviceState.Y = yPos;
		//NotifyObservers();
	}

	virtual Bool	Poll() {
		DIMOUSESTATE	state;
		HRESULT hr = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&state);
		if (FAILED(hr)) {
			return Acquire();
		}

		bool	notifyListeners = false;

		m_DeviceState.DeltaX = state.lX;
		notifyListeners |= m_DeviceState.DeltaX != 0;

		m_DeviceState.DeltaY = state.lY;
		notifyListeners |= m_DeviceState.DeltaY != 0;
		
		m_DeviceState.DeltaZ = state.lZ;
		notifyListeners |= m_DeviceState.DeltaZ != 0;

		for (int ix = 0; ix < STATIC_ARRAY_LENGTH(m_DeviceState.Btns); ++ix) {
			m_DeviceState.Btns[ix].Set(state.rgbButtons[ix] & 0x80, state.rgbButtons[ix] ^ m_DeviceState.Btns[ix].IsSet());
			notifyListeners |= m_DeviceState.Btns[ix].Changed();
		}

		if (notifyListeners) {
			NotifyObservers();
		}
		return true;
	}

protected:
	virtual void NotifyObservers() const {
		OnChanged(m_DeviceState);
	}
};