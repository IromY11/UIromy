#include "precompiled_engine.h"

#ifndef _ITF_PLAYANIMCOMPONENT_H_
#include "engine/actors/components/PlayAnimComponent.h"
#endif //_ITF_PLAYANIMCOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

namespace ITF
{
	//////////////////////////////////////////////////////////////////////////
	BEGIN_SERIALIZATION_SUBCLASS(PlayAnimComponent,strPlayAnimParams)
		SERIALIZE_ANIM("AnimToPlay", m_animToPlay, m_animComponent);
		SERIALIZE_MEMBER("WaitAnimEnd", m_waitAnimEnd);
		if(!m_waitAnimEnd)
		{
			SERIALIZE_MEMBER("AnimTime", m_timer);
		}
	END_SERIALIZATION()

	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_OBJECT_RTTI(PlayAnimComponent)

	BEGIN_SERIALIZATION_CHILD(PlayAnimComponent)
		SERIALIZE_CONTAINER_OBJECT("SequenceList", m_params);
		SERIALIZE_MEMBER("isLooping", m_isLooping);
		SERIALIZE_MEMBER("startActive", m_activeAtStart);
	END_SERIALIZATION()

	BEGIN_VALIDATE_COMPONENT(PlayAnimComponent)
		VALIDATE_COMPONENT_PARAM("AnimLightComponent", m_animComponent, "AnimLightComponent mandatory");
	END_VALIDATE_COMPONENT()

	//------------------------------------------------------------------------------
	PlayAnimComponent::PlayAnimComponent() : Super()
		, m_animComponent(NULL)
		, m_currentTimer(-1.0f)
		, m_isLooping(bfalse)
		, m_waitAnimEnd(bfalse)
		, m_activeAtStart(btrue)
		, m_currentIndex(0)
	{
	}

	//------------------------------------------------------------------------------
	PlayAnimComponent::~PlayAnimComponent()
	{
	}	

	//------------------------------------------------------------------------------
	void PlayAnimComponent::Update( f32 _deltaTime )
	{
		Super::Update(_deltaTime);

		bbool canContinue = bfalse;
		if( m_waitAnimEnd )
		{
			if( m_animComponent->isAnimToPlayFinished() )
				canContinue = btrue;
		}

		if( m_currentTimer >= 0.0f )
		{
			m_currentTimer -= _deltaTime;
			if( m_currentTimer < 0.0f )
				canContinue = btrue;
		}

		if(canContinue)
		{
			++ m_currentIndex;
			if( m_currentIndex >= m_params.size() && m_isLooping )
				m_currentIndex = 0;

			playCurrentIndexAnim();
		}
	}	

	//------------------------------------------------------------------------------
	void PlayAnimComponent::playCurrentIndexAnim()
	{
		if( m_currentIndex < m_params.size() && m_params[m_currentIndex].m_animToPlay.isValid() )
		{
			m_waitAnimEnd = m_params[m_currentIndex].m_waitAnimEnd;
			m_currentTimer = m_waitAnimEnd ? -1.0f : m_params[m_currentIndex].m_timer;
			m_animComponent->setAnim(m_params[m_currentIndex].m_animToPlay);
			m_animComponent->resetCurTime();
		}
		else
		{
			if( m_currentIndex < m_params.size() )
			{
				ITF_WARNING(m_actor, m_params[m_currentIndex].m_animToPlay.isValid(), "Anim anme not valid");
			}
			m_waitAnimEnd = bfalse;
			m_currentTimer = -1.0f;
		}
	}

	//------------------------------------------------------------------------------
	void PlayAnimComponent::onFinalizeLoad()
	{
		Super::onFinalizeLoad();

		m_animComponent = m_actor->GetComponent<AnimLightComponent>();
		for (u32 u = 0; u < m_params.size(); ++u)
			m_params[u].m_animComponent = m_animComponent;

		//events
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor, EventTrigger_CRC, this);
	}

	//------------------------------------------------------------------------------
	void PlayAnimComponent::onBecomeActive()
	{
		Super::onBecomeActive();
		
		if( m_activeAtStart )
		{
			m_currentIndex = 0;
			playCurrentIndexAnim();
		}
		else
		{
			m_currentIndex = 0;
			m_waitAnimEnd = bfalse;
			m_currentTimer = -1.0f;
		}
	}

	//------------------------------------------------------------------------------
	void PlayAnimComponent::onEvent(Event * _event)
	{
		Super::onEvent(_event);

		if( const EventTrigger *const evt = DYNAMIC_CAST(_event, EventTrigger) )
		{
			if( evt->getActivated() )
			{
				m_currentIndex = 0;
				playCurrentIndexAnim();
			}
			else
			{
				m_currentIndex = 0;
				m_waitAnimEnd = bfalse;
				m_currentTimer = -1.0f;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_OBJECT_RTTI(PlayAnimComponent_Template)

	BEGIN_SERIALIZATION_CHILD(PlayAnimComponent_Template)
	END_SERIALIZATION()

	//------------------------------------------------------------------------------
	PlayAnimComponent_Template::PlayAnimComponent_Template() : Super()
	{	
	}

} // ITF
