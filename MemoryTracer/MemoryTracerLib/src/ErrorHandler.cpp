#pragma once

#include "ErrorHandler.h"
#include "spdlog/sinks/daily_rotating_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"    // Rotate when file size exceeds, re-write the following log in the new file
#include "spdlog/sinks/stdout_color_sinks.h"
#include "Util.h"

#include <filesystem>
#include <iostream>
#include <locale>
#include <strsafe.h>

namespace fs = std::experimental::filesystem;
using namespace Toolset;

ErrorHandler::ErrorHandler(token)
{
    try {
        fs::create_directories("./log");

        spdlog::sinks::daily_rotating_config config;
        config.base_filename = "./log/daily.txt";
        config.max_size = 1048576;
        config.max_files = (std::numeric_limits<size_t>::max)();
        config.rotation_hour = std::chrono::hours(0);
        config.rotation_minute = std::chrono::minutes(0);
        auto FileLog = spdlog::daily_rotating_logger_mt("FileLog", config);
        auto ConsoleLog = spdlog::stdout_color_mt("ConsoleLog");

        // Set recorded level to two kinds of logger
        spdlog::get("FileLog")->set_level(spdlog::level::err);
        spdlog::get("ConsoleLog")->set_level(spdlog::level::trace);
    }
    catch (const spdlog::spdlog_ex &e) {
        std::cout << "[Error] Log init failed: " << e.what() << std::endl;
    }
}

ErrorHandler::~ErrorHandler()
{
}

void ErrorHandler::combine(LogLevel level, std::wstring combineMessage)
{
    std::wstring_convert<std::codecvt_utf8<TCHAR>, TCHAR> msConverter;
    log(level, msConverter.to_bytes(combineMessage));
}

void ErrorHandler::log(LogLevel level, std::string message)
{
    switch (level) {
    case Critical:
        spdlog::get("FileLog")->critical(message);
        spdlog::get("ConsoleLog")->critical(message);
        break;
    case Error:
        spdlog::get("FileLog")->error(message);
        spdlog::get("ConsoleLog")->error(message);
        break;
    case Warn:
        spdlog::get("FileLog")->warn(message);
        spdlog::get("ConsoleLog")->warn(message);
        break;
    case Info:
        spdlog::get("FileLog")->info(message);
        spdlog::get("ConsoleLog")->info(message);
        break;
    case Debug:
        spdlog::get("FileLog")->debug(message);
        spdlog::get("ConsoleLog")->debug(message);
        break;
    }

    spdlog::get("FileLog")->flush(); //§ï¦¨flush()
}