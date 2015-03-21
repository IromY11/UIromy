#include "precompiled_gameplay.h"

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif // _ITF_SCENE_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif

#include "engine/actors/managers/UIMenuManager.h"

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIMenu)
    BEGIN_SERIALIZATION_ABSTRACT_CHILD(UIMenu)
    SERIALIZE_MEMBER("loadResource", m_loadResources);
    SERIALIZE_MEMBER("afxDuration", m_afxDuration);
    SERIALIZE_ENUM_BEGIN("menuType", m_menuType);
    SERIALIZE_ENUM_VAR(MenuType_None);
    SERIALIZE_ENUM_VAR(MenuType_InputListener);
    SERIALIZE_ENUM_VAR(MenuType_JustDisplay);
    SERIALIZE_ENUM_VAR(MenuType_JustDisplayCanNotBeMasked);
    SERIALIZE_ENUM_VAR(MenuType_InputListenerCannotBeMasked);
    SERIALIZE_ENUM_VAR(MenuType_InputListenerWithBack);
	SERIALIZE_ENUM_VAR(MenuType_InputListenerWithExit);
    SERIALIZE_ENUM_VAR(MenuType_InputListenerWithHiddenBack);
    SERIALIZE_ENUM_VAR(MenuType_InputListenerWithBackAndAfx);
    SERIALIZE_ENUM_VAR(MenuType_InputListenerWithExitAndAfx);
    SERIALIZE_ENUM_VAR(MenuType_InputListenerWithHiddenBackAndAfx);
    SERIALIZE_ENUM_VAR(MenuType_InputListenerWithoutBackAndAfx);
	SERIALIZE_ENUM_VAR(MenuType_InputListenerAlways);
    SERIALIZE_ENUM_VAR(MenuType_InputListenerAlwaysWithHiddenBack);
    SERIALIZE_ENUM_END();
    END_SERIALIZATION()

    UIMenu::UIMenu()
    : m_analogDir(Vec2d::Zero)
	, m_lastAnalogDir(Vec2d::Zero)
	, m_menuType(MenuType_InputListener)
    , m_menuTypeMask(MenuType_None)
    , m_openDuration(-1.0f)
    , m_name(StringID::InvalidId)
    , m_listenControllerId(U32_INVALID)
    , m_loadResources(bfalse)
    , m_canBack(btrue)
    , m_canValidate(btrue)
    , m_canAction(btrue)
    , m_afxDuration(0.3f)
    {
        addUIState(UI_STATE_Menu);
    }

    
    UIMenu::~UIMenu()
    {    

    }

    void UIMenu::onActorLoaded(Pickable::HotReloadType _hot)
    {
        Super::onActorLoaded(_hot);

        m_menuTypeMask = m_menuType;

		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventShow_CRC,this);
    }

	void UIMenu::onEvent(Event * _event)
	{
		Super::onEvent(_event);

		if ( EventShow* eventShow = DYNAMIC_CAST(_event,EventShow) )
		{   
			//get binded objects
			AIUtils::ActorBindIterator iterator(m_actor);
			while (Actor* child = iterator.getNextChild())
			{
				child->onEvent(eventShow);
			}
		}
	}

    void UIMenu::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        if(!getParentObject().isValid())
        {
            UI_MENUMANAGER->registerUIMenu(this);
        }

//        View::MaskIds viewMask = getUIDisplayMask();
/*#ifndef ITF_PROJECT_WW1
        if((viewMask & View::MASKID_REMOTEONLY) == 0)
		{
            addMenuTypeMask(MenuType_DoNotListenTouch);
        }
#endif
*/	}

    void UIMenu::onStartDestroy( bbool _hotReload )
{
        Super::onStartDestroy(_hotReload);

        if(!getParentObject().isValid())
        {
            UI_MENUMANAGER->unregisterUIMenu(this);
        }
    }

    void UIMenu::onBecomeActive()
    {
        Super::onBecomeActive();

        m_openDuration = 0.0f;

        if(isMenuType(MenuType_InputListener) && !isMenuType(MenuType_DoNotListenPad))
        {
            UI_MENUMANAGER->pausePlayers(btrue);
        }
    }

    void UIMenu::onBecomeInactive()
    {
        if(isMenuType(MenuType_InputListener) && !isMenuType(MenuType_DoNotListenPad))
        {
            UI_MENUMANAGER->pausePlayers(bfalse);
        }

        m_openDuration = -1.0f;

        Super::onBecomeInactive();
    }

    void UIMenu::Update(f32 _dt)
    {
        Super::Update(_dt);

        m_openDuration += _dt;
    }

    void UIMenu::setIsMasked(const bbool _isMasked)
    {
        if(!_isMasked || !isMenuType(MenuType_CanNotBeMasked))
        {
            Super::setIsMasked(_isMasked);
        }
    }

    Vec2d UIMenu::getInputDirection(StringID _input)
    {
        Vec2d direction(Vec2d::Zero);

		if(_input == input_actionID_UpHold)
		{
			direction.y() = -1.0;
		}
		else if(_input == input_actionID_DownHold)
		{
			direction.y() = 1.0;
		}
		else if(_input == input_actionID_RightHold)
		{
			direction.x() = 1.0;
		}
		else if(_input == input_actionID_LeftHold)
		{
			direction.x() = -1.0;
		}

        return direction;
    }

    UIMenu::MenuType UIMenu::getMenuType() const 
    {
        return m_menuTypeMask;
    }

    void UIMenu::setCanBack(bbool _canBack) 
    {
        if(_canBack != m_canBack)
        {
            m_canBack = _canBack;
            onCanBack(getCanBack());
        }
    }

    void UIMenu::setCanValidate(bbool _canValidate) 
    {
        if(_canValidate != m_canValidate)
        {
            m_canValidate = _canValidate;
            onCanValidate(getCanValidate());
        }
    }    

    void UIMenu::setCanAction(bbool _canAction) 
    {
        if(_canAction != m_canAction)
        {
            m_canAction = _canAction;
            onCanAction(getCanAction());
        }
    }    

    void UIMenu::switchInputAdd(StringID _from, StringID _to)
    {
        m_switchedInput[_from] = _to;
    }

    StringID UIMenu::switchInputGet(StringID _from) const 
    {
        ITF_MAP<StringID, StringID>::const_iterator it = m_switchedInput.find(_from);
        if(it!=m_switchedInput.end())
        {
            return it->second;
        }

        return _from;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(UIMenu_Template)

    UIMenu_Template::UIMenu_Template()
    {  
    }

    
    UIMenu_Template::~UIMenu_Template()
    {    
    }

}
