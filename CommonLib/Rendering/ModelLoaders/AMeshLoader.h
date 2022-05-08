#pragma once

#include "System/Typedefs.h"
#include "System/StaticString.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "System/List.h"
#include "Rendering/Mesh.h"

class AMeshLoader {
public:
	struct JointInfo {
		StaticString	Name;
		Int32			ParentIndex;
	};

protected:
	VertexBuffer	m_VertexBuffer;
	IndexBuffer		m_IndexBuffer;
	List<Neo::Mesh::AMaterialSlot*>	m_MaterialSlots;

	List<JointInfo>			m_Joints;
	List<AnimationClip*>	m_AnimationClips;

public:
	virtual Bool	Load(const StaticString& fileName) = 0;
	virtual	void	Clear() = 0;

	const VertexBuffer& VB() const {
		return m_VertexBuffer;
	}

	const IndexBuffer& IB() const {
		return m_IndexBuffer;
	}

	const List<AnimationClip*>& AnimationClips() const {
		return m_AnimationClips;
	}

	const List<Neo::Mesh::AMaterialSlot*>& MaterialSlots() const {
		return m_MaterialSlots;
	}

	const List<JointInfo>& Joints() const {
		return m_Joints;
	}

	virtual glm::mat4	GetJointLocalTransform(const StaticString& jointName, Float32 time) const = 0;

	/*virtual void	CopyComponentsTo(List<GeometricComponent*>& comps) const {
		comps.Clear();
		for (int ix = 0; ix < m_Components.Length(); ++ix) {
			comps.Add(m_Components[ix]);
		}
	}*/
};