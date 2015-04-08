#include "precompiled_engine.h"

#ifndef ITF_SAVEACTORSTATECOMPONENT_H_
#include "engine/actors/components/SaveActorStateComponent.h"
#endif //ITF_SAVEACTORSTATECOMPONENT_H_

namespace ITF
{
	IMPLEMENT_OBJECT_RTTI(SaveActorStateComponent_Template)

    BEGIN_SERIALIZATION_CHILD(SaveActorStateComponent_Template)
    END_SERIALIZATION()

    SaveActorStateComponent_Template::SaveActorStateComponent_Template()
        : Super()
    {
    }

    SaveActorStateComponent_Template::~SaveActorStateComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(SaveActorStateComponent)

    BEGIN_SERIALIZATION_CHILD(SaveActorStateComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(SaveActorStateComponent)
    END_VALIDATE_COMPONENT()

    SaveActorStateComponent::SaveActorStateComponent()
    {
    }

    SaveActorStateComponent::~SaveActorStateComponent()
    {
    }

    void SaveActorStateComponent::Update(f32 _deltaTime)
    {
		m_savedAbsoluteAABB = m_actor->getAABB();
    }
}