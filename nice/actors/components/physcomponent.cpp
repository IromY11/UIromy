#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PhysComponent)

BEGIN_SERIALIZATION_CHILD(PhysComponent)
    SERIALIZE_MEMBER("Mass", m_mass)
    SERIALIZE_MEMBER("Friction", m_friction)
    SERIALIZE_MEMBER("FrictionMultiplier", m_initialFrictionMultiplier)
#ifdef ITF_PROJECT_POP
    SERIALIZE_MEMBER("GravityMultiplier", m_initialGravityMultiplier)
#endif
END_SERIALIZATION()

PhysComponent::PhysComponent()
: Super()
, m_speed(Vec2d::Zero)
, m_force(Vec2d::Zero)
, m_impulses(Vec2d::Zero)
, m_currentGravity(Vec2d::Zero)
, m_currentGravityDir(Vec2d::Zero)
, m_disabled(bfalse)
, m_mass(1.0f)
, m_friction(0.5f)
, m_frictionMultiplier(1.0f)
, m_initialFrictionMultiplier(1.0f)
, m_gravityMultiplier(1.f)
, m_initialGravityMultiplier(1.0f)
{
}

PhysComponent::~PhysComponent()
{
}

void PhysComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
#ifndef ITF_PROJECT_POP
    m_initialGravityMultiplier = getTemplate()->getInitialGravityMultiplier();
#endif

    m_frictionMultiplier = m_initialFrictionMultiplier;
    m_gravityMultiplier = m_initialGravityMultiplier;

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventQueryPhysicsData_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventAddForce_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSequenceActorActivate_CRC,this);
}

void PhysComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    setSpeed(Vec2d::Zero);
    resetForces();
}

void PhysComponent::onForceMove( )
{
    m_speed = Vec2d::Zero;
}



void PhysComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( EventQueryPhysicsData* queryPhysics = DYNAMIC_CAST(_event,EventQueryPhysicsData) )
    {
        processQueryPhysics(queryPhysics);
    }
    else if (EventAddForce * onAddForce = DYNAMIC_CAST(_event,EventAddForce))
    {
        addForceEvent(onAddForce);
    }
    else if (EventSequenceActorActivate * sequenceActivate = DYNAMIC_CAST(_event,EventSequenceActorActivate))
    {
        setDisabled(sequenceActivate->getActivate());
        resetForces();
    }
}

void PhysComponent::processQueryPhysics( class EventQueryPhysicsData* _query ) const
{
    _query->setSpeed(m_speed);
}

void PhysComponent::addForceEvent( EventAddForce * _onAddForce )
{
    ITF_ASSERT_CRASH(_onAddForce!=NULL,"Invalid EventAddForce");
    addForce(_onAddForce->getForce());
}


//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(PhysComponent_Template)
BEGIN_SERIALIZATION_CHILD(PhysComponent_Template)
#ifndef ITF_PROJECT_POP
    SERIALIZE_MEMBER("physGravityMultiplier", m_initialGravityMultiplier);
#endif
END_SERIALIZATION()

PhysComponent_Template::PhysComponent_Template()
#ifndef ITF_PROJECT_POP
: m_initialGravityMultiplier(1.0f)
#endif
{
}

}

