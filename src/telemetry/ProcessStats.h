#pragma once

#include <cstdint>

struct process_stats {
    int64_t rss = 0;
    int64_t vms = 0;
    double cpu = 0.0;
    int64_t fds = 0;
};

#if defined(__APPLE__) || defined(HAVE_LINUX)

#define HAVE_PROCESS_STAT_METRICS 1

process_stats get_process_stats();

#endif
