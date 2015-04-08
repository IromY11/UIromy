#include "precompiled_engine.h"

#ifndef _ITF_PROCEDURALBONECOMPONENT_H_
#include "engine/actors/components/ProceduralBoneComponent.h"
#endif //_ITF_PROCEDURALBONECOMPONENT_H_

#ifndef _ITF_PROCEDURALBONE_H_
#include "engine/actors/ProceduralBone.h"
#endif //_ITF_PROCEDURALBONE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(ProceduralBoneComponent_Template)

    BEGIN_SERIALIZATION_CHILD(ProceduralBoneComponent_Template)
    END_SERIALIZATION()


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(ProceduralBoneComponent)

    BEGIN_SERIALIZATION_CHILD(ProceduralBoneComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(ProceduralBoneComponent)
    END_VALIDATE_COMPONENT()

    ProceduralBoneComponent::ProceduralBoneComponent()
        : Super()
    {
    }

    ProceduralBoneComponent::~ProceduralBoneComponent()
    {
    }

    const ProceduralBone* ProceduralBoneComponent::getBone( const StringID& _id ) const
    {
        const u32 boneCount = m_bones.size();
        for (u32 i = 0; i < boneCount; i++)
        {
            const ProceduralBone* bone = m_bones[i];
            if (bone->getId() == _id)
            {
                return bone;
            }
        }
        return NULL;
    }

#ifdef ITF_SUPPORT_EDITOR
    void ProceduralBoneComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags );

        if (_flags != ActorComponent::DrawEditFlag_All && _flags != ActorComponent::DrawEditFlag_Collisions)
            return;

        const u32 boneCount = m_bones.size();
        for (u32 i = 0; i < boneCount; i++)
        {
            const ProceduralBone* bone = m_bones[i];
            DebugDraw::circle(bone->getTransform().getPos(),0.1f,Color::yellow(),0.0f,bone->getId().getDebugString());
        }
    }
#endif

}
