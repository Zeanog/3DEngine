#pragma once

#include "System/Typedefs.h"
#include "ImageLoader.h"

namespace Neo {
	class Image {
	public:
		UInt32			m_PixelFormat;
		UInt32			m_Id;

	protected:

	public:
		Image() {
			m_Id = 0;
			m_PixelFormat = 0;
		}

		Image(UInt32 id, UInt32 format) {
			m_PixelFormat = format;
			m_Id = id;
		}

		Image(const ImageLoader& loader) {
			m_Id = 0;
			m_PixelFormat = 0;
			verify(UploadData(loader));
		}

		~Image();

		__inline static Bool		IsPowerOfTwo(UInt32 val) {
			return val && (val & (val - 1)) == 0;
		}

		__inline static Bool		IsPowerOfTwo(Int32 val) {
			return val && (val & (val - 1)) == 0;
		}

		static UInt32		NearestPowerOfTwo(UInt32 val);

		static Int32		NearestPowerOfTwo(Int32 val);

		void	Bind() const;
		void	Unbind() const;

		Bool	UploadData(const ImageLoader& loader);
	};
};