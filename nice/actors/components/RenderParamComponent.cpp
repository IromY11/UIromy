#include "precompiled_engine.h"

#ifndef ITF_RENDERPARAMCOMPONENT_H_
#include "engine/actors/components/RenderParamComponent.h"
#endif //ITF_RENDERPARAMCOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(RenderParamComponent_Template)

    BEGIN_SERIALIZATION_CHILD(RenderParamComponent_Template)
		SERIALIZE_OBJECT("input",m_inputColor);
        SERIALIZE_OBJECT("inputFactor",m_inputFactor);
    END_SERIALIZATION()

    RenderParamComponent_Template::RenderParamComponent_Template()
        : Super()
    {
    }

    RenderParamComponent_Template::~RenderParamComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(RenderParamComponent)

    BEGIN_SERIALIZATION_CHILD(RenderParamComponent)
        SERIALIZE_OBJECT("ClearColor", m_clearColor);
        SERIALIZE_OBJECT("Lighting", m_lighting);
        SERIALIZE_OBJECT("Miscellaneous", m_misc);
        SERIALIZE_OBJECT("Mask", m_mask);
        SERIALIZE_MEMBER("Priority", m_renderParam.m_priority);
		SERIALIZE_MEMBER("ViewportVisibility", m_renderParam.m_viewportVisibility);
		SERIALIZE_MEMBER("AlwaysActive",m_alwaysActive);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(RenderParamComponent)
    END_VALIDATE_COMPONENT()

    RenderParamComponent::RenderParamComponent()
    {
		m_renderParam.pushSubRenderParam(m_clearColor);
		m_renderParam.pushSubRenderParam(m_lighting);
		m_renderParam.pushSubRenderParam(m_misc);
		m_renderParam.pushSubRenderParam(m_mask);
        m_inputColor.setType(InputType_F32);
        m_inputColor.setValue(0.0f);
        m_inputFactor.setType(InputType_F32);
        m_inputFactor.setValue(1.0f);
		m_alwaysActive = false;
    }

    RenderParamComponent::~RenderParamComponent()
    {
    }


    void RenderParamComponent::Update(f32 _deltaTime)
    {
        computeUnitScaledAABB();
        f32 fadeFactor = 0;
        getInput(fadeFactor);
		if ( m_alwaysActive )
			fadeFactor = 1.0f;
        m_renderParam.m_weight = fadeFactor;
    }

    void RenderParamComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if (EventSetFloatInput * onSetInput = DYNAMIC_CAST(_event,EventSetFloatInput))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
        }
    }

#ifdef ITF_SUPPORT_EDITOR

    void RenderParamComponent::onEditorMove( bbool _modifyInitialPos )
    {
        Super::onEditorMove(_modifyInitialPos);

        if(m_actor)
        {
            computeUnitScaledAABB();
        }
    }

#endif // ITF_SUPPORT_EDITOR

    void RenderParamComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
        computeUnitScaledAABB();

	    // Input color
        const InputDesc& inputColorDesc = getTemplate()->m_inputColor;
        if(inputColorDesc.getId().isValid())
        {
            m_inputColor.setId(inputColorDesc.getId());
            m_inputColor.setType(inputColorDesc.getType());

            if ( inputColorDesc.getType() == InputType_F32 )
            {
                m_inputColor.setValue(0.f);
            }
            else
            {
                m_inputColor.setValue(static_cast<u32>(0));
            }
        }

        // Input factor
        const InputDesc& inputFactorDesc = getTemplate()->m_inputFactor;
        if(inputFactorDesc.getId().isValid())
        {
            m_inputFactor.setId(inputFactorDesc.getId());
            if ( inputFactorDesc.getType() == InputType_F32 )
            {
                m_inputFactor.setValue(0.f);
            }
            else
            {
                m_inputFactor.setValue(static_cast<u32>(0));
            }
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetFloatInput_CRC,this);
	}

    void RenderParamComponent::computeUnitScaledAABB()
    {
        Actor * actor = GetActor();
        AABB aabb(Vec2d(-0.5, -0.5), Vec2d(0.5, 0.5));
        aabb.Scale(actor->getScale());
        aabb.Translate(actor->get2DPos());
        actor->growAABB(aabb);
    }

    void RenderParamComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
		Super::batchPrimitives(_views);

        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_RENDER_PARAM>(_views, &m_renderParam, GetActor()->getDepth(), GetActor()->getRef()); 
    }
}

