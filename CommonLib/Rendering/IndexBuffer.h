#pragma once

#include "System\Typedefs.h"
#include "System\File.h"

#include "System\List.h"

#include "System\JsonSerializer.h"
#include "System\json.h"

class IndexBuffer : public IJsonSerializable {
	CLASS_TYPEDEFS( IndexBuffer );

public:
	typedef List<UInt32>	TContainer;

protected:
	TContainer	m_Indices;

public:
	void		AddIndex( UInt32 index ) {
		m_Indices.Add( index );
	}

	UInt32&	operator[](UInt32 index) {
		return m_Indices[index];
	}

	const UInt32&	operator[](UInt32 index) const {
		return m_Indices[index];
	}

	UInt32		Count() const {
		return m_Indices.Length();
	}

	void		Clear() {
		m_Indices.Clear();
	}

	void		Resize( UInt32 size ) {
		m_Indices.Resize(size);
	}

	TSelf&	operator=( const TSelf& rhs ) {
		m_Indices = rhs.m_Indices;
		return *this;
	}

	TSelf&	operator+=(const TSelf& rhs) {
		m_Indices += rhs.m_Indices;
		return *this;
	}

	virtual Bool	ReadFrom(json_value* root) {
		Int32	size = root->u.array.length;

		Clear();
		for (int ix = 0; ix < size; ++ix) {
			m_Indices.Add(JsonSerializer<UInt32>::ReadFrom(root->u.array.values[ix]));
		}

		return true;
	}

	/*virtual Bool	WriteTo( File& file ) const {
		Int32 size = m_Indices.Length();
		file.Write( size );

		return file.Write( &m_Indices[0], m_Indices.Length() );
	}*/
};

template<>
class JsonSerializer<IndexBuffer> {
public:
	static Bool	ReadFrom(json_value* jsonVal, IndexBuffer& list) {
		return list.ReadFrom(jsonVal);
	}
};