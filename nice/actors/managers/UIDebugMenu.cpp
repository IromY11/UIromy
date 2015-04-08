#include "precompiled_engine.h"

#if defined(ITF_SUPPORT_CHEAT) && !defined(ITF_FINAL)

#include "core/Config.h"

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif // #ifndef _ITF_UIMENUMANAGER_H_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif

#ifndef _ITF_UIDEBUGMENU_H_
#include "engine/actors/managers/UIDebugMenu.h"
#endif //_ITF_UIDEBUGMENU_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "gameplay/managers/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

#include "engine/bloombergInterface/ITFBloombergInterface.h"

#ifndef _ITF_FILE_H_
# include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

#ifndef _ITF_SPEECHRECOGNITION_H_
# include "engine/AdaptersInterfaces/SpeechRecognition.h"
#endif // _ITF_SPEECHRECOGNITION_H_

#ifndef _ITF_TOUCHSCREENADAPTER_WIN_H__
#include "adapters/TouchScreenAdapter_Win/TouchScreenAdapter_Win.h"
#endif // _ITF_TOUCHSCREENADAPTER_WIN_H__

#ifndef _ITF_CHEATMANAGER_H_
#include "gameplay/managers/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_ENGINE_COMMANDS_H_
#include "engine/commands/engine_commands.h"
#endif //_ITF_ENGINE_COMMANDS_H_
#include "W1W/Basic/W1W_GameManager.h"

#include "engine/resources/ResourceManager.h"
#ifdef ITF_IOS
vm_size_t usedMemory(void);
vm_size_t freeMemory(void);
#endif

