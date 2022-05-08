#pragma once

#include "System/Typedefs.h"
#include <vector>

class Model;
class ALight;

class Scene {
protected:
	std::vector<Model*>		m_Models;
	std::vector<ALight*>	m_Lights;

	Neo::Bounds				m_Bounds;

public:
	void		Update();
	void		Render();
};