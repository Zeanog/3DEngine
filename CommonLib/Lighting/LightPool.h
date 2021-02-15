#pragma once

#include "../System/Typedefs.h"
#include "../System/Singleton.h"
#include "Light.h"
#include "../System/Pool.h"
#include <vector>


class DirectionalLightPool : public Pool<Light_Directional> {
	INHERITEDCLASS_TYPEDEFS( DirectionalLightPool, Pool<Light_Directional> );
	SINGLETON_DECLARATIONS( DirectionalLightPool ) {
	}

public:
	typedef TSuper::TContainer		TContainer;
	typedef TSuper::Iterator		Iterator;
	typedef TSuper::ConstIterator	ConstIterator;

protected:
	std::vector<Float32>	m_Directions;

public:
};

class SpotLightPool : public Pool<Light_Spot> {
	INHERITEDCLASS_TYPEDEFS( SpotLightPool, Pool<Light_Spot> );
	SINGLETON_DECLARATIONS( SpotLightPool ) {
	}

public:
	typedef TSuper::TContainer		TContainer;
	typedef TSuper::Iterator		Iterator;
	typedef TSuper::ConstIterator	ConstIterator;

protected:
	std::vector<Float32>	m_Positions;
	std::vector<Float32>	m_Directions;
	std::vector<Float32>	m_Radii;

public:;
};

class PointLightPool : public Pool<Light_Point> {
	INHERITEDCLASS_TYPEDEFS( PointLightPool, Pool<Light_Point> );
	SINGLETON_DECLARATIONS( PointLightPool ) {
	}

public:
	typedef TSuper::TContainer		TContainer;
	typedef TSuper::Iterator		Iterator;
	typedef TSuper::ConstIterator	ConstIterator;

protected:
	std::vector<Float32>	m_Positions;
	std::vector<Float32>	m_Radii;

public:
};