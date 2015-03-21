
#include "precompiled_gameplay.h"

#ifndef _ITF_UIMENUBASIC_H_
#include "gameplay/components/UI/UIMenuBasic.h"
#endif //_ITF_UIMENUBASIC_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif // _ITF_SCENE_H_

#ifndef _ITF_UIITEM_H_
#include "gameplay/components/UI/UIItem.h"
#endif //_ITF_UIITEM_H_

#ifndef _ITF_UIITEMDROPDOWN_H_
#include "gameplay/components/UI/UIItemDropdown.h"
#endif //_ITF_UIITEMDROPDOWN_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif

#include "engine/actors/managers/UIMenuManager.h"
#ifdef ITF_PROJECT_WW1
#include "W1W/Basic/W1W_GameManager.h"
#endif
namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIMenuBasic)
    BEGIN_SERIALIZATION_CHILD(UIMenuBasic)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("defaultItem", m_defaultItemFriendly);
        SERIALIZE_MEMBER("backItem", m_backItemFriendly);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    UIMenuBasic::UIMenuBasic()
    : m_validateItemControllerID(-1)
    , m_backItemControllerID(-1)
    , m_actionItemControllerID(-1)
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
    , m_lastPadCursorSelectedItem(NULL)
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD
#ifdef ITF_PC
    , m_isMouseLocked(bfalse)
#endif //ITF_PC
    {
    }

    
    UIMenuBasic::~UIMenuBasic()
    {    
    }

    void UIMenuBasic::onActorLoaded(Pickable::HotReloadType _hot)
    {
        Super::onActorLoaded(_hot);

        setValidateAllowedInput(UI_MENUMANAGER->getTemplate()->m_defaultValidInput, btrue);
        setBackAllowedInput(UI_MENUMANAGER->getTemplate()->m_defaultBackInput, btrue);
        setBackAllowedInput(input_actionID_Esc, btrue);

		setSlideAllowedInput(input_actionID_LeftHold, btrue);
		setSlideAllowedInput(input_actionID_RightHold, btrue);
		setSlideAllowedInput(input_actionID_onPressed, btrue);
		setSlideAllowedInput(input_actionID_onReleased, btrue);
		setSlideAllowedInput(input_actionID_LeftStickRelease, btrue);
    }

    void UIMenuBasic::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        if ( UIItem* backItem = getChildComponent<UIItem>(m_backItemFriendly) )
        {
            backItem->addUIState(UI_STATE_ActivateByBack);
        }

        onCanBack(getCanBack());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    UIItem* UIMenuBasic::computeNextItem(const Vec2d& _direction, i32 _neededState, i32 _forbiddenState) const
    {
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
        UIItem* currItem = UI_MENUMANAGER->getPadCursorSelectedItem();
#else
        UIItem* currItem = getSelectedItem();
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

        UIItem* nextItem = (UIItem*) getChildComponentToDirection(currItem, _direction, btrue, UI_STATE_Item | _neededState, UI_STATE_Hidden | UI_STATE_CanNotBeSelected | _forbiddenState);

        //if(nextItem == currItem)
        //{
        //    u32 itemsCount = getChildComponentCount(UI_STATE_Item | _neededState, UI_STATE_Hidden | UI_STATE_CanNotBeSelected | _forbiddenState);
        //    if(itemsCount>=2)
        //    {
        //        nextItem = (UIItem*) getChildComponentToDirection(currItem, _direction * -1.0f, bfalse, UI_STATE_Item | _neededState, UI_STATE_Hidden | UI_STATE_CanNotBeSelected | _forbiddenState);
        //    }
        //}

        return nextItem;
    }

    void UIMenuBasic::onInput(const i32 _controllerID, const f32 _axis, const StringID _input)
	{
		//Don't select next item when 'Validate' button is pressed.
		if (!UI_MENUMANAGER->isValidateButtonPressed(_controllerID))
		{
			Vec2d direction = getInputDirection(_input);

			if(direction != Vec2d::Zero)
			{
				UIItem* selectedItem = computeNextItem(direction);
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
				if(selectedItem && (selectedItem != UI_MENUMANAGER->getPadCursorSelectedItem()))
				{
					BEGIN_MOBILE_USE_GAMEPAD
						UI_MENUMANAGER->setPadCursorSelectedItem(selectedItem);
						m_lastPadCursorSelectedItem = selectedItem;
						UI_MENUMANAGER->playSound(UIMenu_Sound_Move,UIMenu_Sound_Move_DRC,m_actor);
					END_MOBILE_USE_GAMEPAD					
				}
#else
				if(selectedItem && (selectedItem->GetActor()->getRef() !=m_selectedItem))
				{
					select(selectedItem);
					UI_MENUMANAGER->playSound(UIMenu_Sound_Move,UIMenu_Sound_Move_DRC,m_actor);
				}
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

			}
		}

		if (m_validateAllowedInput.find(_input)!=U32_INVALID)
		{
			BEGIN_MOBILE_USE_GAMEPAD
				UI_MENUMANAGER->padCursorTouchInput();
			ELSE_MOBILE_USE_GAMEPAD
				valide(_controllerID, _input);
			END_MOBILE_USE_GAMEPAD
		}
		else if (m_backAllowedInput.find(_input)!=U32_INVALID)
		{
			back(_controllerID, _input);
		}
		else if (m_actionAllowedInput.find(_input)!=U32_INVALID)
		{
			action(_controllerID, _input);
		}
		else if (m_slideAllowedInput.find(_input) != U32_INVALID)
		{
			slide(_input);
		}
    }

