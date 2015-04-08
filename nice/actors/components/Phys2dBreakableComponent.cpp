#include "precompiled_engine.h"
#ifdef USE_BOX2D

#ifndef _ITF_PHYS2DBREAKABLECOMPONENT_H_
#include "engine/actors/components/Phys2dBreakableComponent.h"
#endif //_ITF_PHYS2DBREAKABLECOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_RENDERSIMPLEANIMCOMPONENT_H_
#include "gameplay/Components/Display/RenderSimpleAnimComponent.h"
#endif //_ITF_RENDERSIMPLEANIMCOMPONENT_H_

#ifndef _ITF_PHYS2D_H_
#include "engine/Phys2d/Phys2d.h"
#endif //_ITF_PHYS2D_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

namespace ITF
{
#define SERIALIZE_BREAKABLEBEHAVE(name,val) \
    SERIALIZE_ENUM_BEGIN(name,val); \
    SERIALIZE_ENUM_VAR(eDoNothing); \
    SERIALIZE_ENUM_VAR(eDestroyAfterBreak); \
    SERIALIZE_ENUM_VAR(eRebuildAfterBreak); \
    SERIALIZE_ENUM_END();


    IMPLEMENT_OBJECT_RTTI(sBreakAnim)
    BEGIN_SERIALIZATION(sBreakAnim)
     SERIALIZE_MEMBER("angle", m_angle)
     SERIALIZE_MEMBER("animBreak", m_animBreak)
    END_SERIALIZATION()  


    IMPLEMENT_OBJECT_RTTI(Phys2dBreakableComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Phys2dBreakableComponent_Template)
        SERIALIZE_MEMBER("BreakingImpulse", m_breakingImpulse)
        SERIALIZE_MEMBER("AnimBreak", m_animBreak)
        SERIALIZE_CONTAINER_OBJECT("AnimBreakAngle", m_animBreakAngle)
        SERIALIZE_MEMBER("animbreakPriority",m_animBreakPriority)
        SERIALIZE_MEMBER("AnimHit", m_animHit)
        SERIALIZE_CONTAINER_OBJECT("AnimHitAngle", m_animHitAngle)
        SERIALIZE_MEMBER("animHitPriority",m_animHitPriority)
        SERIALIZE_MEMBER("AnimRebuild", m_animRebuild)
        SERIALIZE_BREAKABLEBEHAVE("BehaveAfterBreaking", m_behaveAfterBreak)
        SERIALIZE_MEMBER("collisionFilter", m_collisionFilter)
        
        if(m_behaveAfterBreak != eDoNothing)
            SERIALIZE_MEMBER("BehaveDelay", m_behaveDelay)
    END_SERIALIZATION()



    Phys2dBreakableComponent_Template::Phys2dBreakableComponent_Template()
    : Super()
    , m_breakingImpulse(0.4f)
    , m_animBreak("Break")
    , m_animBreakPriority(256)
    , m_animHit(StringID::Invalid)  // Default NO HIT ANIMATION
    , m_animHitPriority(128)
    , m_animRebuild("Rebuild")
    , m_behaveAfterBreak(eDoNothing)
    , m_behaveDelay(5.0f)
    , m_collisionFilter(0xFFFFFFFF)
    {
    }

