#ifndef _ITF_UICOMPONENT_H_
#define _ITF_UICOMPONENT_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_
#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#include "engine/AdaptersInterfaces/GFXAdapter_Constant.h"

namespace ITF
{
	#define UIComponent_CRC ITF_GET_STRINGID_CRC(UIComponent,2232305413)

    enum UIState
    {
        UI_STATE_None               = 0,
        UI_STATE_Hidden             = (1 << 0),
        UI_STATE_Showed             = (1 << 1),
        UI_STATE_Item               = (1 << 2),
        UI_STATE_Menu               = (1 << 3),
        UI_State_SmartItem          = (1 << 4),
        UI_State_ModelItem          = (1 << 5),
        UI_STATE_ListElement        = (1 << 6),
        UI_STATE_LockedByDefault    = (1 << 7),
        UI_STATE_LockedByParent     = (1 << 8),
        UI_STATE_LockedByOnline     = (1 << 9),
        UI_STATE_DownByDefault      = (1 << 10),
		UI_STATE_Hiding				= (1 << 11),
		UI_STATE_Showing			= (1 << 12),
        UI_STATE_OnlineWaiting  	= (1 << 13),
        UI_STATE_IsMasked         	= (1 << 14),
        UI_STATE_IsSelected         = (1 << 15),
        UI_STATE_IsValidate         = (1 << 16),
        UI_STATE_IsBack         	= (1 << 17),
        UI_STATE_IsAction         	= (1 << 18),
        UI_STATE_IsLocked           = (1 << 19),
        UI_STATE_IsDown         	= (1 << 20),
        UI_STATE_IsActivating       = (1 << 21),
        UI_STATE_ActivateByBack     = (1 << 22),
        UI_STATE_ActivateByAction   = (1 << 23),
		UI_STATE_IsSliding			= (1 << 24),
        UI_STATE_LockedButSelectable= (1 << 25),
        UI_STATE_Custom1            = (1 << 31),
        UI_STATE_LockedMask         = (UI_STATE_LockedByDefault | UI_STATE_LockedByParent | UI_STATE_LockedByOnline | UI_STATE_LockedButSelectable),
        UI_STATE_DisplayMask        = (UI_STATE_Hidden | UI_STATE_Showed | UI_STATE_Showing | UI_STATE_Hiding),
        UI_STATE_OnlineMask         = (UI_STATE_OnlineWaiting | UI_STATE_LockedByOnline),
        UI_STATE_CanNotBeValidate   = (UI_STATE_LockedMask | UI_State_SmartItem | UI_State_ModelItem ),
        UI_STATE_CanNotBeSelected   = (UI_STATE_Hidden | UI_State_ModelItem | UI_STATE_LockedByDefault | UI_STATE_LockedByOnline),
        ENUM_FORCE_SIZE_32(UIState)
    };

	enum UITransitionMode
	{
		UI_TRANSITION_FarToClose = 0,
		UI_TRANSITION_LeftToRight,
		UI_TRANSITION_Tween,
		ENUM_FORCE_SIZE_32(UITransitionMode)
	};

    static const f32 UI_DIRECTION_SELECTION_DEFAULT_DOT = 0.5f; // cos(PI/3)
    
    class UIComponent : public ActorComponent
    {
        friend class UIMenuManager;
        DECLARE_OBJECT_CHILD_RTTI(UIComponent, ActorComponent,2232305413);

    public:
        DECLARE_SERIALIZE()

        UIComponent();
        virtual ~UIComponent();

        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const             { return bfalse; }
        virtual     bbool       needsDraw2D         (       ) const             { return bfalse; }
        virtual     bbool       is2D                (       ) const;
		virtual		bbool       needsDraw2DNoScreenRatio(   ) const;

        virtual     void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual     void        onEvent(Event * _event);
        virtual     void        onFinalizeLoad();
        virtual     void        Update              ( f32 _deltaTime );
        virtual     void        setIsDisplay(const bbool _isDisplay);
        virtual     void        setIsMasked(const bbool _isMasked);

        static UIComponent*     getUIComponent (ObjectRef _ref);
        static bbool            isEditionMode(const Actor* actor, bbool _checkSelected = btrue);

