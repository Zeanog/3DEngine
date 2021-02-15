#pragma once

#include "../System/Singleton.h"
#include <list>

template< class _TData >
class DataStructureLibrary {
	CLASS_TYPEDEFS(DataStructureLibrary);
	SINGLETON_DECLARATIONS(DataStructureLibrary);

public:
	typedef _TData	TData;

protected:
	std::list<TData*>	m_Available;
	std::list<TData*>	m_CheckedOut;

public:
	TData*	CheckOut() {
		TData*	data = NULL;

		if (m_Available.size() <= 0) {
			data = new TData();
		}
		else {
			data = m_Available.front();
			m_Available.pop_front();
		}

		m_CheckedOut.push_back(data);
		return data;
	}

	void	Return(TData*	data) {
		m_CheckedOut.remove(data);
		m_Available.push_back(data);
	}

protected:
};