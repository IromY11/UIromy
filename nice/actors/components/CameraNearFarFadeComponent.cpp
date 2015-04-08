#include "precompiled_engine.h"

#ifndef ITF_CAMERANEARFARFADECOMPONENT_H_
#include "engine/actors/components/CameraNearFarFadeComponent.h"
#endif //ITF_CAMERANEARFARFADECOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(CameraNearFarFadeComponent_Template)

    BEGIN_SERIALIZATION_CHILD(CameraNearFarFadeComponent_Template)
        SERIALIZE_MEMBER("inputInterp", m_interpInput);
        SERIALIZE_MEMBER("inputNear", m_nearInput);
        SERIALIZE_MEMBER("inputFar", m_farInput);
        SERIALIZE_MEMBER("inputFadeRange", m_fadeInput);
    END_SERIALIZATION()

    CameraNearFarFadeComponent_Template::CameraNearFarFadeComponent_Template()
        : Super()
        , m_interpInput(ITF_GET_STRINGID_CRC(CameraNearFarFadeInterp,1017869650))
        , m_nearInput(ITF_GET_STRINGID_CRC(CameraNearSet,1976586513))
        , m_farInput(ITF_GET_STRINGID_CRC(CameraFarSet,2457948055))
        , m_fadeInput(ITF_GET_STRINGID_CRC(CameraFadeSet,3691559220))
    {
    }

    CameraNearFarFadeComponent_Template::~CameraNearFarFadeComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(CameraNearFarFadeComponent)

    BEGIN_SERIALIZATION_CHILD(CameraNearFarFadeComponent)
        SERIALIZE_MEMBER("Near", m_near);
        SERIALIZE_MEMBER("Fade", m_fadeRange);
        SERIALIZE_MEMBER("Far", m_far);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(CameraNearFarFadeComponent)
    END_VALIDATE_COMPONENT()

    CameraNearFarFadeComponent::CameraNearFarFadeComponent()
        :   m_interpValue(0.f)
        ,   m_near(0.f)
        ,   m_nearForced(0.f)
        ,   m_far(0.f)
        ,   m_farForced(0.f)
        ,   m_fadeRange(0.f)
        ,   m_fadeRangeForced(0.f)
    {
    }

    CameraNearFarFadeComponent::~CameraNearFarFadeComponent()
    {
    }

    void CameraNearFarFadeComponent::batchPrimitives( const ITF_VECTOR <View*>& _views )
    {
        Super::batchPrimitives(_views);

        for(ITF_VECTOR<View*>::const_iterator it = _views.begin(); it!=_views.end(); ++it)
        {
            View * pView = *it;
            if (pView)
                setViewParams(*pView);
        }
    }

    void CameraNearFarFadeComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_actor->registerEvent(EventSetFloatInput_CRC, this);
    }

    void CameraNearFarFadeComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if (EventSetFloatInput * onSetInput = DYNAMIC_CAST(_event, EventSetFloatInput))
        {
            if (getTemplate()->getInterpInput() == onSetInput->getInputName())
            {
                m_interpValue = onSetInput->getInputValue();
            }
            else if (getTemplate()->getNearInput() == onSetInput->getInputName())
            {
                m_nearForced = onSetInput->getInputValue();
            }
            else if (getTemplate()->getFarInput() == onSetInput->getInputName())
            {
                m_farForced = onSetInput->getInputValue();
            }
            else if (getTemplate()->getFadeInput() == onSetInput->getInputName())
            {
                m_fadeRangeForced = onSetInput->getInputValue();
            }
        }
    }

    void CameraNearFarFadeComponent::setViewParams(View & _view)
    {
        if (m_fadeRange>0.f)
        {
            _view.getCamera()->setNearFadeRange(f32_Lerp(_view.getCamera()->getNearFadeRange(), m_fadeRange, m_interpValue));
        }
        if (m_near>0.f)
        {
            _view.getCamera()->setNearPlaneDist(f32_Lerp(_view.getCamera()->getNearPlaneDist(), m_near, m_interpValue));
        }
        if (m_far>0.f)
        {
            _view.getCamera()->setFarPlaneDist(f32_Lerp(_view.getCamera()->getFarPlaneDist(), m_far, m_interpValue));
        }
        // forced values (reset each frame)
        if (m_fadeRangeForced>0.f)
        {
            _view.getCamera()->setNearFadeRange(m_fadeRangeForced);
            m_fadeRangeForced = 0.f;
        }
        if (m_farForced>0.f)
        {
            _view.getCamera()->setFarPlaneDist(m_farForced);
            m_farForced = 0.f;
        }
        if (m_nearForced>0.f)
        {
            _view.getCamera()->setNearPlaneDist(m_nearForced);
            m_nearForced = 0.f;
        }
    }
}
