#include <MaiLib.h>
#include <MaiJobs.h>

#ifdef _MSC_VER
//#define VC_EXTRALEAN
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
#endif

int GetThreadCount(void)
{
#ifdef _MSC_VER
    //SYSTEM_INFO system_info;
    //GetSystemInfo(&system_info);
    //return (int)system_info.dwNumberOfProcessors;
    return 0;
#else
#error "unsupported"
#endif
}

void ExecuteJobs(const Job jobs[], int jobCount, float timeStep)
{
    for (int i = 0; i < jobCount; i++)
    {
        Job job = jobs[i];
        job.executor(job.data, job.size, timeStep);
    }
}
