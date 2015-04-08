#include "precompiled_engine.h"

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#include "gameplay/GameInterface.h"

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

namespace ITF {

#define CONTEXTICONSCONFIG_PATH GETPATH_ALIAS("contexticons")

//------------------------------------------------------------------------------
ContextIconsManager::ContextIconsManager()
    : m_template(NULL)
    , m_configPath()
    , m_left(ContextIcon_Invalid)
    , m_right(ContextIcon_Invalid)
    , m_topLeft(ContextIcon_Invalid)
    , m_topRight(ContextIcon_Invalid)
    , m_useRuntimeDepthRank(bfalse)
    , m_runtimeDepthRank(0)
{
}

//------------------------------------------------------------------------------
ContextIconsManager::~ContextIconsManager()
{
    if (TEMPLATEDATABASE && !m_configPath.isEmpty())
    {
        TEMPLATEDATABASE->releaseTemplate(m_configPath);
    }
}

//------------------------------------------------------------------------------
void ContextIconsManager::init()
{
    // load config file
    m_configPath = CONTEXTICONSCONFIG_PATH;

    bbool ok = btrue;

    if ( !m_configPath.isEmpty() )
    {
        const ContextIconsManager_Template* config = TEMPLATEDATABASE->requestTemplate<ContextIconsManager_Template>(m_configPath);
        if (!config)
        {
            ITF_FATAL_ERROR("Couldn't load context icons config: %s", m_configPath.toString8().cStr());
        }

        // store template
        m_template = const_cast<ContextIconsManager_Template*>(config);

        // safety checks
        if (m_template->getLineIds().size() != ContextIcon_Count)
        {
            ok = bfalse;
        }
        else if (m_template->getButtonNames().size() != InputAdapter::PadType_Count)
        {
            ok = bfalse;
        }
        else
        {
            for (u32 i=0; i<InputAdapter::PadType_Count; ++i)
            {
                if (m_template->getButtonNames()[i].m_names.size() != ContextIconType_Count)
                {
                    ok = bfalse;
                }
            }
        }
    }

    if (!ok)
    {
        ITF_FATAL_ERROR("Error loading context icons config: %s", m_configPath.toString8().cStr());
    }
}

//------------------------------------------------------------------------------
void ContextIconsManager::update()
{
    if (!isVisible())
    {
        return;
    }

    setupMenu();
}

//------------------------------------------------------------------------------
bbool ContextIconsManager::isVisible() const
{
    if (!UI_MENUMANAGER->areMenusLoaded())
    {
        return bfalse;
    }

    return m_template && UI_MENUMANAGER->isDisplayUIMenu( m_template->getMenuId());
}

//------------------------------------------------------------------------------
void ContextIconsManager::show(
    EContextIcon _left,
    EContextIcon _right,
    EContextIcon _topLeft /*= ContextIcon_Invalid*/,
    EContextIcon _topRight /*= ContextIcon_Invalid*/
    )
{
    if (!UI_MENUMANAGER->areMenusLoaded())
    {
        return;
    }

    // init/change info
    m_left = _left;
    m_right = _right;
    m_topLeft = _topLeft;
    m_topRight = _topRight;

    // menu visible, don't touch it
    if (isVisible())
    {
        return;
    }

    // show context icons "menu"
    UI_MENUMANAGER->showUIMenu(m_template->getMenuId());

    // setup menu right away (fix for RO-13343)
    setupMenu();
}

//------------------------------------------------------------------------------
void ContextIconsManager::hide()
{
    if (!UI_MENUMANAGER->areMenusLoaded())
    {
        return;
    }

    // reset info
    m_left = ContextIcon_Invalid;
    m_right = ContextIcon_Invalid;
    m_topLeft = ContextIcon_Invalid;
    m_topRight = ContextIcon_Invalid;

    // menu hidden, don't touch it
    if (!isVisible())
    {
        return;
    }

    UI_MENUMANAGER->hideUIMenu(m_template->getMenuId());
}

//------------------------------------------------------------------------------
void ContextIconsManager::changeLeftIcon(EContextIcon _icon)
{
    if (isVisible())
    {
        m_left = _icon;
    }
}

//------------------------------------------------------------------------------
void ContextIconsManager::changeRightIcon(EContextIcon _icon)
{
    if (isVisible())
    {
        m_right = _icon;
    }
}

//------------------------------------------------------------------------------
void ContextIconsManager::changeTopLeftIcon(EContextIcon _icon)
{
    if (isVisible())
    {
        m_topLeft = _icon;
    }
}

//------------------------------------------------------------------------------
void ContextIconsManager::changeTopRightIcon(EContextIcon _icon)
{
    if (isVisible())
    {
        m_topRight = _icon;
    }
}

//------------------------------------------------------------------------------
EContextIconType ContextIconsManager::getType(EContextIcon _icon) const
{
    if (_icon<0 || _icon>=ContextIcon_Count)
    {
        return ContextIconType_Invalid;
    }

    EContextIconType iconType = s_iconsTypes[_icon];

    if (SYSTEM_ADAPTER->isBackAndSelectButtonsInverted())
    {
        if (iconType == ContextIconType_Select)
        {
            iconType = ContextIconType_Back;
        }
        else if (iconType == ContextIconType_Back)
        {
            iconType = ContextIconType_Select;
        }
    }

    return iconType;
}

//------------------------------------------------------------------------------
const EContextIconType ContextIconsManager::s_iconsTypes[ContextIcon_Count] =
{
    // ContextIcon_Select
    ContextIconType_Select,
    // ContextIcon_Continue
    ContextIconType_Select,
    // ContextIcon_Enter
    ContextIconType_Select,
    // ContextIcon_Skip
    ContextIconType_Back,
    // ContextIcon_Back
    ContextIconType_Back,
    // ContextIcon_Retry
    ContextIconType_Back,
    // ContextIcon_Delete
    ContextIconType_Delete,
    // ContextIcon_RayHome
    ContextIconType_RayHome,
    // ContextIcon_Controls
    ContextIconType_Delete
};

//------------------------------------------------------------------------------
const String8& ContextIconsManager::getIconStr(u32 _padType, EContextIconType _context)
{
    ITF_ASSERT(_padType<InputAdapter::PadType_Count);
    ITF_ASSERT(_context<ContextIconType_Count && _context!=ContextIconType_Invalid);

    if(_context>=ContextIconType_Count || _context==ContextIconType_Invalid) _context = ContextIconType_Select;
    if(_padType>=InputAdapter::PadType_Count) _padType = 0;

    if(!m_template)
        return String8::emptyString;

    return m_template->getButtonNames()[_padType].m_names[_context];
}

//------------------------------------------------------------------------------
void ContextIconsManager::setupMenu()
{
    // retrieve UI menu pointer
    UIMenu* uiMenu = (UIMenu*) UI_MENUMANAGER->getUIMenu(m_template->getMenuId());

    // warn if not found
    ITF_WARNING(
        NULL, uiMenu,
        "Can't find UIMenu '%s'",
        m_template->getMenuId().getDebugString()
        );
    if (!uiMenu)
    {
        return;
    }

    // get left icon UI components
    static const String8 s_icon_left = "icon_left";
    static const String8 s_text_left = "text_left";
    UIComponent* iconLeft = (UIComponent*) uiMenu->getChildComponent(s_icon_left);
    UIComponent* textLeft = (UIComponent*) uiMenu->getChildComponent(s_text_left);
    ITF_WARNING(NULL, iconLeft, "Can't find UIComponent '%s'", s_icon_left.cStr());
    ITF_WARNING(NULL, textLeft, "Can't find UIComponent '%s'", s_text_left.cStr());
    if (!(iconLeft && textLeft))
    {
        return;
    }

    // get right icon UI components
    static const String8 s_text_right = "text_right";
    static const String8 s_icon_right = "icon_right";
    UIComponent* iconRight = (UIComponent*) uiMenu->getChildComponent(s_icon_right);
    UIComponent* textRight = (UIComponent*) uiMenu->getChildComponent(s_text_right);
    ITF_WARNING(NULL, iconRight, "Can't find UIComponent '%s'", s_icon_right.cStr());
    ITF_WARNING(NULL, textRight, "Can't find UIComponent '%s'", s_text_right.cStr());
    if (!(iconRight && textRight))
    {
        return;
    }

    // get top-left icon UI components
    static const String8 s_icon_top_left = "icon_top_left";
    static const String8 s_text_top_left = "text_top_left";
    UIComponent* iconTopLeft = (UIComponent*) uiMenu->getChildComponent(s_icon_top_left);
    UIComponent* textTopLeft = (UIComponent*) uiMenu->getChildComponent(s_text_top_left);
    ITF_WARNING(NULL, iconTopLeft, "Can't find UIComponent '%s'", s_icon_top_left.cStr());
    ITF_WARNING(NULL, textTopLeft, "Can't find UIComponent '%s'", s_text_top_left.cStr());
    if (!(iconTopLeft && textTopLeft))
    {
        return;
    }

    // get top_right icon UI components
    static const String8 s_icon_top_right = "icon_top_right";
    static const String8 s_text_top_right = "text_top_right";
    UIComponent* iconTopRight = (UIComponent*) uiMenu->getChildComponent(s_icon_top_right);
    UIComponent* textTopRight = (UIComponent*) uiMenu->getChildComponent(s_text_top_right);
    ITF_WARNING(NULL, iconTopRight, "Can't find UIComponent '%s'", s_icon_top_right.cStr());
    ITF_WARNING(NULL, textTopRight, "Can't find UIComponent '%s'", s_text_top_right.cStr());
    if (!(iconTopRight && textTopRight))
    {
        return;
    }

    // setup
    setupIcon(m_left, iconLeft, textLeft);
    setupIcon(m_right, iconRight, textRight);
    setupIcon(m_topLeft, iconTopLeft, textTopLeft);
    setupIcon(m_topRight, iconTopRight, textTopRight);
}

//------------------------------------------------------------------------------
void ContextIconsManager::setupIcon(EContextIcon _icon, UIComponent* _iconUI, UIComponent* _textUI)
{
    if (_icon == ContextIcon_Invalid)
    {
        _iconUI->GetActor()->disable();
        _textUI->GetActor()->disable();
    }
    else
    {
        // get index of the player in control
        // NB[LaurentCou]: assumes the main player is always the one concerned,
        // we'll have to adapt this for the pause menu which should be controlled
        // by whoever opened it
        EContextIconType iconType = getType(_icon);
        InputAdapter::PadType padType = INPUT_ADAPTER->getDebugInputPadType( GAMEMANAGER->getMainIndexPlayerControllerId() );

        if(iconType!=ContextIconType_Invalid)
        {
            String8 content = m_template->getButtonNames()[padType].m_names[iconType];
            //_iconUI->forceContent(content);
        }

        LocalisationId lineId = m_template->getLineIds()[_icon];
        //_textUI->setLineId(lineId);

        _iconUI->GetActor()->enable();
        _textUI->GetActor()->enable();
    }

    // apply runtime deth rank
    if (m_useRuntimeDepthRank)
    {
        _iconUI->setAbsoluteDepth(m_runtimeDepthRank);
        _textUI->setAbsoluteDepth(m_runtimeDepthRank);
    }
    else
    {
        _iconUI->setAbsoluteDepth(0);
        _textUI->setAbsoluteDepth(0);
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(ContextIconsManager_Template)
    BEGIN_SERIALIZATION_CHILD(ContextIconsManager_Template)
    SERIALIZE_MEMBER("menuId", m_menuId);
    SERIALIZE_CONTAINER("lineIds", m_lineIds);
    SERIALIZE_CONTAINER_OBJECT("buttonNames", m_buttonNames);
END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(ContextIconsManager_Template,ButtonName)

    SERIALIZE_CONTAINER("names",m_names);

END_SERIALIZATION()

//------------------------------------------------------------------------------
ContextIconsManager_Template::ContextIconsManager_Template()
    : Super()
    , m_menuId()
    , m_lineIds()
    , m_buttonNames()
{
}

//------------------------------------------------------------------------------
ContextIconsManager_Template::~ContextIconsManager_Template()
{
}

} // namespace ITF
