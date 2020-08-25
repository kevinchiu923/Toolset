#include "MemoryTracer.h"

#include <windows.h>
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <psapi.h>
#include <stdio.h>

#include "ErrorHandler.h"

using namespace nlohmann;
using namespace std;
using namespace Toolset;
namespace fs = std::experimental::filesystem;

int main(int argc, char* argv[])
{
    ErrorHandler::getInstance()->log(Info, LOG_HEADER + "[Main] MemoryTracer started.");

    MemoryTracer mMemoryTracer;
    if (mMemoryTracer.getAllProcessInfo()) {
        string outFilePath = fs::current_path().string() + "/MemoryTracer_result.json";
        ofstream writeInFile(outFilePath);
        writeInFile << mMemoryTracer.GetResultJson() << endl;
        ErrorHandler::getInstance()->log(Info, LOG_HEADER + "[Main] MemoryTracer finished.");
    }
    // else, print exception

    return 0;
}

/*
{
    "MemoryTracer": [
        {
            "MemoryUsage": 0,
            "ParentPid": 0,
            "Pid": 4,
            "ProcessExecutePath": "<Unknown>",
            "ProcessName": "<Unknown>",
            "Timetag": "<Unknown>"
        },
        {
            "ErrorCode": 1,
            "MemoryUsage": 19780,
            "ParentPid": 23712,
            "Pid": 19164,
            "ProcessExecutePath": "C:\\Spotify\\Spotify.exe",
            "ProcessName": "Spotify.exe",
            "Timetag": "Wed Mar  4 00:31:50 2020"
        }
    ],
    "SystemKernalCount": 11,
    "TotalCount": 264
}
*/