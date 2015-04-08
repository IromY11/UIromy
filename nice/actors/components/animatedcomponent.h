
#ifndef _ITF_ANIMATEDCOMPONENT_H_
#define _ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ANIMTREE_H_
#include "engine/blendTree/animTree/AnimTree.h"
#endif //_ITF_ANIMTREE_H_

namespace ITF
{
	class GhostRecorderInfo;
    class GhostPlayerInfo;

#define AnimatedComponent_CRC ITF_GET_STRINGID_CRC(AnimatedComponent,1654726928)
    class AnimatedComponent : public AnimLightComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimatedComponent,AnimLightComponent,1654726928);

    public:
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        AnimatedComponent();
        ~AnimatedComponent();

        //brut de brut pour l'instant
        typedef ITF_VECTOR<Vertex> VertexContainer;


        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onResourceLoaded();
        virtual void                onEvent( Event* _event );

        virtual bbool               isSynchronous() const { return Super::isSynchronous() || isLockMagicBox(); }
        virtual bool                updateData(f32 _deltaTime);
        virtual bool                postUpdateData();

		virtual void				SerializeAnim( CSerializerObject* serializer, const char* name, StringID &animName, u32 flags );

        virtual bbool               setAnim(const StringID & _anim, u32 _blendFrames = U32_INVALID, bbool _forceResetTree = bfalse, u32 _priority = 0);
		virtual StringID			getLastAnimSet() const { return m_lastAction; }
        virtual bbool				isInTransition() const { return m_animTree->isInTransition(); }
        bbool                       isAnimNodeFinished() const { return ( m_animTree->isValid() ? m_animTree->isAnimFinished() : isSubAnimFinished() ); }
		bbool                       isAnimNodeLooped() const { return ( m_animTree->isValid() ? m_animTree->isAnimLooped() : getIsLooped() ); }
		virtual bbool               getIsLooped() const { return isAnimNodeLooped(); }
        bbool                       isCurrentAnimNodePlaying() const { return !m_action.isValid(); }
        bbool                       isMainNodeFinished() const { return !isInTransition() && isAnimNodeFinished() && isCurrentAnimNodePlaying(); }
        u32                         getNumAnimTreeNodes() const { return m_animTree->getNumNodes(); }
        virtual bbool               isAnimToPlayFinished() const { return isMainNodeFinished(); }

        virtual u32                     getNumAnimsPlaying() const;
        virtual const StringID&         getAnimsPlayingName(u32 _index);
        virtual u32                     getNumAnims() const { return m_animTree->getNumNodes(); }
        virtual const StringID&         getAnimsName(u32 _index) {return m_animTree->getNodeID(_index);}
        
#ifdef ITF_DEBUG_STRINGID
        const char*                 getAnimTreeNodeName( u32 _index) { return m_animTree->getNodeID(_index).getDebugString(); }
#endif
        const AnimTree*             getAnimTree() { return m_animTree; }

        template <typename T>
        void                        setInput(StringID _inputName, T _value );

        template <typename T>
        void                        setInputFromIndex(u32 _index, T _value );

        bool                        getMagicBox(Vec3d & _pos, bbool _local = btrue);
        bbool                       getRoot( Vec3d& _pos ) const;

        void                        lockMagicBox( const Vec3d& _pos, f32 _interpolationTime = s_MagicBoxLockTimer );
        void                        resetLockMagicBoxFrom() { m_lockMagicBoxResetFrom = btrue; }
        const Vec3d&                getLockedMagicBoxPos() const { return m_lockMagicBoxPosTo; }
        void                        resetLockMagicBox() { m_lockMagicBox = bfalse; }
        bbool                       isLockMagicBox() const { return m_lockMagicBox; }
        bbool                       isLockMagicBoxInterpolating() const { return isLockMagicBox() && m_lockMagicBoxTimer != 0.f; }

        void                        setUpdateAnimInput( IUpdateAnimInputCriteria* _update ) { m_animInputUpdate = _update; }

		void                        dbgGetCurRule(const Color& _color);
		void                        dbgGetCurRuleNoPrint(String8 & _text);

        u32                         getNumInputs() const { return m_inputList.size(); }
        u32                         findInputIndex( StringID _id ) const;
