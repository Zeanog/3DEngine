#include "pch.h"

#include "Clock.h"
#include "System\MathUtils.h"
#include <glfw\glfw3.h>

Clock::Clock(Float32 time) {
	m_Time = time;
	m_DeltaTime = 0.0f;
}

void Clock::Tick() {
	m_DeltaTime = Singleton<SystemClock>::GetInstance()->DeltaTime();
	m_Time += m_DeltaTime;
}

void SystemClock::Tick() {
	Float32 newTime = (Float32)glfwGetTime();
	m_DeltaTime = MathUtils::Clamp(newTime - m_Time, 0.0f, 1.0f / 30.0f);
	m_Time = newTime;
}