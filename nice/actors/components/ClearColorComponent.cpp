    #include "precompiled_engine.h"

#ifndef ITF_CLEARCOLORCOMPONENT_H_
#include "engine/actors/components/ClearColorComponent.h"
#endif //ITF_CLEARCOLORCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(ClearColorComponent_Template)

    BEGIN_SERIALIZATION_CHILD(ClearColorComponent_Template)
    END_SERIALIZATION()

    ClearColorComponent_Template::ClearColorComponent_Template()
        : Super()
    {
    }

    ClearColorComponent_Template::~ClearColorComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(ClearColorComponent)

    BEGIN_SERIALIZATION_CHILD(ClearColorComponent)
        SERIALIZE_OBJECT("clearColor", m_clearColor);
        SERIALIZE_MEMBER("Weight", m_renderParam.m_weight);
        SERIALIZE_MEMBER("Priority", m_renderParam.m_priority);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(ClearColorComponent)
    END_VALIDATE_COMPONENT()

    ClearColorComponent::ClearColorComponent()
    {
		m_renderParam.pushSubRenderParam(m_clearColor);
		m_renderParam.m_weight = 1.0f;
    }

    ClearColorComponent::~ClearColorComponent()
    {
    }


    void ClearColorComponent::Update(f32 _deltaTime)
    {
        computeUnitScaledAABB();
    }

#ifdef ITF_SUPPORT_EDITOR

    void ClearColorComponent::onEditorMove( bbool _modifyInitialPos )
    {
        Super::onEditorMove(_modifyInitialPos);

        if(m_actor)
        {
            computeUnitScaledAABB();
        }
    }

#endif // ITF_SUPPORT_EDITOR

    void ClearColorComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
        computeUnitScaledAABB();
    }

    void ClearColorComponent::computeUnitScaledAABB()
    {
        Actor * actor = GetActor();
        AABB aabb(Vec2d(-0.5, -0.5), Vec2d(0.5, 0.5));
        aabb.Scale(actor->getScale());
        aabb.Translate(actor->get2DPos());
        actor->growAABB(aabb);
    }

    void ClearColorComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
		Super::batchPrimitives(_views);

        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_RENDER_PARAM>(_views, &m_renderParam, GetActor()->getDepth(), GetActor()->getRef()); 
    }
}
