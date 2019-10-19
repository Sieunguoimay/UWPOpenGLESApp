#include "Timer.h"
#include"../Utils/Utils.h"
#include<iostream>
void Timer::Init()
{
	Utils::GetInstance()->Log("TIMER", "Created timer successfully");
	m_oldTime = std::chrono::steady_clock::now();
	char*buffer = NULL;
	Utils::GetInstance()->loadFile(buffer, "a.txt");
	Utils::GetInstance()->Log("TIMER", "%s", buffer);
	free(buffer);
}

void Timer::Update(float deltaTime)
{
	std::chrono::steady_clock::time_point newTime = std::chrono::steady_clock::now();

	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - m_oldTime);

	m_deltaTime = time_span.count();

	m_oldTime = newTime;
}

void Timer::Cleanup()
{
	Utils::GetInstance()->Log("TIMER", "Goodbye Timer");
}
