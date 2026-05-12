#pragma once

#include "System\Typedefs.h"
#include <xaudio2.h>
#include <memory>

//namespace Neo {
class Sound {
	protected:
		UInt32						m_DataSize;
		::std::unique_ptr<Byte>		m_Data;

		DEFINE_MEMBER_EX(UInt64, FormatHash)
		DEFINE_MEMBER_EX(WAVEFORMATEXTENSIBLE, Format)
		DEFINE_MEMBER_EX(Float32, Duration)

	public:
		virtual ~Sound();

		void	PopulateBuffer(XAUDIO2_BUFFER& buffer) const;
		Bool	UploadData(class AAudioLoader& loader);
};
//}