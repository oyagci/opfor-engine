//
// Created by Marc on 01/06/2018.
//

#include <fstream>
#include <sstream>
#include <regex>
#include "utils/fileutils.hpp"

namespace lazy
{
	namespace utils
	{
		std::string		LoadFile(const std::string &path)
		{
			std::stringstream result;
			std::ifstream in(path);

			std::cout << "Loading file: " << path << "\n";

			if (in.is_open() && !in.bad())
			{
				result << in.rdbuf();
				in.close();
				return result.str();
			}

			std::cerr << "Failed to load file: " << path << "\n";
			exit(EXIT_FAILURE);
			return "";
		}

		std::string		LoadShader(const std::string &path)
		{
			std::string result = "";
			std::string dir = GetDirectory(path);
			std::ifstream ifs(path);
			std::regex incMatch("#.*include.*\"(.*)\"");
			std::smatch match;

			if (ifs.is_open())
			{
				std::string line;
				while (std::getline(ifs, line))
				{
					if (std::regex_match(line, match, incMatch))
						result += LoadShader(dir + "/" + match[1].str());
					else
						result += line + "\n";
				}
				ifs.close();
			}
			else
			{
				std::cerr << "Failed to load: " << path << "\n";
				exit(EXIT_FAILURE);
			}
			return result;
		}

		std::string GetDirectory(std::string const &file)
		{
			std::string result;

			std::size_t lastSlashIndex = file.rfind('/');
			if (std::string::npos != lastSlashIndex)
				result = file.substr(0, lastSlashIndex);
			return result;
		}
	}
}