#ifdef ITF_W1W_MOBILE
	void UIMenuBasic::onTouchInput(const TouchDataArray & _touchArray, u32 _i)
	{
		const TouchData& data = _touchArray[_i];
		Vec2d position = data.getCurrentMainViewPos();

		UIItem* curItem = SAFE_DYNAMIC_CAST(getChildComponentFromPosition(position, UI_STATE_Item, UI_STATE_Hidden | UI_STATE_CanNotBeSelected), UIItem);

		switch(data.state)
		{
		case TouchData::Invalid:
			{
				break;
			}
		case TouchData::Begin:
			{
				if(curItem)
				{
					select(curItem);
					
					if (curItem->isSlidable())
					{
						curItem->onSlideBegin(position);
					}
                }
				break;
			}
		case TouchData::Moving:
			{
				UIItem* selectedItem = getSelectedItem();
				if (selectedItem && selectedItem->isSlidable())
				{
					selectedItem->onSliding(position);
				}
				else if(curItem != selectedItem)
				{		
	               select(NULL);
				}

				break;
			}
		case TouchData::End:
			{

				UIItem* selectedItem = getSelectedItem();
				if (selectedItem && selectedItem->isSlidable())
				{
					selectedItem->onSlideEnd(position);
				}
				
				if(curItem && curItem == selectedItem) 
				{
					valide(data.controllerId);
				}

                if(isMenuType(MenuType_DoNotListenPad) || isMenuType(MenuType_DoNotListenNavigation))
                {
                    select(NULL);
                }

				break;
			}
		}
	}
#else
	void UIMenuBasic::onTouchInput(const TouchDataArray & _touchArray, u32 _i)
	{
		const TouchData& data = _touchArray[_i];
		Vec2d position = data.getCurrentMainViewPos();

		UIItem* curItem = SAFE_DYNAMIC_CAST(getChildComponentFromPosition(position, UI_STATE_Item, UI_STATE_Hidden | UI_STATE_CanNotBeSelected), UIItem);

		switch(data.state)
		{
		case TouchData::Invalid:
			{
				break;
			}
		case TouchData::Begin:
			{
				if(curItem)
				{
					select(curItem);
					
					if (curItem->isSlidable())
					{
						curItem->onSlideBegin(position);
					}
                }
				break;
			}
		case TouchData::Moving:
			{
				UIItem* selectedItem = getSelectedItem();
				if (selectedItem && selectedItem->isSlidable())
				{
					selectedItem->onSliding(position);
				}
				else
				{		
					if(curItem || isMenuType(MenuType_DoNotListenPad) || isMenuType(MenuType_DoNotListenNavigation))
					{
	                    select(curItem);
					}
				}

				break;
			}
		case TouchData::End:
			{
				if(curItem || isMenuType(MenuType_DoNotListenPad) || isMenuType(MenuType_DoNotListenNavigation))
				{
					select(curItem);
				}

				UIItem* selectedItem = getSelectedItem();
				if (selectedItem && selectedItem->isSlidable())
				{
					selectedItem->onSlideEnd(position);
				}
				
                // JR : commented this so that skip message can be used with touch
				// SF : satan My Icons are validated when I clic everywhere !
				if(curItem) 
				{
					valide(data.controllerId);
				}

                if(isMenuType(MenuType_DoNotListenPad) || isMenuType(MenuType_DoNotListenNavigation))
                {
                    select(NULL);
                }

				break;
			}
		}
	}
