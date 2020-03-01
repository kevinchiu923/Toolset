#include "MemoryTracer.h"

#include <windows.h>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <psapi.h>
#include <stdio.h>

using namespace nlohmann;
using namespace std;
using namespace Toolset;

int main(int argc, char* argv[])
{
    MemoryTracer mMemoryTracer;
    if (mMemoryTracer.getAllProcessInfo()) {
        string outFilePath = "MemoryTracer_result.json";
        ofstream writeInFile(outFilePath);
        writeInFile << mMemoryTracer.GetOutJsonArray() << endl;
        cout << "Memory tracer finished!\n";
    }
    // else, print exception

    return 0;
}