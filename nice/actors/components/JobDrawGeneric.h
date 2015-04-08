#ifndef ITF_JOBDRAWGENERIC_H_
#define ITF_JOBDRAWGENERIC_H_

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

namespace ITF
{
    // This job class can be used as a base for jobs pushed during the draw and synced at the end of batch primitive
    class JobDrawGeneric : public Job
    {
    public:

        virtual u32 getTag() const { return ms_jobGroupTag; }

        static u32 getJobGroupTag() { return ms_jobGroupTag; }

    protected:
        friend class ApplicationFramework;
        static u32 ms_jobGroupTag;
    };

}


#endif //ITF_JOBDRAWGENERIC_H_