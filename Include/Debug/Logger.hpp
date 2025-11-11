#pragma once

#include <iostream>
#include <string>
#include <format>

namespace Debug
{
    class Logger
    {
    public:
        static bool init()
        {
            isEnabled = true;
            return true;
        }

        static bool shutdown()
        {
            isEnabled = false;
            return true;
        }

        template<typename... Args>
        static bool printf(std::format_string<Args...> fmt, Args&&... args);

    private:
        static inline bool isEnabled = false;
    };

    template <typename... Args>
    bool Logger::printf(std::format_string<Args...> fmt, Args&&... args)
    {
        try
        {
            std::string formatted = std::format(fmt, std::forward<Args>(args)...);
            std::cout << "[Aurora] " << formatted << std::endl;
            return true;
        }
        catch (const std::format_error& e)
        {
            std::cerr << "[Aurora][format error] " << e.what() << std::endl;
            return false;
        }
    }
}