        ObjectRef               getParentObject() const;
        UIComponent*            getParentComponent() const;
        template<class T> T*    getParentComponent() const;
        ObjectRefList&          getChildObjectsList() const;
        ObjectRef               getChildObject(const StringID _friendly) const;
        template<class T> T*    getChildComponent( const StringID _friendly ) const;
        UIComponent*            getChildComponent( const StringID _friendly, bbool _rec = bfalse ) const;
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
        UIComponent*            getStackedChildComponent( const StringID _friendly, bbool _rec = bfalse ) const;
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD
		Actor*					getChildActor( const StringID _friendly, bbool _rec = bfalse );
		virtual UIComponent*	getChildComponentFromPosition(const Vec2d& _position, i32 _neededState = UI_STATE_None, i32 _forbiddenState = UI_STATE_None) const;
        virtual UIComponent*    getChildComponentToDirection(UIComponent* _currentComponent, Vec2d _direction, bbool _closest = btrue, i32 _neededState = UI_STATE_None, i32 _forbiddenState = UI_STATE_None, f32 _maxDot = UI_DIRECTION_SELECTION_DEFAULT_DOT, f32* _bestDistance = NULL ) const;
        u32                     getChildComponentCount(i32 _neededState = UI_STATE_None, i32 _forbiddenState = UI_STATE_None) const;
        
        bbool                   getIsMasterComponent    (       ) const {return m_isMasterComponent;}
        StringID                getFriendlyID           (       ) const;
		
        void                    setRelativePosition(const Vec2d& _position, bbool _setInit = bfalse);
        void                    setAbsolutePosition(const Vec2d& _position, bbool _setInit = bfalse);
        void                    setAbsoluteDepth(const i32 _depth);
        void                    setAbsoluteScale(const Vec2d& _scale);

        bbool                   getIsDisplay() const;
        Vec2d                   getRelativePosition() const;
        Vec2d                   getAbsolutePosition() const;
        i32                     getAbsoluteDepth() const;
        i32                     getAbsoluteDepthMax() const;
        Vec2d                   getAbsoluteScale() const;

        void                        resetInitState(bbool _recursive = bfalse);
        void			            setUseTransition(bbool _inTransition, bbool _recursive = bfalse);
		ITF_INLINE bbool		    getUseTransition() const { return m_useTransition; }
		ITF_INLINE UITransitionMode	getTransitionMode() const { return m_transitionMode; }
		ITF_INLINE void			    setTransitionMode(UITransitionMode _inTransitionmode) {m_transitionMode = _inTransitionmode; }

       	UIComponent*            getChildComponentByDirection(UIComponent* _currentComponent, const Vec2d& _direction) const;

        ITF_INLINE StringID		getLeftComponentID() const { return m_leftComponentID; }
        ITF_INLINE void			setLeftComponentID(StringID _leftComponentID) { m_leftComponentID = _leftComponentID; }   
        ITF_INLINE StringID		getRightComponentID() const { return m_rightComponentID; } 
        ITF_INLINE void			setRightComponentID(StringID _rightComponentID) { m_rightComponentID = _rightComponentID; }   
		ITF_INLINE StringID		getUpComponentID() const { return m_upComponentID; } 
        ITF_INLINE void			setUpComponentID(StringID _upComponentID) { m_upComponentID = _upComponentID; }   
        ITF_INLINE StringID		getDownComponentID() const { return m_downComponentID; }   
        ITF_INLINE void			setDownComponentID(StringID _downComponentID) { m_downComponentID = _downComponentID; }   
        ITF_INLINE Vec2d		getCursorOffset() const { return m_cursorOffset; }   
        ITF_INLINE Vec2d		getCursorOffset_4_3() const { return m_cursorOffset_4_3; }   
        ITF_INLINE f32			getCursorAngle() const { return m_cursorAngle; }   

        const String8&          getText() const;
		virtual void            setText(const String8 & _text);
		virtual void			setLoc(const LocalisationId& _locId);
        virtual bbool           contains(const Vec2d& _position) const;
        ITF_INLINE bbool        hasUIState(i32 _state) const {return (_state & m_UIState) == _state;}
        ITF_INLINE bbool        matchUIState(i32 _neededState, i32 forbiddenState) const {return ((_neededState & m_UIState) == _neededState) &&  ((forbiddenState & m_UIState) == 0);}
        ITF_INLINE void         addUIState(i32 _state) {m_UIState |= _state;}
        ITF_INLINE void         removeUIState(i32 _state) {m_UIState &= ~_state;}
        ITF_INLINE void         setUIState(i32 _state, bbool _isSet) {_isSet ? addUIState(_state) : removeUIState(_state);}
        View::MaskIds           getUIDisplayMask()  const;
        void                    setUIDisplayMask( View::MaskIds _mask, bbool _recursive = btrue);
        void                    setUIDisplayViewId( View::EditableViewIds _editableViewId, bbool _recursive = btrue);
        View::EditableViewIds   getUIDisplayViewId() const {return m_displayMask;}
        bbool					getIsInTransition() const;
        bbool                   getWantDisplay() const {return m_wantDisplay;}

