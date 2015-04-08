#ifndef _ITF_RANDOMANIMBANKCHANGECOMPONENT_H_
#define _ITF_RANDOMANIMBANKCHANGECOMPONENT_H_

#define RandomAnimBankChangeComponent_CRC ITF_GET_STRINGID_CRC(RandomAnimBankChangeComponent,3445990800)
#define RandomAnimBankChangeComponent_Template_CRC ITF_GET_STRINGID_CRC(RandomAnimBankChangeComponent_Template,4214636168)

namespace ITF
{
	// forward declaration
	class AnimLightComponent;
	class RandomAnimBankChangeComponent_Template;

	class strRandomPatchName
	{
		DECLARE_SERIALIZE()
	public:
		strRandomPatchName(StringID _name = StringID::Invalid, const RandomAnimBankChangeComponent_Template * _this = NULL) : m_name(_name), m_this(_this) {}
		bool operator==(const strRandomPatchName& rOther) const	{ return m_name == rOther.m_name; }

		StringID										m_name;
		const RandomAnimBankChangeComponent_Template	*m_this;
	};

	class RandomAnimBankChangeComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(RandomAnimBankChangeComponent, ActorComponent, RandomAnimBankChangeComponent_CRC)
		DECLARE_SERIALIZE()
		DECLARE_VALIDATE_COMPONENT()

	public:
		RandomAnimBankChangeComponent();
		virtual ~RandomAnimBankChangeComponent();

		virtual bbool       needsUpdate() const { return bfalse; }
		virtual bbool       needsDraw() const { return bfalse; }
		virtual bbool       needsDraw2D() const { return bfalse; }
		virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

		virtual void        onFinalizeLoad();
		virtual void        onBecomeActive();

#ifdef ITF_SUPPORT_EDITOR
		virtual void                                onEditorCreated( class Actor* _original );
#endif

	private:
		ITF_INLINE const RandomAnimBankChangeComponent_Template* getTemplate() const;

		void				refreshAnimBankChanges();
		void				onPropertyChanged();
		void				tryToComputeRand();

	private:
		class strRandomAnimBankPart 
		{
			DECLARE_SERIALIZE()
		public:
			strRandomAnimBankPart(StringID _src = StringID::Invalid, StringID _name = StringID::Invalid, AnimLightComponent* _component = NULL) 
				: m_src(_src)
				, m_name(_name)
				, m_animComponent(_component) {}

			StringID				m_src;
			StringID				m_name;	
			AnimLightComponent		*m_animComponent;		
		};

		AnimLightComponent					*m_animComponent;
		bbool								m_forceRecompute;
		bbool								m_alreadyCompute;
		bbool								m_recomputeOnDuplicate;
		u32									m_nbItem;
#define NB_BANK_MAX 6
		strRandomAnimBankPart				m_randomAnimBank[NB_BANK_MAX];
		ITF_VECTOR<strRandomPatchName>		m_patchNamesToRemove;
	};


	//---------------------------------------------------------------------------------------------------

	class RandomAnimBankChangeComponent_Template : public ActorComponent_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(RandomAnimBankChangeComponent_Template, ActorComponent_Template, RandomAnimBankChangeComponent_Template_CRC)
		DECLARE_ACTORCOMPONENT_TEMPLATE(RandomAnimBankChangeComponent)
		DECLARE_SERIALIZE()

	public:
		RandomAnimBankChangeComponent_Template();
		virtual ~RandomAnimBankChangeComponent_Template();

		class tplRandomPatchNames
		{
			DECLARE_SERIALIZE()
		public:
			tplRandomPatchNames(StringID _name = StringID::Invalid, f32 _percent = 0.0f) : m_name(_name), m_percent(_percent) {}
			bool operator==(const tplRandomPatchNames& rOther) const	{ return m_name == rOther.m_name; } // use in find method

			StringID	m_name;
			f32			m_percent;
		};

		void									getRandomPatchNameToRemoved(ITF_VECTOR<strRandomPatchName> &_out) const;
		const ITF_VECTOR<tplRandomPatchNames>		&getPatchNamesCanBeRemoved() const { return m_patchNamesCanBeRemoved; }

	private:
		ITF_VECTOR<tplRandomPatchNames>			m_patchNamesCanBeRemoved;
	};


	//---------------------------------------------------------------------------------------------------

	ITF_INLINE const RandomAnimBankChangeComponent_Template* RandomAnimBankChangeComponent::getTemplate() const
	{
		return static_cast<const RandomAnimBankChangeComponent_Template*>(m_template);
	}
}

#endif // _ITF_RANDOMANIMBANKCHANGECOMPONENT_H_
