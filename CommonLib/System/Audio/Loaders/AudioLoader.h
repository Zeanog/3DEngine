#pragma once

#include "..\..\Typedefs.h"
#include "..\..\String.h"
#include "..\..\StaticString.h"
#include <xaudio2.h>
#include <memory>

class AudioLoader {
protected:
	std::unique_ptr<BYTE>	m_AudioData;
	UInt32					m_AudioDataSize;

	WAVEFORMATEXTENSIBLE	m_Format{};

public:
	virtual Bool	Load(const Char* fileName) = 0;

	virtual Bool	Load(const StaticString& fileName) {
		return Load(fileName.CStr());
	}

	virtual Bool	Load(const String& fileName) {
		return Load(fileName.CStr());
	}

	virtual UInt32	Length() const {
		return m_AudioDataSize;
	}

	virtual Bool	IsValid() const {
		return m_AudioData != nullptr;
	}

	virtual void	Clear() {

	}

	virtual const WAVEFORMATEXTENSIBLE& Format() const {
		return m_Format;
	}

	virtual WAVEFORMATEXTENSIBLE& Format() {
		return m_Format;
	}

	virtual std::unique_ptr<Byte>&& Data() {
		return std::move(m_AudioData);
	}

	UInt32	DataSize() const {
		return m_AudioDataSize;
	}
};