        void                    checkComponentState(bbool _forceSetPos = bfalse);

		void					updateUIPositionForDeviceAspectRatio();

    protected:

        const class UIComponent_Template* getTemplate() const;
 
		Vec2d					fixUIPositionForAspectRatio( Vec2d& _inPos );

        void                    checkDisplayState(f32 _deltaTime = 0.0f);
        void                    updateState (bbool _mustDisplay, f32 _deltaTime);

        virtual void            onBecomeActive();
        virtual bbool           onIsShowing(f32 _dt);
        virtual void            onShowingEnd();
        virtual void            onUpdate(f32 _dt){}
        virtual void            onHiddingBegin();
        virtual bbool           onIsHidding(f32 _dt);
        virtual void            onBecomeInactive();

		f32						m_lastViewportRatio;

        bbool                   m_isMasterComponent;
        bbool                   m_wantDisplay;
        bbool                   m_initDisplay;
        Vec2d                   m_screenSpace;
        Vec2d                   m_currentScreenSpace;
        i32                     m_UIState;
        bbool                   m_needReactive;
        View::EditableViewIds   m_displayMask;

		UITransitionMode		m_transitionMode;
		bbool					m_useTransition;
        bbool                   m_isTransitionReady;
        bbool                   isTransitionReady() const;
        void					setIsInTransition(bbool _hide);
        bbool					updateTransition(const f32 _dt);
        void					setBaseScale(const Vec2d& _inBaseScale);
        Vec2d					getBaseScale();
		
		f32						m_transitionTime;
        f32                     m_fadeCursor;

		// transition values
		Vec2d m_SpringVelocity;
		Vec2d m_SpringLength;

        StringID                m_leftComponentID;
        StringID                m_rightComponentID;
        StringID                m_upComponentID;
        StringID                m_downComponentID;
        Vec2d					m_cursorOffset;
        Vec2d					m_cursorOffset_4_3;
        f32						m_cursorAngle;
	
#ifdef ITF_SUPPORT_EDITOR
    public:
        virtual     void        drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const ;
        virtual     void        onEditorCreated( class Actor* _original );
        virtual     void        onEditorMove(bbool _modifyInitialPos = btrue);
        virtual     void        onEditorSelected    () {}
        static      void        drawBox2D(const Vec2d & _minBound, const Vec2d & _maxBound, const f32 _border = 1.0f, const Color _color = COLOR_RED);
        void                    drawBox3D(const Vec2d & _minBound, const Vec2d & _maxBound, const f32 _border = 1.0f, const Color _color = COLOR_RED) const;
        virtual     void        onPostPropertyChange();
#endif // ITF_SUPPORT_EDITOR

    private:
        Vec2d					m_baseScale;	// NOT THE SAME AS INITIAL SCALE!!!!

    };


    class UIComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIComponent_Template, ActorComponent_Template,1440308778);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIComponent);

    public:
        UIComponent_Template();
        ~UIComponent_Template();


		static const f32		g_transitionTimeBase;
        bbool                   getIs2D() const { return m_is2D; }
        bbool                   getIs2DNoScreenRatio() const { return m_is2DNoScreenRatio; }
        f32                     getShowingFadeDuration() const { return m_showingFadeDuration; }
        f32                     getHidingFadeDuration() const { return m_hidingFadeDuration; }


    protected:
        bbool m_is2D;
        bbool m_is2DNoScreenRatio;
        f32 m_showingFadeDuration;
        f32 m_hidingFadeDuration;
    };

    ITF_INLINE const UIComponent_Template* UIComponent::getTemplate() const
    {
        return static_cast<const UIComponent_Template*>(m_template);
    }

    template<class T> 
    T* UIComponent::getChildComponent(const StringID _friendly) const
    {
        const ObjectRefList& componentObjectList = getChildObjectsList();

        for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
        {
            const Actor* actor = (const Actor*) iter->getObject();
            if(actor && actor->getUserFriendlyID()==_friendly)
            {
                if(T* component = actor->GetComponent<T>())
                {
                    return component;
                }
            }
        }

        return NULL;
    }

    template<class T> 
    T* UIComponent::getParentComponent() const
    {
        if(const Actor* actor = (const Actor*) getParentObject().getObject())
        {
            if(T* component = actor->GetComponent<T>())
            {
                return component;
            }
        }

        return NULL;
    }
}
#endif // _ITF_UICOMPONENT_H_
