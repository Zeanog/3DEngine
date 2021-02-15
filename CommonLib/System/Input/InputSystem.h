#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "System/Singleton.h"

class InputSystem {
	CLASS_TYPEDEFS( InputSystem );
	SINGLETON_DECLARATIONS(InputSystem) {
	}

protected:
	LPDIRECTINPUT8	m_pSystem;

	Keyboard		m_Keyboard;
	Mouse			m_Mouse;

public:
	Bool		Init(HWND hWnd) {
		HRESULT hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pSystem, NULL );
		if (FAILED(hr)) {
			return false;
		}

		hr = m_Keyboard.Init(hWnd, m_pSystem);
		if (FAILED(hr)) {
			return false;
		}
		verify(SUCCEEDED(m_Keyboard.Acquire()));

		hr = m_Mouse.Init(hWnd, m_pSystem);
		if (FAILED(hr)) {
			return false;
		}
		verify(SUCCEEDED(m_Mouse.Acquire()));

		return true;
	}

	Keyboard*	GetKeyboard() {
		return &m_Keyboard;
	}

	Mouse*	GetMouse() {
		return &m_Mouse;
	}

	Bool	Poll() {
		if (!m_Keyboard.Poll()) {
			m_Keyboard.Acquire();
		}

		if (!m_Mouse.Poll()) {
			m_Mouse.Acquire();
		}

		return false;
	}
};