#endif //ITF_W1W_MOBILE

#ifdef ITF_PC
    void UIMenuBasic::onMouseButton(const Vec2d& _pos, InputAdapter::MouseButton _but, InputAdapter::PressStatus _status)
    {
        switch (_status)
        {
        case InputAdapter::Pressed:
            if (_but == InputAdapter::MB_Left && !m_isMouseLocked)
            {
                if(UIItem* curItem = SAFE_DYNAMIC_CAST(getChildComponentFromPosition(_pos, UI_STATE_Item, UI_STATE_Hidden | UI_STATE_CanNotBeSelected), UIItem))
                {
                    select(curItem);

                    if (curItem->isSlidable())
                        curItem->onSlideBegin(_pos);

                    m_isMouseLocked = btrue;
                }
            }
            break;
        case InputAdapter::Released:
            if (_but == InputAdapter::MB_Left && m_isMouseLocked)
            {
                UIItem* curItem = SAFE_DYNAMIC_CAST(getChildComponentFromPosition(_pos, UI_STATE_Item, UI_STATE_Hidden | UI_STATE_CanNotBeSelected), UIItem);
                if (curItem && curItem == getSelectedItem())
                {
                    if (curItem->isSlidable())
                        curItem->onSlideEnd(_pos);

                    valide(GAMEMANAGER->getMainIndexPlayerControllerId());
                }

                m_isMouseLocked = bfalse;
            }
            break;
        default:
            break;
        }
    }

    void UIMenuBasic::onMousePos(const Vec2d& _pos)
    {
        if (m_isMouseLocked)
        {
            UIItem* selectedItem = getSelectedItem();
            if (selectedItem && selectedItem->isSlidable())
            {
                selectedItem->onSliding(_pos);
            }
        }
        else
        {
            UIItem* curItem = SAFE_DYNAMIC_CAST(getChildComponentFromPosition(_pos, UI_STATE_Item, UI_STATE_Hidden | UI_STATE_CanNotBeSelected), UIItem);
            if(curItem && curItem != getSelectedItem())
            {
                select(curItem);
            }
        }
    }
#endif //ITF_PC

#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
    void UIMenuBasic::forcePadCursorToLastSelectedItem()
	{
		if(m_lastPadCursorSelectedItem)
		{
			UI_MENUMANAGER->setPadCursorSelectedItem(m_lastPadCursorSelectedItem);
		}
		else if ( UIItem* defaultItem = getChildComponent<UIItem>(m_defaultItemFriendly) )
        {
			UI_MENUMANAGER->setPadCursorSelectedItem(defaultItem);
        }
	}

    void UIMenuBasic::forcePadCursorToDefaultItem()
	{
		if ( UIItem* defaultItem = getChildComponent<UIItem>(m_defaultItemFriendly) )
        {
			UI_MENUMANAGER->setPadCursorSelectedItem(defaultItem);
        }
	}

    void UIMenuBasic::resetLastPadCursorSelectedItem()
	{
		m_lastPadCursorSelectedItem = NULL;
	}

