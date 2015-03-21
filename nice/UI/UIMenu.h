#ifndef _ITF_UIMENU_H_
#define _ITF_UIMENU_H_

#ifndef _ITF_UIITEM_H_
#include "gameplay/components/UI/UIItem.h"
#endif //_ITF_UIITEM_H_

#ifndef _ITF_TOUCHSCREENADAPTER_H_
#include "engine/AdaptersInterfaces/TouchScreenAdapter.h"
#endif  //_ITF_TOUCHSCREENADAPTER_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    #define UIMenu_CRC ITF_GET_STRINGID_CRC(UIMenu,3688013168)
    #define UIMenu_Template_CRC ITF_GET_STRINGID_CRC(UIMenu_Template,986418860)

	#define input_actionID_None                 StringID::Invalid

	#define input_actionID_NavLeft              ITF_GET_STRINGID_CRC(NAVMENU_LEFT, 1882292689)
	#define input_actionID_NavRight             ITF_GET_STRINGID_CRC(NAVMENU_RIGHT, 3387471567)

	#define input_actionID_Up                   ITF_GET_STRINGID_CRC(MENU_UP, 501263146)
    #define input_actionID_Down                 ITF_GET_STRINGID_CRC(MENU_DOWN, 1280587374)
    #define input_actionID_Right                ITF_GET_STRINGID_CRC(MENU_RIGHT, 532126592)
    #define input_actionID_Left                 ITF_GET_STRINGID_CRC(MENU_LEFT, 2917793312)
	
	#define input_actionID_LeftStickRelease     ITF_GET_STRINGID_CRC(MENU_LX_RELEASE, 2017445310)

    #define input_actionID_UpHold               ITF_GET_STRINGID_CRC(MENU_UP_HOLD, 4172004239)
    #define input_actionID_DownHold             ITF_GET_STRINGID_CRC(MENU_DOWN_HOLD, 3693162641)
    #define input_actionID_RightHold            ITF_GET_STRINGID_CRC(MENU_RIGHT_HOLD, 3422970606)
    #define input_actionID_LeftHold             ITF_GET_STRINGID_CRC(MENU_LEFT_HOLD, 747984973)

    #define input_actionID_Valid                ITF_GET_STRINGID_CRC(MENU_VALID, 1084313942)
	#define input_actionID_Valid_Release        ITF_GET_STRINGID_CRC(MENU_VALID_RELEASE, 2910578407)
    #define input_actionID_Start                ITF_GET_STRINGID_CRC(MENU_START, 56410331)
    #define input_actionID_Back                 ITF_GET_STRINGID_CRC(MENU_BACK, 2477582220)
    #define input_actionID_Characters_Diries    ITF_GET_STRINGID_CRC(CHARACTERS_DIARIES_ACTION, 346926370)
    #define input_actionID_Clue                 ITF_GET_STRINGID_CRC(CLUE_ACTION, 1374367223 )
    #define input_actionID_Other                ITF_GET_STRINGID_CRC(MENU_OTHER, 2656090922)
    #define input_actionID_Other2               ITF_GET_STRINGID_CRC(MENU_OTHER2, 2348584654)
    #define input_actionID_onPressed            ITF_GET_STRINGID_CRC(MENU_ONPRESSED, 185785632)
    #define input_actionID_onReleased           ITF_GET_STRINGID_CRC(MENU_ONRELEASED, 14213630)
    #define input_actionID_Delete_Save          ITF_GET_STRINGID_CRC(MENU_DELETE_SAVE, 349253420)
    #define input_actionID_Wiki_Leave           ITF_GET_STRINGID_CRC(WIKI_LEAVE, 1953898323)
    #define input_actionID_Skip_Cine_Suggest    ITF_GET_STRINGID_CRC(SKIP_CINE_SUGGEST, 574236986)
    #define input_actionID_Esc                  ITF_GET_STRINGID_CRC(MENU_ESC, 2663858356)

    ///////////////////////////////////////////////////////////////////////////////////////////

    class UIMenu : public UIItem
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(UIMenu, UIItem, UIMenu_CRC);
        DECLARE_SERIALIZE()

    public:

        enum MenuType
        {
            MenuType_None                               = 0,
            MenuType_JustDisplay                        = (1 << 0),
            MenuType_InputListener                      = (1 << 1),
            MenuType_WithAfx                            = (1 << 2),
            MenuType_WithBack                           = (1 << 3),
            MenuType_HideBack                           = (1 << 4),
			MenuType_BackIsExit						    = (1 << 5),
            MenuType_DoNotListenPad                     = (1 << 6),
            MenuType_DoNotListenTouch                   = (1 << 7),
            MenuType_CanNotBeMasked                     = (1 << 8),
            MenuType_DoNotListenNavigation              = (1 << 9),
            MenuType_DoNotLockLowerMenuInputListening   = (1 << 10),

            MenuType_JustDisplayCanNotBeMasked          = (MenuType_JustDisplay | MenuType_CanNotBeMasked),
            MenuType_InputListenerCannotBeMasked        = (MenuType_InputListener | MenuType_CanNotBeMasked),
            MenuType_InputListenerWithBack              = (MenuType_InputListener | MenuType_WithBack),
			MenuType_InputListenerWithExit			    = (MenuType_InputListener | MenuType_WithBack | MenuType_BackIsExit),
            MenuType_InputListenerWithHiddenBack        = (MenuType_InputListener | MenuType_WithBack | MenuType_HideBack),
            MenuType_InputListenerWithBackAndAfx        = (MenuType_InputListener | MenuType_WithBack | MenuType_WithAfx),
            MenuType_InputListenerWithExitAndAfx		= (MenuType_InputListener | MenuType_WithBack | MenuType_BackIsExit| MenuType_WithAfx),
            MenuType_InputListenerWithHiddenBackAndAfx  = (MenuType_InputListener | MenuType_WithBack | MenuType_HideBack| MenuType_WithAfx),
            MenuType_InputListenerWithoutBackAndAfx     = (MenuType_InputListener | MenuType_WithAfx),
			MenuType_InputListenerAlways				= (MenuType_InputListener | MenuType_DoNotLockLowerMenuInputListening),
            MenuType_InputListenerAlwaysWithHiddenBack  = (MenuType_InputListener | MenuType_DoNotLockLowerMenuInputListening | MenuType_WithBack | MenuType_HideBack),
            ENUM_FORCE_SIZE_32(MenuType)
        };

        UIMenu  ();
        virtual ~UIMenu ();

        virtual void onActorLoaded(Pickable::HotReloadType _hot);
        virtual void onFinalizeLoad();
        virtual void onStartDestroy(bbool _hotReload);
        virtual void Update(f32 _dt);
        virtual void onBecomeActive();
        virtual void onBecomeInactive();
		virtual void onEvent(Event * _event);

        virtual void onInput(const i32 _controllerID, const f32 _axis, const StringID _action) = 0;
		virtual void onTouchInput(const TouchDataArray & _touchArray, u32 _i = 0) = 0;

