#pragma once

#include "MemoryTracer.h"
#include "Util.h"

#include <windows.h>
#include <codecvt>
#include <iostream>
#include <psapi.h>
#include <tlhelp32.h>
//#include <winternl.h> // NtQueryInformationProcess

#pragma comment(lib, "psapi.lib")

using namespace std;
using namespace Toolset;

json MemoryTracer::mOutJson = json::object();
json MemoryTracer::mOutJsonArray = json::array();

class HandleGuard
{
public:
    explicit HandleGuard(HANDLE *handle) : mHandle(handle) {}
    ~HandleGuard() {
        try {
            if (*mHandle != INVALID_HANDLE_VALUE) {
                CloseHandle(*mHandle);
            }
        }
        catch (exception &e) {
            cout << "[Error] " + string(e.what());
        }
    }

private:
    HANDLE *mHandle;
};
// PUBLIC
MemoryTracer::MemoryTracer() {}
MemoryTracer::~MemoryTracer() {}

bool MemoryTracer::getAllProcessInfo()
{
    bool ret = false;

    // Enable debug privilege
    if (!_enableTokenDebugPrivilege()) {
        cout << "Cannot enable debug privikege" << endl;
        return ret;
    }

    // Take a snapshot of all processes in the system.
    PROCESSENTRY32 pe32;
    HANDLE processSnapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    HandleGuard objectHandleGuard(&processSnapHandle);

    if (processSnapHandle == INVALID_HANDLE_VALUE) {
        return ret;
    }

    ZeroMemory(&pe32, sizeof(pe32));
    pe32.dwSize = sizeof(pe32);
    if (!Process32First(processSnapHandle, &pe32)) {
        return ret;
    }

    do {
        MemoryTracerDetail mtd;
        mtd.Pid = pe32.th32ProcessID;
        if (mtd.Pid == 0) {
            continue;
        }

        mtd.ParentPid = pe32.th32ParentProcessID;
        _fetchProcessDetail(mtd);
        mMemoryTracerBase.TotalRecordCount += 1;

        // Push into output cache
        mMemoryTracerBase.memoryTracerDetail = mtd;
        mOutCache.push_back(mMemoryTracerBase);
    } while (Process32Next(processSnapHandle, &pe32));

    if (_toJson(mOutCache)) {
        ret = true;
    }

    return ret;
}


// PRIVATE
bool MemoryTracer::_toJson(MemoryTracerBaseVec& mOutCache)
{
    bool ret = false;
    try {
        for (auto& iter : mOutCache) {
            json tmpJson = json::object();
            /// Base
            tmpJson[INFO_MEM_TRACER_BASE][INFO_MEM_TRACER_TOTAL_COUNT] = iter.TotalRecordCount;
            tmpJson[INFO_MEM_TRACER_BASE][INFO_MEM_TRACER_SYS_KERNAL_COUNT] = iter.SystemKernalCount;

            /// MemoryTracerDetail
            tmpJson[INFO_MEM_TRACER_BASE][INFO_MEM_TRACER_DETAIL][INFO_MEM_TRACER_PID] = iter.memoryTracerDetail.Pid;
            tmpJson[INFO_MEM_TRACER_BASE][INFO_MEM_TRACER_DETAIL][INFO_MEM_TRACER_PPID] = iter.memoryTracerDetail.ParentPid;
            tmpJson[INFO_MEM_TRACER_BASE][INFO_MEM_TRACER_DETAIL][INFO_MEM_TRACER_PROCESS_NAME] = iter.memoryTracerDetail.ProcessName;
            tmpJson[INFO_MEM_TRACER_BASE][INFO_MEM_TRACER_DETAIL][INFO_MEM_TRACER_PROCESS_EXE_PATH] = iter.memoryTracerDetail.ProcessExecutePath;
            tmpJson[INFO_MEM_TRACER_BASE][INFO_MEM_TRACER_DETAIL][INFO_MEM_TRACER_MEMORY_USAGE] = iter.memoryTracerDetail.MemoryUsage;
            tmpJson[INFO_MEM_TRACER_BASE][INFO_MEM_TRACER_DETAIL][INFO_MEM_TRACER_TIMETAG] = iter.memoryTracerDetail.Timetag;
            if (iter.memoryTracerDetail.ErrorCode != 0) {
                tmpJson[INFO_MEM_TRACER_BASE][INFO_MEM_TRACER_DETAIL][INFO_MEM_TRACER_ERROR_CODE] = iter.memoryTracerDetail.ErrorCode;
            }

            mOutJsonArray.push_back(tmpJson);
        }
        ret = true;
    }
    catch (exception e) {
        cout << "[Error] " + string(e.what());
    }

    return ret;
}

