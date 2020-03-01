#ifndef _MEMORY_TRACER_
#define _MEMORY_TRACER_

#include <list>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

/// MemoryTracerDetail
#define INFO_MEM_TRACER_PID "Pid"
#define INFO_MEM_TRACER_PPID "ParentPid"
#define INFO_MEM_TRACER_PROCESS_NAME "ProcessName"
#define INFO_MEM_TRACER_PROCESS_EXE_PATH "ProcessExecutePath"
#define INFO_MEM_TRACER_MEMORY_USAGE "MemoryUsage"
#define INFO_MEM_TRACER_TIMETAG "Timetag"
#define INFO_MEM_TRACER_ERROR_CODE "ErrorCode"

/// MemoryTracerBase
#define INFO_MEM_TRACER_BASE "MemoryTracer"
#define INFO_MEM_TRACER_DETAIL "Detail"
#define INFO_MEM_TRACER_TOTAL_COUNT "TotalCount"
#define INFO_MEM_TRACER_SYS_KERNAL_COUNT "SystemKernalCount"


using namespace nlohmann;

namespace Toolset
{
    struct MemoryTracerDetail
    {
        unsigned long Pid = 0;
        unsigned long ParentPid = 0;
        std::string ProcessName = "<Unknown>";
        std::string ProcessExecutePath = "<Unknown>";
        size_t MemoryUsage = 0;
        std::string Timetag = "<Unknown>";
        unsigned long ErrorCode = 0;
    };

    struct MemoryTracerBase
    {
        int SystemKernalCount = 0;
        int TotalRecordCount = 0;
        MemoryTracerDetail memoryTracerDetail;
    };

    typedef std::vector<MemoryTracerBase> MemoryTracerBaseVec;

    class MemoryTracer
    {
    public:
        MemoryTracer();
        ~MemoryTracer();

        // Get all pid
        bool getAllProcessInfo();

        // Get result json
        static json GetOutJsonArray() noexcept { return mOutJsonArray; };

    protected:

    private:
        // Enable debug privilege
        bool _enableTokenDebugPrivilege();

        static json mOutJsonArray;
        static json mOutJson;

        MemoryTracerBaseVec mOutCache;
        MemoryTracerBase mMemoryTracerBase;

        // Error code
        unsigned long mErrorCode = 0;

        // Combine json result
        bool _toJson(MemoryTracerBaseVec& mOutCache);

        // Fetch process name, execute path, memory usage, current timetag
        bool _fetchProcessDetail(MemoryTracerDetail& mtd);

        // Reset structure
        void _resetStruct(MemoryTracerDetail& mtd);
    };
}
#endif // _MEMORY_TRACER_

/*
[
    {
        "MemoryTracer": {
            "Detail": {
                "MemoryUsage": 11208,
                "ParentPid": 940,
                "Pid": 3604,
                "ProcessExecutePath": "C:\\WINDOWS\\system32\\svchost.exe",
                "ProcessName": "svchost.exe",
                "Timetag": "Sun Mar  1 20:21:48 2020\n"
            },
            "SystemKernalCount": 7,
            "TotalCount": 46
        }
    }
]
*/