#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD
    void UIMenuBasic::onBecomeActive()
	{
		resetValidateState();
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
		forcePadCursorToLastSelectedItem();
#else
        if ( UIItem* defaultItem = getChildComponent<UIItem>(m_defaultItemFriendly) )
        {
			select(defaultItem);
            //UI_MENUMANAGER->playSound(UIMenu_Sound_EnterPause,UIMenu_Sound_EnterPause_DRC,m_actor);
        }
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

		ITF_VECTOR<StringID> ids = getTemplate()->getWwisOnActivateSounds();
		for(u32 i = 0; i < ids.size(); i++)
		{
			UI_MENUMANAGER->playSound(ids[i],ids[i],m_actor);
		}

        Super::onBecomeActive();
    }
    void UIMenuBasic::onBecomeInactive()
    {
        Super::onBecomeInactive();

		ITF_VECTOR<StringID> ids = getTemplate()->getWwisOnDeactivateSounds();
		for(u32 i = 0; i < ids.size(); i++)
		{
			UI_MENUMANAGER->playSound(ids[i],ids[i],m_actor);
		}

    }

    void UIMenuBasic::onHiddingBegin()
    {
        Super::onHiddingBegin();

        if ( UIItem* defaultItem = getChildComponent<UIItem>(m_defaultItemFriendly) )
        {
           // UI_MENUMANAGER->playSound(UIMenu_Sound_EnterPause,UIMenu_Sound_EnterPause_DRC,m_actor);
        }

    }

    void UIMenuBasic::onCanBack(bbool _canBack)
    {
        Super::onCanBack(_canBack);

        if(UIComponent* backItem = getChildComponent(m_backItemFriendly))
        {
            backItem->setIsDisplay(_canBack);
        }
    }

    UIItem* UIMenuBasic::getSelectedItem() const 
    {
        if(Actor* actor = (Actor*) m_selectedItem.getObject())
        {
            return actor->GetComponent<UIItem>();
        }

        return NULL;
    }

    UIItem* UIMenuBasic::getValidateItem(bbool _waitActivatingEnd) const 
    {
        if(Actor* actor = (Actor*) m_validateItem.getObject())
        {
            if(UIItem* item = actor->GetComponent<UIItem>())
            {
                if(!_waitActivatingEnd || !item->hasUIState(UI_STATE_IsActivating))
                {
                    return item;
                }
            }
        }

        return NULL;
    }

	void UIMenuBasic::resetValidateState()
	{
        if(UIItem* validateItem = getValidateItem())
        {
            validateItem->onValidate(bfalse, -1);
        }

        if(UIItem* backItem = getBackItem())
        {
            backItem->onBack(bfalse, -1);
        }

        if(UIItem* actionItem = getActionItem())
        {
            actionItem->onAction(bfalse, -1);
        }

		m_backItem.invalidate();
		m_validateItem.invalidate();
        m_actionItem.invalidate();
		m_validateItemControllerID = -1;
		m_backItemControllerID = -1;
        m_actionItemControllerID = -1;
        m_validateItemInput = StringID::InvalidId;
        m_backItemInput = StringID::InvalidId;
        m_actionItemInput = StringID::InvalidId;

#ifdef ITF_W1W_MOBILE
		select(NULL);
#endif //ITF_W1W_MOBILE
	}

    UIItem* UIMenuBasic::getBackItem(bbool _waitActivatingEnd) const 
    {
        if(Actor* actor = (Actor*) m_backItem.getObject())
        {
            if(UIItem* item = actor->GetComponent<UIItem>())
            {
                if(!_waitActivatingEnd || !item->hasUIState(UI_STATE_IsActivating))
                {
                    return item;
                }
            }
        }

        return NULL;
    }

    UIItem* UIMenuBasic::getActionItem(bbool _waitActivatingEnd) const 
    {
        if(Actor* actor = (Actor*) m_actionItem.getObject())
        {
            if(UIItem* item = actor->GetComponent<UIItem>())
            {
                if(!_waitActivatingEnd || !item->hasUIState(UI_STATE_IsActivating))
                {
                    return item;
                }
            }
        }

        return NULL;
    }

    void UIMenuBasic::select(UIItem* _item)
    {
        UIItem* selectedItem = getSelectedItem();

        if(selectedItem!=_item)
        {
            if(selectedItem)
            {
                selectedItem->onSelected(bfalse);
                m_selectedItem.invalidate();
            }

            if(_item)
            {
                _item->onSelected(btrue);
                m_selectedItem = _item->GetActor()->getRef();
            }
        }
    }

    bbool UIMenuBasic::valide(i32 _controllerID, StringID _input)
    {
        UIItem* selectedItem = getSelectedItem();

        if(m_backItemFriendly.isValid() && selectedItem && selectedItem->getFriendlyID() == m_backItemFriendly)
        {
            return back(_controllerID, _input);
        }
        else if(!getIsLocked() && getCanValidate())
        {
            if(UIItem* validateItem = getValidateItem())
            {
                validateItem->onValidate(bfalse, _controllerID);
            }

			bbool playSound = bfalse;
            if(selectedItem)
            {
                selectedItem->onValidate(btrue, _controllerID);

                if(selectedItem->matchUIState(UI_STATE_None, UI_STATE_CanNotBeValidate))
                {
					playSound = btrue;
                    m_validateItem = m_selectedItem;
                    m_validateItemControllerID = _controllerID;
                    m_validateItemInput = _input;
                }
            }
            else
            {
                onValidate(btrue, _controllerID);

                if(matchUIState(UI_STATE_None, UI_STATE_CanNotBeValidate))
                {
					playSound = btrue;
                    m_validateItem = GetActor()->getRef();
                    m_validateItemControllerID = _controllerID;
                    m_validateItemInput = _input;
                }
            }

			if(playSound)
			{
#ifdef ITF_PROJECT_POP
				ITF_VECTOR<StringID> ids = getTemplate()->getWwisOnValidateSounds();
				for(u32 i = 0; i < ids.size(); i++)
				{
					UI_MENUMANAGER->playSound(ids[i],ids[i],m_actor);
				}
#else
				// Buttons play the sound

				
				// THE BUTTON SHOULD PLAY ITS SOUND....
				StringID mainSound, remoteSound;
				getTemplate()->getValidateItemSound(getFriendlyID(), mainSound, remoteSound);
				UI_MENUMANAGER->playSound(mainSound,remoteSound,m_actor);
#endif
			}

            return btrue;
        }

        return bfalse;
    }

    bbool UIMenuBasic::back(i32 _controllerID, StringID _input)
    {
        if(!getIsLocked() && getCanBack())
        {
            if(UIItem* backItem = getBackItem())
            {
                backItem->onBack(bfalse, _controllerID);
            }

            UIItem* selectedItem = getSelectedItem();

			bbool playSound = bfalse;
            if(selectedItem)
            {
                playSound = btrue;
                selectedItem->onBack(btrue, _controllerID);
                m_backItem = m_selectedItem;
                m_backItemControllerID = _controllerID;
                m_backItemInput = _input;

            }
            else
			{
				playSound = btrue;
                onBack(btrue, _controllerID);
                m_backItem = GetActor()->getRef();
                m_backItemControllerID = _controllerID;
                m_backItemInput = _input;
            }

			if(playSound)
			{
#ifdef ITF_PROJECT_POP
				ITF_VECTOR<StringID> ids = getTemplate()->getWwisOnBackSounds();
				for(u32 i = 0; i < ids.size(); i++)
				{
					UI_MENUMANAGER->playSound(ids[i],ids[i],m_actor);
				}
#else
				UI_MENUMANAGER->playSound(UIMenu_Sound_Back,UIMenu_Sound_Back_DRC,m_actor);
#endif
			}

            return btrue;
        }

        // This is a new behavior. In that case the back item is validated as it is done with touch.
        // So in the client code you can make the same test for touch and for classic controller.
        if(Actor* actor = (Actor*) m_backItemNew.getObject())
        {
            m_validateItem = m_backItemNew;
            m_validateItemControllerID = _controllerID;
            m_validateItemInput = _input;
            getValidateItem()->onValidate(btrue, _controllerID);
        }

        return bfalse;
    }

    bbool UIMenuBasic::action(i32 _controllerID, StringID _input)
    {
        if(!getIsLocked() && getCanAction())
        {
            if(UIItem* actionItem = getActionItem())
            {
                actionItem->onAction(bfalse, _controllerID);
            }

            UIItem* selectedItem = getSelectedItem();

            if(selectedItem)
            {
                StringID mainSound, remoteSound;
                getTemplate()->getValidateItemSound(getFriendlyID(), mainSound, remoteSound);
                UI_MENUMANAGER->playSound(mainSound,remoteSound,m_actor);
                selectedItem->onAction(btrue, _controllerID);
                m_actionItem = m_selectedItem;
                m_actionItemControllerID = _controllerID;
                m_actionItemInput = _input;

            }
            else
            {
                StringID mainSound, remoteSound;
                getTemplate()->getValidateItemSound(getFriendlyID(), mainSound, remoteSound);
                UI_MENUMANAGER->playSound(mainSound,remoteSound,m_actor);
                onAction(btrue, _controllerID);
                m_actionItem = GetActor()->getRef();
                m_actionItemControllerID = _controllerID;
                m_actionItemInput = _input;
            }

            return btrue;
        }

        return bfalse;
    }

	void UIMenuBasic::slide(StringID _input)
	{
		UIItem* selectedItem = getSelectedItem();
		if (selectedItem && selectedItem->isSlidable())
		{
			selectedItem->onSlide(_input);
		}
	}

    void UIMenuBasic::setActionAllowedInput(StringID _input, bbool _allowed)
    {
        u32 index = m_actionAllowedInput.find(_input);

        if(_allowed && index==U32_INVALID)
        {
            m_actionAllowedInput.push_back(_input);
        }
        else if(!_allowed && index!=U32_INVALID)
        {
            m_actionAllowedInput.removeAtUnordered(index);
        }
    }

    void UIMenuBasic::setValidateAllowedInput(StringID _input, bbool _allowed)
    {
        u32 index = m_validateAllowedInput.find(_input);

        if(_allowed && index==U32_INVALID)
        {
            m_validateAllowedInput.push_back(_input);
        }
        else if(!_allowed && index!=U32_INVALID)
        {
            m_validateAllowedInput.removeAtUnordered(index);
        }
    }

    void UIMenuBasic::setBackAllowedInput(StringID _input, bbool _allowed)
    {
        u32 index = m_backAllowedInput.find(_input);

        if(_allowed && index==U32_INVALID)
        {
            m_backAllowedInput.push_back(_input);
        }
        else if(!_allowed && index!=U32_INVALID)
        {
            m_backAllowedInput.removeAtUnordered(index);
        }
    }

	void UIMenuBasic::setSlideAllowedInput(StringID _input, bbool _allowed)
	{
		u32 index = m_slideAllowedInput.find(_input);

		if(_allowed && index==U32_INVALID)
		{
			m_slideAllowedInput.push_back(_input);
		}
		else if(!_allowed && index!=U32_INVALID)
		{
			m_slideAllowedInput.removeAtUnordered(index);
		}
	}

    UIMenu::MenuType UIMenuBasic::getMenuType() const
    {
        MenuType type = Super::getMenuType();

        if(m_backItemFriendly.isValid())
        {
            type = MenuType(type | MenuType_HideBack);
        }

        return type;
    }

    void UIMenuBasic::Update( f32 _deltaTime )
    {
		Super::Update(_deltaTime);

		m_lastAnalogDir = m_analogDir;
		m_analogDir = Vec2d::Zero;
    }

	ITF_VECTOR<StringID> UIMenuBasic::getWwisOnValidateSounds() const
	{
		return getTemplate()->getWwisOnValidateSounds();
	}


    ///////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_SERIALIZATION_SUBCLASS(UIMenuBasic_Template,ValidateItemSound)
    SERIALIZE_MEMBER("itemId", m_itemId);
    SERIALIZE_MEMBER("mainSoundId", m_mainSoundId);
    SERIALIZE_MEMBER("remoteSoundId", m_remoteSoundId);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(UIMenuBasic_Template)
	BEGIN_SERIALIZATION_CHILD(UIMenuBasic_Template)
	SERIALIZE_CONTAINER_OBJECT("validateItemSounds", m_validateItemSounds);
	SERIALIZE_CONTAINER("WwisActivateSounds", m_WwisOnActivate);
	SERIALIZE_CONTAINER("WwisDeactivateSounds", m_WwisOnDeactivate);
	SERIALIZE_CONTAINER("WwisValidateSounds", m_WwisOnValidate);
	SERIALIZE_CONTAINER("WwisBackSounds", m_WwisOnBack);
    END_SERIALIZATION()

    UIMenuBasic_Template::UIMenuBasic_Template()
    {  
    }

    
    UIMenuBasic_Template::~UIMenuBasic_Template()
    {    
    }

    bbool UIMenuBasic_Template::getValidateItemSound(StringID _itemId, StringID& _mainSound, StringID& _remoteSound) const
    {
        _mainSound = UIMenu_Sound_Validate;
        _remoteSound = UIMenu_Sound_Validate_DRC;

        for(ITF_VECTOR<ValidateItemSound>::const_iterator it = m_validateItemSounds.begin(); it!=m_validateItemSounds.end(); ++it)
        {
            if(it->m_itemId == _itemId)
            {
                _mainSound = it->m_mainSoundId;
                _remoteSound = it->m_remoteSoundId;

                return btrue;
            }
        }

        return bfalse;
    }

}
