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
//#define INFO_MEM_TRACER_DETAIL "Detail"
#define INFO_MEM_TRACER_TOTAL_COUNT "TotalCount"
#define INFO_MEM_TRACER_SYS_KERNAL_COUNT "SystemKernalCount"

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

    typedef std::unordered_map<std::string, MemoryTracerDetail> MemoryTracerDetailMap; // <Pid, MemoryTracerDetail>

    class MemoryTracer
    {
    public:
        MemoryTracer();
        ~MemoryTracer();

        // Get all pid
        bool getAllProcessInfo();

        // Get result json
        nlohmann::json GetOutJson() noexcept { return mResultJson; };

    protected:

    private:
        // Enable debug privilege
        bool _enableTokenDebugPrivilege();

        // Counter
        int mSystemKernalCount = 0;
        int mTotalRecordCount = 0;

        // Return cache
        MemoryTracerDetailMap mOutCache;

        // Error code
        unsigned long mErrorCode = 0;

        // Combine json result
        nlohmann::json mResultJson = nlohmann::json::object();
        bool _toJson(nlohmann::json& mResultJson, const MemoryTracerDetailMap& mOutCache);

        // Fetch process name, execute path, memory usage, current timetag
        bool _fetchProcessDetail(MemoryTracerDetail& mtd);

        // Reset structure
        void _resetStruct(MemoryTracerDetail& mtd);
    };
}
#endif // _MEMORY_TRACER_