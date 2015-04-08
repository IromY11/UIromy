#include "precompiled_engine.h"

#ifndef _ITF_ATLASANIMATIONCOMPONENT_H_
#include "engine/actors/components/AtlasAnimationComponent.h"
#endif //_ITF_ATLASANIMATIONCOMPONENT_H_

namespace ITF
{


IMPLEMENT_OBJECT_RTTI(AtlasAnimationComponent)
BEGIN_SERIALIZATION_CHILD(AtlasAnimationComponent)
END_SERIALIZATION()

AtlasAnimationComponent::AtlasAnimationComponent()
: Super()
, m_currentIndex(0)
, m_numIndices(0)
, m_timer(0.0f)
{
}

AtlasAnimationComponent::~AtlasAnimationComponent()
{
}


void AtlasAnimationComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_atlasObject.initialize(getTemplate()->getMaterial(), 1);
    
    updatePosition();
    updateAABB();
}

void AtlasAnimationComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
{
    Super::batchPrimitives(_views);

    GFXPrimitiveParam gfxParam = getGfxPrimitiveParam();
    gfxParam.m_colorFactor.m_a *= getAlpha();
    m_atlasObject.setGFXPrimitiveParam(gfxParam);
    m_atlasObject.addQuad(m_currentIndex,m_p1,m_p2,m_p3,m_p4);
    m_atlasObject.draw(_views, m_actor->getDepth(), getDepthOffset(), btrue, GetActor()->getRef());

}

void AtlasAnimationComponent::Update( f32 _deltaTime )
{
    Super::Update( _deltaTime );
    if (m_numIndices == 0)
    {
        m_numIndices = m_atlasObject.getNumIndices();
    }
    else
    {
        updatePosition();
        
        updateAnimation(_deltaTime);
    }

    updateAABB();

}

void AtlasAnimationComponent::updatePosition()
{
    const f32 halfWidth = 0.5f * getTemplate()->getWidth() * m_actor->getScale().x();
    const f32 halfHeight = 0.5f * getTemplate()->getHeight() * m_actor->getScale().y();
    const Vec2d scaledOffset = (getTemplate()->getPosOffset() * m_actor->getScale());
    const Vec2d p1=Vec2d(-halfWidth,halfHeight) + scaledOffset;
    const Vec2d p2=Vec2d(-halfWidth,-halfHeight) + scaledOffset;
    const Vec2d p3=Vec2d(halfWidth,-halfHeight) + scaledOffset;
    const Vec2d p4=Vec2d(halfWidth,halfHeight) + scaledOffset;

    const Transform2d transform(m_actor->get2DPos(), m_actor->getAngle() + getTemplate()->getRotOffset().ToRadians(), m_actor->getIsFlipped());
    m_p1 = transform.transformPos(p1);
    m_p2 = transform.transformPos(p2);
    m_p3 = transform.transformPos(p3);
    m_p4 = transform.transformPos(p4);
}

void AtlasAnimationComponent::updateAnimation( f32 _deltaTime )
{
    m_timer = m_timer + getTemplate()->getPlayRate() * _deltaTime; 

    u32 add = (u32)(m_timer/_deltaTime);
    if (add > 0)
        m_timer = f32_Modulo(m_timer,f32(add) * _deltaTime);

    m_currentIndex = (m_currentIndex + add) % m_numIndices;
}

void AtlasAnimationComponent::updateAABB()
{
    AABB aabb(m_p1);
    aabb.grow(m_p2);
    aabb.grow(m_p3);
    aabb.grow(m_p4);
    m_actor->growAABB(aabb);
}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AtlasAnimationComponent_Template)
BEGIN_SERIALIZATION_CHILD(AtlasAnimationComponent_Template)
    // insure retro compatibility of material with texture = 
    BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
        SERIALIZE_MEMBER("texture", m_material.getTexturePathSet().getTexturePath());
    END_CONDITION_BLOCK();
    //
    SERIALIZE_OBJECT("material", m_material);
    SERIALIZE_MEMBER("playRate", m_playRate);
    SERIALIZE_MEMBER("width", m_width);
    SERIALIZE_MEMBER("height", m_height);
END_SERIALIZATION()

AtlasAnimationComponent_Template::AtlasAnimationComponent_Template()
: m_width(1.0f)
, m_height(1.0f)
, m_playRate(1.0f)
{
}

AtlasAnimationComponent_Template::~AtlasAnimationComponent_Template()
{
}

bbool AtlasAnimationComponent_Template::onTemplateLoaded(bbool _hotReload)
{
    bbool ret = Super::onTemplateLoaded(_hotReload);
    m_material.onLoaded(m_actorTemplate->getResourceContainer());
    return ret;
}

void AtlasAnimationComponent_Template::onTemplateDelete(bbool _hotReload)
{
    m_material.onUnLoaded(m_actorTemplate->getResourceContainer());
    Super::onTemplateDelete(_hotReload);
}

}

