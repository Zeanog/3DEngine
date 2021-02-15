#pragma once

#include "System\Typedefs.h"
#include "System/Singleton.h"

class Clock {
	CLASS_TYPEDEFS(Clock)

protected:
	Float32	m_Time;
	Float32	m_DeltaTime;

public:
	Clock() {
		m_Time = 0.0f;
		m_DeltaTime = 0.0f;
	}

	Clock(Float32 time);

	DECLARE_GETSET( Time)
	DECLARE_GETSET( DeltaTime)

	virtual void	Tick();
};

class SystemClock : public Clock {
	INHERITEDCLASS_TYPEDEFS( SystemClock, Clock )
	SINGLETON_DECLARATIONS(SystemClock) {
	}

protected:
	Int32	DetermineSystemTime() const;

public:
	virtual void	Tick() override;
};