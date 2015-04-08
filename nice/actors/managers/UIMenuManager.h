#ifndef _ITF_UIMENUMANAGER_H_
#define _ITF_UIMENUMANAGER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_
#ifndef _ITF_TOUCHSCREENADAPTER_H_
#include "engine/AdaptersInterfaces/TouchScreenAdapter.h"
#endif // _ITF_TOUCHSCREENADAPTER_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif //_LOCALISATION_ID_H_


#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

#ifndef _ITF_RESOURCE_CONTAINER_H_
#include "engine/resources/ResourceContainer.h"
#endif //_ITF_RESOURCE_CONTAINER_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifdef ITF_SUPPORT_CHEAT
#ifndef _ITF_UIDEBUGMENU_H_
#include "engine/actors/managers/UIDebugMenu.h"
#endif //_ITF_UIDEBUGMENU_H_
#endif
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"

#ifndef _ITF_EVENTLISTENER_H_
#include "engine/events/IEventListener.h"
#endif //_ITF_EVENTLISTENER_H_

namespace ITF
{
    class UIComponent;
    class UIMenu;
    class SoundComponent;
    class ResourceContainer;

#define UIMenuManager_CRC ITF_GET_STRINGID_CRC(UIMenuManager, 4291072416)
#define UIMenuManager_Template_CRC ITF_GET_STRINGID_CRC(UIMenuManager_Template, 2370785895)

// TODO : PUT IT IN A TEMPLATE
#define UIMenu_Sound_Move ITF_GET_STRINGID_CRC(Play_Menu_Butt_Selected_Gen, 893376831)
#define UIMenu_Sound_Validate ITF_GET_STRINGID_CRC(Play_Menu_Butt_Validate_Gen, 1191456473)
#define UIMenu_Sound_Back ITF_GET_STRINGID_CRC(Play_Menu_Butt_Validate_Back, 2549927402)

#define UIMenu_Sound_Check ITF_GET_STRINGID_CRC(Menu_CheckBox, 4131890553)
//#define UIMenu_Sound_EnterPause ITF_GET_STRINGID_CRC(Menu_Pause, 1699628770)
#define UIMenu_Sound_DRCMessage_Touch ITF_GET_STRINGID_CRC(MenuDRC_Touch,1734364511)
#define UIMenu_Sound_DRCMessage_Release ITF_GET_STRINGID_CRC(MenuDRC_release,4244452410)
#define UIMenu_Sound_DRCMessage_Snap ITF_GET_STRINGID_CRC(MenuDRC_Snap,1767081097)
#define UIMenu_Sound_DRC_Select_Friend ITF_GET_STRINGID_CRC(DRC_Select_Friend,2445343327)
#define UIMenu_Sound_DRC_Switch_IN ITF_GET_STRINGID_CRC(DRC_Switch_IN,220372398)
#define UIMenu_Sound_DRC_Switch_OUT ITF_GET_STRINGID_CRC(DRC_Switch_OUT,141881862)
#define UIMenu_Sound_DRC_Stat_IN ITF_GET_STRINGID_CRC(DRC_Stat_IN,4287485818)
#define UIMenu_Sound_DRC_Stat_OUT ITF_GET_STRINGID_CRC(DRC_Stat_OUT,2160364102)    

#define UIMenu_Sound_Move_DRC ITF_GET_STRINGID_CRC(Menu_Move_DRC,1429537934)
#define UIMenu_Sound_Validate_DRC ITF_GET_STRINGID_CRC(Menu_Validate_DRC,4119658540)
#define UIMenu_Sound_Back_DRC ITF_GET_STRINGID_CRC(Menu_Back_DRC,318173910)
#define UIMenu_Sound_Check_DRC ITF_GET_STRINGID_CRC(Menu_CheckBox_DRC,4056167278)
#define UIMenu_Sound_EnterPause_DRC ITF_GET_STRINGID_CRC(Menu_Pause_DRC,771747709)
#define UIMenu_Sound_DRCMessage_Touch_DRC ITF_GET_STRINGID_CRC(MenuDRC_Touch_DRC,1074414451)
#define UIMenu_Sound_DRCMessage_Release_DRC ITF_GET_STRINGID_CRC(MenuDRC_release_DRC,1399655852)
#define UIMenu_Sound_DRCMessage_Snap_DRC ITF_GET_STRINGID_CRC(MenuDRC_Snap_DRC,2628321440)
#define UIMenu_Sound_DRCMessage_Tease_DRC ITF_GET_STRINGID_CRC(Menu_Message_DRC,4179361580)
#define UIMenu_Sound_PromptReward_Appear ITF_GET_STRINGID_CRC(Menu_Prompt_Reward_Appear,3816525731)
#define UIMenu_Sound_MessageReward_Appear ITF_GET_STRINGID_CRC(IG_Reward_Appear,1866264524)
    
    

#define UIMenu_Sound_HUD_Appear_Little ITF_GET_STRINGID_CRC(HUD_Appear_Little, 4188561713)
#define UIMenu_Sound_HUD_Appear_Big ITF_GET_STRINGID_CRC(HUD_Appear_Big, 1200133390)
#define UIMenu_Sound_HUD_Beaten_Stomp ITF_GET_STRINGID_CRC(HUD_Beaten_Stomp, 2828799486)
#define UIMenu_Sound_MUS_Percu ITF_GET_STRINGID_CRC(MUS_Percu, 2299167461)

#define UIMenu_TRC_menuRatingEurope ITF_GET_STRINGID_CRC(menuGenericRatingEurope,2414613346)
#define UIMenu_TRC_menuRatingAmerica ITF_GET_STRINGID_CRC(menuGenericRatingAmerica,3811601665)
#define UIMenu_Generic_textTitle_ID ITF_GET_STRINGID_CRC(textGenericTitle, 3168443065)
#define UIMenu_Generic_textMessage_ID ITF_GET_STRINGID_CRC(textGenericMessage, 3287031059)
#define UIMenu_Generic_itemGenericButton1_ID ITF_GET_STRINGID_CRC(itemGenericButton1, 33399514)
#define UIMenu_Generic_itemGenericButton2_ID ITF_GET_STRINGID_CRC(itemGenericButton2, 1036335009)
#define UIMenu_Generic_itemGenericButton3_ID ITF_GET_STRINGID_CRC(itemGenericButton3, 3723558424)

#define UIMenuManager_MenuCommonHud_ID ITF_GET_STRINGID_CRC(menuCommonHud, 4216829525)  
#define UIMenuManager_MenuCommonHud_ItemBack_ID ITF_GET_STRINGID_CRC(itemCommonHudBack, 1182260903)
#define UIMenuManager_MenuCommonHud_ItemExit_ID ITF_GET_STRINGID_CRC(itemCommonHudExit, 1776223204)
#define UIMenuManager_MenuCommonHud_UiAfx_ID ITF_GET_STRINGID_CRC(uiAfx, 1001921928)
#define UIMenuManager_MenuCommonHud_CommonBackground_ID ITF_GET_STRINGID_CRC(backgroundCommon, 3822023743)
#define UIMenuManager_MenuCommonHud_NetwokText_ID ITF_GET_STRINGID_CRC(itemCommonnetworktext, 1741906083)
// TODO : END

