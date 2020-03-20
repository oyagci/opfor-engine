#include "Settings.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <string>
#include <map>

Settings::Settings()
{
	loadDefaults();
}

Settings::~Settings()
{
}

void Settings::loadDefaults()
{
	_values["renderDistance"] = 14;
	{
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
		auto epoch = now_ms.time_since_epoch();
		auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
		long duration = value.count();

		_values["seed"] = static_cast<unsigned int>(duration);
	}
}

bool Settings::load(std::string const &filename)
{
	std::ifstream file(filename, std::ios_base::in);

	if (!file.is_open()) {
		file.open(filename, std::ios_base::out | std::ios_base::in);
		return true;
	}

	std::string key;
	std::string value;
	while (std::getline(std::getline(file, key, '=') >> std::ws, value)) {
		switch (getType(key)) {
		case INT:
			_values[key] = std::atoi(value.c_str());
			break ;
		case FLOAT:
			_values[key] = std::atof(value.c_str());
			break ;
		case STRING:
			_values[key] = value;
			break ;
		default:
			_values[key] = value;
			break ;
		}
	}

	return true;
}

const std::any &Settings::get(std::string const &name) const
{
	return _values.at(name);
}

void Settings::set(std::string const &name, std::any const &value)
{
	_values[name] = value;
}

auto Settings::getType(std::string const &name) -> SettingType
{
	std::map<std::string, SettingType> vars = {
		{ "renderDistance", INT }
	};

	if (vars.find(name) != vars.end()) {
		return vars[name];
	}
	return STRING;
}
