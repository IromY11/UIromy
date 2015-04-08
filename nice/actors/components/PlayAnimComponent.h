#ifndef _ITF_PLAYANIMCOMPONENT_H_
#define _ITF_PLAYANIMCOMPONENT_H_

#define PlayAnimComponent_CRC               ITF_GET_STRINGID_CRC(PlayAnimComponent,1839890091)
#define PlayAnimComponent_Template_CRC      ITF_GET_STRINGID_CRC(PlayAnimComponent_Template,3004324331)

namespace ITF
{
	//////////////////////////////////////////////////////////////////////////
	class PlayAnimComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(PlayAnimComponent, ActorComponent, PlayAnimComponent_CRC)
		DECLARE_SERIALIZE()
		DECLARE_VALIDATE_COMPONENT()

	public:
		PlayAnimComponent();
		virtual ~PlayAnimComponent();

		virtual bbool       needsUpdate() const { return btrue; }
		virtual bbool       needsDraw() const { return bfalse; }
		virtual bbool       needsDraw2D() const { return bfalse; }
		virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }		

		virtual void        onFinalizeLoad();
		virtual void		onBecomeActive();
		virtual void        Update(f32 _deltaTime);
		virtual void		onEvent(Event * _event);

	private:
		ITF_INLINE const class PlayAnimComponent_Template* getTemplate() const;

		void	playCurrentIndexAnim();

	private:
		class AnimLightComponent *			m_animComponent;
		f32									m_currentTimer;
		u32									m_currentIndex;
		bbool								m_isLooping;
		bbool								m_waitAnimEnd;
		bbool								m_activeAtStart;
		//--------------------------------------------------------------------------------------------------------------------------------
		class strPlayAnimParams
		{
			DECLARE_SERIALIZE()
		public:
			strPlayAnimParams(bbool _waitAnimEnd = bfalse, f32 _timer = 0.0f, const StringID &_animToPlay = StringID::Invalid, AnimLightComponent *_animComponent = NULL) 
				: m_waitAnimEnd(_waitAnimEnd)
				, m_timer(_timer)
				, m_animToPlay(_animToPlay)
				, m_animComponent(_animComponent) 
			{
			}
		public:
			bbool					m_waitAnimEnd;
			f32						m_timer;
			StringID				m_animToPlay;
			AnimLightComponent		*m_animComponent;
		};
		//--------------------------------------------------------------------------------------------------------------------------------
		ITF_VECTOR<strPlayAnimParams>		m_params;
	};


	//////////////////////////////////////////////////////////////////////////
	class PlayAnimComponent_Template : public ActorComponent_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(PlayAnimComponent_Template, ActorComponent_Template, PlayAnimComponent_Template_CRC)
		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(PlayAnimComponent);

	public:		
		PlayAnimComponent_Template();
		virtual	~PlayAnimComponent_Template() {}

	};

	//////////////////////////////////////////////////////////////////////////
	ITF_INLINE const class PlayAnimComponent_Template * PlayAnimComponent::getTemplate() const
	{
		return static_cast<const class PlayAnimComponent_Template *>(m_template);
	}
}

#endif // _ITF_PLAYANIMCOMPONENT_H_