namespace ITF {

//-------------------------------------------------------------------------------------------------
static const f32 heightStart = 100.0f;
static const f32 widthStart = 75.0f;
static const f32 heightIncrease = 50.0f;
static const f32 widthByCol = 400.0f;
static const f32 border = 5.0f;

static inline bbool useBigFont()
{
#if defined(ITF_IOS) || defined(ITF_ANDROID)
    return  btrue;
#elif defined(ITF_WINDOWS) && defined(ITF_USE_TOUCHSCREEN)
    return TOUCHSCREEN_ADAPTER_WIN->getSimuMode();
#else
    return bfalse;
#endif
}
static inline f32 debugCharHeight()
{
    return useBigFont() ? (f32)GFX_ADAPTER->getBigDebugFontSize() : (f32)GFX_ADAPTER->getDebugFontSize();
}
static inline f32 debugCharWidth()
{
    return debugCharHeight() / 2.0f;
}
static inline f32 buttonMargin()
{
    return debugCharHeight() / 4.0f;
}
static inline f32 buttonSpacing()
{
    return debugCharHeight() / 4.0f;
}
static inline f32 lineHeight()
{
    return debugCharHeight() + 2 * buttonMargin();
}
static inline f32 scrollButtonSize()
{
    return useBigFont() ? 100.0f : 50.0f;
}
static inline f32 scrollStep()
{
    return debugCharHeight() + 2 * buttonMargin() + buttonSpacing();
}


//-------------------------------------------------------------------------------------------------
UIDebugMenu::UIDebugMenu()
    : m_currentPageDBGMenu(0)
    , m_inDBGMenu(bfalse)
    , m_timerDBGMenu(0.0f)
    , m_timeoutDBGMenu(0.05f)
    , m_player(U32_INVALID)
    , m_activatingPadIndex(0)
    , m_timeoutJustPressed(0.3f)
    , m_firstPressed(btrue)
    , m_joyReleased(btrue)
    , m_displayedController(U32_INVALID)
#if defined(ITF_IOS) || defined(ITF_ANDROID)
    , m_touchMode(btrue)
#else
    , m_touchMode(bfalse)
#endif
    , m_countdownRunning(bfalse)
    , m_contactCount(0)
    , m_inDBGMemory(bfalse)
    ,m_inDBGMemoryDisplay(0)
{
}

//-------------------------------------------------------------------------------------------------
UIDebugMenu::~UIDebugMenu()
{
    for (ITF_VECTOR<DebugMenu*>::iterator iter = m_DBGmenus.begin(); iter!=m_DBGmenus.end(); iter++)
        SF_DEL(*iter);
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::update(f32 _dt)
{
#if defined(ITF_WINDOWS) && defined(ITF_USE_TOUCHSCREEN)
    const bbool simuMode = TOUCHSCREEN_ADAPTER_WIN->getSimuMode();
    if (!m_touchMode && simuMode)
        setTouchMode(btrue);
    else if (m_touchMode && !simuMode)
        setTouchMode(bfalse);
#endif

    // debug menu
    updateDBGMenuStatus();
    updateTouchInput(_dt);

    getCurrentMenu()->update();

    if (m_inDBGMenu)
    {
        // Update touch buttons positions and sizes
        if (getTouchMode())
        {
            const u32 screenWidth = GFX_ADAPTER->getScreenWidth();
            const u32 screenHeight = GFX_ADAPTER->getScreenHeight();

            // Left button
            m_touchLeftButton.m_position = Vec2d(0.0f, scrollButtonSize() + buttonSpacing());
            m_touchLeftButton.m_size = Vec2d(scrollButtonSize(), screenHeight - 2 * (scrollButtonSize() + buttonSpacing()));

            // Right button
            m_touchRightButton.m_position = Vec2d(screenWidth - scrollButtonSize(), scrollButtonSize() + buttonSpacing());
            m_touchRightButton.m_size = Vec2d(scrollButtonSize(), screenHeight - 2 * (scrollButtonSize() + buttonSpacing()));

            // Top button
            m_touchTopButton.m_position = Vec2d(scrollButtonSize() + buttonSpacing(), 0.0f);
            m_touchTopButton.m_size = Vec2d(screenWidth - 2 * (scrollButtonSize() + buttonSpacing()), scrollButtonSize());

            // Bottom button
            m_touchBottomButton.m_position = Vec2d(scrollButtonSize() + buttonSpacing(), screenHeight - scrollButtonSize());
            m_touchBottomButton.m_size = Vec2d(screenWidth - 2 * (scrollButtonSize() + buttonSpacing()), scrollButtonSize());
        }

        displayDebugMenu();
    }
    else if (m_displayedController != U32_INVALID)
    {
        displayControllerState(m_displayedController);
    }

    if(GFX_ADAPTER->getfPs())
        m_timerDBGMenu += 1.f / GFX_ADAPTER->getfPs();
    if (m_inDBGMenu)
        updateDBGMenuPad();

    if(m_inDBGMemory)
        updateDBGMenuMemory();
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::updateTouchInput(f32 _dt)
{
#ifndef ITF_FINAL
    
#if defined(ITF_IOS) || defined(ITF_ANDROID)
    const u32 displayContactCount = 3;
#else
    const u32 displayContactCount = 2;
#endif

    u32 newContactCount = 0;
    ITF_VECTOR<TouchContact>::iterator it = m_currentContacts.begin();
    while (it != m_currentContacts.end())
    {
        if (m_inDBGMenu)
            getCurrentMenu()->setSelectionAt(it->m_position);
        if (it->m_ended)
        {
            if (m_inDBGMenu)
                onContactEnd(it->m_position);
            it = m_currentContacts.erase(it);
        }
        else
        {
            ++newContactCount;
            ++it;
        }
    }

    if (m_countdownRunning)
    {
        if (newContactCount > m_contactCount)
        {
            m_countdownRunning = bfalse;
        }
        else if (newContactCount == 0)
        {
            setIsInDBGMenu(!m_inDBGMenu, GAMEMANAGER->getMainIndexPlayer());
            m_countdownRunning = bfalse;
        }
    }
    else
    {
        if (m_contactCount >= displayContactCount)
            m_countdownRunning = btrue;
    }
    m_contactCount = newContactCount;
    
#endif
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::onContactEnd(const Vec2d &_position)
{
    if (m_touchLeftButton.isPointInside(_position))
        setPreviousDBGMenu();
    else if (m_touchRightButton.isPointInside(_position))
        setNextDBGMenu();
    else if (m_touchTopButton.isPointInside(_position))
        scrollUp();
    else if (m_touchBottomButton.isPointInside(_position))
        scrollDown();
    else
        getCurrentMenu()->onContactEnd(_position);
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::updateDBGMenuPad()
{
    for (u32 i = 0; i < INPUT_ADAPTER->getMaxPadCount(); i++)
    {
        if(CHEATMANAGER->getSwapPlayerIndex0and1() && i != 1)
            continue;
        if(CHEATMANAGER->getBlockJoinPlayers() && i != 0)
            continue;

        u32 controllerId = i;//GAMEMANAGER->getPlayerControllerId( i );
        if ( controllerId != U32_INVALID && m_activatingPadIndex == controllerId )
        {
            // Buttons
            InputAdapter::PressStatus buts[JOY_MAX_BUT];
            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll,controllerId, buts, JOY_MAX_BUT);

            // Joysticks
            float pos[JOY_MAX_AXES];
            INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll,controllerId, pos, JOY_MAX_AXES);

            applyGamePadButtonDBGMenu (controllerId, buts,pos);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::setIsInDBGMenu( bbool _isInDBGMenu, u32 _player )
{
    m_inDBGMenu = _isInDBGMenu;
    m_currentPageDBGMenu = 0;
    m_player = m_inDBGMenu ? _player : U32_INVALID;
    if(getCurrentMenu()->getEntriesSize() == 0)
        setNextDBGMenu();
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::setNextDBGMenu()
{
    do 
    {
        m_currentPageDBGMenu = (m_currentPageDBGMenu+1)%m_DBGmenus.size();
    } while ( getCurrentMenu()->getEntriesSize() == 0 );
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::setPreviousDBGMenu()
{
    do 
    {                
        if(m_currentPageDBGMenu==0)
            m_currentPageDBGMenu = m_DBGmenus.size();

        m_currentPageDBGMenu = (m_currentPageDBGMenu-1)%m_DBGmenus.size();
    } while ( getCurrentMenu()->getEntriesSize() == 0 );
}

//-------------------------------------------------------------------------------------------------
bbool UIDebugMenu::canScrollUp()
{
    DebugMenu* const menu = getCurrentMenu();
    return menu->getScreenPos().y() + scrollStep() <= menu->getClippingRect().m_position.y();
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::scrollUp()
{
    if (canScrollUp())
    {
        DebugMenu* const menu = getCurrentMenu();
        menu->setScreenPos(menu->getScreenPos() + Vec2d(0.0f, scrollStep()));
    }
}

//-------------------------------------------------------------------------------------------------
bbool UIDebugMenu::canScrollDown()
{
    DebugMenu* const menu = getCurrentMenu();
    return menu->getScreenPos().y() - scrollStep() + menu->getSize().y() >= menu->getClippingRect().m_position.y() + menu->getClippingRect().m_size.y();
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::scrollDown()
{
    if (canScrollDown())
    {
        DebugMenu* const menu = getCurrentMenu();
        menu->setScreenPos(menu->getScreenPos() - Vec2d(0.0f, scrollStep()));
    }
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::applyGamePadButtonDBGMenu (u32 _numPad, InputAdapter::PressStatus* _buts, float* _pos)
{
    u32 playerIdx = GAMEMANAGER->getLocalPlayerIdFromControllerId( _numPad );

    // Page change in debug menu
    if (_buts[m_joyButton_RB] == InputAdapter::JustPressed)
    {
        setNextDBGMenu();
        m_player = playerIdx; // update player index
    }
    else if (_buts[m_joyButton_LB] == InputAdapter::JustPressed)
    {
        setPreviousDBGMenu();
        m_player = playerIdx; // update player index
    }

    // Use of the D-pad: the button is just pressed -> go up or down
    else if (_buts[m_joyButton_DPadU] == InputAdapter::JustPressed )
    {
        m_timerDBGMenu = 0.0f;
        getCurrentMenu()->up();
        m_firstPressed = btrue;
        m_player = playerIdx; // update player index
    }
    else if (_buts[m_joyButton_DPadD] == InputAdapter::JustPressed )
    {
        m_timerDBGMenu = 0.0f;
        getCurrentMenu()->down();
        m_firstPressed = btrue;
        m_player = playerIdx; // update player index
    }

    // Use of the D-pad: the button is pressed -> scroll up or down
    else if (m_timerDBGMenu > m_timeoutDBGMenu && _buts[m_joyButton_DPadU] == InputAdapter::Pressed)
    {
        if (m_firstPressed && m_timerDBGMenu > m_timeoutJustPressed)
        {
            m_timerDBGMenu = 0.0f;
            m_firstPressed = bfalse;
        }
        if (!m_firstPressed)
        {
            m_timerDBGMenu = 0.0f;
            getCurrentMenu()->up();
        }
    }
    else if (m_timerDBGMenu > m_timeoutDBGMenu && _buts[m_joyButton_DPadD] == InputAdapter::Pressed)
    {
        if (m_firstPressed && m_timerDBGMenu > m_timeoutJustPressed)
        {
            m_timerDBGMenu = 0.0f;
            m_firstPressed = bfalse;
        }
        if (!m_firstPressed)
        {
            m_timerDBGMenu = 0.0f;
            getCurrentMenu()->down();
        }
    }

    // Use of the joystick: the joy is just pressed -> go up or down
    else if (m_joyReleased && _pos[1] > 0.7f)
    {
        if(getCurrentMenu()->getEntriesSize() > 0)
        {
            m_joyReleased = bfalse;
            m_timerDBGMenu = 0.0f;
            getCurrentMenu()->up();
            m_firstPressed = btrue;
            m_player = playerIdx; // update player index            
        }
    }
    else if (m_joyReleased && _pos[1] < -0.7f)
    {
        if(getCurrentMenu()->getEntriesSize() > 0)
        {
            m_joyReleased = bfalse;
            m_timerDBGMenu = 0.0f;
            getCurrentMenu()->down();
            m_firstPressed = btrue;
            m_player = playerIdx; // update player index            
        }
    }

    // Use of the joystick: the joy is pressed -> scroll up or down
    else if (m_timerDBGMenu > m_timeoutDBGMenu && _pos[1] > 0.7f)
    {
        if (m_firstPressed && m_timerDBGMenu > m_timeoutJustPressed)
        {
            m_timerDBGMenu = 0.0f;
            m_firstPressed = bfalse;
        }
        if (!m_firstPressed)
        {
            m_timerDBGMenu = 0.0f;
            getCurrentMenu()->up();
        }
    }
    else if (m_timerDBGMenu > m_timeoutDBGMenu && _pos[1] < -0.7f)
    {
        if (m_firstPressed && m_timerDBGMenu > m_timeoutJustPressed)
        {
            m_timerDBGMenu = 0.0f;
            m_firstPressed = bfalse;
        }
        if (!m_firstPressed)
        {
            m_timerDBGMenu = 0.0f;
            getCurrentMenu()->down();
        }
    }

    // The joystick is released
    else if (_pos[1] != 0.0f && _pos[1] > -0.7f && _pos[1] < 0.7f)
        m_joyReleased = btrue;

    // Others buttons
    else if (_buts[m_joyButton_X] == InputAdapter::JustPressed)
        CHEATMANAGER->setActive( !CHEATMANAGER->getActive() );
    else if (_buts[m_joyButton_A] == InputAdapter::JustPressed)
    {
        getCurrentMenu()->select(playerIdx);
        m_player = playerIdx; // update player index
    }
    else if (_buts[m_joyButton_DPadL] == InputAdapter::JustPressed)
    {
        getCurrentMenu()->onDirection(true);
    }
    else if (_buts[m_joyButton_DPadR] == InputAdapter::JustPressed)
    {
        getCurrentMenu()->onDirection(false);
    }
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::updateDBGMenuStatus()
{
    if (!CONFIG->m_allowDebug)
    {
        return;
    }

    for (u32 i = 0; i < INPUT_ADAPTER->getMaxPadCount(); i++)
    {
        if(CHEATMANAGER->getSwapPlayerIndex0and1() && i != 1)
            continue;

        if(CHEATMANAGER->getBlockJoinPlayers() && i != 0)
            continue;

        u32 controllerId = i; //GAMEMANAGER->getPlayerControllerId( i );
        if ( controllerId != U32_INVALID )
        {
            InputAdapter::PressStatus buts[JOY_MAX_BUT];
            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll,controllerId, buts, JOY_MAX_BUT);

            bbool bPressedLBRB = buts[m_joyButton_LB] == InputAdapter::Pressed &&  buts[m_joyButton_RB] == InputAdapter::Pressed;
#ifdef ITF_WII //LG :tmp code please fix it
            bPressedLBRB = btrue;
#endif //ITF_WII

            // LB+RB+Back to show debug menu
            if ( bPressedLBRB &&
                (buts[m_joyButton_Back] == InputAdapter::JustPressed ||
                buts[m_joyButton_TouchPad] == InputAdapter::JustPressed))
            {
                bool newDebugMenuEnabled = !UI_MENUMANAGER->getIsInDBGMenu();
                if ( !newDebugMenuEnabled && m_activatingPadIndex != controllerId )
                {
                    continue;
                } else if ( newDebugMenuEnabled )
                    m_activatingPadIndex = controllerId;

                setIsInDBGMenu(newDebugMenuEnabled, i);

                // Pause the game
                if (newDebugMenuEnabled /*|| UI_MENUMANAGER->getIsInMenu()*/)
                    INPUT_ADAPTER->disableEnvironment(InputAdapter::EnvironmentEngine);
                else
                    INPUT_ADAPTER->enableEnvironment(InputAdapter::EnvironmentEngine);

                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::addDebugMenu( DebugMenu* _menu )
{
    _menu->setParent(this);
    m_DBGmenus.push_back(_menu);

    if (m_touchMode)
    {
        const Vec2d menuPos(scrollButtonSize() + buttonSpacing(), scrollButtonSize() + buttonSpacing());
        _menu->setScreenPos(menuPos);
        _menu->setClippingRect(DebugMenuRect(
            menuPos,
            Vec2d(
            GFX_ADAPTER->getScreenWidth() - 2 * (scrollButtonSize() + buttonSpacing()),
            GFX_ADAPTER->getScreenHeight() - 2 * (scrollButtonSize() + buttonSpacing()))));
    }
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::setTouchMode( bbool _val )
{
    if (_val == m_touchMode)
        return;

    m_touchMode = _val;

    // Reset menu positions
    if (_val)
    {
        const Vec2d menuPos(scrollButtonSize() + buttonSpacing(), scrollButtonSize() + buttonSpacing());
        for (u32 i = 0; i < m_DBGmenus.size(); ++i)
        {
            DebugMenu * const menu = m_DBGmenus[i];
            menu->setScreenPos(menuPos);
            menu->setClippingRect(DebugMenuRect(
                menuPos,
                Vec2d(
                    GFX_ADAPTER->getScreenWidth() - 2 * (scrollButtonSize() + buttonSpacing()),
                    GFX_ADAPTER->getScreenHeight() - 2 * (scrollButtonSize() + buttonSpacing()))));
        }
    }
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::displayDebugMenu()
{
    DebugMenu* const menu = getCurrentMenu();
    if (!getTouchMode())
    {
        f32 height = heightStart;
        f32 width   = widthStart;
        f32 R = 1.f, G = .2f, B = .2f;

        GFX_ADAPTER->drawDBG2dBox(Vec2d(width-border, height-border), 
            widthByCol + border *2, 
            60.0f + border *2, 
            0xBBcccccc, 0xBBcccccc,0xBBcccccc, 0xBBcccccc);

        String8 help;help.setTextFormat("X -> Cheat Mode %s", CHEATMANAGER->getActive() ? "ON" : "OFF");
        GFX_ADAPTER->drawDBGText(help,width,height,R,G,B);
        height += 15;

        help = "Y -> Swap player";
        GFX_ADAPTER->drawDBGText(help,width,height,R,G,B);
        height += 15;

        help = "RB / LB -> cycle menu pages";
        GFX_ADAPTER->drawDBGText(help,width,height,R,G,B);
        height += 15;

        help.setTextFormat("Player index: %d", m_player);
        GFX_ADAPTER->drawDBGText(help,width,height,R,G,B);

        height+=heightIncrease;

        const Vec2d menuPos(width, height);
        const Vec2d menuSize(GFX_ADAPTER->getScreenWidth() - width, GFX_ADAPTER->getScreenHeight() - height);
        menu->setScreenRect(DebugMenuRect(menuPos, menuSize));
        menu->setClippingRect(DebugMenuRect(menuPos, menuSize));
    }
    else
    {
        const u32 backgroundColor = 0x2000cc00;
        const f32 R = 1.f, G = .2f, B = .2f;

        // Left button
        GFX_ADAPTER->drawDBG2dBox(
            m_touchLeftButton.m_position, 
            m_touchLeftButton.m_size.x(), 
            m_touchLeftButton.m_size.y(),
            backgroundColor, backgroundColor,backgroundColor, backgroundColor);
        GFX_ADAPTER->drawDBGText(
            "<",
            m_touchLeftButton.m_position.x() + m_touchLeftButton.m_size.x() / 2,
            m_touchLeftButton.m_position.y() + m_touchLeftButton.m_size.y() / 2,
            R, G, B);

        // Right button
        GFX_ADAPTER->drawDBG2dBox(
            m_touchRightButton.m_position, 
            m_touchRightButton.m_size.x(), 
            m_touchRightButton.m_size.y(),
            backgroundColor, backgroundColor,backgroundColor, backgroundColor);
        GFX_ADAPTER->drawDBGText(
            ">",
            m_touchRightButton.m_position.x() + m_touchRightButton.m_size.x() / 2,
            m_touchRightButton.m_position.y() + m_touchRightButton.m_size.y() / 2,
            R, G, B);

        // Top button
        if (canScrollUp())
        {
            GFX_ADAPTER->drawDBG2dBox(
                m_touchTopButton.m_position, 
                m_touchTopButton.m_size.x(), 
                m_touchTopButton.m_size.y(),
                backgroundColor, backgroundColor,backgroundColor, backgroundColor);
            GFX_ADAPTER->drawDBGText(
                "^",
                m_touchTopButton.m_position.x() + m_touchTopButton.m_size.x() / 2,
                m_touchTopButton.m_position.y() + m_touchTopButton.m_size.y() / 2,
                R, G, B);
        }

        // Bottom button
        if (canScrollDown())
        {
            GFX_ADAPTER->drawDBG2dBox(
                m_touchBottomButton.m_position, 
                m_touchBottomButton.m_size.x(), 
                m_touchBottomButton.m_size.y(),
                backgroundColor, backgroundColor,backgroundColor, backgroundColor);
            GFX_ADAPTER->drawDBGText(
                "v",
                m_touchBottomButton.m_position.x() + m_touchBottomButton.m_size.x() / 2,
                m_touchBottomButton.m_position.y() + m_touchBottomButton.m_size.y() / 2,
                R, G, B);
        }
    }

    menu->display();
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::displayControllerState(u32 _controllerId) const
{
    f32 x = widthStart;
    f32 y = heightStart;
    const f32 R = 0.9f;
    const f32 G = 0.9f;
    const f32 B = 0.9f;

    char buf[256];
    SPRINTF_S(buf, sizeof(buf), "State of controller %u:", _controllerId);
    GFX_ADAPTER->drawDBGText(buf, x, y, R, G, B);
    y += lineHeight();

    InputAdapter * const inputs = INPUT_ADAPTER;
    if (inputs->isPadConnected(_controllerId))
    {
        InputAdapter::PadType padType = inputs->getPadType(_controllerId);
        switch(padType)
        {
        case InputAdapter::Pad_X360:
            GFX_ADAPTER->drawDBGText("Type : X360", x, y, R, G, B);
            y += lineHeight();
            displayBaseControllerState(_controllerId, x, y, bfalse);
            break;
        case InputAdapter::Pad_Orbis:
            GFX_ADAPTER->drawDBGText("Type : PS4", x, y, R, G, B);
            y += lineHeight();
            displayBaseControllerState(_controllerId, x, y, btrue);
            displayTouchScreenControllerState(_controllerId, x, y);
            break;
        case InputAdapter::Pad_TouchScreen:
            GFX_ADAPTER->drawDBGText("Type : Touch Screen", x, y, R, G, B);
            y += lineHeight();
            displayTouchScreenControllerState(_controllerId, x, y);
            break;
		case InputAdapter::Pad_Durango:
			GFX_ADAPTER->drawDBGText( "Type : XOne", x, y, R, G, B );
			y += lineHeight();
			displayBaseControllerState( _controllerId, x, y, bfalse );
			break;
        }
    }
    else
    {
        GFX_ADAPTER->drawDBGText("DISCONNECTED", x, y, 0.9f, 0.9f, 0.9f);
    }
}

//-------------------------------------------------------------------------------------------------
static String8 buttonIndexToX360ButtonName(const u32 _buttonIndex)
{
    switch(_buttonIndex)
    {
    case m_joyButton_A:
        return "A";
    case m_joyButton_B:
        return "B";
    case m_joyButton_X:
        return "X";
    case m_joyButton_Y:
        return "Y";
    case m_joyButton_LB:
        return "LB";
    case m_joyButton_RB:
        return "RB";
    case m_joyButton_Back:
        return "Back";
    case m_joyButton_Start:
        return "Start";
    case m_joyButton_DPadL:
        return "Left";
    case m_joyButton_DPadR:
        return "Right";
    case m_joyButton_DPadU:
        return "Up";
    case m_joyButton_DPadD:
        return "Down";
    case m_joyStick_Left:
        return "Left Joy";
    case m_joyStick_Right:
        return "Right Joy";
    case m_joyButton_ThumbLeft:
        return "Left Thumb";
    case m_joyButton_ThumbRight:
        return "Right Thumb";
    case m_joyButton_TriggerLeft:
        return "LT";
    case m_joyButton_TriggerRight:
        return "RT";
    }
    return "Unknown";
}

//-------------------------------------------------------------------------------------------------
static String8 buttonIndexToPS4ButtonName(const u32 _buttonIndex)
{
    switch(_buttonIndex)
    {
    case m_joyButton_A:
        return "Cross";
    case m_joyButton_B:
        return "Circle";
    case m_joyButton_X:
        return "Square";
    case m_joyButton_Y:
        return "Triangle";
    case m_joyButton_LB:
        return "L1";
    case m_joyButton_RB:
        return "R1";
    case m_joyButton_Back:
        return "Select";
    case m_joyButton_Start:
        return "Options";
    case m_joyButton_DPadL:
        return "Left";
    case m_joyButton_DPadR:
        return "Right";
    case m_joyButton_DPadU:
        return "Up";
    case m_joyButton_DPadD:
        return "Down";
    case m_joyStick_Left:
        return "Left Joy";
    case m_joyStick_Right:
        return "Right Joy";
    case m_joyButton_ThumbLeft:
        return "L3";
    case m_joyButton_ThumbRight:
        return "R3";
    case m_joyButton_TriggerLeft:
        return "L2";
    case m_joyButton_TriggerRight:
        return "R2";
//    case m_joyButton_TouchPad:
//        return "Touch Pad";
    }
    return "Unknown";
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::displayBaseControllerState(u32 _controllerId, f32 &_x, f32 &_y, bbool _isPS4) const
{
    const f32 R = 0.9f;
    const f32 G = 0.9f;
    const f32 B = 0.9f;
    char buf[256];

    InputAdapter * const inputAdapter = INPUT_ADAPTER;
    InputAdapter::PressStatus buts[JOY_MAX_BUT];
    inputAdapter->getGamePadButtons(InputAdapter::EnvironmentAll, _controllerId, buts, JOY_MAX_BUT);
    String8 justPressed("Buttons just pressed: ");
    String8 pressed("Buttons pressed: ");
    const u32 buttonCount = (_isPS4 ? m_joyButton_TouchPad + 1 : m_joyButton_CommonCount);
    for (u32 buttonIndex = 0; buttonIndex < buttonCount; ++buttonIndex)
    {
        const InputAdapter::PressStatus status = buts[buttonIndex];
        if (status == InputAdapter::JustPressed)
            justPressed += (_isPS4 ? buttonIndexToPS4ButtonName(buttonIndex) : buttonIndexToX360ButtonName(buttonIndex)) + ", ";
        else if (status == InputAdapter::Pressed)
            pressed += (_isPS4 ? buttonIndexToPS4ButtonName(buttonIndex) : buttonIndexToX360ButtonName(buttonIndex)) + ", ";
    }
    GFX_ADAPTER->drawDBGText(justPressed, _x, _y, R, G, B);
    _y += lineHeight();
    GFX_ADAPTER->drawDBGText(pressed, _x, _y, R, G, B);
    _y += lineHeight();

    f32 joyPosArray[JOY_MAX_AXES];
    inputAdapter->getGamePadPos(InputAdapter::EnvironmentAll, _controllerId, joyPosArray, JOY_MAX_AXES);

    Vec2d joy1 = Vec2d(joyPosArray[0], joyPosArray[1]);
    joy1 = joy1.normalize();
    SPRINTF_S(buf, sizeof(buf), "Left Joy: %f %f", joy1.x(), joy1.y());
    GFX_ADAPTER->drawDBGText(buf, _x, _y, R, G, B);
    _y += lineHeight();

    Vec2d joy2 = Vec2d(joyPosArray[3], joyPosArray[4]);
    joy2 = joy2.normalize();
    SPRINTF_S(buf, sizeof(buf), "Right Joy: %f %f", joy2.x(), joy2.y());
    GFX_ADAPTER->drawDBGText(buf, _x, _y, R, G, B);
    _y += lineHeight();

    SPRINTF_S(buf, sizeof(buf), "Left Trigger: %f", joyPosArray[2]);
    GFX_ADAPTER->drawDBGText(buf, _x, _y, R, G, B);
    _y += lineHeight();

    SPRINTF_S(buf, sizeof(buf), "Right Trigger: %f", joyPosArray[5]);
    GFX_ADAPTER->drawDBGText(buf, _x, _y, R, G, B);
    _y += lineHeight();

	_y += SpeechRecognition::debugDraw( _x, _y, lineHeight() );
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::displayTouchScreenControllerState(u32 _controllerId, f32 &_x, f32 &_y) const
{
    const f32 R = 0.9f;
    const f32 G = 0.9f;
    const f32 B = 0.9f;
    char buf[256];

    SPRINTF_S(buf, sizeof(buf), "Last touch data of controller %u:", _controllerId);
    GFX_ADAPTER->drawDBGText(buf, _x, _y, R, G, B);
    _y += lineHeight();

    for (TouchDataArray::const_iterator it = m_lastTouchData.begin(); it != m_lastTouchData.end(); ++it)
    {
        const TouchData &data = *it;
        SPRINTF_S(buf, sizeof(buf), "Touch Id %u: %s (x=%f y=%f)",
            data.id,
            data.state == TouchData::End ? "End" : (data.state == TouchData::Begin ? "Begin" : "Moving"),
            data.coordinates.x(),
            data.coordinates.y());
        GFX_ADAPTER->drawDBGText(buf, _x, _y, R, G, B);
        _y += lineHeight();
    }
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::Receive(const TouchDataArray & _touchArray)
{
    if (!m_inDBGMenu)
    {
        if (m_displayedController != U32_INVALID)
        {
            m_lastTouchData.clear();
            for (TouchDataArray::const_iterator it = _touchArray.begin(); it != _touchArray.end(); ++it)
            {
                const TouchData &data = *it;
                if (data.controllerId == m_displayedController)
                {
                    m_lastTouchData.push_back(data);
                }
            }
        }
    }

    if (m_touchMode)
    {
        for (TouchDataArray::const_iterator it = _touchArray.begin(); it != _touchArray.end(); ++it)
        {
            const TouchData &data = *it;
            if (data.state == TouchData::Begin)
            {
                m_currentContacts.push_back(TouchContact(
                    data.controllerId,
                    data.id,
                    data.getCoordinates(),
                    (f32)SYSTEM_ADAPTER->getTime()));
            }
            else if (data.state == TouchData::Moving)
            {
                TouchContact* const contact = getTouchContact(data.controllerId, data.id);
                if (contact)
                    contact->m_position = data.getCoordinates();
            }
            else if (data.state == TouchData::End)
            {
                TouchContact* const contact = getTouchContact(data.controllerId, data.id);
                if (contact)
                {
                    contact->m_position = data.getCoordinates();
                    contact->m_ended = btrue;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
UIDebugMenu::TouchContact * UIDebugMenu::getTouchContact( u32 _controller, u32 _touchId )
{
    for (u32 i = 0; i < m_currentContacts.size(); ++i)
    {
        TouchContact &contact = m_currentContacts[i];
        if (contact.m_controller == _controller && contact.m_touchId == _touchId)
            return &contact;
    }
    return NULL;
}

//=================================================================================================
// Debug Menu for some general purpose (URL / FPS)
//=================================================================================================

#pragma region Declarations

//-----------------------------------------------------------------------
class DebugMenuEntry_DisplayControllerInput : public DebugMenuEntry
{
public:
    DebugMenuEntry_DisplayControllerInput() : DebugMenuEntry("") {}
    virtual void select(u32 _player) const;
    virtual const String8 getName(u32 _player) const;
};

#if ITF_DEBUGEVENTS
//-----------------------------------------------------------------------
class DebugMenuEntry_DisplayEvents : public DebugMenuEntry
{
public:
    DebugMenuEntry_DisplayEvents() : DebugMenuEntry("") {}
    virtual void select(u32 _player) const;
    virtual const String8 getName(u32 _player) const;
};
#endif

//-----------------------------------------------------------------------
class DebugMenuEntry_DebugDog : public DebugMenuEntry
{
public:
	DebugMenuEntry_DebugDog() : DebugMenuEntry("") {}
	virtual void select(u32 _player) const;
	virtual const String8 getName(u32 _player) const;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_DisplaySoundBank : public DebugMenuEntry
{
public:
    DebugMenuEntry_DisplaySoundBank() : DebugMenuEntry("") {}
    virtual void select(u32 _player) const;
    virtual const String8 getName(u32 _player) const;
};

//-----------------------------------------------------------------------
class RumbleManagerDebugMenu_Entry : public DebugMenuEntry
{
public:
    enum EntryId
    {
        ConfigSelect,
        IncremValue,
        DecremValue,
    };
    RumbleManagerDebugMenu_Entry(const String8 & _name, EntryId _id ) : DebugMenuEntry(_name), m_id(_id)
    {}
    virtual void select(u32 _playerIndex) const;
    virtual const String8 getName(u32 _player) const;
private :
    EntryId m_id;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_ChangeAppFPS : public DebugMenuEntry
{
public:
    DebugMenuEntry_ChangeAppFPS(u32 _fps);
    virtual void select(u32 _index) const;
private:
    u32             m_fps;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_ChangeLogicFPS : public DebugMenuEntry
{
public:
    DebugMenuEntry_ChangeLogicFPS(u32 _fps);
    virtual void select(u32 _index) const;
private:
    u32             m_fps;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_MapURL: public DebugMenuEntry
{
public:
    DebugMenuEntry_MapURL(const String8 & _name) : DebugMenuEntry(_name) {}
    virtual void select(u32 _unused) const;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_SaveCategoryMememory: public DebugMenuEntry
{
public:
    DebugMenuEntry_SaveCategoryMememory(const String8 & _name) : DebugMenuEntry(_name) {}
    virtual void select(u32 _unused) const;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_BloombergTriggerFlag: public DebugMenuEntry
{
public:
    DebugMenuEntry_BloombergTriggerFlag() : DebugMenuEntry("Bloomberg Trigger Flag") {}
    virtual void select(u32 _unused) const;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_BloombergTriggerCrash: public DebugMenuEntry
{
public:
    DebugMenuEntry_BloombergTriggerCrash() : DebugMenuEntry("Bloomberg Trigger Crash") {}
    virtual void select(u32 _unused) const;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_ForceCrash: public DebugMenuEntry
{
public:
    DebugMenuEntry_ForceCrash() : DebugMenuEntry("Force Crash") {}
    virtual void select(u32 _unused) const;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_ToggleZPrepass : public DebugMenuEntry
{
public:
    DebugMenuEntry_ToggleZPrepass() : DebugMenuEntry("") {}
    virtual void select(u32 _player) const;
    virtual const String8 getName(u32 _player) const;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_LoadCheckpoint : public DebugMenuEntry
{
public:
    DebugMenuEntry_LoadCheckpoint() : DebugMenuEntry("Hot Reload") {}
    virtual void select(u32 _player) const;
};

//-----------------------------------------------------------------------
class DebugMenuEntry_DisplayDebugInfo : public DebugMenuEntry
{
public:
    DebugMenuEntry_DisplayDebugInfo() : DebugMenuEntry("") {}
    virtual void select(u32 _player) const;
    virtual const String8 getName(u32 _player) const;
};

#pragma endregion


#pragma region Implementations

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_DisplayControllerInput::select(u32 _player) const
{
    u32 controllerId = UI_MENUMANAGER->getDebugMenu().getDisplayedController();
    if (controllerId == U32_INVALID)
    {
        const u32 mainControllerId = GAMEMANAGER->getMainIndexPlayerControllerId();
        UI_MENUMANAGER->getDebugMenu().setDisplayedController(mainControllerId);
    }
    else
    {
        UI_MENUMANAGER->getDebugMenu().setDisplayedController(U32_INVALID);
    }
}

//-------------------------------------------------------------------------------------------------
const String8 DebugMenuEntry_DisplayControllerInput::getName( u32 _player ) const
{
    if (UI_MENUMANAGER->getDebugMenu().getDisplayedController() == U32_INVALID)
        return "Display main controller state";
    else
        return "Hide main controller state";
}

#if ITF_DEBUGEVENTS
//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_DisplayEvents::select(u32 _player) const
{
    EVENTMANAGER->setEventDebugMode(!EVENTMANAGER->isInEventDebugMode());
}

//-------------------------------------------------------------------------------------------------
const String8 DebugMenuEntry_DisplayEvents::getName( u32 _player ) const
{
    if (EVENTMANAGER->isInEventDebugMode())
        return "Deactivate event debug mode";
    else
        return "Activate event debug mode";
}
#endif

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_DebugDog::select(u32 _player) const
{
	W1W_GAMEMANAGER->setDogDebugMode( !W1W_GAMEMANAGER->isInDogDebugMode());
}

//-------------------------------------------------------------------------------------------------
const String8 DebugMenuEntry_DebugDog::getName( u32 _player ) const
{
	if ( W1W_GAMEMANAGER->isInDogDebugMode())
		return "Deactivate dog debug mode";
	else
		return "Activate dog debug mode";
}


//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_DisplaySoundBank::select(u32 _player) const
{
	if(AUDIO_ADAPTER)
		AUDIO_ADAPTER->setInDebugMode(!AUDIO_ADAPTER->isInDebugMode());
}

//-------------------------------------------------------------------------------------------------
const String8 DebugMenuEntry_DisplaySoundBank::getName( u32 _player ) const
{
    if (AUDIO_ADAPTER && !AUDIO_ADAPTER->isInDebugMode())
        return "Display Sound Banks loaded";
    else
        return "Hide Sound Banks loaded";
}

//-------------------------------------------------------------------------------------------------
DebugMenuEntry_ChangeAppFPS::DebugMenuEntry_ChangeAppFPS(u32 _fps): DebugMenuEntry(""), m_fps(_fps)
{
	m_name.setTextFormat("App loop %dfps", _fps);
}

void DebugMenuEntry_ChangeAppFPS::select(u32 _index) const
{
	Singletons::get().setAppLoopFPS(static_cast<f32>(m_fps));
}

//-------------------------------------------------------------------------------------------------
DebugMenuEntry_ChangeLogicFPS::DebugMenuEntry_ChangeLogicFPS(u32 _fps): DebugMenuEntry(""), m_fps(_fps)
{
	if (m_fps == 0)
		m_name.setTextFormat( "Game logic fps not fixed ");
	else
		m_name.setTextFormat("Game logic %dfps", _fps);
}
void DebugMenuEntry_ChangeLogicFPS::select(u32 _index) const
{
	if (m_fps == 0)
		Singletons::get().setUseFixDt( bfalse );
	else
	{
		Singletons::get().setUseFixDt( btrue );
		Singletons::get().setLogicFPS(static_cast<f32>(m_fps));
	}
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_MapURL::select(u32 _unused) const
{
	String8 url;
	CHEATMANAGER->getCurrentMapURL(url);
	LOG(url.cStr());
#if defined(ITF_CAFE) && defined(FILE_CAFE_SD)
	hwFile f;
	if( f.openSdCafe("mapurl.txt","a"))
	{
		for(int i = 0; i < 2; ++i)
			url+="\r\n";
		f.write(url.cStr(), url.getLen());
		f.close();
	}
#endif // ITF_CAFE

#if defined (ITF_PS3) || defined (ITF_X360)
#ifdef ITF_PS3
	Path file_url("/app_home/mapurl.txt");
#else
	Path file_url("e://mapurl.txt");
#endif //ITF_PS3

	file_url.setFlag(Path::FlagNonData, btrue);

	u32 creationFlags = FILEMANAGER->fileExists(file_url) ? 0 : ITF_FILE_ATTR_CREATE_NEW;

	File* pFile = FILEMANAGER->openFile(file_url,creationFlags |ITF_FILE_ATTR_WRITE);
	if (pFile)
	{
		if (creationFlags == 0)
			pFile->seek(pFile->getLength(),0);

		for(int i = 0; i < 2; ++i)
			url+="\r\n";
		pFile->write(url.cStr(), url.getLen());

		FILEMANAGER->closeFile(pFile);
	}
#endif //ITF_PS3 && ITF_X360
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_SaveCategoryMememory::select(u32 _unused) const
{
#if defined (ITF_X360) && defined  (ITF_CATEGORY_MEMORY_ENABLE) //only avaible on hdd version
	Path catfile("d:\\categoryMem.csv");
	catfile.setFlag(Path::FlagNonData,btrue);
	String8 tmp;
	memoryStatsManager::get().dumpCategoryStatsExcel(tmp);

	File* file = FILEMANAGER->openFile(catfile,ITF_FILE_ATTR_WRITE | ITF_FILE_ATTR_CREATE_NEW);
    if (file)
    {
        file->write(tmp.cStr(),tmp.getLen());
        FILEMANAGER->closeFile(file);
    }
#endif //ITF_X360
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_BloombergTriggerFlag::select( u32 _unused ) const
{
	BLOOMBERGINTERFACE->TriggerFlag();
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_BloombergTriggerCrash::select( u32 _unused ) const
{
    BLOOMBERGINTERFACE->TriggerCrash(::Bloomberg::APPL_EXCEPTION, "Trigger a crash for a Bloomberg test.");
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_ForceCrash::select( u32 _unused ) const
{
    *(long*) 2 = 2;
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_ToggleZPrepass::select(u32 _player) const
{
    const bbool value = GFX_ADAPTER->useZPrepassByDefault();
    GFX_ADAPTER->setUseZPrepassByDefault(!value);
}

//-------------------------------------------------------------------------------------------------
const String8 DebugMenuEntry_ToggleZPrepass::getName( u32 _player ) const
{
    if (GFX_ADAPTER->useZPrepassByDefault())
        return "Disable Z prepass";
    else
        return "Enable Z prepass";
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_LoadCheckpoint::select(u32 _player) const
{
    GAMEMANAGER->checkpointLoad();
    UI_MENUMANAGER->setIsInDBGMenu(bfalse,_player);
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_DisplayDebugInfo::select(u32 _player) const
{
    CONFIG->m_hideDebugInfo = !CONFIG->m_hideDebugInfo;
}

//-------------------------------------------------------------------------------------------------
const String8 DebugMenuEntry_DisplayDebugInfo::getName( u32 _player ) const
{
    if (CONFIG->m_hideDebugInfo)
        return "Show debug info";
    else
        return "Hide debug info";
}

#pragma endregion


//-------------------------------------------------------------------------------------------------
void UIDebugMenu::addDebugMenu_System()
{
	DebugMenu* menu = newAlloc(mId_UI, DebugMenu("System"));

    menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_LoadCheckpoint()));
#if defined(ITF_CONSOLE) || defined(ITF_PC)
    menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_DisplayDebugInfo()));
#endif
    menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_DisplayControllerInput()));
#if ITF_DEBUGEVENTS
    menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_DisplayEvents()));
#endif
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_DebugDog()));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_DisplaySoundBank()));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_MapURL("Log map URL")));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_ChangeAppFPS(60)));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_ChangeAppFPS(50)));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_ChangeAppFPS(30)));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_ChangeLogicFPS(60)));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_ChangeLogicFPS(50)));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_ChangeLogicFPS(30)));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_ChangeLogicFPS(0)));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_SaveCategoryMememory("Save category memory")));
	menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_BloombergTriggerFlag()));
    menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_BloombergTriggerCrash()));
    menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_ForceCrash()));
    menu->addEntry(newAlloc(mId_UI, DebugMenuEntry_ToggleZPrepass()));

	addDebugMenu( menu );
}

//-------------------------------------------------------------------------------------------------
void UIDebugMenu::addDebugMenu_MapList()
{
    DebugMenu* mapMenu = newAlloc(mId_UI, DebugMenu("Maps"));

    u32 count = CHEATMANAGER->getMapListCount();
    for(u32 i = 0; i < count; i++)
    {
        String8 mapName = CHEATMANAGER->getMapListName(count - 1 - i);
        Path mapPath = CHEATMANAGER->getMapListPath(count - 1 - i);

        bbool fileExists = FILEMANAGER->fileExists(mapPath);
        if (!fileExists && FILEMANAGER->isBundleSupported())
            fileExists = FILEMANAGER->fileExists( FILESERVER->getCookedName(mapPath) );
        if (!fileExists)
            mapName = String8("(X) ") + mapName;

        mapMenu->addEntry( newAlloc(mId_UI, DebugMenuEntry_ChangeMap(mapName, mapPath)), btrue, 0 );
    }

    addDebugMenu( mapMenu );
}

void UIDebugMenu::updateDBGMenuMemory()
{
    if(m_inDBGMemory)
    {
        const f32 R = 0.9f;
        const f32 G = 0.1f;
        const f32 B = 0.1f;

        String8 dump;

        //TEXTURE MEMORY
        u32 textureMemory=0, textureCount=0;
        RESOURCE_MANAGER->getMemoryUsage(Resource::ResourceType_Texture, textureMemory, textureCount);
#ifdef ITF_IOS
        u32 curMemUsage  =(u32)usedMemory();
        u32 freeMemUsage =(u32)freeMemory();
        dump.setTextFormat("Memory used %d MB , free %d MB,  (Texture:: Size %d MB, count %d) ", curMemUsage / 1024 / 1024,freeMemUsage/ 1024 / 1024,textureMemory/ 1024 / 1024 , textureCount );
        //LOG("Memory used %f MB , free %f MB,  Texture(Size %f MB, count %f) ", curMemUsage ,freeMemUsage,textureMemory , textureCount );
#else
         dump.setTextFormat("Texture(Size %d MB, count %d) ",textureMemory / 1024 / 1024, textureCount );
#endif
        GFX_ADAPTER->drawDBGText(dump, 300, 100, R, G, B);

        m_inDBGMemoryDisplay--;
        if(m_inDBGMemoryDisplay<3)
        {
            m_inDBGMemory = bfalse;
        }
    }
}

#pragma region DebugMenu
//=================================================================================================
// Debug Menu 
//=================================================================================================

//-------------------------------------------------------------------------------------------------
DebugMenu::DebugMenu( const String8 &_label )
    : m_parent(NULL)
    , m_label(_label)
    , m_selection(0)
{

}

//-------------------------------------------------------------------------------------------------

DebugMenu::~DebugMenu()
{
    clear();
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::clear()
{
    for (ITF_VECTOR<DebugMenuEntry* >::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
        if (*it)
            delete *it;
    m_entries.clear();
    m_selection = 0;
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::select( u32 _player /*= 0*/ )
{
    m_entries[m_selection]->select(_player);
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::onDirection( bool _left )
{
    m_entries[m_selection]->onDirection(_left);
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::up()
{
    u32 size = m_entries.size();
    m_selection = (size ? (size + m_selection - 1)%size : m_selection);
}

//-------------------------------------------------------------------------------------------------
u32 DebugMenu::getEntriesSize()
{
    return m_entries.size();
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::down()
{
    u32 size = m_entries.size();
    m_selection = (size ? (m_selection+1)%size : m_selection);
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::setEntryName( const char* _name, u32 _index )
{
    m_entries[_index]->setName(_name);
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::addEntry(DebugMenuEntry* _entry, bbool _unique, i32 _index /*= -1*/)
{
    if(_unique)
    {
        const String8& entryName = _entry->getName(0);
        for(ITF_VECTOR<DebugMenuEntry* >::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
        {
            if((*it)->getName(0) == entryName)
            {
                delete _entry;
                return;
            }
        }
    }

    _entry->setParent(this);
    if(_index != -1)
    {
        m_entries.insertAt(_index, _entry);
    }
    else
    {
        m_entries.push_back(_entry);
    }
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::updateLayout()
{
    if (!getTouchMode())
    {
        f32 currentX = 0.0f;
        f32 currentY = 0.0f;
        const f32 maxY = getSize().y() - lineHeight();
        for (u32 i = 0; i < m_entries.size(); ++i)
        {
            DebugMenuEntry* const entry = m_entries[i];

            entry->updateSize();

            entry->setRelativePos(Vec2d(currentX, currentY));
            currentY += lineHeight();

            // End of the column?
            if (currentY > maxY)
            {
                currentX += widthByCol;
                currentY = 0.0f;
            }
        }
    }
    else
    {
        f32 currentX = 0.0f;
        f32 currentY = 0.0f;
        Vec2d &size = m_screenRect.m_size;
        size = Vec2d::Zero;
        for (u32 i = 0; i < m_entries.size(); ++i)
        {
            DebugMenuEntry* const entry = m_entries[i];

            entry->updateSize();

            entry->setRelativePos(Vec2d(currentX, currentY));
            currentY += entry->getSize().y() + buttonSpacing();

            size.x() = f32_Max(size.x(), entry->getSize().x());
            size.y() += entry->getSize().y() + ((i == 0) ? 0.0f : buttonSpacing());
        }
    }
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::update()
{
    updateLayout();

    const u32 entryCount = m_entries.size();
    m_selection = Clamp<u32>(m_selection, 0, entryCount);

    for (u32 i = 0; i < entryCount; ++i)
    {
        DebugMenuEntry *entry = m_entries[i];
        entry->setSelected(i == m_selection);
        entry->update();
    }
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::display()
{
    if (m_entries.size() == 0)
        return;

    if (!m_parent->getTouchMode())
    {
        Vec2d minCoord, maxCoord;
        computeAABB(minCoord, maxCoord);
        GFX_ADAPTER->drawDBG2dBox(
            Vec2d(minCoord.x()-border, minCoord.y()-border), 
            maxCoord.x() - minCoord.x() + border *2,
            maxCoord.y() - minCoord.y() + border *2,
            0xBBcccccc, 0xBBcccccc,0xBBcccccc, 0xBBcccccc);
    }

    for (u32 i = 0; i < m_entries.size(); ++i)
    {
        DebugMenuEntry* const entry = m_entries[i];
        if (entry->isInRect(m_clippingRect))
            entry->display();
    }
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::computeAABB(Vec2d &_min, Vec2d &_max)
{
    if (m_entries.size() == 0)
    {
        _min = Vec2d::Zero;
        _max = Vec2d::Zero;
    }

    DebugMenuEntry &entry = *m_entries[0];
    const Vec2d &entryPos = entry.getScreenPos();
    const Vec2d &entrySize = entry.getSize();
    _min.x() = entryPos.x();
    _max.x() = entryPos.x() + entrySize.x();
    _min.y() = entryPos.y();
    _max.y() = entryPos.y() + entrySize.y();
    for (u32 i = 1; i < m_entries.size(); ++i)
    {
        DebugMenuEntry &entry = *m_entries[i];
        const Vec2d &entryPos = entry.getScreenPos();
        const Vec2d &entrySize = entry.getSize();
        _min.x() = f32_Min(_min.x(), entryPos.x());
        _max.x() = f32_Max(_max.x(), entryPos.x() + entrySize.x());
        _min.y() = f32_Min(_min.y(), entryPos.y());
        _max.y() = f32_Max(_max.y(), entryPos.y() + entrySize.y());
    }
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::removeEntry( u32 _index )
{
    DebugMenuEntry* entry = m_entries[_index];
    SF_DEL(entry);
    m_entries.removeAt(_index);

    if(m_selection >= _index)
    {
        m_selection = Min(m_selection, (u32)m_entries.size() - 1);
    }
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::onContactEnd( const Vec2d &_screenPos )
{
    for (u32 i = 0; i < m_entries.size(); ++i)
    {
        DebugMenuEntry* const entry = m_entries[i];
        if (entry->isPointInside(_screenPos))
        {
            setSelectionIndex(i);
            entry->select(getPlayer());
        }
    }
}

//-------------------------------------------------------------------------------------------------
void DebugMenu::setSelectionAt( const Vec2d &_screenPos )
{
    for (u32 i = 0; i < m_entries.size(); ++i)
    {
        DebugMenuEntry* const entry = m_entries[i];
        if (entry->isPointInside(_screenPos))
            setSelectionIndex(i);
    }
}

#pragma endregion

#pragma region DebugMenuEntry

//=================================================================================================
// DebugMenuEntry
//=================================================================================================

//-------------------------------------------------------------------------------------------------
DebugMenuEntry::DebugMenuEntry( const String8 & _name )
    : m_name(_name)
    , m_parent(NULL)
    , m_relativeRect(Vec2d::Zero, Vec2d(0.0f, lineHeight()))
    , m_screenRect(Vec2d::Zero, Vec2d(0.0f, lineHeight()))
    , m_selected(bfalse)
{

}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry::update()
{
    updateScreenPos();
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry::display()
{
    Vec2d pos(getScreenPos());

    if (getTouchMode())
    {
        GFX_ADAPTER->drawDBG2dBox(
            pos, 
            getSize().x(),
            getSize().y(),
            0xBBcccccc, 0xBBcccccc,0xBBcccccc, 0xBBcccccc);
        pos.x() += buttonMargin();
        pos.y() += buttonMargin();
    }

    f32 R,G,B;
    if (m_selected)
    {
        R = 0.9f;
        G = 0.9f;
        B = 0.9f;
    }
    else
    {
        R = 0.7f;
        G = 0.1f;
        B = 0.1f;
    }
    GFX_ADAPTER->drawDBGText(getName(getPlayer()), pos.x(), pos.y(), R, G, B, useBigFont());
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry::updateScreenPos()
{
    m_screenRect.m_position = m_parent->getScreenPos() + m_relativeRect.m_position;
    m_screenRect.m_size = m_relativeRect.m_size;
}

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry::updateSize()
{
    if (!getTouchMode())
    {
        getSize().x() = widthByCol;
        getSize().y() = lineHeight();
    }
    else
    {
        const String8 &name = getName(getPlayer());
        getSize().x() = name.getLen() * debugCharWidth() + 2 * buttonMargin();
        getSize().y() = debugCharHeight() + 2 * buttonMargin();
    }
}

#pragma endregion

//=================================================================================================
// RumbleManagerDebugMenu_Entry !!!
//=================================================================================================

//-------------------------------------------------------------------------------------------------
void RumbleManagerDebugMenu_Entry::select( u32 _player ) const
{
    switch( m_id )
    {
    case ConfigSelect : PADRUMBLEMANAGER->dbgMenuChangeConfig(); break;
    case IncremValue : PADRUMBLEMANAGER->dbgMenuIncremValue(); break;
    case DecremValue : PADRUMBLEMANAGER->dbgMenuDecremValue(); break;
    }        
}

//-------------------------------------------------------------------------------------------------
const String8 RumbleManagerDebugMenu_Entry::getName(u32 _player) const
{
    String8 temp = "";

    switch( m_id )
    {
    case ConfigSelect : 
        {
            temp = "config : ";
            temp = temp + PADRUMBLEMANAGER->dbgMenuGetCurConfig();
            break;
        }
    case IncremValue : 
        {
            temp = "(+) ";
            temp = temp + PADRUMBLEMANAGER->dbgMenuGetCurValue();
            break;
        }
    case DecremValue : 
        {
            temp = "(-) ";
            temp = temp + PADRUMBLEMANAGER->dbgMenuGetCurValue();
            break;
        }
    }
    return temp;
}

//=================================================================================================
// DebugMenuEntry_ChangeMap !!!
//=================================================================================================

//-------------------------------------------------------------------------------------------------
void DebugMenuEntry_ChangeMap::select(u32 _playerIndex) const
{
    bbool fileExists = FILEMANAGER->fileExists(m_mapPath);
    if (!fileExists && FILEMANAGER->isBundleSupported())
        fileExists = FILEMANAGER->fileExists( FILESERVER->getCookedName(m_mapPath) );
    if (fileExists)
    {
        GAMEMANAGER->loadMap(m_mapPath);
        UI_MENUMANAGER->setIsInDBGMenu(bfalse,_playerIndex);
    }
}

//=================================================================================================
// ChangeCheckpoint_Entry !!!
//=================================================================================================

//-------------------------------------------------------------------------------------------------
void ChangeCheckpoint_Entry::select( u32 _player ) const
{
    GAMEMANAGER->setCurrentCheckpoint(m_checkpoint);
    GAMEMANAGER->teleportToCurrentCheckpoint(btrue);
}

} // namespace ITF

#endif // ITF_SUPPORT_CHEAT
