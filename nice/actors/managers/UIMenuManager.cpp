#include "precompiled_engine.h"

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "gameplay/managers/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_ZINPUT_MANAGER_H
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUT_MANAGER_H

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_TOUCHSCREENADAPTER_WIN_H__
#include "adapters/TouchScreenAdapter_Win/TouchScreenAdapter_Win.h"
#endif // _ITF_TOUCHSCREENADAPTER_WIN_H__

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_UIMENUSOUNDCOMPONENT_H_
#include "engine/actors/components/UIMenuSoundComponent.h"
#endif //_ITF_UIMENUSOUNDCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#include "adapters/AudioSerializedData/SoundComponent_common.h"

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT_H_

#ifdef ITF_CAFE
#ifndef _ITF_INPUTADAPTER_CAFE_H_
#include "adapters/InputAdapter_Cafe/InputAdapter_Cafe.h"
#endif //_ITF_INPUTADAPTER_CAFE_H_
#endif //ITF_CAFE

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifdef ITF_PC
#ifndef _ITF_UIMENUBASIC_H_
#include "gameplay/components/UI/UIMenuBasic.h"
#endif //_ITF_UIMENUBASIC_H_
#endif //ITF_PC

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_CrossPromotionAdapter_H_
#include "engine/CrossPromotion/CrossPromotionAdapter.h"
#endif //_ITF_CrossPromotionAdapter_H_

#ifndef _ITF_W1W_GAMEPLAYEVENTS_H_
#include "W1W/Basic/W1W_GameplayEvents.h"
#endif //_ITF_W1W_GAMEPLAYEVENTS_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    setWorldEditable_editorUI UIMenuManager::s_editorCallback = NULL;

    UIMenuManager::UIMenuManager ()  
    : m_pauseCount(0)
    , m_commonMenuComponent(NULL)
    , m_commonSoundComponent(NULL)
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
    , m_padCursorSelectedItem(NULL)
    , m_disablePadCursor(bfalse)
    , m_hidePadCursor(bfalse)
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD
    , m_commonAfxCursor(0.0f)
    , m_currentAfxDuration(0.0f)
	, m_delayRefreshMaskTime(0)
    , m_template(NULL)
    , m_listenerRegistered(bfalse)
    , m_areMenuLoaded(bfalse)
    , m_screenTransitionDisplayTouchCanceled(bfalse)
#ifdef ITF_SUPPORT_EDITOR
    , m_aspectRatioChanged(bfalse)
#endif
#ifdef ITF_PC
    , m_mouseStatus(MouseStatus_Hidden)
    , m_forceHideCursor(bfalse)
#endif //ITF_PC
    {  
    }

    UIMenuManager::~UIMenuManager ()
    {  
		if(m_listenerRegistered)
		{
            if (ZINPUTMANAGER)
			    ZINPUTMANAGER->RemoveListener(this);

			EVENTMANAGER->unregisterEvent(EventDisablePadCursor_CRC, this);


#ifdef ITF_USE_TOUCHSCREEN
            if (INPUT_ADAPTER)
            {
#ifdef ITF_PC
                if (!CONFIG->m_touchScreenActivated)
                    INPUT_ADAPTER->removeListener(this);
#endif //ITF_PC
                TOUCHSURFACESMANAGER.removeListener(this, TouchSurfacesManager::Environment_UI);
#if defined(ITF_SUPPORT_CHEAT) && !defined(ITF_FINAL)
                TOUCHSURFACESMANAGER.removeListener(&m_debugMenu, TouchSurfacesManager::Environment_UI);
#endif
            }
#endif

			m_listenerRegistered=bfalse;
		}

		ITF_VECTOR<DeviceState*>::iterator iter = m_deviceStates.begin();
		ITF_VECTOR<DeviceState*>::iterator iterEnd = m_deviceStates.end();
		for(; iter != iterEnd; ++iter)
		{
			SF_DEL((*iter));
		}

        if(m_resContainer.getId()!=U32_INVALID)
        {
            m_resContainer.unloadResources();
            m_resContainer.clear();
            RESOURCE_MANAGER->unregisterResourceContainer(&m_resContainer);
        }
		
        if(m_template)
        {
            TEMPLATEDATABASE->releaseTemplate( m_template->getFile());
        }
    }

    void UIMenuManager::init()
    {
        Path isg = GETPATH_ALIAS("menuconfig");
        if(!isg.isEmpty())
        {
            m_template = TEMPLATEDATABASE->requestTemplate<UIMenuManager_Template>( isg );
        }

        RESOURCE_MANAGER->registerResourceContainer(&m_resContainer,"MenuManager");
        m_resContainer.loadResources();
    }

	///////////////////////////////////////////////////////////////////////////////////////////


	void UIMenuManager::Receive( u32 deviceID, f32 axis, const StringID& action )
	{
		
		if(action == input_actionID_Valid)
		{
			m_deviceStates[deviceID]->setValidatePressed(btrue);
		}
		else if(action == input_actionID_Valid_Release)
		{
			m_deviceStates[deviceID]->setValidatePressed(bfalse);
		}

        if( m_disablePadCursor )
        {
            return;
        }
        
        if( CROSSPROMOTION->isWebNewsVisible() )
        {
            return;
        }
        
#ifdef INPUT_USE_VKEYBOARD
		if(	INPUT_ADAPTER->VKeyboardGetCurrentState() == InputAdapter::VK_state_open )
		{
			return ;
		}
#endif //INPUT_USE_VKEYBOARD

		if(m_deviceStates[deviceID] == NULL || !m_deviceStates[deviceID]->isConnected())
		{
			return;
		}

#if defined(ITF_SUPPORT_CHEAT) && !defined(ITF_FINAL)
        if( m_debugMenu.getIsInDBGMenu() )
        {
            return; 
        }
#endif //ITF_SUPPORT_CHEAT

        if(GAMEMANAGER->getCurrentPauseLevel() > GamePauseLevel_MENU)
            return;

        //  [8/2/2013 rferrier]
        // seems useless for us, and today this lock the main 1st menu
// #ifndef ITF_SUPPORT_EDITOR
//          if ( GAMEMANAGER->getLocalPlayerIdFromControllerId( deviceID ) == U32_INVALID && !TRC_ADAPTER->isDisplayingError())
//              return;
// #endif
		

		// analog direction axis to vector, 
		Vec2d dir = Vec2d::Zero;
		if(action == input_actionID_DownHold || action == input_actionID_UpHold)
		{
			dir.y() = axis;
		}
		else if(action == input_actionID_LeftHold || action == input_actionID_RightHold)
		{
			dir.x() = axis;
		}

		// dpad fix:
		if(action == input_actionID_DownHold && axis > 0.0f)
			dir.y() = -1.0f;
		else if(action == input_actionID_UpHold && axis < 0.0f)
			dir.y() = 1.0f;
		else if(action == input_actionID_LeftHold && axis > 0.0f)
			dir.x() = -1.0f;
		else if(action == input_actionID_RightHold && axis < 0.0f)
			dir.x() = 1.0f;

        bool _isNavigationAction = isNavigationAction(action);

        if(_isNavigationAction)
        {
            m_deviceStates[deviceID]->receive(axis, action);
        }

        if( UIMenu* topMenu = getTopUIMenu(UIMenu::MenuType_None, UIMenu::MenuType_DoNotListenPad))
        {
            u32 listenControllerId = getValidListenerControllerId(topMenu);
            if(topMenu->getOpenDuration()>0.4f && (listenControllerId==U32_INVALID || listenControllerId==deviceID))
            {
                if (!_isNavigationAction)
                    topMenu->onInput(deviceID, axis, topMenu->switchInputGet(action));
                if (dir != Vec2d::Zero)
                    topMenu->setAnalogDir(dir);
            }
        }
	}

	void UIMenuManager::Receive(const TouchDataArray& _touchArray)
	{

		// Do not listen if the keyboard is active
#ifdef INPUT_USE_VKEYBOARD
		if(	INPUT_ADAPTER->VKeyboardGetCurrentState() == InputAdapter::VK_state_open )
		{
			return ;
		}
#endif //INPUT_USE_VKEYBOARD

		if(_touchArray.size() > 0)
		{
            GamePauseLevel curPauseLevel = GAMEMANAGER->getCurrentPauseLevel();
            bbool transitionDisplayed = GFX_ADAPTER->getScreenTransitionManager().isDisplaying();

            bbool check = bfalse;
#ifdef ITF_CAFE
            if ( !TOUCHSCREEN_ADAPTER->isEnvironmentActive( TouchScreenAdapter::Environment_UI ) && ( curPauseLevel == GamePauseLevel_TRC ) )
            {
                check = btrue;
            }
#endif //ITF_CAFE

            if ( transitionDisplayed )
            {
                if ( !m_screenTransitionDisplayTouchCanceled )
                {
                    cancelTouchInput();
                }
                m_screenTransitionDisplayTouchCanceled = btrue;
            }
            else
            {
                m_screenTransitionDisplayTouchCanceled = bfalse;
            }

            if( !transitionDisplayed && ( ( curPauseLevel <= GamePauseLevel_MENU) || ( curPauseLevel == GamePauseLevel_MAX ) || check ) )
            {
                for (ITF_LIST<UIMenu*>::const_reverse_iterator iter = m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend(); ++iter)
                {
                    UIMenu* menu = *iter;
                    if (menu->matchMenuType(UIMenu::MenuType_None, UIMenu::MenuType_DoNotListenTouch))
                    {
                        u32 listenControllerId = getValidListenerControllerId(menu);
						
#ifdef _ITF_TOUCH_SCREEN_MULTIPLE_TOUCH_BY_FRAME_
                        for( u32 i = 0; i < _touchArray.size(); i++ )
#else
                        const u32 i = 0;
#endif
                        {
                            const TouchData& data = _touchArray[i];
                            if(listenControllerId == U32_INVALID || listenControllerId == data.controllerId)
                            {
                                if(m_commonMenuComponent)
                                {
                                    Vec2d position = data.getCurrentMainViewPos();

                                    if(UIItem* curItem = (UIItem*)m_commonMenuComponent->getChildComponentFromPosition(position, UI_STATE_Item, UI_STATE_Hidden | UI_STATE_CanNotBeSelected))
                                    {
                                        if(data.state == TouchData::End && (curItem->getFriendlyID()==UIMenuManager_MenuCommonHud_ItemBack_ID || curItem->getFriendlyID()==UIMenuManager_MenuCommonHud_ItemExit_ID))
                                        {
                                            menu->onInput(data.controllerId, 0.0f, menu->switchInputGet(m_template->m_defaultBackInput));
                                        }
                                    }
                                }

#ifdef _ITF_TOUCH_SCREEN_MULTIPLE_TOUCH_BY_FRAME_
							    menu->onTouchInput(_touchArray, i);
#else//_ITF_TOUCH_SCREEN_MULTIPLE_TOUCH_BY_FRAME_
                                menu->onTouchInput(_touchArray);
#endif//_ITF_TOUCH_SCREEN_MULTIPLE_TOUCH_BY_FRAME_
                            }
					    }

                        if(!menu->isMenuType(UIMenu::MenuType_DoNotLockLowerMenuInputListening))
                        {
                            break;
                        }
                    }
                }
            }
		}
	}

    bbool UIMenuManager::isActionListenable(const StringID& action, u32& deviceID) const
    {
        deviceID = U32_INVALID;

#ifdef INPUT_USE_VKEYBOARD
        if(INPUT_ADAPTER->VKeyboardGetCurrentState() == InputAdapter::VK_state_open)
            return bfalse;
#endif // INPUT_USE_VKEYBOARD

#if defined(ITF_SUPPORT_CHEAT) && !defined(ITF_FINAL)
        if( m_debugMenu.getIsInDBGMenu() )
            return bfalse; 
#endif // ITF_SUPPORT_CHEAT

        if(GAMEMANAGER->getCurrentPauseLevel() > GamePauseLevel_MENU)
            return bfalse;

        if(m_deviceStates[deviceID] == NULL || !m_deviceStates[deviceID]->isConnected())
            return bfalse;

        if(isNavigationAction(action))
        {
            deviceID = 0;
            if(m_deviceStates[deviceID] && !m_deviceStates[deviceID]->hasAction(action))
                return bfalse;
        }
        else
        {
            UIMenu* topMenu = getTopUIMenu(UIMenu::MenuType_None, UIMenu::MenuType_DoNotListenPad);
            if(!topMenu || (topMenu->getOpenDuration() <= 0.4f))
                return bfalse;
            deviceID = getValidListenerControllerId(topMenu);
            if(deviceID == U32_INVALID)
                deviceID = 0;
        }

#ifndef ITF_SUPPORT_EDITOR
        if((GAMEMANAGER->getLocalPlayerIdFromControllerId(deviceID) == U32_INVALID) && !TRC_ADAPTER->isDisplayingError())
            return bfalse;
#endif // ITF_SUPPORT_EDITOR

        return btrue;
    }

	bbool UIMenuManager::isValidateButtonPressed(const i32 _controllerID) const
	{
		return m_deviceStates[_controllerID]->isValidatePressed();
	}

    void UIMenuManager::resetMenuListenerId(u32 _controllerID)
    {
        for (ITF_LIST<UIMenu*>::iterator iter = m_stackedUIComponents.begin(); iter!=m_stackedUIComponents.end(); ++iter)
        {
            u32 listenControllerId = (*iter)->getListenControllerId();
            if (listenControllerId != U32_INVALID)
                (*iter)->setListenControllerId(_controllerID);
        }
    }

