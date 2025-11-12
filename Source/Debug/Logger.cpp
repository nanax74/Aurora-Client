#include "Debug/Logger.hpp"
#include <iostream>

namespace Debug
{
    bool Logger::init()
    {
        isEnabled = true;
        return true;
    }

    bool Logger::shutdown()
    {
        isEnabled = false;
        return true;
    }

    bool Logger::write(const std::string& msg)
    {
        std::cout << msg << std::endl;
        return true;
    }
}
