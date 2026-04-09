#include "System/Typedefs.h"
#include "System/List.h"
#include "System/StaticString.h"
#include "Rendering/Joint.h"

class ASkeletonLoader {
	ABSTRACT_CLASS_TYPEDEFS(ASkeletonLoader) {}

protected:
	List<Joint>							m_Joints;

public:
	virtual	void	Clear() = 0;

	UInt32 NumJoints() const {
		return m_Joints.Length();
	}

	const Joint& GetJoint(int index) const {
		return m_Joints[index];
	}

	Joint& GetJoint(int index) {
		return m_Joints[index];
	}
};