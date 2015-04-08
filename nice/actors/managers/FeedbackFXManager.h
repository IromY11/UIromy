#ifndef _ITF_FEEDBACKFXMANAGER_H_
#define _ITF_FEEDBACKFXMANAGER_H_


#include "adapters/AudioSerializedData/SoundDescriptor_common.h"


#ifndef _ITF_FXDESCRIPTOR_H_
#include "engine/display/FxDescriptor.h"
#endif //_ITF_FXDESCRIPTOR_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

#include "adapters/AudioSerializedData/SoundDescriptorTemplate_common.h"
#include "core/StringID.h"

namespace ITF
{
    //forward
    class HitStim;

	///////////////////////////////////////////////////////////////////////////////////////////
	//	FXCONTROL
	///////////////////////////////////////////////////////////////////////////////////////////
	struct FXControl
	{
		DECLARE_SERIALIZE()
		static const u32 flag_sound     = 0x1;
		static const u32 flag_material  = 0x2;
		static const u32 flag_particle  = 0x4;
        static const u32 flag_music     = 0x8;
        static const u32 flag_busMix    = 0x10;
		static const u32 flag_fluid		= 0x20;

		FXControl() : 
		m_fxStopOnEndAnim(bfalse)
			, m_fxPlayOnce(bfalse)
			, m_pickColorFromFreeze(bfalse)
			, m_fxInstanceOnce(bfalse)
			, m_fxEmitFromBase(btrue)
			, m_fxUseActorSpeed(btrue)
            , m_fxUseActorOrientation(bfalse)
            , m_fxUseActorAlpha(btrue)
			, m_fxBoneIndex(U32_INVALID)
            , m_fxUseBoneOrientation(BOOL_false)
			, m_fxFlags(0)
            , m_busMixActivate(btrue)
            , m_owner(StringID::Invalid)
            , m_music(StringID::Invalid)
            , m_busMix(StringID::Invalid)
            , m_fxDontStopSound(bfalse)
			, m_fxAttach(bfalse)
			
		{
		}

        // Specific to feedback
        StringID    m_owner;

		StringID    m_name;
		bbool       m_fxStopOnEndAnim;
		bbool       m_fxPlayOnce;
		bbool       m_pickColorFromFreeze;
        bbool       m_fxInstanceOnce;
		bbool       m_fxEmitFromBase;
		u32         m_fxFlags;
		bbool       m_fxUseActorSpeed;
        bbool       m_fxUseActorOrientation;
        bbool       m_fxUseActorAlpha;
		StringID    m_fxBoneName;
		u32         m_fxBoneIndex;
        BOOL_COND   m_fxUseBoneOrientation;
        bbool       m_busMixActivate;
        bbool       m_fxDontStopSound;
		bbool		m_fxAttach;

		ITF_VECTOR<StringID> m_sounds;
		ITF_VECTOR<StringID> m_particles;
		ITF_VECTOR<StringID> m_fluids;

        StringID    m_music;
        StringID    m_busMix;

		void init();
	};

	///////////////////////////////////////////////////////////////////////////////////////////
	//	ACTIONMAP
	///////////////////////////////////////////////////////////////////////////////////////////
	class Action
	{
	public:
		DECLARE_SERIALIZE()

		Action();
		~Action();

		typedef ITF_MAP<StringID, FXControl>  ActionMap;

		FXControl *	getFeedbacks(const StringID& _action);
		ActionMap*	getActionMap() { return &m_actions; }

		const FXControl *	getFeedbacks(const StringID& _action) const;
		const ActionMap*	getActionMap() const { return &m_actions; }

	private:

		ActionMap	m_actions;
	};

	///////////////////////////////////////////////////////////////////////////////////////////
	//	TARGETMAP
	///////////////////////////////////////////////////////////////////////////////////////////
	class Target
	{
	public:

		DECLARE_SERIALIZE()

		Target();
		~Target();

		typedef ITF_MAP<StringID, Action>		TargetMap;

		Action*			getActions(const StringID& _target);
		TargetMap*		getTargetMap() { return &m_targets; }

		const Action*		getActions (const StringID& _target) const;
		const TargetMap*	getTargetMap() const { return &m_targets; }

	private:

		TargetMap	m_targets;
	};

	///////////////////////////////////////////////////////////////////////////////////////////
	//  FEEDBACK FX MANAGER
	///////////////////////////////////////////////////////////////////////////////////////////
	class FeedbackFXManager
	{
	public:

		DECLARE_SERIALIZE()

		FeedbackFXManager();
		~FeedbackFXManager();

		typedef ITF_MAP<StringID,Target>		    ActorMap;

