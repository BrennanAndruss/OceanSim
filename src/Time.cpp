#include "Time.h"

Time* Time::getInstance()
{
	static Time instance;
	return &instance;
}

Time::Time() : time(glfwGetTime()), deltaTime(0.0f) {}

Time::~Time() {}

void Time::updateTime()
{
	double lastTime = time;
	time = glfwGetTime();
	deltaTime = time - lastTime;
}

float Time::getTime() const
{
	return time;
}

float Time::getDeltaTime() const
{
	return deltaTime;
}
