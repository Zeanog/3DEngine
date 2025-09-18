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

	Map<UINT64, List<SourceVoice*>>	m_FormatToSourceMap{};

protected:
	MasteringVoice* CreateMasteringVoice();

	static UINT64		GenerateHash(const WAVEFORMATEX& format) {
		static constexpr Byte numChannelShift = 56;
		static constexpr Byte samplesPerSecShift = 32;
		static constexpr Byte bitsPerSampleShift = 16;

		auto val = (UINT64)format.nChannels << numChannelShift | (UINT64)format.nSamplesPerSec << samplesPerSecShift | (UINT64)format.wBitsPerSample << bitsPerSampleShift | format.wFormatTag;

		assert( (val >> numChannelShift) == format.nChannels );
		assert( ((val >> samplesPerSecShift) & 0xFFFF ) == format.nSamplesPerSec );
		assert( ((val >> bitsPerSampleShift) & 0xFF) == format.wBitsPerSample );
		assert( (val & 0xFF) == format.wFormatTag );

		return val;
	}

	static UINT64		GenerateHash(UInt32 numChannels, UInt32 sampleRate) {
		static constexpr Byte numChannelShift = 16;

		auto val = (UINT64)numChannels << numChannelShift | (UINT64)sampleRate;

		assert((val >> numChannelShift) == numChannels);
		assert((val & 0xFF) == sampleRate);

		return val;
	}

	Bool FindSourceVoice(const WAVEFORMATEX& format, UINT64& outHash, SourceVoice*& outVoice);
	
public:
	virtual Bool	Init();
	virtual void	Release();

	//SourceVoice* CreateSourceVoice(const WAVEFORMATEX& format, SubmixVoice* destVoice);
	SourceVoice* CreateSourceVoice(const WAVEFORMATEX& format);
	//SourceVoice* CreateSourceVoice(Sound* sound, SubmixVoice* destVoice);
	SourceVoice* CreateSourceVoice(const Sound* sound);
	SubmixVoice* CreateSubmixVoice(UInt32 numChannels, UInt32 sampleRate);

	Bool	CommitChanges(UInt32 operationSet) {
		return SUCCEEDED(m_Audio2->CommitChanges(operationSet));
	}
};