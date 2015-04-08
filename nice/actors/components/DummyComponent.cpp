#include "precompiled_engine.h"

#ifndef _ITF_DUMMYCOMPONENT_H_
#include "engine/actors/Components/DummyComponent.h"
#endif //_ITF_DUMMYCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(DummyComponent_Template)

    BEGIN_SERIALIZATION_CHILD(DummyComponent_Template)
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(DummyComponent)

    BEGIN_SERIALIZATION_CHILD(DummyComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(DummyComponent)
    END_VALIDATE_COMPONENT()

    DummyComponent::DummyComponent()
    {
    }

    DummyComponent::~DummyComponent()
    {
    }


};