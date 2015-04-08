#include "precompiled_engine.h"

#ifndef _ITF_KEEPALIVECOMPONENT_H_
#include "engine/actors/components/KeepAlivecomponent.h"
#endif //_ITF_KEEPALIVECOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(KeepAliveComponent)
BEGIN_SERIALIZATION_CHILD(KeepAliveComponent)
END_SERIALIZATION()

KeepAliveComponent::KeepAliveComponent()
: Super()
, m_updateDone(bfalse)
, m_tryDying(bfalse)
, m_disableOnly(bfalse)
, m_graphicComponent(NULL)
, m_fadeStarted(bfalse)
, m_fadeTimer(0.0f)
, m_fadeStartAlpha(0.0f)
{ 
}

KeepAliveComponent::~KeepAliveComponent()
{
}

void KeepAliveComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_tryDying      = bfalse;
    m_updateDone    = bfalse;

    m_graphicComponent = m_actor->GetComponent<GraphicComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventDie_CRC,this);
}

void KeepAliveComponent::Update(f32 _deltaTime)
{
    Super::Update(_deltaTime);

    if (!m_tryDying)
        return;

    if (!m_updateDone)
    {
        // send another time a die event to be sure ...
        EventDie eventDie(m_disableOnly);
        m_actor->onEvent(&eventDie);
    }

    const Actor::ComponentsVector& actorComponents = m_actor->GetAllComponents();
    bbool keepAlive = bfalse;

    u32 count = actorComponents.size();
    for (u32 i=0; i<count; i++)
    {
        ActorComponent * component = actorComponents[i];
        if (component->keepAlive())
        {
            keepAlive = btrue;
            break;
        }
    }

    if(m_fadeStarted)
    {
        keepAlive = keepAlive || updateFade(_deltaTime);
    }

    if (!keepAlive)
    {
        // seb : it could happen
        //ITF_WARNING_CATEGORY(GPP,m_actor, m_updateDone, "Using KeepAliveComponent on actor with no component alive -> actor is born dead !");

        Scene* scene = m_actor->getScene();
        ITF_ASSERT(scene);
        if (scene)
        {
            if (m_disableOnly)
            {
                AIUtils::setAlwaysActive(m_actor, bfalse);
                m_actor->disable();
            } else
            {
                m_actor->requestDestruction();
            }
        }
        m_tryDying = bfalse;
    }

    m_updateDone = btrue;
} 

void KeepAliveComponent::onEvent(Event *_event)
{
    Super::onEvent(_event);

    if (_event->IsClassCRC(EventDie::GetClassCRCStatic()))
    {
        m_tryDying = btrue;
        m_disableOnly = ((EventDie *)_event)->isDisableOnly();
        if(m_graphicComponent != NULL && getTemplate()->getUseFade() && getTemplate()->getFadeDuration() > 0.0f)
        {
            m_fadeStarted = btrue;
            m_fadeTimer = getTemplate()->getFadeDuration();
            m_fadeStartAlpha = m_graphicComponent->getGfxPrimitiveParam().m_colorFactor.getAlpha();
        }
        else
        {
            m_fadeStarted = bfalse;
        }
    }
}

void KeepAliveComponent::onBecomeActive()
{
    Super::onBecomeActive();
    if (getTemplate()->getStartDying())
    {
        EventDie eventDie;
        m_actor->onEvent(&eventDie);
    }
}

bbool KeepAliveComponent::updateFade(const f32 _dt)
{
    ITF_ASSERT_CRASH(m_graphicComponent != NULL, "");

    m_fadeTimer = f32_Max(0.0f, m_fadeTimer - _dt);
    const f32 alphaRatio = m_fadeTimer / getTemplate()->getFadeDuration();
    const f32 newAlpha = m_fadeStartAlpha * alphaRatio;
    m_graphicComponent->getGfxPrimitiveParam().m_colorFactor.setAlpha(newAlpha);
    return newAlpha > 0.0f;
}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(KeepAliveComponent_Template)
BEGIN_SERIALIZATION_CHILD(KeepAliveComponent_Template)

    SERIALIZE_MEMBER("startDying", m_startDying);
    SERIALIZE_MEMBER("useFade", m_useFade);
    SERIALIZE_MEMBER("fadeDuration", m_fadeDuration);

END_SERIALIZATION()

KeepAliveComponent_Template::KeepAliveComponent_Template()
: m_startDying(bfalse)
, m_useFade(bfalse)
, m_fadeDuration(1.0f)
{
}

}

