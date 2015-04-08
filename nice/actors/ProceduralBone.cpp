#include "precompiled_engine.h"

#ifndef _ITF_PROCEDURALBONE_H_
#include "engine/actors/ProceduralBone.h"
#endif //_ITF_PROCEDURALBONE_H_

#ifndef _ITF_PROCEDURALBONECOMPONENT_H_
#include "engine/actors/components/ProceduralBoneComponent.h"
#endif //_ITF_PROCEDURALBONECOMPONENT_H_

namespace ITF
{
    const ProceduralBone* ProceduralBone::getBone( const Actor* _actor, const StringID _id )
    {
        if (ProceduralBoneComponent* component = _actor->GetComponent<ProceduralBoneComponent>())
        {
            return component->getBone(_id);
        }

        return NULL;
    }

    void ProceduralBone::registerBone( const Actor* _actor, const ProceduralBone* _bone )
    {
        if (ProceduralBoneComponent* component = _actor->GetComponent<ProceduralBoneComponent>())
        {
            return component->registerBone(_bone);
        }
    }

}
