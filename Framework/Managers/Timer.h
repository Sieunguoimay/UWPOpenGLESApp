#pragma once
#include<chrono>
#include"../OnlineElement.h"
class Timer: public OnlineElement{
	float m_deltaTime;
	std::chrono::steady_clock::time_point m_oldTime;
public:
	//interface to the framework
	void Init()override;
	void Update(float deltaTime)override;
	void Cleanup()override;

	//interface to user
	float GetDeltaTime() { return m_deltaTime; }
};