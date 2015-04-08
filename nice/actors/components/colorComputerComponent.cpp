#include "precompiled_engine.h"

#ifndef _ITF_COLORCOMPUTERCOMPONENT_H_
#include "engine/actors/components/colorComputercomponent.h"
#endif //_ITF_COLORCOMPUTERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(colorComputerComponent)

BEGIN_SERIALIZATION_CHILD(colorComputerComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("near",m_near);
        SERIALIZE_MEMBER("far",m_far);
        SERIALIZE_MEMBER("nearColor",m_nearColor); 
        SERIALIZE_MEMBER("farColor",m_farColor);
        SERIALIZE_MEMBER("tagId",m_tagId);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

colorComputerComponent::colorComputerComponent()
: Super()
, m_near(10.f)
, m_far(-10.f)
, m_nearColor(0.f,1.f,1.f,1.f)
, m_farColor(Color::white())
, m_tagId(0)
{
    m_width = 5.f;
    m_height = 5.f;
}

f32 colorComputerComponent::getFactorFromZ(f32 _z)
{
    f32 retz = 0.f; 
    f32 actorDepth = m_actor->getDepth();
    f32 posNear = actorDepth + m_near;
    f32 posFar = actorDepth + m_far;

    if (_z >= posFar && _z <= posNear)
    {
        f32 depthsize = f32_Abs(m_far - m_near);
        retz = ((posNear - _z) / depthsize);
    }
    else if (_z < posFar)
    {
        retz = 1.f;
    }

    return retz;
}

colorComputerComponent::~colorComputerComponent()
{
}

#ifdef ITF_SUPPORT_EDITOR
void colorComputerComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    f32 scalex = m_actor->getScale().x();
    f32 scaley = m_actor->getScale().y();

    const f32 radNear   = scalex * m_width;
    const f32 radFar    = scaley * m_height;

    //    f32 depthsize = f32_Abs(m_far - m_near);

    const Vec2d myPos(m_actor->getPos().x(), m_actor->getPos().y());

    if (m_actor->isSelected())
    {
        f32 actorDepth = m_actor->getDepth();

        // Draw box with near radius
        const Vec2d vN1(myPos.x() - radNear, myPos.y() - radFar);
        const Vec2d vN2(myPos.x() + radNear, myPos.y() - radFar);
        const Vec2d vN3(myPos.x() + radNear, myPos.y() + radFar);
        const Vec2d vN4(myPos.x() - radNear, myPos.y() + radFar);

        GFX_ADAPTER->drawDBGLine(vN1, vN2, 1.f, 1.f, 0.f, 0.0f, 1.f, actorDepth + m_near);
        GFX_ADAPTER->drawDBGLine(vN2, vN3, 1.f, 1.f, 0.f, 0.0f, 1.f, actorDepth + m_near);
        GFX_ADAPTER->drawDBGLine(vN3, vN4, 1.f, 1.f, 0.f, 0.0f, 1.f, actorDepth + m_near);
        GFX_ADAPTER->drawDBGLine(vN4, vN1, 1.f, 1.f, 0.f, 0.0f, 1.f, actorDepth + m_near);

        // Draw box with far radius
        const Vec2d vF1(myPos.x() - radNear, myPos.y() - radFar);
        const Vec2d vF2(myPos.x() + radNear, myPos.y() - radFar);
        const Vec2d vF3(myPos.x() + radNear, myPos.y() + radFar);
        const Vec2d vF4(myPos.x() - radNear, myPos.y() + radFar);

        GFX_ADAPTER->drawDBGLine(vF1, vF2, 1.f, 1.f, 1.f, 0.0f, 1.f, actorDepth + m_far);
        GFX_ADAPTER->drawDBGLine(vF2, vF3, 1.f, 1.f, 1.f, 0.0f, 1.f, actorDepth + m_far);
        GFX_ADAPTER->drawDBGLine(vF3, vF4, 1.f, 1.f, 1.f, 0.0f, 1.f, actorDepth + m_far);
        GFX_ADAPTER->drawDBGLine(vF4, vF1, 1.f, 1.f, 1.f, 0.0f, 1.f, actorDepth + m_far);
    }
}
#endif // ITF_SUPPORT_EDITOR

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(colorComputerComponent_Template)
BEGIN_SERIALIZATION_CHILD(colorComputerComponent_Template)
END_SERIALIZATION()

colorComputerComponent_Template::colorComputerComponent_Template()
{
}

colorComputerComponent_Template::~colorComputerComponent_Template()
{
}


}

