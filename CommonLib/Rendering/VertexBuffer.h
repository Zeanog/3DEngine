#pragma once

#include "System/Typedefs.h" 
#include "Math/Vector.h"
#include "Math/glm/vec3.hpp"
#include "Math/glm/vec2.hpp"
#include "System/List.h"
#include "System/Map.h"
#include "System/StackString.h"
#include "Math/MathUtils.h"
#include "Rendering/VertexBufferObject.h"

#define DEFINE_LIST_INTERFACES(name, list)	\
void				Add##name( const decltype(list)::TData& n ) {		\
	list.Add(n);										\
}														\
decltype(list)::TData& name(UInt32 index) {								\
	return list[index];									\
}														\
const decltype(list)::TData& name(UInt32 index) const {					\
	return list[index];									\
}														\
const decltype(list)& name##s() const {					\
	return list;										\
}

#define DEFINE_MAP_INTERFACES(name, map)		\
void				Add##name( decltype(map)::TKey key, const decltype(map)::TValue& n ) {		\
	map.Add(key, n);									\
}														\
decltype(map)::TValue& name(decltype(map)::TKey key) {	\
	return map[key];									\
}														\
const decltype(map)::TValue& name(decltype(map)::TKey key) const {					\
	return map[key];									\
}														\
const decltype(map)& name##s() const {					\
	return map;											\
}														\
decltype(map)& name##s() {								\
	return map;											\
}

namespace fbxsdk {
	class FbxStringList;
}

namespace Neo {
	class VertexBuffer {
		CLASS_TYPEDEFS(VertexBuffer)

	public:
		typedef glm::vec3	TPosition;
		typedef glm::vec2	TTextureCoordinate;
		typedef glm::vec3	TNormal;

		enum VertexAttributes : UInt32 {
			PositionAttrib = SetBit<UInt32>(0U),
			NormalAttrib = SetBit<UInt32>(1U),
			TangentAttrib = SetBit<UInt32>(2U),
			BiTangentAttrib = SetBit<UInt32>(3U),
			TexCoordsAttrib0 = SetBit<UInt32>(4U),
			TexCoordsAttrib1 = SetBit<UInt32>(5U),
			TexCoordsAttrib2 = SetBit<UInt32>(6U),
		};

	protected:
		UInt32							m_Handle{};

		VertexArrayObject<TPosition>	m_Positions;
		Map<StaticString, VertexArrayObject<TTextureCoordinate>>	m_TextureCoordinates;
		VertexArrayObject<TNormal>		m_Normals;
		VertexArrayObject<TNormal>		m_Tangents;
		VertexArrayObject<TNormal>		m_BiTangents;

	public:
		VertexBuffer();

		Bool	Bind(VertexAttributes attribs) const;
		void	Unbind() const;

		UInt32			NumVerts() const {
			return m_Positions.Length();
		}

		DEFINE_LIST_INTERFACES(Position, m_Positions)
		DEFINE_LIST_INTERFACES(Normal, m_Normals)
		DEFINE_LIST_INTERFACES(Tangent, m_Tangents)
		DEFINE_LIST_INTERFACES(BiTangent, m_BiTangents)
		DEFINE_MAP_INTERFACES(TextureCoordinate, m_TextureCoordinates)

		TSelf& operator=(const TSelf& rhs) {
			m_Positions = rhs.m_Positions;
			m_Normals = rhs.m_Normals;
			m_Tangents = rhs.m_Tangents;
			m_BiTangents = rhs.m_BiTangents;
			m_TextureCoordinates = rhs.m_TextureCoordinates;
			return *this;
		}

		void	Clear() {
			m_Positions.Clear();
			m_Normals.Clear();
			m_Tangents.Clear();
			m_BiTangents.Clear();
			m_TextureCoordinates.Clear();
		}

		void	Resize(UInt32 size) {
			m_Positions.Resize(size);
			m_Normals.Resize(size);
			//m_Tangents.Resize(size);
			//m_BiTangents.Resize(size);
			FOREACH(iter, m_TextureCoordinates) {
				iter->second.Resize(size);
			}
		}

		void	Resize(Int32 size, const fbxsdk::FbxStringList& texCoordGroups);

		TSelf& operator+=(const TSelf& rhs) {
			m_Positions += rhs.m_Positions;
			m_Normals += rhs.m_Normals;
			m_Tangents += rhs.m_Tangents;
			m_BiTangents += rhs.m_BiTangents;
			m_TextureCoordinates += rhs.m_TextureCoordinates;
			return *this;
		}
	};
}