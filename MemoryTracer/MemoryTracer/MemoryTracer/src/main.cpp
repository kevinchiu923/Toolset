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

        //---------------------

        //string outPath = "MemoryTracer_result.csv";
        //ofstream writeInFile(outPath);

        //writeInFile << "[Total count]: " << resultList.size() << endl;
        //writeInFile << "[System kernal process]: " << mMemoryTracer.GetSystemKernalPidCount() << endl << endl;

        //int itemCount = 0;
        //for (auto& it : resultList) {
        //    itemCount += 1;
        //    writeInFile << "< " << itemCount << " >" << ",";
        //    writeInFile << "[Pid]: " << it.Pid << ",";
        //    writeInFile << "[PPid]: " << it.ParentPid << ",";
        //    writeInFile << "[Name]: " << it.ProcessName << ",";
        //    writeInFile << "[Execute path]: " << it.ProcessExecutePath << ",";
        //    writeInFile << "[Memory usage]: " << it.MemoryUsage << "K" << ",";
        //    writeInFile << "[Time]: " << it.Timetag << endl;
        //}
        //cout << "Memory tracer finished!\n";
    }
    // else, print exception

    return 0;
}


////--- Memory percentage
//#include<windows.h>
//#include<stdio.h>   
//#include<tchar.h>
//
//// Use to convert bytes to MB
//#define DIV 1048576
//
//// Use to convert bytes to MB
////#define DIV 1024
//
//// Specify the width of the field in which to print the numbers. 
//// The asterisk in the format specifier "%*I64d" takes an integer 
//// argument and uses it to pad and right justify the number.
//
//#define WIDTH 7
//
//void _tmain()
//{
//    MEMORYSTATUSEX statex;
//
//    statex.dwLength = sizeof(statex);
//
//    GlobalMemoryStatusEx(&statex);
//
//
//    _tprintf(TEXT("There is  %*ld percent of memory in use.\n"), WIDTH, statex.dwMemoryLoad);
//    _tprintf(TEXT("There are %*I64d total Mbytes of physical memory.\n"), WIDTH, statex.ullTotalPhys / DIV);
//    _tprintf(TEXT("There are %*I64d free Mbytes of physical memory.\n"), WIDTH, statex.ullAvailPhys / DIV);
//    _tprintf(TEXT("There are %*I64d total Mbytes of paging file.\n"), WIDTH, statex.ullTotalPageFile / DIV);
//    _tprintf(TEXT("There are %*I64d free Mbytes of paging file.\n"), WIDTH, statex.ullAvailPageFile / DIV);
//    _tprintf(TEXT("There are %*I64d total Mbytes of virtual memory.\n"), WIDTH, statex.ullTotalVirtual / DIV);
//    _tprintf(TEXT("There are %*I64d free Mbytes of virtual memory.\n"), WIDTH, statex.ullAvailVirtual / DIV);
//    _tprintf(TEXT("There are %*I64d free Mbytes of extended memory.\n"), WIDTH, statex.ullAvailExtendedVirtual / DIV);
//}