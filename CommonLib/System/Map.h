#pragma once

#include "Typedefs.h"
#include <map>

template< typename _TKey, typename _TValue >
void DeleteContents( std::map<_TKey, _TValue*>& stlContainer ) {
	for( typename std::map<_TKey, _TValue*>::iterator iter = stlContainer.begin(); iter != stlContainer.end(); iter++ ) {   
		DeletePtr( iter->second );             
	}
}

template< typename _TKey, typename _TValue >
class AMap {
	CLASS_TYPEDEFS(AMap);

public:
	typedef _TKey	TKey;
	typedef _TValue	TValue;
	typedef std::map<TKey, TValue>	TContainer;
	typedef typename TContainer::iterator	TIterator;
	typedef typename TContainer::const_iterator	TConstIterator;

protected:
	TContainer	m_Data;

public:
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

	TValue&	operator[](typename Param<TKey>::Type key) {
		return m_Data[key];
	}

	const TValue&	operator[](typename Param<TKey>::Type key) const {
		TConstIterator iter = m_Data.find(key);
		return iter->second;
	}

	UInt32	Length() const {
		return m_Data.size();
	}

	virtual void	Add(typename Param<TKey>::Type key, typename Param<TValue>::Type value) {
		m_Data.insert(TContainer::value_type(key, value));
	}

	void	Remove(typename Param<TKey>::Type key) {
		m_Data.erase(key);
	}

	/*Int32	AddUnique(const TData& data) {
		for (Int32 ix = 0; ix < Length(); ++ix) {
			if (m_Data[ix] == data) {
				return ix;
			}
		}
		m_Data.push_back(data);
		return Length() - 1;
	}*/

	/*TConstIterator	Find(const TData& data) const {
		FOREACH_CONST(iter, m_Data) {
			TData d = *iter;
			if (d == data) {
				return iter;
			}
		}

		return cend();
	}*/

	TValue&	Find(typename Param<TKey>::Type key) {
		return m_Data.at(key);
	}

	const TValue&	Find(typename Param<TKey>::Type key) const {
		return m_Data.at(key);
	}

	void	EnsureSize(UInt32 size) {
		if (size > Length()) {
			m_Data.reserve(size);
		}
	}

	void	Resize(UInt32 size) {
		m_Data.resize(size);
	}

	void	Clear() {
		m_Data.clear();
	}

	/*virtual Bool	ReadFrom( File& file ) {
	return true;
	}*/

	//virtual Bool	WriteTo( File& file ) const {
	//	//file.Write( Length() );
	//	//file.Write( &m_Data[0], Length() );
	//	return true;
	//}
};

template< typename _TKey, typename _TValue >
class Map : public AMap<_TKey, _TValue> {
	INHERITEDCLASS_TYPEDEFS(Map, TEMPLATE_2(AMap, _TKey, _TValue) );

protected:

public:
};

template< typename _TKey, typename _TValue >
class Map<_TKey, _TValue*> : public AMap<_TKey, _TValue*> {
	INHERITEDCLASS_TYPEDEFS(Map, TEMPLATE_2(AMap, _TKey, _TValue*));

protected:

public:
	virtual void	Add(typename Param<TKey>::Type key, typename Param<_TValue*>::Type value) override {
		m_Data.insert(TContainer::value_type(key, value));
	}
};

template< typename _TKey, typename _TValue >
class Map<_TKey, const _TValue*> : public AMap<_TKey, const _TValue*> {
	INHERITEDCLASS_TYPEDEFS(Map, TEMPLATE_2(AMap, _TKey, const _TValue*));

protected:

public:
	virtual void	Add(typename Param<TKey>::Type key, typename Param<const _TValue*>::Type value) override {
		m_Data.insert(TContainer::value_type(key, value));
	}
};


template<typename _TKey, typename _TValue>
class ContainerIterator<Map<_TKey, _TValue>> {
public:
	typedef typename Map<_TKey, _TValue>	TContainer;
	typedef typename TypeInfo<TContainer>::TUndecorated TUndecorated;
	typedef typename TUndecorated::TIterator		Iterator;
	typedef typename TUndecorated::TConstIterator	ConstIterator;

	static Iterator	Begin(TUndecorated& container) {
		return container.Begin();
	}

	static ConstIterator	Begin(const TUndecorated& container) {
		return container.Begin();
	}

	static Iterator	End(TUndecorated& container) {
		return container.End();
	}

	static ConstIterator	End(const TUndecorated& container) {
		return container.End();
	}
};

/*
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
		for (Value::ConstArray::ConstValueIterator iter = arr.Begin(); iter != arr.End(); ++iter) {
			Value elemVal = (*iter);
			typename List<TElem>::TData elem = JsonSerializer<typename List<TElem>::TData>::ReadFrom(elemVal);
			outVal.Add(elem);
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
		for (Value::Array::ConstValueIterator iter = arr.Begin(); iter != arr.End(); ++iter) {
			const Value::Array::ValueType* elemVal = iter;
			typename List<TElem*>::TData* elem = JsonSerializer<typename List<TElem*>::TData*>::ReadFrom(*elemVal);
			outVal.Add(elem);
		}

		return true;
	}
};
*/