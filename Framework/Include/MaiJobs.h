#pragma once

#include "./MaiDef.h"

typedef void (*JobExecutor)(void* data, int size, float timeStep);

typedef struct Job
{
    void* data;
    int   size;

    JobExecutor executor;
} Job;

#ifdef __cpluscplus
extern "C" {
#endif

MAILIB_API int  GetThreadCount(void);

MAILIB_API void ExecuteJobs(const Job jobs[], int jobCount, float timeStep);

#ifdef __cpluscplus
}
#endif
