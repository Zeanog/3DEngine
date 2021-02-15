#pragma once

#include "ADevice.h"
#include "System/Functors/MultiFunctor.h"

class KeyboardState {
public:
	static const Int32	NumKeys = 256;

protected:
	Byte				m_CompressedData[NumKeys / 8];
	Byte				m_PrevCompressedData[NumKeys / 8];

protected:
	Bool	IsKeyDown(Int32 byteIndex, Int32 bitIndex) const {
		return (m_CompressedData[byteIndex] & (1 << bitIndex)) != 0;
	}

	Bool	WasKeyDown(Int32 byteIndex, Int32 bitIndex) const {
		return (m_PrevCompressedData[byteIndex] & (1 << bitIndex)) != 0;
	}

	static Bool	ShouldNotify(const Byte* lhs, const Byte* rhs, UInt32 length) {
		if (0 != memcmp(lhs, rhs, length)) {
			return true;
		}

		Bool notify = false;
		for (UInt32 ix = 0; ix < length; ++ix) {
			notify |= (lhs[ix] != 0);
		}

		return notify;
	}

	static void	Copy(Byte* dst, const Byte* src, UInt32 length) {
		memcpy(dst, src, length);
	}

public:
	KeyboardState() {
		memset(m_CompressedData, 0, STATIC_ARRAY_LENGTH(m_CompressedData));
		memset(m_PrevCompressedData, 0, STATIC_ARRAY_LENGTH(m_PrevCompressedData));
	}

	Bool	KeyIsDown(UInt32 vk) const {
		Int32 byteIndex = vk / 8;
		Int32 bitIndex = vk % 8;
		return IsKeyDown(byteIndex, bitIndex);
	}

	Bool	KeyWasPressed(UInt32 vk) const {
		Int32 byteIndex = vk / 8;
		Int32 bitIndex = vk % 8;
		return IsKeyDown(byteIndex, bitIndex) && !WasKeyDown(byteIndex, bitIndex);
	}

	Bool	KeyWasReleased(UInt32 vk) const {
		Int32 byteIndex = vk / 8;
		Int32 bitIndex = vk % 8;
		return !IsKeyDown(byteIndex, bitIndex) && WasKeyDown(byteIndex, bitIndex);
	}

	Bool	Compress(const Byte* srcUncompressed, UInt32 numBytes) {
		Copy(m_PrevCompressedData, m_CompressedData, STATIC_ARRAY_LENGTH(m_CompressedData));
		memset(m_CompressedData, 0, STATIC_ARRAY_LENGTH(m_CompressedData));

		UInt32 bitIndex = -1;
		UInt32 byteIndex = -1;
		for (UInt32 ix = 0; ix < numBytes; ++ix) {
			bitIndex = ix % 8;
			if (!bitIndex) {
				++byteIndex;
			}

			bool keyDown = !!srcUncompressed[ix];
			if (keyDown) {
				int b = 3;
			}

			m_CompressedData[byteIndex] |= (keyDown) << bitIndex;
		}

		return ShouldNotify(m_CompressedData, m_PrevCompressedData, STATIC_ARRAY_LENGTH(m_CompressedData));
	}
};

class Keyboard : public ADevice {
protected:
	KeyboardState		m_State;

public:
	Delegate<TYPELIST_1(KeyboardState)>			OnKeydown;

public:
	virtual Bool	Init(HWND hWnd, LPDIRECTINPUT8 system) {
		HRESULT hr = system->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL);
		if (FAILED(hr)) {
			return false;
		}
		

		m_pDevice->SetDataFormat(&c_dfDIKeyboard);
		m_pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		Acquire(); //fails with DIERR_INVALIDPARAM

		return true;
	}

	virtual Bool	Acquire() const {
		return SUCCEEDED(m_pDevice->Acquire());
	}

	virtual Bool	Unacquire() const {
		return SUCCEEDED(m_pDevice->Unacquire());
	}

	virtual Bool	Poll() {
		HRESULT hr = m_pDevice->Poll();
		if (FAILED(hr)) {
			return Acquire();
		}

		Byte uncompressedData[KeyboardState::NumKeys];
		hr = m_pDevice->GetDeviceState(STATIC_ARRAY_LENGTH(uncompressedData), uncompressedData);
		if (FAILED(hr)) {
			return false;
		}

		if (m_State.Compress(uncompressedData, STATIC_ARRAY_LENGTH(uncompressedData))) {
			NotifyObservers();
		}
		return true;
	}

protected:
	virtual void NotifyObservers() const {
		OnKeydown(m_State);
	}
};