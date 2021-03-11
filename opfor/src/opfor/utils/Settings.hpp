#pragma once

#include <any>
#include <optional>
#include <string>
#include <unordered_map>

class Settings
{
  private:
    enum SettingType
    {
        INT,
        FLOAT,
        STRING
    };

  public:
    static Settings &Get()
    {
        static Settings s;
        return s;
    }

    Settings(Settings const &) = delete;
    void operator=(Settings const &) = delete;

    ~Settings();

  public:
    bool load(std::string const &filename);
    const std::any &get(std::string const &name) const;
    void set(std::string const &name, std::any const &value);

  private:
    Settings();

    void loadDefaults();
    SettingType getType(std::string const &name);

  private:
    std::unordered_map<std::string, std::any> _values;
};
