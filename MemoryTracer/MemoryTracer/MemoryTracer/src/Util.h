#pragma once

#include <string>

#define UTC_TIME "%Y-%m-%dT%H:%M:%S"

namespace Toolset
{
    class Util
    {
    public:
        static std::string currentTimetag();
    };
}