    Phys2dBreakableComponent_Template::~Phys2dBreakableComponent_Template()
    {
    }

    
    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Phys2dBreakableComponent)

    BEGIN_SERIALIZATION_CHILD(Phys2dBreakableComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Phys2dBreakableComponent)
        VALIDATE_COMPONENT_PARAM("Component's Dependence", m_animComponent, "AngleAnimatedComponent requires an AnimLightComponent");
        VALIDATE_COMPONENT_PARAM("Component's Dependence", m_phys2dComponent, "AngleAnimatedComponent requires a Phys2dComponent");
    END_VALIDATE_COMPONENT()

    Phys2dBreakableComponent::Phys2dBreakableComponent()
    : Super()
    , m_resitance(10.0f)
    {
    }


    Phys2dBreakableComponent::~Phys2dBreakableComponent()
    {
    }

    void Phys2dBreakableComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_phys2dComponent = m_actor->GetComponent<Phys2dComponent>();
        if(m_phys2dComponent)
            m_phys2dComponent->registerListener(this);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,AnimGameplayEvent_CRC, this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,AnimPolylineEvent_CRC, this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventCustomStateSetup_CRC,this);
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventCustomStateCheck_CRC,this);
		m_resitance = getTemplate()->getBreakingImpulse();
    }

	// Check this function, it does never gets called, tested on F5, Ctrl F5 and when the actor gets created.
    void Phys2dBreakableComponent::onResourceReady()
    {

    }

    void Phys2dBreakableComponent::onStartDestroy(bbool _hotReload)
    {
        Super::onStartDestroy(_hotReload);

		EventDestroyRequest* _evt = new EventDestroyRequest();
		_evt->setSender(m_actor->getRef());
		_evt->setActivator(m_actor->getRef());
		for(ITF_VECTOR<Actor*>::iterator it = m_onDestroyListeners.begin(); it != m_onDestroyListeners.end(); it++)
		{
			(*it)->onEvent(_evt);
		}
		delete _evt;

		m_onDestroyListeners.clear();
    }

    void Phys2dBreakableComponent::beginContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* contact )
    {
//         m_animComponent->setAnim(getTemplate()->getDefaultAnimBreak());
//         //TODO: ALL :-)
    }

    void Phys2dBreakableComponent::postSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* contact, const b2ContactImpulse* impulse )
    {
        if (isBroken())
        {
            // The body already broke.
            return;
        }

        // Should the body break?
        int32 count = contact->GetManifold()->pointCount;

        float32 maxImpulse = 0.0f;
        for (int32 i = 0; i < count; ++i)
        {
            maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
        }

        if (maxImpulse >= m_resitance)
        {
            Vec2d v(impulse->normalImpulses[0], impulse->normalImpulses[1]);
            breakObject(v);
        }
        else
        {
            Vec2d v(impulse->normalImpulses[0], impulse->normalImpulses[1]);
            hitObject(v);
        }

    }

    void Phys2dBreakableComponent::addFakeCollision( Vec2d _impulse, Vec2d _pos, u32 _filter /*= 0xFFFFFFFF*/)
    {
        b2Body* body = m_phys2dComponent->getBody();
        body->ApplyLinearImpulse(Phys2d::Vec2dToB2Vec2(_impulse),Phys2d::Vec2dToB2Vec2(_pos), true);

        if (_impulse.norm() >= m_resitance && checkFilter(_filter))
        {
            breakObject(_impulse);
        }
        else
        {
            hitObject(_impulse);
        }
    }

	void Phys2dBreakableComponent::setPercentualDamage( Vec2d _axis, f32 _damage, u32 _filter)
	{
        if(isBroken())
            return;

		if(_damage >= 1.0f && checkFilter(_filter))
			breakObject(_axis);
		else if(m_animComponent->isAnimationsProcedural()) 
			m_animComponent->setProceduralCursor(_damage);
        else
            hitObject(_axis);
	}

    void Phys2dBreakableComponent::onEvent( Event * _event )
    {
        if(EventCustomStateSetup* customState = DYNAMIC_CAST(_event, EventCustomStateSetup))
        {
            EventCustomStateSetup::ComponentData myComponentData;
            myComponentData.m_componentName = "Phys2dBreakableComponent";
            myComponentData.m_stateName.push_back("Full");
            myComponentData.m_stateName.push_back("Break");
            const ITF_VECTOR<ActorComponent*> components = m_actor->GetAllComponents();
            for (u32 index = 0; index < components.size(); index++)
            {
                if (components[index] == this)
                {
                    myComponentData.m_componentIndex  = index;
                }
            }

            customState->addComponent(myComponentData);
        }
        else if(EventCustomStateCheck* customState = DYNAMIC_CAST(_event, EventCustomStateCheck) )
        {
            customState->setTestValid(customState->getTestState() == (u32) isBroken());
        }
    }

    void Phys2dBreakableComponent::breakObject(Vec2d &_vec)
    {
        EventDead* _evt = new EventDead();
        _evt->setSender(m_actor->getRef());
        _evt->setActivator(m_actor->getRef());
        for(ITF_VECTOR<Actor*>::iterator it = m_onDestroyListeners.begin(); it != m_onDestroyListeners.end(); it++)
        {
            (*it)->onEvent(_evt);
        }
        delete _evt;
    }

    void Phys2dBreakableComponent::hitObject(Vec2d &_vec)
    {
    }

	void Phys2dBreakableComponent::registerOnDestroyListener( Actor* _actor )
	{
		m_onDestroyListeners.push_back(_actor);
	}

    ITF_INLINE bbool Phys2dBreakableComponent::checkFilter( u32 _filter )
    {
        return (bbool)(_filter&getTemplate()->getCollisionFilter());
    }


}

#endif