#ifdef ITF_PC
        virtual void onMouseButton(const Vec2d& _pos, InputAdapter::MouseButton _but, InputAdapter::PressStatus _status) {}
        virtual void onMousePos(const Vec2d& _pos) {}
#endif //ITF_PC

#ifdef ITF_W1W_MOBILE
        virtual UIItem* getValidateItem(bbool _waitActivatingEnd = btrue) const {return NULL;}
#else
        virtual UIItem* getValidateItem(bbool _waitActivatingEnd = bfalse) const {return NULL;}
#endif //ITF_W1W_MOBILE

        virtual UIItem* getBackItem(bbool _waitActivatingEnd = bfalse) const {return NULL;}
        virtual UIItem* getActionItem(bbool _waitActivatingEnd = bfalse) const {return NULL;}
        virtual void resetValidateState() {}
        virtual i32 getValidateItemControllerID() const {return -1;}
        virtual i32 getBackItemControllerID() const {return -1;}
        virtual i32 getActionItemControllerID() const {return -1;}
        virtual StringID getValidateItemInput() const {return StringID::InvalidId; }
        virtual StringID getBackItemInput() const {return StringID::InvalidId; }
        virtual StringID getActionItemInput() const {return StringID::InvalidId; }
        virtual void setIsMasked(const bbool _isMasked);
        virtual MenuType getMenuType() const;
        virtual void onCanBack(bbool _canBack) {};
        virtual void onCanValidate(bbool _canValidate) {};
        virtual void onCanAction(bbool _canValidate) {};

        static Vec2d getInputDirection(StringID _input);

        f32 getOpenDuration() const { return m_openDuration; }
        f32 getAfxDuration() const {return m_afxDuration;}
        bbool isMenuType(MenuType _menuType) const {return (getMenuType() & _menuType)!=0;}
        bbool matchMenuType(MenuType _neededMask, MenuType _forbiddenMask) const {return ((_neededMask & getMenuType()) == _neededMask) &&  ((_forbiddenMask & getMenuType()) == 0);}
        void  addMenuTypeMask(MenuType _mask) {m_menuTypeMask = MenuType(m_menuTypeMask | _mask);}
        void  removeMenuTypeMask(MenuType _mask) {m_menuTypeMask = MenuType(m_menuTypeMask & (~_mask));}
        void  setMenuTypeMask(MenuType _mask, bbool _isSet) {_isSet ? addMenuTypeMask(_mask) : removeMenuTypeMask(_mask);}
        void setCanBack(bbool _canBack);
        void setCanValidate(bbool _canValidate);
        void setCanAction(bbool _canValidate);
        bbool getCanValidate() const {return m_canValidate;}
        bbool getCanAction() const {return m_canAction;}
        bbool getCanBack() const {return isMenuType(MenuType_WithBack) && m_canBack;}
        StringID getName() const {return m_name;}
        void setName(StringID _name) {m_name = _name;}
        void setListenControllerId(u32 _controllerId) {m_listenControllerId = _controllerId;}
        u32 getListenControllerId() const {return m_listenControllerId;}

		void setAnalogDir(const Vec2d inDir) { m_analogDir = inDir; }
		virtual Vec2d getAnalogDir() const { return m_lastAnalogDir; }

        StringID switchInputGet(StringID _from) const;
        void switchInputAdd(StringID _from, StringID _to);
        void switchInputClear() {m_switchedInput.clear();}
        bbool hasToLoadResources()  {return m_loadResources;}

    protected:

        const class UIMenu_Template* getTemplate() const;

		Vec2d m_analogDir, m_lastAnalogDir;
        MenuType m_menuType;
        MenuType m_menuTypeMask;
        f32 m_openDuration;
        f32 m_afxDuration;
        StringID m_name;
        u32 m_listenControllerId;
        bbool   m_loadResources;
        bbool   m_canBack;
        bbool   m_canValidate;
        bbool   m_canAction;

        ITF_MAP<StringID, StringID> m_switchedInput;
    };


    class UIMenu_Template : public UIItem_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenu_Template, UIItem_Template, UIMenu_Template_CRC)
    public:

        UIMenu_Template();
        ~UIMenu_Template();
    };

    ITF_INLINE const UIMenu_Template* UIMenu::getTemplate() const
    {
        return static_cast<const UIMenu_Template*>(m_template);
    }
}
#endif // _ITF_UIMENUITEMTEXT_H_