    class UIMenuManager_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenuManager_Template, TemplateObj, UIMenuManager_Template_CRC );
        DECLARE_SERIALIZE()

    public :
        UIMenuManager_Template() : m_useRemoteUI(btrue){}
        ~UIMenuManager_Template(){}

        static const u32 FLAG_None = 0;
        static const u32 FLAG_LanguageJapaneseExcept = (1 << 0);
        static const u32 FLAG_LanguageJapaneseOnly = (1 << 1);
        static const u32 FLAG_ShowInEditor = (1 << 2);
        static const u32 FLAG_RegionUSAOnly = (1 << 3);
        static const u32 FLAG_AlwaysLoaded = (1 << 4);

        struct MenuInfo
        {
            DECLARE_SERIALIZE()

            MenuInfo() : m_depth(0), m_flags(FLAG_None){}

            Path m_path;
            i32  m_depth;
            u32  m_flags;
            bbool  m_use_16_9_Version;
            Path m_path_16_9;
        };


        const MenuInfo* getMenuInfo(UIMenu* _menu) const;

    
        ITF_VECTOR<MenuInfo>            m_menuInfos;
        bbool                           m_useRemoteUI;
        StringID                        m_defaultValidInput;
        StringID                        m_defaultBackInput;
    };

    
    typedef void (*setWorldEditable_editorUI) (World* _world, bbool _editable);

    class UIMenuManager : public IInputListener, public ITouchListener, public IEventListener
#ifdef ITF_PC
        , public Interface_InputListener
