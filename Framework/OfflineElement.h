#pragma once
class OfflineElement {
public:
	virtual void Init() = 0;
	virtual void Cleanup() = 0;
};