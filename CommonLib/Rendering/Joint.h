#pragma once

#include "System/StaticString.h"
#include "System/List.h"
#include <glm/mat4x4.hpp>

class Joint {
public:
	static void			Render(const glm::mat4& transform, float radius, UInt32 meshPrecision);

protected:
	StaticString	m_Name;
	Int32			m_Parent;

public:
	Joint();
	Joint(Int32 parentIndex);

	DECLARE_GETSET(Name)
	DECLARE_GETSET(Parent)
};

class IMeshLoader;

namespace Neo {
	class Skeleton {
	protected:
		List<Joint>			m_Joints;

	public:
		Skeleton() {
		}

		~Skeleton() {
		}

		List<Joint>&		Joints() {
			return m_Joints;
		}

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