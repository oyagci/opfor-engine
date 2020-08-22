//
// Created by Marc on 01/06/2018.
//

#pragma once

#include <iostream>

namespace lazy
{
	namespace utils
	{
		std::string		LoadFile(const std::string &path);
		std::string		LoadShader(const std::string &path);
		std::string		GetDirectory(std::string const &file);
	}
}