#ifdef ITF_DEBUG_STRINGID
        const char*                 getInputName( u32 _index ) const { return m_inputList[_index].getName(); }
#endif
        InputType                   getInputType( u32 _index ) const { return m_inputList[_index].getType(); }

        template <typename T>
        void                        getInputValue( u32 _index, T& _val ) const
        {
            const Input& input = m_inputList[_index];
            input.getValue(_val);
        }
        StringID                    getInputID( u32 _index ) const { return m_inputList[_index].getId(); }

        virtual void                updateAnimTime( f32 _dt );
        virtual void                resetCurTime(bbool _sendChangeEvent = bfalse);
        void                        resetTree();
        void                        resetTransition();

        void                        setCurTimeCursor(f32 _value);
        void                        transfertTimeCursor(u32 _animIndex);

        void                        setCurrentTime(u32 _animId, f32 _value);
        f32                         getCurrentTime(u32 _animId);
        
		f32                         getAnimDuration( StringID _friendlyName ) const;
		bbool						isNodeValid(const StringID& _nodeName) const;

        u32                         getPlayingAnimFlags(u32 _layer = 0);

        f32                         getPrevBlending() const;

        void                        setDisableInputUpdate( bbool _val ) { m_disableInputUpdate = _val; }
        bbool                       getDisableInputUpdate() { return m_disableInputUpdate;}
        void                        onRecordGhost(GhostRecorderInfoBase* _ghostRecorderInfo);
    protected:

        ITF_INLINE const class AnimatedComponent_Template* getTemplate() const;

        virtual void                setFirstAnim();
        virtual void                processAnim();

        void                        updateLockMagicBox();

        typedef SafeArray<StringID> AnimsPlayingContainer;
        AnimsPlayingContainer       m_previousAnimsPlaying;
        AnimsPlayingContainer       m_currentAnimsPlaying;
        void                        getResultSubAnimNames(const AnimTreeResult & _result, AnimsPlayingContainer& _anims);

        IUpdateAnimInputCriteria*   m_animInputUpdate;
        VertexContainer             m_vertexs;

        AnimTree*                   m_animTree;

        StringID                    m_action;
        StringID                    m_lastAction;
        
        u32                         m_magicBoxIndex;
        Vec3d                       m_base;
        f32                         m_currentDt;

        Vec3d                       m_prevMagicBox;
        bbool                       m_lockMagicBox;
        Vec3d                       m_lockMagicBoxPosFrom;
        Vec3d                       m_lockMagicBoxPosTo;
        f32                         m_lockMagicBoxTimer;
        f32                         m_lockMagicBoxTotalTime;
        bbool                       m_lockMagicBoxResetFrom;
        static const f32            s_MagicBoxLockTimer;

        InputContainer              m_inputList;
        bbool                       m_disableInputUpdate;

        bbool                       m_forceResetTree;
    };

    template <typename T>
    ITF_INLINE void AnimatedComponent::setInput(StringID _inputName, T _value )
    {
        u32 numInputs = m_inputList.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            Input& input = m_inputList[i];

            if ( input.getId() == _inputName )
            {
                input.setValue(_value);
                break;
            }
        }
    }

    template <typename T>
    ITF_INLINE void AnimatedComponent::setInputFromIndex( u32 _index, T _value )
    {
        m_inputList[_index].setValue(_value);
    }

#define AnimatedComponent_Template_CRC ITF_GET_STRINGID_CRC(AnimatedComponent_Template,2655002388) 
    class AnimatedComponent_Template : public AnimLightComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimatedComponent_Template,AnimLightComponent_Template,2655002388);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(AnimatedComponent);

    public:

        AnimatedComponent_Template();
        ~AnimatedComponent_Template();

        virtual bbool                   onTemplateLoaded( bbool _hotReload );
        virtual void                    onTemplateDelete( bbool _hotReload );

        const ITF_VECTOR <InputDesc>&   getInputList() const { return m_inputList; }
        const AnimTree_Template&        getAnimTreeTemplate() const { return m_animTree; }
        
        u32                             findInputIndex( const StringID& _id ) const;
        AnimTree*                       createTreeInstance() const;

    private:

        ITF_VECTOR <InputDesc>      m_inputList;
        AnimTree_Template           m_animTree;
        ArchiveMemory               m_instanceData;
    };

    ITF_INLINE const class AnimatedComponent_Template* AnimatedComponent::getTemplate() const { return static_cast<const class AnimatedComponent_Template*>(m_template); }

	///////////////////////////////////////////////////////////////////////////////////////////
