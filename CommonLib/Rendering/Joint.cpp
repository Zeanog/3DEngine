#include "Joint.h"
#include "Rendering/ModelLoaders/MeshLoader_FBX.h"

#include <windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

Joint::Joint() {
	m_Parent = -1;
}

Joint::Joint(Int32 parentIndex) {
	Parent(parentIndex);
}

void Joint::Render( const glm::mat4& transform, float radius, UInt32 meshPrecision) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(glm::value_ptr(transform));

	GLUquadricObj *sphere = gluNewQuadric();
	gluQuadricTexture(sphere, true);
	gluSphere(sphere, radius, meshPrecision, meshPrecision);
	gluDeleteQuadric(sphere);

	glPopMatrix();
}

Joint* Neo::Skeleton::FindJoint(const StaticString& name) {
	FOREACH(joint, m_Joints) {
		if (joint->Name() == name) {
			return &(*joint);
		}
	}

	return NULL;
}

const Joint* Neo::Skeleton::FindJoint(const StaticString& name) const {
	FOREACH_CONST(joint, m_Joints) {
		if (joint->Name() == name) {
			return &(*joint);
		}
	}

	return NULL;
}

const glm::mat4& AnimKeyFrame::GetLocalTransform(int index) const {
	return m_FrameData[index];
}

glm::mat4 AnimKeyFrame::GetGlobalTransform(int index, const Neo::Skeleton& skeleton) const {
	glm::mat4 mat(glm::identity<glm::mat4>());

	Int32 currentIndex = index;
	const Joint* joint = NULL;
	while (currentIndex >= 0) {
		mat *= GetLocalTransform(currentIndex);
		joint = skeleton.FindJoint(currentIndex);
		currentIndex = joint->Parent();
	};

	return mat;
}