#pragma once

#include "System\Typedefs.h"
#include "System/Singleton.h"
#include "System\List.h"
#include "System\Map.h"

#include <xaudio2.h>

class AVoice;
class MasteringVoice;
class SourceVoice;
class SubmixVoice;
class Sound;

class AudioSystem {
	CLASS_TYPEDEFS(AudioSystem)
	SINGLETON_DECLARATIONS(AudioSystem) {
	}

protected:
	IXAudio2*		m_Audio2{};

	MasteringVoice* m_MasteringVoice{};

	List<AVoice*>	m_Voices{};

	Map<UINT64, SourceVoice*>	m_FormatMap{};

protected:
	MasteringVoice* CreateMasteringVoice();

	static UINT64		GenerateHash(const WAVEFORMATEX& format) {
		auto val = (UINT64)format.nChannels << 56 | (UINT64)format.nSamplesPerSec << 32 | (UINT64)format.wBitsPerSample << 16 | format.wFormatTag;

		assert( (val >> 56) == format.nChannels );
		assert( ((val >> 32) & 0xFFFF ) == format.nSamplesPerSec );
		assert( ((val >> 16) & 0xFF) == format.wBitsPerSample );
		assert( (val & 0xFF) == format.wFormatTag );

		return val;
	}

	Bool FindSourceVoice(const WAVEFORMATEX& format, SourceVoice*& outVoice) {
		UINT64 hash = GenerateHash(format);
		if (!m_FormatMap.Contains(hash)) {
			outVoice = nullptr;
			return false;
		}

		outVoice = m_FormatMap[hash];
		return true;
	}
	
public:
	virtual Bool	Init();
	virtual void	Release();

	SourceVoice* CreateSourceVoice(const WAVEFORMATEX& format, SubmixVoice* destVoice);
	SourceVoice* CreateSourceVoice(const WAVEFORMATEX& format);
	SourceVoice* CreateSourceVoice(Sound* sound, SubmixVoice* destVoice);
	SubmixVoice* CreateSubmixVoice(UInt32 numChannels, UInt32 sampleRate);

	Bool	CommitChanges(UInt32 operationSet) {
		return SUCCEEDED(m_Audio2->CommitChanges(operationSet));
	}
};