		typedef ITF_VECTOR<StringID>				ActionIDs;
		typedef ITF_MAP<StringID, ActionIDs>		ActionsPerActorMap;
		typedef ITF_MAP<StringID, ActionIDs>		ActionsPerTargetMap;
        typedef ITF_VECTOR<FXControl *>             FXControls;
        typedef ITF_MAP<StringID, FXControls>       OwnerMap;

		typedef ITF_VECTOR<ResourceID>				ResourceList;

		void				init();

        const FXControl*	getFeedback( const ITF_VECTOR<StringID>& _actorTags
                                        , const StringID& _action
                                        , const ITF_VECTOR<StringID>& _targetTags);

        void                acquireExtraResources( const ITF_VECTOR<StringID>& _actorTags, ITF_VECTOR<const GFX_MATERIAL *>& _materials, ITF_VECTOR<Path>& _sounds);
        void                releaseExtraResources( const ITF_VECTOR<StringID>& _actorTags);

        void                acquireAdditionnalFXDescriptors(const ITF_VECTOR<StringID>& _actorTags, ITF_VECTOR<FxDescriptor_Template *> & _fxDescriptors);
        void                releaseAdditionnalFXDescriptors(const ITF_VECTOR<StringID>& _actorTags);

        // TODO -> add refcount and resource management on sound descriptors (acquire and release AdditionnalSoundDescriptors)
        void                getAdditionnalSoundDescriptors(const ITF_VECTOR<StringID>& _actorTags, ITF_VECTOR<const class SoundDescriptor_Template *>& _soundDescriptors);

		bbool				hasFeedback( const StringID& _actor
										, const StringID& _actorArchetype
                                        , const StringID& _defaultActor
										, const StringID& _action
										, const StringID& _target
										, const StringID& _targetArchetype
                                        , const StringID& _defaultTarget );

		SoundDescriptor *		getNewSoundDescriptor( const Actor * _owner, const SoundDescriptor_Template * _soundDesc, class SoundComponent * _component);
		void					releaseSoundDescriptors( const Actor * _owner, SoundDescriptor * _soundDesc = NULL );

		FxDescriptor *			getNewFxDescriptor( const Actor * _owner, const FxDescriptor_Template * _FxDesc);
		void					releaseFxDescriptors( const Actor * _owner, FxDescriptor * _FxDesc = NULL );

		const SoundDescriptor_Template*	getSDTemplate(const StringID& _name) const;
		const FxDescriptor_Template*	getFXTemplate(const StringID& _name) const;

        const StringID&             getDefaultMaterial() const {return m_defaultMaterial;}
        const StringID&             getDefaultActor() const {return m_defaultActor;}

        const StringID&             getCustomCategory(const ITF_VECTOR<StringID>& _tags);
        const StringID&             getCustomCategory(const StringID& _type);

#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE
        void                        reloadFeedbackFXManager();
#endif // ITF_SUPPORT_HOTRELOAD_TEMPLATE

#ifdef ITF_SUPPORT_EDITOR
        void                        toggleDebugRules() { m_debugRules = !m_debugRules; }
        bbool                       isDebugRules() const { return m_debugRules; }
        void                        updateDebugInfo();
#endif // ITF_SUPPORT_EDITOR

        // For bundle process
        void                        getDependencies( const StringID& _archetype, const StringID& _type,  ITF_VECTOR<Path>& _dependencies );

	private:
        FxDescriptor_Template * acquireFXTemplate(const StringID& _name);
        void releaseFXTemplate(const StringID& _name);

		// Structures used for descriptors pools
		struct SoundDescriptorPoolElem
		{
			SoundDescriptorPoolElem()
				: m_isFree(true)
				, m_owner(NULL)
			{}

			bbool				m_isFree;
			const Actor*		m_owner;

			SoundDescriptor		m_descriptor;
		};

		struct FxDescriptorPoolElem
		{
			FxDescriptorPoolElem()
				: m_isFree(true)
				, m_owner(NULL)
			{}

			bbool			m_isFree;
			const Actor*	m_owner;

			FxDescriptor	m_descriptor;
		};

		typedef ITF_VECTOR<SoundDescriptorPoolElem>	SoundDescriptorsPool;
		typedef ITF_VECTOR<FxDescriptorPoolElem>	FxDescriptorsPool;

        void                    acquireExtraMaterials(FXControl * _fxControl, ITF_VECTOR<const GFX_MATERIAL *> & _materials);
        void                    releaseExtraMaterials(FXControl * _fxControl);
#ifdef ITF_SUPPORT_RAKI
        void                    getExtraSounds(FXControl * _fxControl, ITF_VECTOR<Path>& _sounds );
#endif

		bbool					hasFeedbackImpl( const StringID& _actor
												, const StringID& _action
												, const StringID& _target);

		const FXControl*		getFeedbackImpl( const StringID& _actor
													, const StringID& _action
													, const StringID& _target);

