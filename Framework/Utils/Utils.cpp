#include "Utils.h"
#include<stdarg.h>
#include<iostream>
#include<fstream>
#include"SDL2/SDL.h"
Utils* Utils::s_instance = 0;
Utils*Utils::GetInstance()
{
	if (s_instance == 0)
		s_instance = new Utils();
	return s_instance;
}

void Utils::Cleanup()
{
	delete s_instance;
}


int Utils::Log(const char* tag, const char* fmt, ...)
{
#ifndef NDEBUG
	va_list args;
	va_start(args, fmt);
	printf("%s: ", tag);
	int s = vprintf(fmt, args);
	printf("\n");
	return s;
#else
	return 0;
#endif
}



void Utils::loadFile(char*& buffer, const std::string& filename) {
	SDL_RWops* rwops = SDL_RWFromFile(filename.c_str(), "rb");
	if (rwops == NULL)
	{
		SDL_Log("Could not open file \"%s\".\n", filename.c_str());
		return;
	}
	SDL_RWseek(rwops, 0, SEEK_END);
	size_t data_max_size = (size_t)SDL_RWtell(rwops);
	SDL_RWseek(rwops, 0, SEEK_SET);

	buffer = (char*)malloc(data_max_size + 1);
	long len = 0;
	while ((len = SDL_RWread(rwops, buffer, sizeof(char), data_max_size)) > 0)
	{
		// Do stuff with your 'len' bytes of data
	}
	buffer[data_max_size] = 0;
	SDL_RWclose(rwops);
}