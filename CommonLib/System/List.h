#pragma once

#include "Typedefs.h"
#include "File.h"

#include <vector>
#include <typeinfo>

template< typename _TData >
class AList {
	CLASS_TYPEDEFS( AList );

public:
	typedef _TData	TData;
	typedef std::vector<TData>	TContainer;

	typedef typename TContainer::iterator		TIterator;
	typedef typename TContainer::const_iterator	TConstIterator;
	typedef typename TContainer::iterator		iterator;
	typedef typename TContainer::const_iterator	const_iterator;

protected:
	TContainer	m_Data;

public:
	AList() {
	}

	AList(File& file) {
	}

	TData&	operator[]( UInt32 index ) {
		return m_Data[index];
	}

	const TData&	operator[]( UInt32 index ) const {
		return m_Data[index];
	}

	UInt32	Length() const {
		return m_Data.size();
	}

	void	Add( const TData& data ) {
		m_Data.push_back( data );
	}

	Int32	AddUnique( const TData& data ) {
		for( Int32 ix = 0; ix < Length(); ++ix ) {
			if( m_Data[ix] == data ) {
				return ix;
			}
		}
		m_Data.push_back( data );
		return Length() - 1;
	}

	void	RemoveIndex(UInt32 index) {
		m_Data.erase(Begin() + index);
	}

	Int32	FindIndexOf(const TData& data) const {
		Int32 index = 0;
		FOREACH(elem, m_Data) {
			if (*elem != data) {
				++index;
				continue;
			}

			return index;
		}

		return -1;
	}

	TConstIterator	Find(const TData& data) const {
		FOREACH_CONST(elem, m_Data) {
			if (*elem == data) {
				return elem;
			}
		}

		return End();
	}

	void	Remove(const TData& data) {
		TConstIterator iter = Find(data);
		if (iter == End()) {
			return;
		}

		m_Data.erase(iter);
	}

	void	EnsureSize(UInt32 size) {
		if (size > Length()) {
			m_Data.resize(size);
		}
	}

	void	Resize(UInt32 size) {
		m_Data.resize(size);
	}

	TIterator	Begin() {
		return m_Data.begin();
	}

	TIterator	End() {
		return m_Data.end();
	}

	TConstIterator	Begin() const {
		return m_Data.begin();
	}

	TConstIterator	End() const {
		return m_Data.end();
	}

	TIterator	begin() {
		return m_Data.begin();
	}

	TIterator	end() {
		return m_Data.end();
	}

	TConstIterator	begin() const {
		return m_Data.begin();
	}

	TConstIterator	end() const {
		return m_Data.end();
	}

	void	Clear() {
		m_Data.clear();
	}

	TSelf&	operator+=(const TSelf& rhs) {
		for (UInt32 ix = 0; ix < rhs.Length(); ++ix) {
			Add(rhs[ix]);
		}
		return *this;
	}
};

template< typename _TData >
class List : public AList<_TData> {
	INHERITEDCLASS_TYPEDEFS( List, AList<_TData> );

public:
	typedef typename TSuper::TData	TData;

	typedef typename TSuper::TIterator		TIterator;
	typedef typename TSuper::TConstIterator	TConstIterator;

protected:

public:
	void	Clear() {
		TSuper::Clear();
	}

	List() {
	}
};

template<typename _TData>
class List<_TData*> : public AList<_TData*> {
	INHERITEDCLASS_TYPEDEFS(List, AList<_TData*>);

public:
	typedef typename _TData	TData;

	typedef typename TSuper::TIterator		TIterator;
	typedef typename TSuper::TConstIterator	TConstIterator;

protected:

public:
	void	Clear() {
		TSuper::Clear();
	}

	List() {
	}

	void	DestroyContents() {
		//DeleteContents(*this);
		Clear();
	}
};

template< typename _TData >
void Destroy(List<_TData*>& stlContainer) {
	DeleteContents(stlContainer);
	stlContainer.Clear();
}

template< typename _TData >
void Destroy(List<const _TData*>& stlContainer) {
	DeleteContents(stlContainer);
	stlContainer.Clear();
}

template< class _TAsset >
class ContainerIterator<List<_TAsset>> {
public:
	typedef typename List<_TAsset>	TContainer;
	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;
	typedef typename TUndecorated::TIterator		Iterator;
	typedef typename TUndecorated::TConstIterator	ConstIterator;

	static Iterator	Begin(TContainer& container) {
		return container.Begin();
	}

	static ConstIterator	Begin(const TContainer& container) {
		return container.Begin();
	}

	static Iterator	End(TContainer& container) {
		return container.End();
	}

	static ConstIterator	End(const TContainer& container) {
		return container.End();
	}
};

#include "System\JsonSerializer.h"

template<typename TElem>
class JsonSerializer<List<TElem>> {
public:
	static List<TElem>	ReadFrom(const Value& root) {
		List<TElem> outVal;

		ReadFrom( root, outVal );

		return outVal;
	}

	static Bool	ReadFrom(const Value& root, List<TElem>& outVal) {
		assert(root.IsArray());

		Value::ConstArray arr = root.GetArray();
		FOREACH(iter, arr) {
			Value elemVal = (*iter);
			typename List<TElem>::TData elem = JsonSerializer<typename List<TElem>::TData>::ReadFrom(elemVal);
			outVal.Add(elem);
		}

		return true;
	}

	static Bool	ReadFrom(json_value* root, List<TElem>& outVal) {
		assert(root->type == json_type::json_array);

		for (UInt32 ix = 0; ix < root->u.array.length; ++ix) {
			outVal.Add(JsonSerializer<TElem>::ReadFrom(root->u.array.values[ix]));
		}

		return true;
	}
};

template<typename TElem>
class JsonSerializer<List<TElem*>> {
public:
	static List<TElem*>	ReadFrom(const Value& root) {
		List<TElem*> outVal;

		ReadFrom(root, outVal);

		return outVal;
	}

	static Bool	ReadFrom(const Value& root, List<TElem*>& outVal) {
		assert(root.IsArray());

		DeleteContents(outVal);
		outVal.Clear();

		Value::ConstArray arr = root.GetArray();
		FOREACH(iter, arr) {
			const Value::Array::ValueType* elemVal = iter;
			typename List<TElem*>::TData* elem = JsonSerializer<typename List<TElem*>::TData*>::ReadFrom(*elemVal);
			outVal.Add(elem);
		}

		return true;
	}

	static Bool	ReadFrom(json_value* root, List<TElem*>& outVal) {
		assert(root->type == json_type::json_array);

		for (int ix = 0; ix < root->u.array.length; ++ix) {
			outVal.Add(JsonSerializer<TElem*>::ReadFrom(root->u.array.values[ix]));
		}

		return true;
	}
};