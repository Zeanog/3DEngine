#pragma once

#include "System/Typedefs.h"

#include <windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <dinput.h>
#include <dinputd.h>

#include <utility>

#include "Math/Vector.h"
#include <list>

class ADevice {
protected:
	LPDIRECTINPUTDEVICE8  m_pDevice;

protected:
	virtual void NotifyObservers() const = 0;

public:
	virtual ~ADevice() {
		if (m_pDevice) {
			m_pDevice->Release();
			m_pDevice = NULL;
		}
	}

	virtual Bool	Init(HWND hWnd, LPDIRECTINPUT8 system) = 0;
	virtual Bool	Acquire() const = 0;
	virtual Bool	Unacquire() const = 0;

	virtual Bool	Poll() = 0;
};