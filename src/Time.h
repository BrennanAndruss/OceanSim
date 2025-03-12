#pragma once

#ifndef TIME_H
#define TIME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Time
{
public:
	// This class implements the singleton design pattern
	static Time* getInstance();
	Time(const Time&) = delete;
	Time& operator= (const Time&) = delete;

	void updateTime();
	float getTime() const;
	float getDeltaTime() const;

private:
	// This class implements the singleton design pattern
	Time();
	~Time();

	float time;
	float deltaTime;
};

#endif // TIME_H
