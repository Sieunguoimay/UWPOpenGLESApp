#pragma once
class OnlineElement {
public:
	virtual void Init() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Cleanup() = 0;
};