#endif //ITF_PC
    {

    public:

        UIMenuManager();
        ~UIMenuManager();

        void                        init();
        void                        update(f32 _deltaTime);
		void						onEvent( Event* _event );
        const UIMenuManager_Template* getTemplate() const {return m_template;}

        bbool                       isDisplayUIMenu(const StringID _friendlyID) const;
        bbool                       isDisplayUIMenu(UIMenu* _component) const;
        UIMenu*                     getUIMenu(const StringID _friendlyID) const;
        UIMenu*						getCommonMenu() const { return m_commonMenuComponent; }
        UIMenu*                     prefetchUIMenu(const StringID _friendly);
        UIMenu*                     prefetchUIMenu(UIMenu* _component);
        UIMenu*                     showUIMenu(const StringID _friendly);
        UIMenu*                     showUIMenu(UIMenu* _component);
        UIMenu*                     hideUIMenu(const StringID _friendly);
        UIMenu*                     hideUIMenu(UIMenu* _component);
        bbool                       isPhotoModeAllowed() const;
        UIMenu*                     getPrevUIMenu(UIMenu* _component) const;
        UIMenu*                     getTopUIMenu(UIMenu::MenuType _neededMask = UIMenu::MenuType_None, UIMenu::MenuType _forbiddenMask = UIMenu::MenuType_None) const;
		void						insertUIMenu(UIMenu* _parentMenu, UIMenu* _inMenu);
        void                        insertUIMenu( i32 _depth, UIMenu* _inMenu );
		void						removeUIMenu(UIMenu* _inMenu);
        i32                         getHighestDepth() const;
        void                        clearUIMenu();
        void                        cancelTouchInput();


        SoundPlayingID              playSound(StringID _sound, StringID _soundDrc, Actor * _actor);
        void                        stopSound(SoundPlayingID _handle);

        void						Receive(u32 deviceID, f32 axis, const StringID& action); // IInput
        void						Receive(const TouchDataArray & _touchArray); // ITouchInput

        void                        registerUIMenu(UIMenu* _component);
        void                        unregisterUIMenu(UIMenu* _component);

        void                        pausePlayers(bbool _pause);
        bbool                       arePlayersPaused() const { return m_pauseCount > 0; }

		void						setDelayNextRefreshMask(f64 _timeToDelay = 0.5) { m_delayRefreshMaskTime = _timeToDelay; }
        void                        loadMenus();
        void                        unloadMenus();
        bbool                       isMenu(World* _world) const;
        bbool                       areMenusLoaded() const { return m_areMenuLoaded; }
        bbool                       useRemoteUI() const;

        bbool                       isActionListenable(const StringID& action, u32& deviceID) const;
		bbool						isValidateButtonPressed(const i32 _controllerID) const;

        void                        resetMenuListenerId(u32 _controllerID);
        
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
		UIItem*						getPadCursorSelectedItem() {return m_padCursorSelectedItem;}
		void						setPadCursorSelectedItem(UIItem* _padCursorSelectedItem) {m_padCursorSelectedItem = _padCursorSelectedItem;}
		
		bbool						isPadCursorDisabled() {return m_disablePadCursor;}
		void						disablePadCursor(bbool _disablePadCursor) {m_disablePadCursor = _disablePadCursor;}

		bbool						isPadCursorHided() {return m_hidePadCursor;}
		void						hidePadCursor(bbool _hidePadCursor) {m_hidePadCursor = _hidePadCursor;}

		ObjectRefList&				getStackedChildObjectsList(); 
		void						updateStackedChildObjectsList();
		void						addChildObject(ObjectRef _objectRef){m_stackedChildObjectsList.push_back(_objectRef);}

        void                        padCursorTouchInput();
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

#ifdef ITF_W1W_MOBILE
        UIMenu*                     get_16_9_UIMenu(UIMenu* _component) const;
        void                        updatePositionsFor_3_2_AspactRatio(UIMenu* _component);
#endif //ITF_W1W_MOBILE

#ifdef ITF_PC
#ifdef ITF_SUPPORT_EDITOR
        virtual char*				interfaceOwnerName() { return "UIMenuManager"; }
#endif //ITF_SUPPORT_EDITOR

        virtual bbool               onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status);
        virtual bbool               onMousePos (i32 _x, i32 _y);
        virtual bbool               onMouseWheel (i32 _wheelValue, i32 _wheelDelta) { return btrue; }
        virtual bbool               onKey(i32 _key, InputAdapter::PressStatus _status) { return btrue; }
        virtual bbool               acceptMouseWheel(String8 *_desc = NULL) { return bfalse; }

        void                        forceHideCursor(bbool _forceHideCursor) { m_forceHideCursor = _forceHideCursor; }

    private:
        enum MouseStatus
        {
            MouseStatus_Hidden,
            MouseStatus_Available,
            MouseStatus_Shown
        };
        MouseStatus                 m_mouseStatus;
        bbool                       m_forceHideCursor;
