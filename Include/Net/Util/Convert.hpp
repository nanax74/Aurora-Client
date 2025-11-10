#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>

namespace Net::Util
{
    class Convert
    {
    public:
        static std::string ipToString(const uint32_t ip);
        static uint32_t stringToIp(const std::string& ipStr);
        static std::string toHexString(const std::vector<std::byte>& data);

        template <typename T>
        static T swap_endian(T val)
        {
            T result = val;
            std::reverse(reinterpret_cast<char*>(&result), reinterpret_cast<char*>(&result) + sizeof(T));
            return result;
        }
    };
}