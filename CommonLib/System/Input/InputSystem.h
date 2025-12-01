#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "System/Singleton.h"

class InputSystem {
	CLASS_TYPEDEFS( InputSystem )
	SINGLETON_DECLARATIONS(InputSystem) {
	}

protected:
	LPDIRECTINPUT8	m_pSystem{};

	Neo::Keyboard	m_Keyboard{};
	Neo::Mouse		m_Mouse{};

public:
	Bool		Init(HWND hWnd) {
		HRESULT hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pSystem, NULL );
		if (FAILED(hr)) {
			return false;
		}

		if (!m_Keyboard.Init(hWnd, m_pSystem)) {
			return false;
		}
		verify(m_Keyboard.Acquire());

		if (!m_Mouse.Init(hWnd, m_pSystem)) {
			return false;
		}
		verify(m_Mouse.Acquire());

		return true;
	}

	void	Release() {
		
	}

	Bool	Poll() {
		if (!m_Keyboard.Poll()) {
			m_Keyboard.Acquire();
		}

		if (!m_Mouse.Poll()) {
			m_Mouse.Acquire();
		}

		return true;
	}

	Neo::Keyboard&		Keyboard() {
		return m_Keyboard;
	}

	Neo::Mouse&			Mouse() {
		return m_Mouse;
	}
};