#pragma once

#include "System/Typedefs.h"
#include "System/List.h"
#include <GL/glew.h>

class AVertexBufferObject {
	CLASS_TYPEDEFS(AVertexBufferObject)

protected:
	UInt32		m_VBO{};

	AVertexBufferObject();

public:
	virtual ~AVertexBufferObject();
	virtual void	UploadData(UInt32 location);

	virtual void Bind() const {
		assert(m_VBO > 0);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		assert(!glGetError());
	}
	virtual void Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		assert(!glGetError());
	}
};

template<typename _TData>
class VertexArrayObject : public AVertexBufferObject {
	INHERITED_CLASS_TYPEDEFS(VertexArrayObject, AVertexBufferObject)

public:
	typedef List<_TData>			TList;
	typedef typename TList::TData	TData;

protected:
	UInt32		m_VAO{};
	TList		m_Data{};

public:
	VertexArrayObject() : TSuper() {
		
	}

	virtual ~VertexArrayObject() override {
		if (m_VAO > 0) {
			glDeleteVertexArrays(1, &m_VAO);
			assert(!glGetError());
			m_VAO = 0;
		}
	}

	virtual void	UploadData( UInt32 location ) override {
		TSuper::UploadData(location);

		assert(m_VAO == 0);
		glGenVertexArrays(1, &m_VAO);
		assert(!glGetError());

		TSuper::Bind();
		Bind();

		constexpr int dataSize = sizeof(decltype(m_Data)::TData);
		glBufferData(GL_ARRAY_BUFFER, m_Data.Length() * dataSize, &m_Data[0], GL_STATIC_DRAW);
		assert(!glGetError());

		constexpr int dataElementCount = TData::length();
		glVertexAttribPointer(location, dataElementCount, GL_FLOAT, GL_FALSE, dataSize, (void*)0);
		assert(!glGetError());

		glEnableVertexAttribArray(location);
		assert(!glGetError());

		Unbind();
		TSuper::Unbind();
	}

	void	Add(const TData& element) {
		m_Data.Add(element);
	}

	TData&	operator[]( UInt32 index ) {
		return m_Data[index];
	}

	const TData& operator[](UInt32 index) const {
		return m_Data[index];
	}

	const TList& operator()() const {
		return m_Data;
	}

	TList& operator()() {
		return m_Data;
	}

	void Resize(UInt32 size) {
		m_Data.Resize(size);
	}

	void EnsureSize(UInt32 size) {
		m_Data.EnsureSize(size);
	}

	void Clear() {
		m_Data.Clear();
	}

	UInt32 	Length() const {
		return m_Data.Length();
	}

	virtual void Bind() const override {
		glBindVertexArray(m_VAO);
		assert(!glGetError());
	}

	virtual void Unbind() const override {
		glBindVertexArray(0);
		assert(!glGetError());
	}

	TSelf& operator=( const TSelf& rhs ) {
		m_VBO = rhs.m_VBO;
		m_VAO = rhs.m_VAO;
		m_Data = rhs.m_Data;
		return *this;
	}

	Bool operator==(const TSelf& rhs) {
		if (m_VBO != rhs.m_VBO) {
			return false;
		}

		if (m_VAO != rhs.m_VAO) {
			return false;
		}

		/*if (m_Data != rhs.m_Data) {

		}*/
		return true;
	}

	TSelf& operator+=(const TSelf& rhs) {
		m_Data += rhs.m_Data;
		return *this;
	}
};