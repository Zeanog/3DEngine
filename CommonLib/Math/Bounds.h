#pragma once

#include "Math/MathUtils.h"
#include "Math/Vector.h"
#include "System/List.h"
#include "glm/glm.hpp"

class ShaderProgram_GLSL;

namespace Neo {
	class Bounds {
	public:

	protected:
		glm::vec3	m_Min;
		glm::vec3	m_Max;

	public:
		Bounds() {
			m_Min = glm::vec3(0.0f, 0.0f, 0.0f);
			m_Max = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		Bounds(const glm::vec3& min, const glm::vec3& max) {
			m_Min = min;
			m_Max = max;
		}

		glm::vec3 GetCenter() const {
			return m_Min + (m_Max - m_Min) * 0.5f;
		}

		Bounds	Shift(const glm::vec3& offset) const {
			Bounds b = *this;
			b.m_Min += offset;
			b.m_Max += offset;
			return b;
		}

		Bounds&	operator+=(const Bounds& rhs) {
			for (int ix = 0; ix < 3; ++ix) {
				m_Min[ix] = MathUtils::Min(m_Min[ix], rhs.m_Min[ix]);
				m_Max[ix] = MathUtils::Max(m_Max[ix], rhs.m_Max[ix]);
			}

			return *this;
		}

		Bounds& operator+=(const glm::vec3& pt) {
			for (int ix = 0; ix < 3; ++ix) {
				m_Min[ix] = MathUtils::Min(m_Min[ix], pt[ix]);
				m_Max[ix] = MathUtils::Max(m_Max[ix], pt[ix]);
			}

			return *this;
		}

		Bounds& operator+=(const List<glm::vec3>& pts) {
			for (UInt32 ix = 0; ix < pts.Length(); ++ix) {
				operator+=(pts[ix]);
			}

			return *this;
		}

		Bounds& operator+=(const Vector<3>& pt) {
			for (int ix = 0; ix < 3; ++ix) {
				m_Min[ix] = MathUtils::Min(m_Min[ix], pt[ix]);
				m_Max[ix] = MathUtils::Max(m_Max[ix], pt[ix]);
			}

			return *this;
		}

		Bounds& operator+=(const List<Vector<3>>& pts) {
			for (UInt32 ix = 0; ix < pts.Length(); ++ix) {
				operator+=(pts[ix]);
			}

			return *this;
		}

		Bounds&	operator*=(Float32 scale) {
			glm::vec3 center = GetCenter();

			glm::vec3 minDir = m_Min - center;
			glm::vec3 maxDir = m_Max - center;

			m_Min = (minDir * scale) + center;
			m_Max = (maxDir * scale) + center;

			return *this;
		}

		void	Render(const ShaderProgram_GLSL& program, const glm::vec3& offset ) const;
	};
}