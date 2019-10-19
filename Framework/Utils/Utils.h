#pragma once
#include<vector>
#include<string>
class Utils {
	Utils(){}
	static Utils* s_instance;
public:
	static Utils*GetInstance();
	void Cleanup();
	int Log(const char* tag, const char* fmt, ...);
	void loadFile(char*& buffer, const std::string& filename);
};
