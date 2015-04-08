#include "precompiled_engine.h"

#ifndef ITF_GFX_CONTEXT_H_
#include "GFXContext.h"
#endif

namespace ITF
{
    void DrawCallStateCache::reset()
    {
        m_stateChange = 0;
        m_stateApply = 0;
    }

    u32 DrawCallStateCache::neededStateApply()
    {
        u32 needChange = (m_stateChange & m_stateApply) | (m_stateChange & ~m_stateApply);
        return needChange;
    }

    u32 DrawCallStateCache::neededStateRestore()
    {
        u32 needChange = (~m_stateChange & m_stateApply);

        return needChange;
    }

    void DrawCallStateCache::computeNewState(u32 _applyFlags)
    {
        m_stateChange = 0;
        m_stateApply = _applyFlags;
    }

} // ITF
