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
	virtual void Bind() const {
		assert(m_VBO > 0);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	}
	virtual void Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
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
		glGenVertexArrays(1, &m_VAO);
	}

	virtual ~VertexArrayObject() override {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}

	void	UploadData( UInt32 location ) {
		Bind();
		TSuper::Bind();

		glBufferData(GL_ARRAY_BUFFER, m_Data.Length() * sizeof(decltype(m_Data)::TData), &m_Data[0], GL_STATIC_DRAW);
		glVertexAttribPointer(location, TData::length(), GL_FLOAT, GL_FALSE, sizeof(decltype(m_Data)::TData), (void*)0);
		glEnableVertexAttribArray(location);

		TSuper::Unbind();
		Unbind();
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

	void Clear() {
		m_Data.Clear();
	}

	UInt32 	Length() const {
		return m_Data.Length();
	}

	virtual void Bind() const override {
		glBindVertexArray(m_VAO);
	}

	virtual void Unbind() const override {
		glBindVertexArray(0);
	}

	TSelf& operator=( const TSelf& rhs ) {
		m_VBO = rhs.m_VBO;
		m_VAO = rhs.m_VAO;
		m_Data = rhs.m_Data;
		return *this;
	}

	TSelf& operator+=(const TSelf& rhs) {
		m_Data += rhs.m_Data;
		return *this;
	}
};