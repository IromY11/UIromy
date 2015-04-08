#ifndef ITF_JOBUPDATEGENERIC_H_
#define ITF_JOBUPDATEGENERIC_H_

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

namespace ITF
{

// This job class can be used as a base for jobs pushed during the update and synced at the batch primitive
class JobUpdateGeneric : public Job
{
public:

    virtual u32 getTag() const { return ms_jobGroupTag; }

protected:
    friend class ApplicationFramework;
    static u32 ms_jobGroupTag;
};

}


#endif //ITF_JOBUPDATEGENERIC_H_