#pragma once

#include "ADevice.h"
#include "System/Functors/MultiFunctor.h"

class KeyboardState {
public:
	static constexpr Int32	NumKeys = 256;

protected:
	static constexpr UInt8	m_CompressedDataSize = NumKeys / 8;
	Byte				m_CompressedData[m_CompressedDataSize];//Bit Array
	
	static constexpr UInt8	m_PrevCompressedDataSize = NumKeys / 8;
	Byte				m_PrevCompressedData[m_PrevCompressedDataSize];//Bit Array

protected:
	Bool	IsKeyDown(Int32 byteIndex, Int32 bitIndex) const {
		return (m_CompressedData[byteIndex] & (1 << bitIndex)) != 0;
	}

	Bool	WasKeyDown(Int32 byteIndex, Int32 bitIndex) const {
		return (m_PrevCompressedData[byteIndex] & (1 << bitIndex)) != 0;
	}

	static Bool	ShouldNotify(const Byte* lhs, const Byte* rhs, UInt32 length) {
		return 0 != memcmp(lhs, rhs, length);
	}

	static void	Copy(Byte* dst, const Byte* src, UInt32 length) {
		memcpy(dst, src, length);
	}

public:
	KeyboardState() {
		memset(m_CompressedData, 0, m_CompressedDataSize);
		memset(m_PrevCompressedData, 0, m_PrevCompressedDataSize);
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

	Bool	Compress(const Byte* srcUncompressed, UInt16 numBytes) {
		assert(m_CompressedDataSize == m_PrevCompressedDataSize);

		Copy(m_PrevCompressedData, m_CompressedData, m_CompressedDataSize);
		memset(m_CompressedData, 0, m_CompressedDataSize);

		Int8 bitIndex = -1;
		Int8 byteIndex = -1;
		for (UInt16 ix = 0; ix < numBytes; ++ix) {
			bitIndex = (Int8)(ix % 8);
			if (!bitIndex) {
				++byteIndex;
			}

			Bool keyDown = !!srcUncompressed[ix];
			if (!keyDown) {
				continue;
			}

			assert(byteIndex < m_CompressedDataSize);
			m_CompressedData[byteIndex] |= (keyDown) << bitIndex;
		}

		return ShouldNotify(m_CompressedData, m_PrevCompressedData, m_CompressedDataSize);
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
		hr = m_pDevice->GetDeviceState(KeyboardState::NumKeys, uncompressedData);
		if (FAILED(hr)) {
			return false;
		}

		if (m_State.Compress(uncompressedData, KeyboardState::NumKeys)) {
			NotifyObservers();
		}
		return true;
	}

protected:
	virtual void NotifyObservers() const {
		OnKeydown(m_State);
	}
};