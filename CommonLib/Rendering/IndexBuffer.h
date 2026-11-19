#pragma once

#include "System\Typedefs.h"
#include "System\File.h"

#include "System\List.h"

namespace Neo {
	class IndexBuffer {
		CLASS_TYPEDEFS(IndexBuffer)

	public:
		typedef List<UInt32>	TContainer;

	protected:
		TContainer	m_Indices;

	public:
		void		Add(TContainer::TData vbIndex) {
			m_Indices.Add(vbIndex);
		}

		void		AddAtIndex(UInt32 index, TContainer::TData vbIndex) {
			m_Indices.EnsureSize(index + 1);
			m_Indices[index] = vbIndex;
		}

		void		AddRange(const TContainer::TData* indices, UInt32 length) {
			m_Indices.AddRange(indices, length);
		}

		void		AddRange(const Int32* indices, UInt32 length) {
			for (UInt32 ix = 0; ix < length; ++ix) {
				Add(indices[ix]);
			}
		}

		TContainer::TData& operator[](UInt32 index) {
			return m_Indices[index];
		}

		const TContainer::TData& operator[](UInt32 index) const {
			return m_Indices[index];
		}

		const TContainer::TData* Data() const {
			return m_Indices.Data().data();
		}

		UInt32		Count() const {
			return m_Indices.Length();
		}

		void		Clear() {
			m_Indices.Clear();
		}

		void		Resize(UInt32 size) {
			m_Indices.Resize(size);
		}

		TSelf& operator=(const TSelf& rhs) {
			m_Indices = rhs.m_Indices;
			return *this;
		}

		TSelf& operator+=(const TSelf& rhs) {
			m_Indices += rhs.m_Indices;
			return *this;
		}

		/*virtual Bool	ReadFrom(json_value* root) {
			Int32	size = root->u.array.length;

			Clear();
			for (int ix = 0; ix < size; ++ix) {
				m_Indices.Add(JsonSerializer<UInt32>::ReadFrom(root->u.array.values[ix]));
			}

			return true;
		}*/

		/*virtual Bool	WriteTo( File& file ) const {
			Int32 size = m_Indices.Length();
			file.Write( size );

			return file.Write( &m_Indices[0], m_Indices.Length() );
		}*/
	};
}