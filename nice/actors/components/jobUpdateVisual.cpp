#include "precompiled_engine.h"

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_JOBUPDATEVISUAL_H_
#include "engine/actors/components/jobUpdateVisual.h"
#endif //_ITF_JOBUPDATEVISUAL_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_*/

namespace ITF
{

u32 JobUpdateVisual::_jobTag_UpdateVisual    = U32_INVALID;

void JobUpdateVisual::execute(bbool _immediate)
{
    PRF_M_SCOPE(JobUpdateVisual);

    mainJobUpdateVisuals(&m_context);
}

//------------------------------------------------------------------------------------------

void mainJobUpdateVisuals(AnimLightComponent*  _pAnimLightComponent, bbool _updateBones, bbool _updatePatches)
{
    //only process if anim data is present
    AnimMeshScene*       pAnimMeshScene       =  _pAnimLightComponent->getAnimMeshScene();

    ITF_ASSERT(pAnimMeshScene);

#ifdef ASSERT_ENABLED
    if (!pAnimMeshScene)
        return;
#endif // ASSERT_ENABLED

    if (_updateBones)
    {
        pAnimMeshScene->m_AnimInfo.SetFrame();

        _pAnimLightComponent->updateEvents();
        _pAnimLightComponent->postUpdateData();
    }

    if (_updatePatches)
    {
        _pAnimLightComponent->updatePatches();
    }
}

void mainJobUpdateVisuals(JobUpdateVisualContext* _context)
{
    mainJobUpdateVisuals(_context->m_pAnimLightComponent, _context->m_updateBones, _context->m_updatePatches);
}
}
