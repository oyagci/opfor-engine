#pragma once
#include <string>
#include <generated/String.rfk.h>

namespace opfor OP4NAMESPACE()
{
    class OP4CLASS() String : public std::string
    {
        public:
            using std::string::string;

        String(const std::string &s)
                : std::string(s)
        {
        }

        String_GENERATED
    };
}

namespace std {

  template <>
  struct hash<opfor::String>
  {
    std::size_t operator()(const opfor::String& k) const noexcept
    {
        return hash<std::string>()(k);
    }
  };

}

File_GENERATED
