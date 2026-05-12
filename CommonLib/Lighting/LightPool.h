#pragma once

#include "../System/Typedefs.h"
#include "../System/Singleton.h"
#include "Light.h"
#include "../System/Pool.h"
#include <vector>


class DirectionalLightPool : public Pool<Light_Directional> {
	INHERITED_CLASS_TYPEDEFS( DirectionalLightPool, Pool<Light_Directional> )
	SINGLETON_DECLARATIONS( DirectionalLightPool ) {
	}

public:
	typedef TSuper::TContainer		TContainer;
	typedef TSuper::Iterator		Iterator;
	typedef TSuper::ConstIterator	ConstIterator;
	typedef TSuper::ReverseIterator		ReverseIterator;
	typedef TSuper::ConstReverseIterator	ConstReverseIterator;

protected:

public:
};

class SpotLightPool : public Pool<Light_Spot> {
	INHERITED_CLASS_TYPEDEFS( SpotLightPool, Pool<Light_Spot> )
	SINGLETON_DECLARATIONS( SpotLightPool ) {
	}

public:
	typedef TSuper::TContainer		TContainer;
	typedef TSuper::Iterator		Iterator;
	typedef TSuper::ConstIterator	ConstIterator;
	typedef TSuper::ReverseIterator		ReverseIterator;
	typedef TSuper::ConstReverseIterator	ConstReverseIterator;

protected:
	std::vector<Float32>	m_Positions;
	std::vector<Float32>	m_Directions;
	std::vector<Float32>	m_Radii;

public:;
};

class PointLightPool : public Pool<Light_Point> {
	INHERITED_CLASS_TYPEDEFS( PointLightPool, Pool<Light_Point> )
	SINGLETON_DECLARATIONS( PointLightPool ) {
	}

public:
	typedef TSuper::TContainer		TContainer;
	typedef TSuper::Iterator		Iterator;
	typedef TSuper::ConstIterator	ConstIterator;
	typedef TSuper::ReverseIterator		ReverseIterator;
	typedef TSuper::ConstReverseIterator	ConstReverseIterator;

protected:
	std::vector<Float32>	m_Positions;
	std::vector<Float32>	m_Radii;

public:
};