bool MemoryTracer::_enableTokenDebugPrivilege()
{
    HANDLE tokenHandle = 0;
    TOKEN_PRIVILEGES tokenPriv = { 0 };

    // Get a token for this process.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tokenHandle)) {
        return false;
    }

    // Get the LUID for the privilege.
    if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tokenPriv.Privileges[0].Luid)) {
        tokenPriv.PrivilegeCount = 1;  // one privilege to set
        tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        // Set the privilege for this process.
        AdjustTokenPrivileges(tokenHandle, FALSE, &tokenPriv, 0, (PTOKEN_PRIVILEGES)NULL, 0);
        if (GetLastError() != ERROR_SUCCESS) {
            return false;
        }
        return true;
    }
    return false;
}

// Get process name / execute path / memory usage / current time / error code
bool MemoryTracer::_fetchProcessDetail(MemoryTracerDetail& mtd)
{
    bool ret = true;

    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, mtd.Pid);
    HandleGuard objectHandleGuard(&processHandle);
    TCHAR buffer[MAX_PATH] = TEXT("<unknown>");

    // Reset struct
    _resetStruct(mtd);

    if (processHandle != NULL) {
        HMODULE moduleHandle;
        unsigned long needByte;

        if (EnumProcessModules(processHandle, &moduleHandle, sizeof(moduleHandle), &needByte)) {
            wstring_convert<codecvt_utf8<TCHAR>, TCHAR> msConverter;

            // Get process name
            ret &= (GetModuleBaseName(processHandle, moduleHandle, buffer, sizeof(buffer) / sizeof(TCHAR)) != 0);
            if (!msConverter.to_bytes(buffer).empty()) {
                mtd.ProcessName = msConverter.to_bytes(buffer);

                // Get execute path
                ret &= (GetModuleFileNameEx(processHandle, moduleHandle, buffer, sizeof(buffer) / sizeof(TCHAR)) != 0);
                if (!msConverter.to_bytes(buffer).empty()) {
                    mtd.ProcessExecutePath = msConverter.to_bytes(buffer);
                }

                //----------------------------------------------------------------------------------
                // Get memory usage
#if (_WIN32_WINNT >= _WIN32_WINNT_WINXP)
                PROCESS_MEMORY_COUNTERS_EX processMemCounter;
#else
                PROCESS_MEMORY_COUNTERS processMemCounter;
#endif          
                ret &= (GetProcessMemoryInfo(processHandle, (PPROCESS_MEMORY_COUNTERS)&processMemCounter, sizeof(processMemCounter)));
                mtd.MemoryUsage = (processMemCounter.PrivateUsage) / 1024; // PrivateUsage has the most similar estimate result with process hacker

                //--- Memory usage result test
                //mtc.MemoryUsageA = (processMemCounter.cb) / 1024;
                //mtc.MemoryUsageB = (processMemCounter.PageFaultCount) / 1024;
                //mtc.MemoryUsageC = (processMemCounter.PagefileUsage) / 1024;
                //mtc.MemoryUsageD = (processMemCounter.PeakPagefileUsage) / 1024;
                //mtc.MemoryUsageE = (processMemCounter.PeakWorkingSetSize) / 1024;
                //mtc.MemoryUsageF = (processMemCounter.QuotaNonPagedPoolUsage) / 1024;
                //mtc.MemoryUsageG = (processMemCounter.QuotaPagedPoolUsage) / 1024;
                //mtc.MemoryUsageH = (processMemCounter.QuotaPeakNonPagedPoolUsage) / 1024;
                //mtc.MemoryUsageI = (processMemCounter.QuotaPeakPagedPoolUsage) / 1024;
                //mtc.MemoryUsageJ = (processMemCounter.WorkingSetSize) / 1024;
                //mtc.MemoryUsageK = (processMemCounter.PrivateUsage) / 1024;
                //----------------------------------------------------------------------------------

                // Get current time
                string currentTimetagStr = Util::currentTimetag();
                if (!currentTimetagStr.empty()) {
                    ret &= true;
                    mtd.Timetag = currentTimetagStr;
                }

                if (!ret) {
                    return false;
                }
            }
            else {
                mErrorCode = GetLastError();
                ret = false;
                return ret;
            }
        }
        else {
            mErrorCode = GetLastError();
            ret = false;
            return ret;
        }

        return ret;
    }
    else {
        // system kernal process cannot be parsed
        mMemoryTracerBase.SystemKernalCount += 1;
    }
}

void MemoryTracer::_resetStruct(MemoryTracerDetail& mtd)
{
    mtd.ProcessName = "<Unknown>";
    mtd.ProcessExecutePath = "<Unknown>";
    mtd.MemoryUsage = 0;
    mtd.Timetag = "<Unknown>";
    mtd.ErrorCode = 0;
}