#ifdef ITF_PC
    bbool UIMenuManager::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status)
    {
        if (!SYSTEM_ADAPTER->isCursorEnabled())
        {
            if (m_mouseStatus == MouseStatus_Available)
            {
                SYSTEM_ADAPTER->enableCursor(btrue);
                m_mouseStatus = MouseStatus_Shown;
            }
            return btrue;
        }

        GamePauseLevel curPauseLevel = GAMEMANAGER->getCurrentPauseLevel();

        if (!GFX_ADAPTER->getScreenTransitionManager().isDisplaying() && 
            (curPauseLevel <= GamePauseLevel_MENU || curPauseLevel == GamePauseLevel_MAX))
        {
            u32 currentMainControllerId = GAMEMANAGER->getMainIndexPlayerControllerId();
            i32 x, y;
            INPUT_ADAPTER->getMousePos(x, y);
            Vec2d mousePos = Vec2d((f32)x, (f32)y);

            for (ITF_LIST<UIMenu*>::const_reverse_iterator iter = m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend(); ++iter)
            {
                UIMenu* menu = *iter;
                if (menu->matchMenuType(UIMenu::MenuType_None, UIMenu::MenuType_DoNotListenTouch))
                {
                    u32 listenControllerId = getValidListenerControllerId(menu);

                    if(listenControllerId == U32_INVALID || listenControllerId == currentMainControllerId)
                    {
                        if(m_commonMenuComponent)
                        {
                            if(UIItem* curItem = (UIItem*)m_commonMenuComponent->getChildComponentFromPosition(mousePos, UI_STATE_Item, UI_STATE_Hidden | UI_STATE_CanNotBeSelected))
                            {
                                if(_status == InputAdapter::Released && (curItem->getFriendlyID()==UIMenuManager_MenuCommonHud_ItemBack_ID || curItem->getFriendlyID()==UIMenuManager_MenuCommonHud_ItemExit_ID))
                                {
                                    menu->onInput(currentMainControllerId, 0.0f, menu->switchInputGet(m_template->m_defaultBackInput));
                                }
                            }
                        }
                        menu->onMouseButton(mousePos, _but, _status);
                    }

                    if(!menu->isMenuType(UIMenu::MenuType_DoNotLockLowerMenuInputListening))
                        break;
                }
            }
        }

        return btrue;
    }

    bbool UIMenuManager::onMousePos(i32 _x, i32 _y)
    {
        if (!SYSTEM_ADAPTER->isCursorEnabled())
        {
            if (m_mouseStatus == MouseStatus_Available)
            {
                SYSTEM_ADAPTER->enableCursor(btrue);
                m_mouseStatus = MouseStatus_Shown;
            }
            return btrue;
        }

        GamePauseLevel curPauseLevel = GAMEMANAGER->getCurrentPauseLevel();

        if (!GFX_ADAPTER->getScreenTransitionManager().isDisplaying() &&
            (curPauseLevel <= GamePauseLevel_MENU || curPauseLevel == GamePauseLevel_MAX))
        {
            u32 currentMainControllerId = GAMEMANAGER->getMainIndexPlayerControllerId();
            Vec2d mousePos = Vec2d((f32)_x, (f32)_y);

            for (ITF_LIST<UIMenu*>::const_reverse_iterator iter = m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend(); ++iter)
            {
                UIMenu* menu = *iter;
                if (menu->matchMenuType(UIMenu::MenuType_None, UIMenu::MenuType_DoNotListenTouch))
                {
                    u32 listenControllerId = getValidListenerControllerId(menu);

                    if(listenControllerId == U32_INVALID || listenControllerId == currentMainControllerId)
                    {
                        menu->onMousePos(mousePos);
                    }

                    if(!menu->isMenuType(UIMenu::MenuType_DoNotLockLowerMenuInputListening))
                        break;
                }
            }
        }

        return btrue;
    }
