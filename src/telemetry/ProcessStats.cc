#include "zeek/telemetry/ProcessStats.h"

#ifdef __APPLE__

#include <libproc.h>
#include <mach/mach.h>
#include <mach/task.h>
#include <math.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

process_stats get_process_stats() {
    process_stats result;

    // Fetch memory usage.
    {
        mach_task_basic_info info;
        mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
        if ( task_info(mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &count) ==
             KERN_SUCCESS ) {
            result.rss = static_cast<int64_t>(info.resident_size);
            result.vms = static_cast<int64_t>(info.virtual_size);
        }
    }
    // Fetch CPU time.
    {
        task_thread_times_info info;
        mach_msg_type_number_t count = TASK_THREAD_TIMES_INFO_COUNT;
        if ( task_info(mach_task_self(), TASK_THREAD_TIMES_INFO, reinterpret_cast<task_info_t>(&info), &count) ==
             KERN_SUCCESS ) {
            // Round to milliseconds.
            result.cpu += info.user_time.seconds;
            result.cpu += ceil(info.user_time.microseconds / 1000.0) / 1000.0;
            result.cpu += info.system_time.seconds;
            result.cpu += ceil(info.system_time.microseconds / 1000.0) / 1000.0;
        }
    }
    // Fetch open file handles.
    {
        // proc_pidinfo is undocumented, but this is what lsof also uses.
        auto suggested_buf_size = proc_pidinfo(getpid(), PROC_PIDLISTFDS, 0, nullptr, 0);
        if ( suggested_buf_size > 0 ) {
            auto buf_size = suggested_buf_size;
            auto buf = malloc(buf_size); // TODO: could be thread-local
            auto res = proc_pidinfo(getpid(), PROC_PIDLISTFDS, 0, buf, buf_size);
            free(buf);
            if ( res > 0 )
                result.fds = static_cast<int64_t>(res / sizeof(proc_fdinfo));
        }
    }

    return result;
}

#else

process_stats get_process_stats() {
    process_stats result;
    return result;
}

#endif
