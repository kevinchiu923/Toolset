#ifndef _ERROR_HANDLER_
#define _ERROR_HANDLER_

#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <windows.h>
#include <codecvt>
#include "Singleton.h"

#define LOG_HEADER std::string(__FILE__) + ",line " + std::to_string(__LINE__) + ": "

namespace Toolset
{
    enum LogLevel
    {
        Critical, Error, Warn, Info, Debug
    };

    class ErrorHandler final : public Singleton<ErrorHandler>
    {
    public:
        ErrorHandler(token);
        ~ErrorHandler();

        void combine(LogLevel level, std::wstring combineMessage);

        void log(LogLevel level, std::string message);
    };
}
#endif // _ERROR_HANDLER_