#endif //ITF_PC

    ///////////////////////////////////////////////////////////////////////////////////////////

    void UIMenuManager::update(f32 _deltaTime)
    {   
        updateMenuLoading();

		if(!m_listenerRegistered)
		{
			ZINPUTMANAGER->AddListener(this, ZInputManager::Cat_Menu);
			EVENTMANAGER_REGISTER_EVENT_LISTENER(EventDisablePadCursor_CRC, this);

#ifdef ITF_PC
            if (!CONFIG->m_touchScreenActivated)
                INPUT_ADAPTER->addListener(this, 0);
#endif //ITF_PC
			m_listenerRegistered = btrue;


#ifdef ITF_USE_TOUCHSCREEN
			TOUCHSURFACESMANAGER.addListener(this, ObjectRef::InvalidRef, 0,TouchSurfacesManager::Environment_UI);
#if defined(ITF_SUPPORT_CHEAT) && !defined(ITF_FINAL)            
            TOUCHSURFACESMANAGER.addListener(&m_debugMenu, ObjectRef::InvalidRef, 0,TouchSurfacesManager::Environment_UI);
#endif
#endif
			m_deviceStates.resize(ZINPUTMANAGER->getDeviceCount());
            for (u32 deviceId = 0; deviceId < m_deviceStates.size(); ++deviceId)
            {
                m_deviceStates[deviceId] = newAlloc(mId_UI, DeviceState);
                m_deviceStates[deviceId]->m_deviceId = deviceId;
            }
		}

		if(!INPUT_ADAPTER)
			return;

#ifdef ITF_PC
        if (GAMEMANAGER->isInPause() || !TRC_ADAPTER->isPlayableScreen())
        {
            if (m_mouseStatus == MouseStatus_Hidden)
                m_mouseStatus = SYSTEM_ADAPTER->isCursorEnabled() ? MouseStatus_Shown : MouseStatus_Available;

            if (m_mouseStatus == MouseStatus_Shown)
                SYSTEM_ADAPTER->enableCursor(!m_forceHideCursor);
        }
        else
        {
            if (SYSTEM_ADAPTER->isCursorEnabled())
            {
                SYSTEM_ADAPTER->enableCursor(bfalse);
                m_mouseStatus = MouseStatus_Hidden;
            }
        }
#endif //ITF_PC

#if defined(ITF_IOS) || defined(ITF_ANDROID)
        // No pad on those devices !
#else
		if(m_deviceStates.size() < 1)
			return;
#endif
        checkUIComponentState();
        updateDisplay();
        updateCommon(_deltaTime);
		updateDevices(_deltaTime);

#if defined(ITF_SUPPORT_CHEAT) && !defined(ITF_FINAL)
        m_debugMenu.update(_deltaTime);
#endif // ITF_SUPPORT_CHEAT

		// update delay timer 
		if(m_delayRefreshMaskTime > 0)
		{
			m_delayRefreshMaskTime -= _deltaTime;
			if ( m_delayRefreshMaskTime < 0 )
			{
				m_delayRefreshMaskTime = 0;
				refreshMask();
			}
		}
    } 

	void UIMenuManager::updateDevices( f32 deltaTime )
	{
        if (UIMenu* topMenu = getTopUIMenu(UIMenu::MenuType_None, UIMenu::MenuType_DoNotListenPad))
        {
            for (u32 deviceId = 0; deviceId < m_deviceStates.size(); ++deviceId)
            {
                if (DeviceState* curDevice = m_deviceStates[deviceId])
                {
                    if (ZINPUTMANAGER->isDeviceValid(deviceId))
                    {
                        curDevice->setIsConnected(btrue);
                        curDevice->update(deltaTime, topMenu);
                    }
                    else
                    {
                        if (curDevice->isConnected())
                            curDevice->reset();

                        curDevice->setIsConnected(bfalse);
                    }
                }
            }
        }
	}

	ITF::bbool UIMenuManager::isNavigationAction( const StringID& _inAction ) const
	{
		if(	_inAction == input_actionID_Down ||
			_inAction == input_actionID_DownHold ||
			_inAction == input_actionID_Up ||
			_inAction == input_actionID_UpHold ||
			_inAction == input_actionID_Left ||
			_inAction == input_actionID_LeftHold ||
			_inAction == input_actionID_Right ||
			_inAction == input_actionID_RightHold)
		{
			return btrue;
		}

		return bfalse;
	}

    void UIMenuManager::loadMenu(UIMenu *_component)
    {
        if( m_resContainer.getId() == U32_INVALID )
            return;
        
        if (_component->hasToLoadResources())
        {
            const World *menuWorld = _component->getWorld();
            if(menuWorld)
            {
                const Scene *rootScene = menuWorld->getRootScene();
                if(rootScene)
                {
                    ITF_VECTOR<Actor *> actors;
                    ITF_VECTOR<Frise *> frises;
                    rootScene->getContainedPickables(actors, frises, btrue);

                    for(ITF_VECTOR<Actor *>::const_iterator it = actors.begin(); it != actors.end(); ++it)
                        m_resContainer.addChild((*it)->getResourceContainer());

                    for(ITF_VECTOR<Frise *>::const_iterator it = frises.begin(); it != frises.end(); ++it)
                        m_resContainer.addChild((*it)->getResourceContainer());
                }
            }
        }
    }

    void UIMenuManager::unloadMenu(UIMenu *_component)
    {
        if( m_resContainer.getId() == U32_INVALID )
            return;

        const World *menuWorld = _component->getWorld();
        if(menuWorld)
        {
            const Scene *rootScene = menuWorld->getRootScene();
            if(rootScene)
            {
                ITF_VECTOR<Actor *> actors;
                ITF_VECTOR<Frise *> frises;
                rootScene->getContainedPickables(actors, frises, btrue);

                for(ITF_VECTOR<Actor *>::const_iterator it = actors.begin(); it != actors.end(); ++it)
                {
                    const ResourceContainer *child = (*it)->getResourceContainer();
                    if(m_resContainer.hasChild(child))
                        m_resContainer.removeChild(child);
                }

                for(ITF_VECTOR<Frise *>::const_iterator it = frises.begin(); it != frises.end(); ++it)
                {
                    const ResourceContainer *child = (*it)->getResourceContainer();
                    if(m_resContainer.hasChild(child))
                        m_resContainer.removeChild(child);
                }
            }
        }
    }



    void UIMenuManager::updateDisplay()
    {
        refreshMask();

        for(ITF_LIST<UIMenu*>::iterator iter=m_currentUIComponents.begin(); iter!=m_currentUIComponents.end();)
        {
            UIMenu *menu = (*iter);
            if(menu->m_wantDisplay)
            {
                // Active world if ready
                World * world = menu->GetActor()->getWorld();
                if (world && isMenu(world) && !world->isSceneAsyncLoadRunning())
                {
                    if(menu->hasUIState(UI_STATE_IsMasked))
                        world->setActive(bfalse);
                    else
                    {
                        if (world->getLockResources())
                        {
                            // Resources always loaded. Don't wait for the physical ready
                            if (!world->isActive())
                            {
                                world->setActive(btrue);
                            }
                        }
                        else
                        {
                            // Wait for the resources to be loaded.
                            // TODO: use isFrustrumPhysicalReady instead
                            if(!world->isActive())
                            {
                                world->setAlwaysActiveContents(btrue);
                                world->setActive(btrue);
                                world->setAllowUpdate(bfalse);
                            }

                            if(!world->getAllowUpdate() &&world->isPhysicalReady())
                            {
                                world->setAllowUpdate(btrue);
                            }
                        }
                    }
                }

                ++iter;
            }
            else
            {
                // Collapse menu
                for(ITF_LIST<UIMenu*>::iterator iter2=m_stackedUIComponents.begin(); iter2!=m_stackedUIComponents.end(); ++iter2)
                {
                    if(*iter2 == menu)
                    {
                        //i32 depthOffset = (*iter)->getAbsoluteDepthMax() - (*iter)->getAbsoluteDepth() + 1;
                        iter2 = m_stackedUIComponents.erase(iter2);
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
						updateStackedChildObjectsList();
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD
                        //for(ITF_LIST<UIMenu*>::iterator iter3=iter2; iter3!=m_stackedUIComponents.end();++iter3)
                        //{
                        //    (*iter3)->setAbsoluteDepth((*iter3)->getAbsoluteDepth() - depthOffset);
                        //}

                        break;
                    }
                }

                // Inactive world
                World * world = (*iter)->GetActor()->getWorld();
                bbool mustInactiveWorld = btrue;
                for(ITF_LIST<UIMenu*>::iterator iter2=m_currentUIComponents.begin(); iter2!=m_currentUIComponents.end(); ++iter2)
                {
                    if(menu != (*iter2) && (*iter2)->GetActor()->getWorld()==world)
                    {
                        mustInactiveWorld = bfalse;
                        break;
                    }
                }   

                if(mustInactiveWorld && world && world->isActive() && !world->isSceneAsyncLoadRunning() && isMenu(world))
                {
                    world->setActive(bfalse);
                    world->setAlwaysActiveContents(bfalse);
                }
                iter = m_currentUIComponents.erase(iter);
            }
        }
    } 

    void UIMenuManager::refreshMask()
    {
		// Added to avoid a fast hide & show. When we want to display 2 trc messages, we can see during few frames the bottom menus... 
		if(m_delayRefreshMaskTime != 0)		
			return ;		

        i32 mainMaskDepth = 0;
        UIMenu* mainTopMenu = NULL;
        for(ITF_LIST<UIMenu*>::reverse_iterator iter=m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend();++iter)
        {
            UIMenu* menu = *iter;

            if((menu->getUIDisplayMask() & View::MASKID_MAINONLY)!=0 && menu->m_wantDisplay)
            {
                mainTopMenu = menu;
                mainMaskDepth = menu->getAbsoluteDepth();
                break;
            }
        }

        for(ITF_LIST<UIMenu*>::iterator iter=m_currentUIComponents.begin(); iter!=m_currentUIComponents.end();++iter)
        {
            UIMenu* menu = *iter;

            if((menu->getUIDisplayMask() & View::MASKID_MAINONLY)!=0)
            {
                menu->setIsMasked(menu->getAbsoluteDepth()<=mainMaskDepth && mainTopMenu!=menu);
            }
        }

        if(useRemoteUI())
        {
            i32 remoteMaskDepth = 0;
            UIMenu* remoteTopMenu = NULL;
            for(ITF_LIST<UIMenu*>::reverse_iterator iter=m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend();++iter)
            {
                UIMenu* menu = *iter;

                if((menu->getUIDisplayMask() & View::MASKID_REMOTEONLY)!=0)
                {
                    remoteTopMenu = menu;
                    remoteMaskDepth = menu->getAbsoluteDepth();
                    break;
                }
            }

            for(ITF_LIST<UIMenu*>::iterator iter=m_currentUIComponents.begin(); iter!=m_currentUIComponents.end();++iter)
            {
                UIMenu* menu = *iter;

                if((menu->getUIDisplayMask() & View::MASKID_REMOTEONLY)!=0)
                {
                    menu->setIsMasked(menu->getAbsoluteDepth()<=remoteMaskDepth && remoteTopMenu!=menu);
                }
            }
        }

        EventUiMask uiMaskEvent( mainMaskDepth>0);
        EVENTMANAGER->broadcastEvent(&uiMaskEvent);
    }


    SoundPlayingID UIMenuManager::playSound(StringID _sound, StringID _soundDrc, Actor * _actor)
    {
        View::MaskIds displayMask = (View::MaskIds)(_actor->getWorldUpdateElement()->getViewMask());

        if(SoundComponent* soundComponent = _actor->GetComponent<SoundComponent>())
        {
            if (displayMask == View::MASKID_REMOTE)
            {
                if (_soundDrc.isValid())
                    return soundComponent->playSound(_soundDrc);
            }

            return soundComponent->playSound(_sound);
        }
        else if(m_commonSoundComponent)
        {
            if (displayMask == View::MASKID_REMOTE)
            {
                if (_soundDrc.isValid())
                    return m_commonSoundComponent->playSound(_soundDrc);
            }

            return m_commonSoundComponent->playSound(_sound);
        }

        return SoundPlayingID::getInvalidHandle();
    }
    

    void UIMenuManager::stopSound(SoundPlayingID _handle)
    {
        if(m_commonSoundComponent)
        {
            m_commonSoundComponent->stopSound(_handle);
        }
    }

    void UIMenuManager::pausePlayers(bbool _pause)
    {
        if(_pause)
        {
            if(m_pauseCount==0)
            {
                ZINPUTMANAGER->setInputMode(ZInputManager::Cat_GamePlay, bfalse);
                TOUCHSURFACESMANAGER.setEnvironmentActive(bfalse, TouchSurfacesManager::Environment_Gameplay);
            }
            m_pauseCount++;
        }
        else
        {
            m_pauseCount--;
            if(m_pauseCount==0)
            {
                TOUCHSURFACESMANAGER.setEnvironmentActive(btrue, TouchSurfacesManager::Environment_Gameplay);
                ZINPUTMANAGER->setInputMode(ZInputManager::Cat_GamePlay, btrue);
            }
        }
    }

    void UIMenuManager::cancelTouchInput()
    {
        // fixed RO2-38119 + fixed RO2-38121
        for (ITF_LIST<UIMenu*>::const_reverse_iterator iter = m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend(); ++iter)
        {
            UIMenu* menu = *iter;
            if (menu->matchMenuType(UIMenu::MenuType_None, UIMenu::MenuType_DoNotListenTouch))
            {
                TouchDataArray touchArray;
                TouchData touchData;
                touchData.controllerId = menu->getListenControllerId();
                touchData.state = TouchData::End;
                // HACK : pos in screen space out of screen to make sure we don't send the touch info on a button in the UI
                // http://mdc-tomcat-jira40.ubisoft.org/jira/browse/RO2-38334
                touchData.coordinates = Vec2d(-1000.0f, -1000.0f);
                touchArray.push_back(touchData);
                menu->onTouchInput(touchArray);

                if(!menu->isMenuType(UIMenu::MenuType_DoNotLockLowerMenuInputListening))
                {
                    break;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    ITF::bbool UIMenuManager::isDisplayUIMenu( const StringID _friendlyID ) const
    {
        return isDisplayUIMenu(getUIMenu(_friendlyID));
    }

    ITF::bbool UIMenuManager::isDisplayUIMenu( UIMenu* _component ) const
    {
        if(_component)
        {
            for(ITF_LIST<UIMenu*>::const_iterator iter=m_currentUIComponents.begin(); iter!=m_currentUIComponents.end();++iter)
            {
                if((*iter) == _component)
                {
                    return btrue;
                }
            }
        }

        return bfalse;
    }

    UIMenu* UIMenuManager::prefetchUIMenu(const StringID _friendlyID)
    {
        UIMenu* component = getUIMenu(_friendlyID);

        ITF_WARNING_CATEGORY(GPP, NULL, component, "You try to prefetch a not loaded UIMenu %s", _friendlyID.getDebugString());

        if(component)
        {
            LOG("[UI] Prefetch '%s'", _friendlyID.getDebugString());
            return prefetchUIMenu(component);
        }

        return NULL;
    }

    UIMenu* UIMenuManager::showUIMenu(const StringID _friendlyID)
    {
        UIMenu* component = getUIMenu(_friendlyID);

        ITF_WARNING_CATEGORY(GPP, NULL, component, "You try to show a not loaded UIMenu %s", _friendlyID.getDebugString());
        
        if(component)
        {
            LOG("[UI] Show '%s'", _friendlyID.getDebugString());
            return showUIMenu(component);
        }
        
        return NULL;
    }

    UIMenu* UIMenuManager::hideUIMenu(const StringID _friendlyID)
    {
        UIMenu* component = getUIMenu(_friendlyID);

        if(component)
        {
            return hideUIMenu(component);
        }
        
        return NULL;
    }

    UIMenu* UIMenuManager::prefetchUIMenu(UIMenu* _component)
    {
        ITF_ASSERT_CRASH(_component, "You try to prefetch an invalid UIMenu");

        bbool isAlreadyShowed = isDisplayUIMenu(_component);

        if(!isAlreadyShowed)
        {
            _component->setIsDisplay(bfalse);
            if (World* world = _component->getWorld())
            {
                world->setAlwaysActiveContents(btrue);
                world->setActive(btrue);
                world->setAllowUpdate(bfalse);
            }
        }

        return (UIMenu*) _component;
    }

    UIMenu* UIMenuManager::showUIMenu(UIMenu* _component)
    {
        ITF_ASSERT_CRASH(_component, "You try to show an invalid UIMenu");

        bbool isAlreadyShowed = isDisplayUIMenu(_component);

        _component->setIsDisplay(btrue);

		if(!isAlreadyShowed)
        {
            cancelTouchInput();

            if( _component->isMenuType(UIMenu::MenuType_InputListener))
            {
                insertUIMenu(_component->getAbsoluteDepth(), _component);
            }

            m_currentUIComponents.push_back(_component);

            refreshMask();
        }

        return (UIMenu*) _component;
    }

    bbool UIMenuManager::isPhotoModeAllowed() const
    {
        bbool modeAllowed = btrue;

        for(ITF_LIST<UIMenu*>::const_iterator it = m_currentUIComponents.begin(), itEnd = m_currentUIComponents.end(); it != itEnd; it++)
        {
            UIMenu * menu = * it;

            if(menu)
            {
                if( ! menu->isMenuType(UIMenu::MenuType_JustDisplay) )
                {
                    modeAllowed = bfalse;
                }  
            }        
        }

        return modeAllowed;
    }

    UIMenu* UIMenuManager::hideUIMenu(UIMenu* _component)
    {
        ITF_ASSERT_CRASH(_component, "Try to hide an invalid UIMenu");

        _component->setIsDisplay(bfalse);

        return NULL;
    }

    void UIMenuManager::clearUIMenu()
    {
        for(ITF_LIST<UIMenu*>::iterator iter=m_currentUIComponents.begin(); iter!=m_currentUIComponents.end();++iter)
        {
            (*iter)->setIsDisplay(bfalse);
        }
    }

    i32 UIMenuManager::getHighestDepth() const
    {
        i32 depth = 5;

        UIMenu* topMenu = getTopUIMenu();

        if(topMenu)
        {
            depth = topMenu->getAbsoluteDepthMax() + 1;
        }

        return depth;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void UIMenuManager::registerUIMenu(UIMenu* _component)
    {
        ITF_ASSERT_CRASH( ThreadSettings::getCurrentThreadId() == eThreadId_mainThread, "Can only be called from main thread" );
        m_registeredUIComponents.push_back(_component);

#ifdef ITF_W1W_MOBILE
		updatePositionsFor_3_2_AspactRatio(_component);
#endif //ITF_W1W_MOBILE

        if(isMenu(_component->GetActor()->getWorld()))
        {
            if(_component->getFriendlyID() == UIMenuManager_MenuCommonHud_ID)
            {
                m_commonMenuComponent = _component;
                m_commonSoundComponent = _component->GetActor()->GetComponent<SoundComponent>();
            }

            if(const UIMenuManager_Template::MenuInfo* menuInfo = m_template->getMenuInfo(_component))
            {
                _component->setAbsoluteDepth(menuInfo->m_depth);
            }
        }

        loadMenu(_component);
    }

    void UIMenuManager::unregisterUIMenu(UIMenu* _component)
    {
        if(_component->getFriendlyID() == UIMenuManager_MenuCommonHud_ID && isMenu(_component->GetActor()->getWorld()))
        {
            m_commonMenuComponent = NULL;
            m_commonSoundComponent = NULL;
        }

        i32 index = m_registeredUIComponents.find(_component);

        if(index!=-1)
        {
            m_registeredUIComponents.removeAtUnordered(index);
        }

        m_currentUIComponents.remove(_component);
        m_stackedUIComponents.remove(_component);

#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
		updateStackedChildObjectsList();
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

        unloadMenu(_component);
    }

    UIMenu* UIMenuManager::getUIMenu(const StringID _friendlyID) const
    {
		UIMenu* component = NULL;
        for (ITF_VECTOR<UIMenu*>::const_iterator iter = m_registeredUIComponents.begin(); iter!=m_registeredUIComponents.end(); ++iter)
        {
            if ((*iter)->getFriendlyID() == _friendlyID)
            {
                component =*iter;
				break;
            }
        }

#ifdef ITF_W1W_MOBILE	
		if(component)
		{
			if(UIMenu* component_16_9 =get_16_9_UIMenu(component))
			{
				component = component_16_9;
			}
		}
#endif //ITF_W1W_MOBILE

        return component;
    }

#ifdef ITF_W1W_MOBILE
    UIMenu* UIMenuManager::get_16_9_UIMenu(UIMenu* _component) const
    {
		if(GAMEMANAGER->is_16_9_Version())
		{
			if(const UIMenuManager_Template::MenuInfo* menuInfo = m_template->getMenuInfo(_component))
			{
				if(menuInfo->m_use_16_9_Version)
				{
					for (ITF_VECTOR<UIMenu*>::const_iterator iter = m_registeredUIComponents.begin(); iter!=m_registeredUIComponents.end(); ++iter)
					{
						 World * world = (*iter)->GetActor()->getWorld();
						if (world && (world->getPath()==menuInfo->m_path_16_9))
						{
							return *iter;
						}
					}
				}
			}
		}

        return NULL;
    }

    void UIMenuManager::updatePositionsFor_3_2_AspactRatio(UIMenu* _component)
    {
		if(GAMEMANAGER->is_16_9_Version())
		{
			if(strstr(_component->GetActor()->getUserFriendly().cStr(), "16_9"))
			{
				const f32 ratio_3_2 = 1.5f; //exemple 480 / 320 or 960 / 640;
				const f32 screenRatio = (f32) GFX_ADAPTER->getScreenWidth() / (f32) GFX_ADAPTER->getScreenHeight();
				if (screenRatio == ratio_3_2)
				{
					//_component->GetActor()->set2DPos(_component->GetActor()->get2DPos() + Vec2d(0.f,45.f));
					const PickableList & pickList = _component->GetActor()->getScene()->getPickableList();
					for (PickableList::const_iterator it = pickList.begin();
						it != pickList.end(); it++)
					{
						if( (!((Actor*)(*it))->getParentBind()) )
						{
							(*it)->set2DPos((*it)->get2DPos() + Vec2d(0.f,45.f));
						}
					}
				}
			}
		}
	}
#endif //ITF_W1W_MOBILE

    UIMenu* UIMenuManager::getPrevUIMenu(UIMenu* _component) const
    {
        UIMenu* prevComponent = NULL;

        for (ITF_LIST<UIMenu*>::const_iterator iter = m_stackedUIComponents.begin(); iter!=m_stackedUIComponents.end(); ++iter)
        {
            if ((*iter) == _component)
            {
                return prevComponent;
            }
            else
            {
                prevComponent = *iter;
            }
        }

        return prevComponent;
    }

    UIMenu* UIMenuManager::getTopUIMenu(UIMenu::MenuType _neededMask, UIMenu::MenuType _forbiddenMask) const
    {
        for (ITF_LIST<UIMenu*>::const_reverse_iterator iter = m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend(); ++iter)
        {
            if ((*iter)->matchMenuType(_neededMask, _forbiddenMask))
            {
                return *iter;
            }
        }

        return NULL;
    }

#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD

    ObjectRefList& UIMenuManager::getStackedChildObjectsList() 
    {
		return m_stackedChildObjectsList;
    }

    void UIMenuManager::updateStackedChildObjectsList() 
    {
		m_stackedChildObjectsList.clear();
        for (ITF_LIST<UIMenu*>::const_reverse_iterator iter = m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend(); ++iter)
        {
			const ObjectRefList& componentObjectList = (*iter)->getChildObjectsList();
			for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
			{
				m_stackedChildObjectsList.push_back(*iter);
			}
        }
    }

    void UIMenuManager::padCursorTouchInput()
    {
		if(!m_padCursorSelectedItem)
			return;

        TouchDataArray touchArray;
        TouchData touchData;
        touchData.controllerId = 0;
		touchData.coordinates = m_padCursorSelectedItem->GetActor()->get2DPos();

		touchData.state = TouchData::Begin;
        touchArray.push_back(touchData);
		TOUCHSURFACESMANAGER.dispatchToEnvironment(0,touchArray);

		touchData.state = TouchData::End;
		touchArray.clear();
        touchArray.push_back(touchData);
		TOUCHSURFACESMANAGER.dispatchToEnvironment(0,touchArray);

		/*
        for (ITF_LIST<UIMenu*>::const_reverse_iterator iter = m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend(); ++iter)
        {
            UIMenu* menu = *iter;
            if (menu->matchMenuType(UIMenu::MenuType_None, UIMenu::MenuType_DoNotListenTouch))
            {
                TouchDataArray touchArray;
                TouchData touchData;
                touchData.controllerId = menu->getListenControllerId();
				touchData.coordinates = m_padCursorSelectedItem->GetActor()->get2DPos();

				touchData.state = TouchData::Begin;
                touchArray.push_back(touchData);
                menu->onTouchInput(touchArray);

				touchData.state = TouchData::End;
				touchArray.clear();
                touchArray.push_back(touchData);
                menu->onTouchInput(touchArray);

                if(!menu->isMenuType(UIMenu::MenuType_DoNotLockLowerMenuInputListening))
                {
                    break;
                }
            }
        }
			*/
    }

#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

	void UIMenuManager::insertUIMenu( UIMenu* _parentMenu, UIMenu* _inMenu )
	{
        for (ITF_LIST<UIMenu*>::iterator iter = m_stackedUIComponents.begin(); iter!=m_stackedUIComponents.end(); ++iter)
        {
            if ((*iter) == _parentMenu)
            {
                m_stackedUIComponents.insert_after(iter, _inMenu);
                break;
            }
        }

#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
		updateStackedChildObjectsList();
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD
	}

    void UIMenuManager::insertUIMenu( i32 _depth, UIMenu* _inMenu )
    {
        ITF_LIST<UIMenu*>::iterator beforeIt = m_stackedUIComponents.end();
        for (ITF_LIST<UIMenu*>::iterator iter = m_stackedUIComponents.begin(); iter!=m_stackedUIComponents.end(); ++iter)
        {
            if ((*iter)->getAbsoluteDepth()<= _depth)
            {
                beforeIt = iter;
            }
            else
            {
                break;
            }
        }

        if(beforeIt==m_stackedUIComponents.end())
        {
            m_stackedUIComponents.push_front(_inMenu);
        }
        else
        {
            m_stackedUIComponents.insert_after(beforeIt, _inMenu);
        }

#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
		updateStackedChildObjectsList();
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

    }

	void UIMenuManager::removeUIMenu( UIMenu* _inMenu )
	{
        for (ITF_LIST<UIMenu*>::iterator iter = m_stackedUIComponents.begin(); iter!=m_stackedUIComponents.end(); ++iter)
        {
            if ((*iter) == _inMenu)
            {
                m_stackedUIComponents.erase(iter);
                break;
            }
        }

#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
		updateStackedChildObjectsList();
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

	}

    void UIMenuManager::checkUIComponentState() 
    {
        for (ITF_VECTOR<UIMenu*>::iterator iter = m_registeredUIComponents.begin(); iter!=m_registeredUIComponents.end(); ++iter)
        {
            World * world = (*iter)->GetActor()->getWorld();
            if(world && world->isActive() && (*iter)->m_wantDisplay && ((*iter)->isMenuType(UIMenu::MenuType_InputListener)))
            {
                bbool isAlreadyDisplayed = bfalse;
                for (ITF_LIST<UIMenu*>::iterator iter2 = m_stackedUIComponents.begin(); iter2!=m_stackedUIComponents.end(); ++iter2)
                {
                    if ((*iter) == (*iter2))
                    {
                        isAlreadyDisplayed = btrue;
                        break;
                    }
                }

                if(!isAlreadyDisplayed)
                {
                    ITF_LIST<UIMenu*>::iterator lastIter = m_stackedUIComponents.begin();
                    for (ITF_LIST<UIMenu*>::iterator iter2 = m_stackedUIComponents.begin(); iter2!=m_stackedUIComponents.end(); ++iter2)
                    {
                        if ((*iter)->getAbsoluteDepth() < (*iter2)->getAbsoluteDepth())
                        {
                            break;
                        }
                        else
                        {
                            lastIter = iter2;
                        }
                    }


                    if(lastIter!=m_stackedUIComponents.end())
                    {
                        m_stackedUIComponents.insert_after(lastIter, (*iter));
                    }
                    else
                    {
                        m_stackedUIComponents.push_back((*iter));
                    }

#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
					updateStackedChildObjectsList();
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

                    m_currentUIComponents.push_back((*iter));
                    (*iter)->setIsDisplay(btrue);
                }
            }
        }
    }

    u32 UIMenuManager::controllerIndexToBackSpriteIndex(u32 _controllerIndex)
    {
        if(_controllerIndex!=U32_INVALID)
        {
            InputAdapter::PadType padType = INPUT_ADAPTER->getPadType(_controllerIndex);
            switch(padType)
            {
#ifdef ITF_CAFE_DRC
            case InputAdapter::Pad_CafeDRC:
                return 5;
#endif
            case InputAdapter::Pad_WiiSideWay:
                return 22;
            case InputAdapter::Pad_WiiClassic:
                #ifdef ITF_CAFE_DRC
                if(INPUT_ADAPTER_CAFE->isURCC(_controllerIndex))
                {
                    return 5;
                }
                #endif
                return 6;
            case InputAdapter::Pad_WiiNunchuk:
                return 7;
            }
        }
        
        return 5;
    }

	void UIMenuManager::updateCommon(f32 _dt)
	{
		if(m_commonMenuComponent)
		{
			showUIMenu(m_commonMenuComponent);

            UIMenu* topMenu = getTopUIMenu();
			UIItem* activeItem = NULL;
			UIItem* inactiveItem = NULL;
			UIItem* backItem = (UIItem*) m_commonMenuComponent->getChildComponent(UIMenuManager_MenuCommonHud_ItemBack_ID);
			UIItem* exitItem = (UIItem*) m_commonMenuComponent->getChildComponent(UIMenuManager_MenuCommonHud_ItemExit_ID);
            UIComponent* uiAfx = m_commonMenuComponent->getChildComponent(UIMenuManager_MenuCommonHud_UiAfx_ID);
            UIComponent* uiBackground = m_commonMenuComponent->getChildComponent(UIMenuManager_MenuCommonHud_CommonBackground_ID);

#ifdef ITF_USE_REMOTEVIEW
            if(topMenu && (topMenu->getUIDisplayMask() & View::MASKID_REMOTE)!=0)
            {
                TOUCHSCREEN_ADAPTER->setOverrideTolerance(1);
            }
            else
            {
                TOUCHSCREEN_ADAPTER->setOverrideTolerance(U32_INVALID);
            }
#endif //ITF_USE_REMOTEVIEW

            if(uiBackground)
            {
                i32 backgroundDepth = 0;
                View::MaskIds backgroundMask = View::MASKID_NONE;
                View::MaskIds colisionMask = View::MASKID_NONE;
                if(m_stackedUIComponents.size()>1)
                {
                    for (ITF_LIST<UIMenu*>::reverse_iterator iter = m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend(); ++iter)
                    {
                        UIMenu* menu = *iter;

                        if(menu->m_wantDisplay && !menu->hasUIState(UI_STATE_IsMasked))
                        {
                            View::MaskIds displayMask = menu->getUIDisplayMask();

                            if((colisionMask & displayMask & ( View::MASKID_MAINONLY| View::MASKID_REMOTEONLY)) != 0)
                            {
                                if( backgroundDepth == 0)
                                {
                                    backgroundDepth = menu->getAbsoluteDepthMax() + 1;
                                }

                                backgroundMask = View::MaskIds(backgroundMask | (colisionMask & displayMask));
                            }

                            colisionMask = View::MaskIds(colisionMask | displayMask);
                        }
                    }
                }


                if(backgroundDepth==0)
                {
                    uiBackground->setIsDisplay(bfalse);
                }
                else
                {
                    uiBackground->setAbsoluteDepth(backgroundDepth);
                    uiBackground->setUIDisplayMask(backgroundMask);
                    uiBackground->setIsDisplay(btrue);
                }
            }

            if(uiAfx)
            {
                if(!uiAfx->GetActor()->getIs2D())
                {
                    //Vec3d pos = (Vec2d((f32) GFX_ADAPTER->getScreenWidth(), (f32) GFX_ADAPTER->getScreenHeight()) * 0.5f).to3d(0);
					const Camera *cam =View::getMainView()->getCamera();
                    Vec3d pos = cam->getPos();
					pos += cam->getLookAtDir() * 10.0f;
					if(pos.isValid())
					{
						uiAfx->setAbsolutePosition(pos.truncateTo2D());
						uiAfx->GetActor()->setDepth(pos.z());
					}
                }
                

                bbool needAfx = bfalse;
                for (ITF_LIST<UIMenu*>::const_reverse_iterator iter = m_stackedUIComponents.rbegin(); iter!=m_stackedUIComponents.rend(); ++iter)
                {
                    const UIMenu* menu = *iter;
                    if (menu->matchMenuType(UIMenu::MenuType_WithAfx, UIMenu::MenuType_None) && !(*iter)->hasUIState(UI_STATE_Hiding))
                    {
                        if(!needAfx)
                        {
                            m_currentAfxDuration = menu->getAfxDuration();
                            needAfx = btrue;
                        }
                        else                        
                        {
                            m_currentAfxDuration = f32_Max(m_currentAfxDuration, menu->getAfxDuration());
                        }
                        break;
                    }
                }

                if(needAfx)
                {
                    if(m_currentAfxDuration > 0)
                        m_commonAfxCursor += _dt / m_currentAfxDuration;
                    else
                        m_commonAfxCursor = 1;
                }
                else if(!GAMEMANAGER->areFadeOutInProgress() || !GAMEMANAGER->isInPause())
                {
                    if(m_currentAfxDuration > 0)
                        m_commonAfxCursor -= _dt / m_currentAfxDuration;
                    else 
                        m_commonAfxCursor = 0;
                }

                m_commonAfxCursor = f32_Clamp(m_commonAfxCursor, 0.0f, 1.0f);
                if(m_commonAfxCursor>0.0f)
                {
                    EventSetFloatInput factor;
                    factor.setInputValue(sin(m_commonAfxCursor * MTH_PIBY2));
                    factor.setInputName(ITF_GET_STRINGID_CRC(factor, 521039410));
                    uiAfx->GetActor()->onEvent(&factor);

                    EventSetFloatInput color;
                    color.setInputValue(1.0f);
                    color.setInputName(ITF_GET_STRINGID_CRC(color, 2531223376));
                    uiAfx->GetActor()->onEvent(&color);

                    uiAfx->setIsDisplay(btrue);
                }
                else
                {
                    uiAfx->setIsDisplay(bfalse);
                }
            }

			if(backItem && exitItem)
			{
				if(topMenu && topMenu->isMenuType(UIMenu::MenuType_BackIsExit))
				{
					activeItem = exitItem;
					inactiveItem = backItem;
				}
				else
				{
					activeItem = backItem;
					inactiveItem = exitItem;
				}

				if( topMenu && topMenu->GetActor()->getWorld()->getAllowUpdate() && topMenu->getIsDisplay())
				{
					//activeItem->setUseTransition(topMenu->getUseTransition());
                    activeItem->setUseTransition(btrue);
					activeItem->setTransitionMode(UI_TRANSITION_LeftToRight);
					
					bbool shouldDisplay = topMenu->getCanBack() && !topMenu->isMenuType(UIMenu::MenuType_HideBack);

					if(shouldDisplay)
					{
						inactiveItem->setIsDisplay(bfalse);
					}
					activeItem->setIsDisplay(shouldDisplay);
					activeItem->setIsLocked(topMenu->getIsLocked());
                    activeItem->setUIDisplayMask( topMenu->getUIDisplayMask());

#ifdef ITF_CAFE
                    if(UITextBox* textBox = activeItem->GetActor()->GetComponent<UITextBox>())
                    {
                        if(textBox->getTextArea().getParsedActors().size()>0)
                        {
                            if(Actor* actorIcon = textBox->getTextArea().getParsedActors()[0].getActor())
                            {
                                if(!actorIcon->isAsyncLoading())
                                {
                                    if(TextureGraphicComponent* texture = actorIcon->GetComponent<TextureGraphicComponent>())
                                    {
                                        u32 spriteIndex = controllerIndexToBackSpriteIndex(getValidListenerControllerId(topMenu));
                                        texture->setNewSpriteIndex(spriteIndex);
                                    }
                                }
                            }
                        }
                    }
#endif

				}
				else
				{
					backItem->setIsDisplay(bfalse);
					backItem->setIsLocked(bfalse);
					exitItem->setIsDisplay(bfalse);
					exitItem->setIsLocked(bfalse);
				}
			}
		}
	}

    u32 UIMenuManager::getValidListenerControllerId( UIMenu* _menu ) const
    {
        if(!_menu)
            return U32_INVALID;

        u32 listenControllerId = _menu->getListenControllerId();
        if(listenControllerId != U32_INVALID)
        {
            // give owner ship to main player only if the pad is disconnected 
            if(!ZINPUTMANAGER->isDeviceValid(listenControllerId))
                listenControllerId = GAMEMANAGER->getMainIndexPlayerControllerId();
        }

        return listenControllerId;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::loadMenus()
    {
        if(m_template)
        {
            u32 menuPathSize = m_template->m_menuInfos.size();
            ITF_LANGUAGE language = LOCALISATIONMANAGER->getCurrentLanguage();
            ITF_REGION region = SYSTEM_ADAPTER->getSystemRegion();
            
            m_worldMenu.reserve( menuPathSize );
            for (u32 i = 0; i < menuPathSize; i++)
            {
                const UIMenuManager_Template::MenuInfo &menuInfo = m_template->m_menuInfos[i];
                if(language==ITF_LANGUAGE_JAPANESE && menuInfo.m_flags & UIMenuManager_Template::FLAG_LanguageJapaneseExcept )
                    continue;

                if(language!=ITF_LANGUAGE_JAPANESE && menuInfo.m_flags & UIMenuManager_Template::FLAG_LanguageJapaneseOnly )
                    continue;

                if(region != ITF_REGION_USA && menuInfo.m_flags & UIMenuManager_Template::FLAG_RegionUSAOnly)
                    continue;

                LoadWorldInfo loadInfoMenu;
                loadInfoMenu.m_mapPath = menuInfo.m_path;
                if (menuInfo.m_flags & UIMenuManager_Template::FLAG_AlwaysLoaded)
                    loadInfoMenu.m_lockResources = btrue;

                World* worldMenu = WORLD_MANAGER->createAndLoadWorld(loadInfoMenu);

                m_worldMenu.push_back(worldMenu);

#ifdef ITF_SUPPORT_EDITOR
                if (s_editorCallback && (menuInfo.m_flags & UIMenuManager_Template::FLAG_ShowInEditor))
                    s_editorCallback(worldMenu, btrue);
#endif //ITF_SUPPORT_EDITOR
            }
        }

        updateMenuLoading();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::unloadMenus()
    {
        for (u32 i=0; i<m_worldMenu.size(); i++)
        {
            World *world = m_worldMenu[i];
            ITF_ASSERT(world);
            if (world)
            {
#ifdef ITF_SUPPORT_EDITOR
                if (s_editorCallback)
                    s_editorCallback(world, bfalse);
#endif //ITF_SUPPORT_EDITOR

                WORLD_MANAGER->deleteWorld(world);
            }
        }
        m_worldMenu.clear();
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool UIMenuManager::isMenu(World* _world) const
    {
        for (u32 i=0; i<m_worldMenu.size(); i++)
        {
            World *world = m_worldMenu[i];
            ITF_ASSERT(world);
            if (world == _world)
            {
                return btrue;
            }
        }

        return bfalse;
    }

    void UIMenuManager::updateMenuLoading()
    {
        u32 count = m_worldMenu.size();

        for ( u32 i = 0; i < m_worldMenu.size(); ++i )
        {
            World* pWorldMenu = static_cast< World* >( m_worldMenu[i] );

            ITF_ASSERT( pWorldMenu != NULL );
            if ( pWorldMenu != NULL)
            {
                ITF_ASSERT_CRASH(!pWorldMenu->isBeingDestroyed(), "Trying to detect repro for RO2-3463");
                if(pWorldMenu->isPhysicalReady() )
                    count -= 1;
            }
        }

        if ( !m_worldMenu.empty() && !m_areMenuLoaded && !count )
        {
            m_areMenuLoaded = btrue;
        }
    }

    bbool UIMenuManager::useRemoteUI() const
    {
#ifdef ITF_USE_REMOTEVIEW
		
		return !m_template ? bfalse : m_template->m_useRemoteUI;		
#else
        return bfalse;
#endif
    }

	//////////////////////////////////////////////////////////////////////////

	const f32 UIMenuManager::InputState::g_inputDelayBase = 0.5f;
	const f32 UIMenuManager::InputState::g_inputDelayDecrease = 0.1f;
	const f32 UIMenuManager::InputState::g_inputResetTime = -10.0f;

	UIMenuManager::DeviceState::DeviceState()
		: m_currentAxis(0.0f)
		, m_lastAxis(0.0f)
		, m_isConnected(bfalse)
        , m_deviceId(0)
		, m_isValidatePressed(bfalse)
	{
		registerAction(input_actionID_DownHold);
		registerAction(input_actionID_UpHold);
		registerAction(input_actionID_LeftHold);
		registerAction(input_actionID_RightHold);
		registerAction(input_actionID_Valid);
		registerAction(input_actionID_Back);
		registerAction(input_actionID_Start);
	}

	UIMenuManager::DeviceState::~DeviceState()
	{
		ITF_MAP<StringID, InputState*>::iterator iterEnd = m_inputState.end();
		ITF_MAP<StringID, InputState*>::iterator iterIS = m_inputState.begin();
		for(; iterIS!=iterEnd; ++iterIS)
		{
			SF_DEL((*iterIS).second);
		}
	}

	void UIMenuManager::DeviceState::update( f32 deltaTime, UIMenu* topMenu )
	{
		// for each Action 
		ITF_MAP<StringID, InputState*>::iterator iter = m_inputState.begin();
		ITF_MAP<StringID, InputState*>::iterator itEnd = m_inputState.end();
		for(;iter!=itEnd; ++iter)
		{
			StringID    action      = (*iter).first;
			InputState* actionState = (*iter).second;

			if(actionState == NULL || !isConnected())
			{
				continue;
			}

			if(actionState->m_currentUsed == btrue)
			{
				// if the delay is over
				if(actionState->m_stateDuration < 0.0f)
				{
					actionState->m_stateDuration = actionState->m_inputDelay;
					actionState->m_inputDelay = f32_Max(InputState::g_inputDelayDecrease, f32(actionState->m_inputDelay - InputState::g_inputDelayDecrease));

					// Fire Action to menu
					StringID fixedAction = StringID::Invalid;
					testRebound(actionState, action, fixedAction);

                    if(topMenu)
                    {
                        u32 listenControllerId = UI_MENUMANAGER->getValidListenerControllerId(topMenu);
                        UIMenu::MenuType forbiddenFlag = UIMenu::MenuType(UIMenu::MenuType_DoNotListenPad | UIMenu::MenuType_DoNotListenNavigation);

                        if(fixedAction != StringID::Invalid && actionState->m_usedDuration<topMenu->getOpenDuration() && (listenControllerId==U32_INVALID || listenControllerId==m_deviceId) && topMenu->matchMenuType(UIMenu::MenuType_None, forbiddenFlag))
                        {
                            topMenu->onInput(m_deviceId, m_currentAxis, topMenu->switchInputGet(fixedAction));
                        }
                    }
                }
			}

			// if this action is on delay, decrease the waiting time.
			if(actionState->m_stateDuration > -1.0f)
			{
				actionState->m_stateDuration -= deltaTime;
			}

			// checking action release
			bbool lastDown = (m_lastAxis != 0.f);
			bbool curDown = (m_currentAxis != 0.f);
			if(lastDown != curDown && curDown == bfalse)
			{
				actionState->m_inputDelay = InputState::g_inputDelayBase;
				actionState->m_stateDuration = InputState::g_inputResetTime;
			}

            if(actionState->m_currentUsed)
            {
                actionState->m_usedDuration += deltaTime;
            }
            else
            {
                actionState->m_usedDuration = 0.0f;
            }

            actionState->m_oldUsed = actionState->m_currentUsed;
			actionState->m_currentUsed = bfalse;
		}

		m_lastAxis = m_currentAxis;
		m_currentAxis = 0.0f;
	}

	void UIMenuManager::DeviceState::registerAction( StringID _inAction )
	{
		if(!m_inputState[_inAction])
		{
			m_inputState[_inAction] = newAlloc(mId_UI, InputState);
		}
		else
		{
			ITF_ASSERT_MSG(m_inputState[_inAction] != NULL, "Action already registred!");
		}
	}

	void UIMenuManager::DeviceState::testRebound( InputState* _inState, StringID& _inAction, StringID& _outAction )
	{
		StringID oppositeAction = StringID::Invalid;
		if(_inAction == input_actionID_Down)
			oppositeAction = input_actionID_Up;
		else if(_inAction == input_actionID_Up)
			oppositeAction = input_actionID_Down;
		else if(_inAction == input_actionID_Left)
			oppositeAction = input_actionID_Right;
		else if(_inAction == input_actionID_Right)
			oppositeAction = input_actionID_Left;
		else if(_inAction == input_actionID_DownHold)
			oppositeAction = input_actionID_UpHold;
		else if(_inAction == input_actionID_UpHold)
			oppositeAction = input_actionID_DownHold;
		else if(_inAction == input_actionID_LeftHold)
			oppositeAction = input_actionID_RightHold;
		else if(_inAction == input_actionID_RightHold)
			oppositeAction = input_actionID_LeftHold;

		// default result: there is no opposite action.
		_outAction = _inAction;

		if(oppositeAction == StringID::Invalid)
		{
			// no valid opposite action for this action.
			return;
		}

		ITF_MAP<StringID, InputState*>::iterator iterEnd = m_inputState.end();
		ITF_MAP<StringID, InputState*>::iterator invIter = m_inputState.find(oppositeAction);

		if(invIter == iterEnd)
		{
			// opposite action not found in m_inputState!
			return;
		}

		InputState* oppositeState = (*invIter).second;
		ITF_ASSERT_CRASH(oppositeState, "Invalid menu input action");

		// Switched from normal to opposite
		if(oppositeState->m_oldUsed)
		{
			_outAction = StringID::Invalid;
		}
	}

	void UIMenuManager::DeviceState::receive(f32 axis, const StringID& action )
	{
		if(m_inputState[action])
		{
			m_currentAxis = axis;
			m_inputState[action]->m_currentUsed = (axis != 0.f);
		}
	}

	void UIMenuManager::DeviceState::reset()
	{
		m_isConnected = bfalse;
		
		ITF_MAP<StringID, InputState*>::iterator iter = m_inputState.begin();
		ITF_MAP<StringID, InputState*>::iterator itEnd = m_inputState.end();
		for(;iter!=itEnd; ++iter)
		{
			InputState* state = (*iter).second;

            if(!state)
                continue;
            
            state->m_usedDuration = 0.0f;
			state->m_currentUsed = bfalse;
			state->m_inputDelay = InputState::g_inputDelayBase;
			state->m_oldUsed = bfalse;
			state->m_stateDuration = InputState::g_inputResetTime;
		}
	}


    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIMenuManager::onEvent(Event* _event)
    {
        if (EventMobileCallbacks* eventMobileCallbacks = DYNAMIC_CAST(_event,EventMobileCallbacks))
        {
			if(eventMobileCallbacks->getEvent()==EventMobileCallbacks::eBackgroundEvent)
			{
				m_disablePadCursor = btrue;
			}
			else
			{
				m_disablePadCursor = bfalse;
			}
        }
        else if (EventDisablePadCursor* eventDisablePadCursor = DYNAMIC_CAST(_event, EventDisablePadCursor))
        {
			if(eventDisablePadCursor->getIsDisabled())
			{
				m_disablePadCursor = btrue;
			}
			else
			{
				m_disablePadCursor = bfalse;
			}
        }	
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    BEGIN_SERIALIZATION_SUBCLASS(UIMenuManager_Template,MenuInfo)
        SERIALIZE_MEMBER("path",m_path);
        SERIALIZE_MEMBER("depth",m_depth);
        SERIALIZE_MEMBER("flags",m_flags);
        SERIALIZE_MEMBER("use_16_9_Version",m_use_16_9_Version);
        SERIALIZE_MEMBER("path_16_9",m_path_16_9);
	END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(UIMenuManager_Template)
	BEGIN_SERIALIZATION_CHILD(UIMenuManager_Template)
        SERIALIZE_CONTAINER_OBJECT("menuInfos",m_menuInfos); 
        SERIALIZE_MEMBER("useRemoteUI",m_useRemoteUI); 
		SERIALIZE_MEMBER("defaultValidInput",m_defaultValidInput); 
		SERIALIZE_MEMBER("defaultBackInput",m_defaultBackInput);
    END_SERIALIZATION()

    const UIMenuManager_Template::MenuInfo* UIMenuManager_Template::getMenuInfo(UIMenu* _menu) const
    {
        if(World* world = _menu->GetActor()->getWorld())
        {
            const Path& path = world->getPath();
            for(ITF_VECTOR<MenuInfo>::const_iterator it =m_menuInfos.begin(); it!=m_menuInfos.end(); ++it)
            {
                const MenuInfo& menuInfo = *it;
                if(menuInfo.m_path == path)
                {
                    return &menuInfo;
                }
            }
        }

        return NULL;
    }
    
}