		void					releaseSoundDescriptor(u32 _index);
		void					releaseFxDescriptor(u32 _index);

		void					initInternal();
		void					updateInternal();

        void                    addFXControlInOwnerTable(FXControl * _fxControl);

        // For bundle process
        void                    getVFXPaths(FXControl * _fxControl, ITF_VECTOR<Path>& _paths);
#ifdef ITF_SUPPORT_RAKI
        void                    getSoundPaths(FXControl * _fxControl, ITF_VECTOR<Path>& _paths);
#endif

		// Used to quickly access descriptors templates
		ITF_MAP<StringID, u32>	m_SDTemplateMap;
        struct RefCountedTemplateID
        {
            RefCountedTemplateID() : refCount(0) {}
            RefCountedTemplateID(u32 _templateID) : refCount(0), templateID(_templateID) {}

            u32 refCount;
            u32 templateID;
        };
		ITF_MAP<StringID, RefCountedTemplateID>	m_FXTemplateMap;

		ActorMap				m_actors;
        OwnerMap                m_owners;
		ActionsPerActorMap		m_actionsPerActors;
		ActionsPerTargetMap		m_actionsPerTargets;

		SoundDescriptorsPool	m_soundDescPool;
		FxDescriptorsPool		m_FxDescPool;

        StringID          m_defaultMaterial;
        StringID          m_defaultActor;

        ITF_MAP<StringID,StringID>      m_busMap;

        const class FeedbackFXManager_Template* m_template;

#ifdef ITF_SUPPORT_EDITOR
        void                storeLastRule(const StringID& _actorName, const StringID& _actionName, const StringID& _targetName, const StringID& _fxControlName, bbool _found = btrue) {m_lastSelectedRule.m_actorName = _actorName; m_lastSelectedRule.m_actionName = _actionName; m_lastSelectedRule.m_targetName = _targetName; m_lastSelectedRule.m_FXControlName = _fxControlName; m_lastSelectedRule.m_found = _found; }
        void                resetLastRule() {storeLastRule(StringID::Invalid,StringID::Invalid,StringID::Invalid,StringID::Invalid, bfalse);}
        bbool               m_debugRules;
        struct st_rule
        {
            st_rule() : m_actorName(StringID::Invalid), m_actionName(StringID::Invalid), m_targetName(StringID::Invalid), m_FXControlName(StringID::Invalid), m_found(btrue) {}
            StringID    m_actorName;
            StringID    m_actionName;
            StringID    m_targetName;
            StringID    m_FXControlName;
            bbool       m_found;
        };
        st_rule                 m_lastSelectedRule;
        ITF_VECTOR<StringID>    m_lastActorTags;
        StringID                m_lastAction;
        ITF_VECTOR<StringID>    m_lastTargetTags;
#endif // ITF_SUPPORT_EDITOR
	};

	///////////////////////////////////////////////////////////////////////////////////////////
	//	FEEDBACKFXMANAGER_TEMPLATE
	///////////////////////////////////////////////////////////////////////////////////////////
	class FeedbackFXManager_Template : public TemplateObj
	{
		DECLARE_OBJECT_CHILD_RTTI(FeedbackFXManager_Template, TemplateObj,152627411);
		DECLARE_SERIALIZE()

	public:

        struct buses
        {
            DECLARE_SERIALIZE()
            StringID m_actorType;
            StringID m_bus;
        };

		FeedbackFXManager_Template()
			: Super()
		{}

		~FeedbackFXManager_Template();


		virtual bbool   onTemplateLoaded( bbool _hotReload );
        virtual void    onTemplateDelete( bbool _hotReload );
        virtual void    setLoadedInPlace();

        const FeedbackFXManager::ActorMap&				getActorsMap() const { return m_actors; }
		const ITF_VECTOR<SoundDescriptor_Template> &	getSDTemplates() const { return m_soundDescriptorTemplates; }
		FxDescriptor_Template &                         getFXTemplate(ux _idx) const { ITF_ASSERT(_idx < getNbFXTemplate()); return m_FxDescriptorTemplates[_idx]; }
        ux                                              getNbFXTemplate() const { return m_FxDescriptorTemplates.size(); }
        const ITF_VECTOR<buses>&                        getBusList() const { return m_busList; }

	private:
        ITF_VECTOR<buses>                       m_busList;

        FeedbackFXManager::ActorMap				m_actors;
		ITF_VECTOR<SoundDescriptor_Template>	m_soundDescriptorTemplates;
        // as resource loading is not done in onTemplateLoaded, we need to get m_FxDescriptorTemplates mutable
		mutable ITF_VECTOR<FxDescriptor_Template> m_FxDescriptorTemplates;
	};
}
#endif // _ITF_FEEDBACKFXMANAGER_H_
