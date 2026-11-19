#pragma once

#include "System/Typedefs.h"
#include "System/String/StaticString.h"
#include "System/List.h"
#include "System/Map.h"
#include "Rendering/Mesh.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Joint.h"
#include "Rendering/ModelLoaders/ModelDef.h"

class IMeshLoader {
	CLASS_TYPEDEFS(IMeshLoader)

protected:
	virtual Bool LoadScene(const StaticString& fileName) = 0;

public:
	virtual Bool	Load(const ModelDef& def, Neo::Mesh& outMesh) = 0;
	virtual	void	Clear() = 0;
};