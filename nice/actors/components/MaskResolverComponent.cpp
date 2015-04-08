#include "precompiled_engine.h"

#ifndef ITF_MASKRESOLVERCOMPONENT_H_
#include "engine/actors/components/MaskResolverComponent.h"
#endif //ITF_MASKRESOLVERCOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(MaskResolverComponent)
    BEGIN_SERIALIZATION_CHILD(MaskResolverComponent)
    SERIALIZE_MEMBER("clearFrontLightBuffer", m_clearFrontLight);
    SERIALIZE_MEMBER("clearFrontLightColor", m_clearFrontLightColor);
    SERIALIZE_MEMBER("clearBackLightBuffer", m_clearBackLight);
    SERIALIZE_MEMBER("clearBackLightColor", m_clearBackLightColor);
    SERIALIZE_MEMBER("blurFrontLightBuffer", m_blurFrontLightBuffer);
    SERIALIZE_MEMBER("blurBackLightBuffer", m_blurBackLightBuffer);
    SERIALIZE_MEMBER("blurQuality", m_blurQuality);
    SERIALIZE_MEMBER("blurSize", m_blurSize);
    END_SERIALIZATION();
    
    MaskResolverComponent::MaskResolverComponent()
        : m_clearFrontLight(btrue)
        , m_clearFrontLightColor(1.f, 0.5f, 0.5f, 0.5f)
        , m_clearBackLight(btrue)
        , m_clearBackLightColor(1.f, 0.0f, 0.0f, 0.0f)
        , m_blurFrontLightBuffer(bfalse)
        , m_blurBackLightBuffer(bfalse)
        , m_blurQuality(1)
        , m_blurSize(8)
    {
    }

    MaskResolverComponent::~MaskResolverComponent()
    {
    }
    
    void MaskResolverComponent::Update(f32 _deltaTime)
    {
        computeUnitScaledAABB();
    }

#ifdef ITF_SUPPORT_EDITOR

    void MaskResolverComponent::onEditorMove( bbool _modifyInitialPos )
    {
        Super::onEditorMove(_modifyInitialPos);

        if(m_actor)
        {
            computeUnitScaledAABB();
        }
    }

#endif // ITF_SUPPORT_EDITOR

    void MaskResolverComponent::computeUnitScaledAABB()
    {
        AABB aabb(Vec2d(-0.5, -0.5), Vec2d(0.5, 0.5));
        aabb.Scale(GetActor()->getScale());
        aabb.Translate(GetActor()->get2DPos());
        GetActor()->growAABB(aabb);
    }

    void MaskResolverComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
        m_maskResolvePrimitive.setClearFrontLight(getClearFrontLight());
        m_maskResolvePrimitive.setFrontLightBufferInverted(getFrontLightBufferInverted());
        m_maskResolvePrimitive.setClearFrontLightColor(getClearFrontLightColor());
        m_maskResolvePrimitive.setClearBackLight(getClearBackLight());
        m_maskResolvePrimitive.setClearBackLightColor(getClearBackLightColor());
        m_maskResolvePrimitive.setBlurFrontLight(getBlurFrontLightBuffer());
        m_maskResolvePrimitive.setBlurBackLight(getBlurBackLightBuffer());
        m_maskResolvePrimitive.setBlurQuality(getBlurQuality());
        m_maskResolvePrimitive.setBlurSize(getBlurSize());

        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_maskResolvePrimitive, m_actor->getDepth(), GetActor()->getRef());
    }

    void MaskResolverComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_maskResolvePrimitive.setFrontLightBuffer(getTemplate()->getFrontLightBuffer());
        m_maskResolvePrimitive.setBackLightBuffer(getTemplate()->getBackLightBuffer());
        computeUnitScaledAABB(); 
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(MaskResolverComponent_Template)
    BEGIN_SERIALIZATION_CHILD(MaskResolverComponent_Template)
        SERIALIZE_MEMBER("resolveFrontLightBuffer", m_resolveFrontLightBuffer);
        SERIALIZE_MEMBER("resolveFrontLightBufferInverted", m_resolveFrontLightBufferInverted);
        SERIALIZE_MEMBER("resolveBackLightBuffer", m_resolveBackLightBuffer);
    END_SERIALIZATION();

    MaskResolverComponent_Template::MaskResolverComponent_Template()
        :   m_resolveFrontLightBuffer(btrue)
        ,   m_resolveBackLightBuffer(btrue)
        ,   m_resolveFrontLightBufferInverted(bfalse)
    {
    }

    bbool MaskResolverComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);
        return bOk;
    }

    void MaskResolverComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        Super::onTemplateDelete(_hotReload);
    }

} // namespace ITF


