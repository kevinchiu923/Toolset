#include "Util.h"

#include <ctime>
#include <iostream>

using namespace std;
using namespace Toolset;

string Util::currentTimetag()
{
    // Format example: Sat Feb 29 17:03:39 2020
    time_t result = time(nullptr);
    string buffer = asctime(localtime(&result));
    

    return buffer;
}

string Util::trimEndingChar(const string& input, const char& target)
{
    string retString = input;
    char ch = target;

    if (!retString.empty() && target != NULL) {
        if (retString.back() == ch) {
            retString.pop_back();
        }
    }

    return retString;
}