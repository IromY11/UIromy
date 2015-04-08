#include "precompiled_engine.h"

#ifndef _ITF_FIXEDCAMERACOMPONENT_H_
#include "engine/actors/components/Camera/FixedCameraComponent.h"
#endif //_ITF_FIXEDCAMERACOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(FixedCameraComponent)
    BEGIN_SERIALIZATION_CHILD(FixedCameraComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("offset", m_offset)
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(FixedCameraComponent_Template)
    BEGIN_SERIALIZATION_CHILD(FixedCameraComponent_Template)
    END_SERIALIZATION()

    void FixedCameraComponent::getCameraData( CameraData& _data ) const
    {
        _data.m_pos = m_actor->getPos() + m_offset;
        _data.m_depth = m_offset.z();
    }

    void FixedCameraComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_actor->registerEvent(EventTrigger_CRC, this);
    }

    void FixedCameraComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventTrigger* trigger = DYNAMIC_CAST(_event, EventTrigger) )
        {
            setAsMainCamera(trigger->getActivated());
        }
    }

    void FixedCameraComponent::onCameraEvent( Event* _event )
    {
        m_actor->onEvent(_event);
    }

}// namespace ITF