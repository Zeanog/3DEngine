#pragma once

#include "System/Typedefs.h"
#include "Math/Vector.h"
#include "System/List.h"
#include "System/StackString.h"

#include "System/File.h"
#include "System/IJsonSerializable.h"
#include "System/JsonSerializer.h"
#include "System/json.h"

class VertexBuffer : public IJsonSerializable {
	CLASS_TYPEDEFS( VertexBuffer );

public:
	typedef Vector<3>	TPosition;
	typedef Vector<2>	TTextureCoordinate;
	typedef Vector<3>	TNormal;
	
	enum VertexAttributes : UInt32 {
		PositionAttrib = 1 << 0,
		NormalAttrib = 1 << 1,
		TexCoordsAttrib = 1 << 2
	};

protected:
	UInt32						m_Handle;

	List<TPosition>				m_Positions;
	List<TNormal>				m_Normals;
	List<TNormal>				m_Tangents;
	List<TNormal>				m_Bitangent;
	List<TTextureCoordinate>	m_TextureCoordinates;

public:
	VertexBuffer();

	Bool	Bind(VertexAttributes attribs) const;
	void	Unbind() const;

	UInt32			NumVerts() const {
		return m_Positions.Length();
	}

	void			AddPosition( const TPosition& pos ) {
		m_Positions.Add( pos );
	}

	TPosition&		Position( UInt32 index ) {
		return m_Positions[index];
	}

	const List<TPosition>&	Positions() const {
		return m_Positions;
	}

	const TPosition&		Position( UInt32 index ) const {
		return m_Positions[index];
	}

	void			AddTextureCoordinate( const TTextureCoordinate& tc ) {
		m_TextureCoordinates.Add( tc );
	}

	TTextureCoordinate&		TextureCoordinate( UInt32 index ) {
		return m_TextureCoordinates[index];
	}

	const TTextureCoordinate&		TextureCoordinate( UInt32 index ) const {
		return m_TextureCoordinates[index];
	}

	void			AddNormal( const TNormal& n ) {
		m_Normals.Add( n );
	}

	TNormal&		Normal( UInt32 index ) {
		return m_Normals[index];
	}

	const TNormal&		Normal( UInt32 index ) const {
		return m_Normals[index];
	}

	TSelf&	operator=( const TSelf& rhs ) {
		m_Positions = rhs.m_Positions;
		m_Normals = rhs.m_Normals;
		m_TextureCoordinates = rhs.m_TextureCoordinates;
		return *this;
	}

	void	Clear() {
		m_Positions.Clear();
		m_Normals.Clear();
		m_TextureCoordinates.Clear();
	}

	void	Resize(UInt32 size) {
		m_Positions.Resize(size);
		m_Normals.Resize(size);
		m_TextureCoordinates.Resize(size);
	}

	TSelf&	operator+=(const TSelf& rhs) {
		m_Positions += rhs.m_Positions;
		m_Normals += rhs.m_Normals;
		m_TextureCoordinates += rhs.m_TextureCoordinates;
		return *this;
	}

	virtual Bool	ReadFrom(json_value* root) {
		if (root->type != json_type::json_object) {
			return false;
		}

		for (UInt32 ix = 0; ix < root->u.object.length; ++ix) {
			if(!String::StrICmp(root->u.object.values[ix].name, "positions")) {
				JsonSerializer<decltype(m_Positions)>::ReadFrom(root->u.object.values[ix].value, m_Positions);
			} else if (!String::StrICmp(root->u.object.values[ix].name, "normals")) {
				JsonSerializer<decltype(m_Normals)>::ReadFrom(root->u.object.values[ix].value, m_Normals);
			} else if (!String::StrICmp(root->u.object.values[ix].name, "texCoords")) {
				JsonSerializer<decltype(m_TextureCoordinates)>::ReadFrom(root->u.object.values[ix].value, m_TextureCoordinates);
			}
		}

		return true;
	}

	/*virtual Bool	WriteTo( File& file ) const {
		UInt32 size = m_Positions.Length();
		file.Write( size );

		return file.Write( &m_Positions[0], m_Positions.Length() );
	}*/
};