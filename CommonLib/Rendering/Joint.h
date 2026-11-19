#pragma once

#include "System/String/StaticString.h"
#include "System/List.h"
#include "System/Map.h"
#include <glm/mat4x4.hpp>

class Joint {
public:
	static void			Render(const glm::mat4& transform, float radius, UInt32 meshPrecision);

protected:
	StaticString	m_Name;
	Int32			m_Index;
	Int32			m_Parent;

public:
	Joint();
	Joint(Int32 parentIndex);

	DECLARE_GETSET(Name)
	DECLARE_GETSET(Index)
	DECLARE_GETSET(Parent)

	Bool	operator==(const Joint& rhs) const {
		return Name() == rhs.Name() && Parent() == rhs.Parent();
	}

	Bool	operator!=(const Joint& rhs) const {
		return !(*this == rhs);
	}
};

class IMeshLoader;

namespace Neo {
	class Skeleton {
	protected:
		Map<StaticString, UInt32>	m_NameToJointIndexMap;
		List<Joint>					m_Joints;

	public:
		Skeleton() {
		}

		~Skeleton() {
		}

		DECLARE_GETSET(Joints);
		
		void		AddJoint(const Joint& joint) {
			//Verify: Idea is that this will check to determine if parent joint is already in the list
			assert(joint.Parent() < (Int32)m_Joints.Length());

#if _DEBUG
			auto index = m_Joints.Length();
			assert(joint.Index() == index);
			//auto index = m_Joints.AddUnique(joint);
			m_Joints.Insert(joint.Index(), joint);
			//assert(index == (m_Joints.Length() - 1));
#else
			auto index = m_Joints.Length();
			assert(joint.Index() == index);
			m_Joints.Insert(joint.Index(), joint);
#endif
			m_NameToJointIndexMap.Add(joint.Name(), index);

		}

		Bool				Contains(const StaticString& jointName) const {
			for (UInt32 ix = 0; ix < m_Joints.Length(); ++ix) {
				if (m_Joints[ix].Name() == jointName) {
					return true;
				}
			}
			return false;
		}

		Int32 FindJointIndex(const StaticString& name) const;

		Joint*		FindJoint(int index) {
			return &m_Joints[index];
		}

		const Joint*		FindJoint(int index) const {
			return &m_Joints[index];
		}

		Joint*		FindJoint(const StaticString& name);
		const Joint*	FindJoint(const StaticString& name) const;

		UInt32					NumJoints() const {
			return m_Joints.Length();
		}
	};
};

class AnimKeyFrame {
protected:
	Float64		m_Time;

	List<glm::mat4>	m_FrameData;

public:
	DECLARE_GETSET(Time)
	UInt32		FrameDataCount() const {
		return m_FrameData.Length();
	}
	
	void	SetLocalTransform(int index, const glm::mat4& transform) {
		m_FrameData.EnsureSize(index + 1);
		m_FrameData[index] = transform;
	}

	const glm::mat4& GetLocalTransform(int index) const;

	glm::mat4 GetGlobalTransform(int index, const Neo::Skeleton& skeletonRef) const;

	Bool	operator==(const AnimKeyFrame& rhs) const {
		for( UInt32 ix = 0; ix < FrameDataCount(); ++ix ) {
			if( m_FrameData[ix] != rhs.m_FrameData[ix] ) {
				return false;
			}
		}

		return true;
	}

	Bool	operator!=(const AnimKeyFrame& rhs) const {
		for (UInt32 ix = 0; ix < FrameDataCount(); ++ix) {
			if (m_FrameData[ix] == rhs.m_FrameData[ix]) {
				return false;
			}
		}

		return true;
	}
};

class AnimationClip {
protected:
	StaticString		m_Name;
	Float64				m_Duration;
	Float32				m_FrameRate;

	List<AnimKeyFrame*>	m_Frames;

public:
	DECLARE_GETSET(Name)
	DECLARE_GETSET(Duration)
	DECLARE_GETSET(FrameRate)

	UInt32		NumFrames() const {
		return m_Frames.Length();
	}

	void			AddFrame(AnimKeyFrame* pKeyFrame) {
		m_Frames.Add(pKeyFrame);
	}

	AnimKeyFrame*	GetFrame(UInt32 index) {
		return m_Frames[index];
	}

	const AnimKeyFrame*	GetFrame(UInt32 index) const {
		return m_Frames[index];
	}

	Bool	IsValid() const {
		Bool result = false;
		for(UInt32 iy = 0; iy < NumFrames(); ++iy) {
			for (UInt32 ix = 0; ix < NumFrames(); ++ix) {
				if(iy == ix) {
					continue;
				}

				result |= (*m_Frames[ix]) != (*m_Frames[iy]);
			}
		}
		return result;
	}
};