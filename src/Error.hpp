#pragma once

#include <string>

class Error
{
	public:
		static void report(std::string text);
		static void reportSDL(std::string text);
};