#define AnimatedWithSubstitionTemplatesComponent_CRC ITF_GET_STRINGID_CRC(AnimatedWithSubstitionTemplatesComponent,2635971270)
	class AnimatedWithSubstitionTemplatesComponent : public AnimatedComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(AnimatedWithSubstitionTemplatesComponent, AnimatedComponent, AnimatedWithSubstitionTemplatesComponent_CRC)
		DECLARE_SERIALIZE()
		DECLARE_VALIDATE_COMPONENT()

	protected:
		struct AnimSubst
		{
			DECLARE_SERIALIZE()
			StringID original;
			StringID final;
		};

		struct AnimSubstsTemplate
		{
			DECLARE_SERIALIZE()
			ITF_VECTOR<AnimSubst> substsList;
		};

	protected:
		ITF_INLINE const class AnimatedWithSubstitionTemplatesComponent_Template* getTemplate() const;

		// Inherited
		virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
		virtual void onEvent( Event* _event );

		// Templating
	private:
		u32 m_AnimSubstsTemplates_CurrentTemplateIndex; // U32_INVALID if no template defined actually
		ITF_VECTOR<AnimSubstsTemplate> m_AnimSubstTemplates_List;

		const StringID& AnimSubstsTemplates_GetTemplated(const StringID& _untemplatedAnimID) const;
		const StringID& AnimSubstsTemplates_GetUntemplated(const StringID& _templatedAnimID) const;

		// Inherited for templating
		virtual bbool			setAnim(const StringID & _anim, u32 _blendFrames = U32_INVALID, bbool _forceResetTree = bfalse, u32 _priority = 0);
		virtual StringID		getLastAnimSet()			const { return AnimSubstsTemplates_GetUntemplated(AnimatedComponent::getLastAnimSet()); }
		virtual const SubAnim*	getCurSubAnim( u32 _index ) const;

		// Constructors / Destructors
	public:
		AnimatedWithSubstitionTemplatesComponent() : AnimatedComponent(), m_AnimSubstsTemplates_CurrentTemplateIndex(U32_INVALID) { }

		// Accessors
		u32 AnimSubstsTemplates_GetCurrentId() const { return m_AnimSubstsTemplates_CurrentTemplateIndex; }
		void AnimSubstsTemplates_SetAtId(u32 _Id)	{ ITF_ASSERT(_Id == U32_INVALID || _Id < m_AnimSubstTemplates_List.size()); m_AnimSubstsTemplates_CurrentTemplateIndex = _Id; }
		void AnimSubstsTemplates_UnSet()			{ m_AnimSubstsTemplates_CurrentTemplateIndex = U32_INVALID; }
	};

#define AnimatedWithSubstitionTemplatesComponent_Template_CRC ITF_GET_STRINGID_CRC(AnimatedWithSubstitionTemplatesComponent_Template, 1546682461)
	class AnimatedWithSubstitionTemplatesComponent_Template : public AnimatedComponent_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(AnimatedWithSubstitionTemplatesComponent_Template, AnimatedComponent_Template, AnimatedWithSubstitionTemplatesComponent_Template_CRC)
		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(AnimatedWithSubstitionTemplatesComponent);
	};

	ITF_INLINE const class AnimatedWithSubstitionTemplatesComponent_Template* AnimatedWithSubstitionTemplatesComponent::getTemplate() const { return static_cast<const class AnimatedWithSubstitionTemplatesComponent_Template*>(m_template); }

}

#endif // _ITF_ANIMATEDCOMPONENT_H_