#endif //ITF_PC
        
    protected:
        class InputState
        {
        public:
			InputState() 
			: m_stateDuration (g_inputResetTime)
			, m_inputDelay (g_inputDelayBase)
			, m_currentUsed (bfalse)
			, m_oldUsed (bfalse)
            , m_usedDuration(0.0f)
			{
				
			}

			// default states.
			static const f32 g_inputDelayBase;
			static const f32 g_inputDelayDecrease;
			static const f32 g_inputResetTime;		

            f32 m_stateDuration;	
            f32 m_inputDelay;		// how fast till the next action.
            f32 m_usedDuration;

			bbool m_currentUsed;	// is the action being used this frame
			bbool m_oldUsed;		// was the action being used last frame
        };

		class DeviceState
		{
		public:
			DeviceState();
			~DeviceState();

			void receive(f32 axis, const StringID& action);
			void reset();
			void update(f32 deltaTime, UIMenu* topMenu);
			void registerAction(StringID _inAction);
            bbool hasAction(const StringID& _inAction) const { return m_inputState[_inAction] != NULL; }
			void testRebound(InputState* _inState, StringID& _inAction, StringID& _outAction);

			void setValidatePressed(bbool _isPressed) { m_isValidatePressed = _isPressed; }
			bbool isValidatePressed() { return m_isValidatePressed; };

			ITF_INLINE void setIsConnected(bbool _connected) { m_isConnected = _connected; }
			ITF_INLINE bbool isConnected() const { return m_isConnected; }

            u32                             m_deviceId;

		private:

			ITF_MAP<StringID, InputState*>	m_inputState;
			f32								m_currentAxis;
			f32								m_lastAxis;
			bbool							m_isConnected;
			bbool							m_isValidatePressed;
            
		};

        u32                         getValidListenerControllerId(UIMenu* _menu) const;
		void						changeNumDevices(i32 _numDevices);
		void						updateDevices(f32 deltaTime);
        void                        updateDisplay();
        void                        checkUIComponentState();
        void                        updateCommon(f32 _dt);
		bbool						isNavigationAction(const StringID& _inAction) const;
        void                        refreshMask();

        void                        loadMenu(UIMenu *_component);
        void                        unloadMenu(UIMenu *_component);

        ITF_LIST<UIMenu*>           m_stackedUIComponents;
        ITF_LIST<UIMenu*>           m_currentUIComponents;
        ITF_VECTOR<UIMenu*>         m_registeredUIComponents;
		ITF_VECTOR<DeviceState*>	m_deviceStates;				// input state per device
		 
        u32                         controllerIndexToBackSpriteIndex(u32 _controllerIndex);
        UIMenu*                     m_commonMenuComponent;
        SoundComponent*             m_commonSoundComponent;
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
		UIItem*						m_padCursorSelectedItem;
		ObjectRefList				m_stackedChildObjectsList;
		bbool						m_disablePadCursor;
		bbool						m_hidePadCursor;
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

        f32                         m_commonAfxCursor;
        f32                         m_currentAfxDuration;

        i32                         m_pauseCount;
		bbool						m_listenerRegistered;
		f64							m_delayRefreshMaskTime;

        ResourceContainer           m_resContainer;
        const UIMenuManager_Template* m_template;

        void                        updateMenuLoading();
        ITF_VECTOR<World*>          m_worldMenu;
        bbool                       m_areMenuLoaded;
        bbool                       m_screenTransitionDisplayTouchCanceled;

#if defined(ITF_SUPPORT_CHEAT) && !defined(ITF_FINAL)
        public :
        UIDebugMenu                 &getDebugMenu() { return m_debugMenu; }
        bbool                       getIsInDBGMenu() const { return m_debugMenu.getIsInDBGMenu(); }
        void                        setIsInDBGMenu(bbool _isInDBGMenu, u32 _player) { m_debugMenu.setIsInDBGMenu(_isInDBGMenu, _player); }
        void                        addDebugMenu( DebugMenu* _cheat ) {m_debugMenu.addDebugMenu(_cheat); }
		void                        addDebugMenu_System() {m_debugMenu.addDebugMenu_System(); }
        void                        addDebugMenu_MapList() {m_debugMenu.addDebugMenu_MapList(); }

        private :
        UIDebugMenu m_debugMenu;
#endif // ITF_SUPPORT_CHEAT

	public:
#ifdef ITF_SUPPORT_EDITOR
        static void         setEditorCallback(setWorldEditable_editorUI _cb) { s_editorCallback = _cb; }

        bbool               m_aspectRatioChanged;
        bbool               getAspectRatioChanged()                          { return m_aspectRatioChanged; }
        void                setAspectRatioChanged(bbool aspectRatioChanged)  { m_aspectRatioChanged = aspectRatioChanged; }
#endif // ITF_SUPPORT_EDITOR

		ITF_INLINE ITF_VECTOR<UIMenu*>::const_iterator getFirstRegisteredUIMenu() { return m_registeredUIComponents.begin(); }
		ITF_INLINE ITF_VECTOR<UIMenu*>::const_iterator getLastRegisteredUIMenu() { return m_registeredUIComponents.end(); }

    private:
        static setWorldEditable_editorUI s_editorCallback;
  };

}
#endif // _ITF_UIMENUMANAGER_H_
