#include "precompiled_gameplay.h"

#ifndef _ITF_UIMENUCONTROLLERS_H_
#include "gameplay/components/UI/UIMenuControllers.h"
#endif //_ITF_UIMENUCONTROLLERS_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTOR_BIND_H_
#include "engine/actors/ActorBind.h"
#endif // _ITF_ACTOR_BIND_H_

#ifdef ITF_CAFE
    #ifndef _ITF_INPUTADAPTER_CAFE_H_
    #include "adapters/InputAdapter_Cafe/InputAdapter_Cafe.h"
    #endif //_ITF_INPUTADAPTER_CAFE_H_
#endif

namespace ITF
{
#define MenuController_ID			ITF_GET_STRINGID_CRC(menuControllers, 1401511430)
#define MenuController_DRC			ITF_GET_STRINGID_CRC(uicontroller_drc,2510977016)
#define MenuController_Nunchuk		ITF_GET_STRINGID_CRC(uicontroller_nunchuk,611708846)
#define MenuController_SideWay		ITF_GET_STRINGID_CRC(uicontroller_wiimote,1457945320)
#define MenuController_URCC			ITF_GET_STRINGID_CRC(uicontroller_propad,4095357898)
#define MenuController_Classic		ITF_GET_STRINGID_CRC(uicontroller_classic,4273471032)


    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIMenuControllers)
    BEGIN_SERIALIZATION_CHILD(UIMenuControllers)
    END_SERIALIZATION()

    UIMenuControllers::UIMenuControllers()
        : m_connectedControllers    (0)
        , m_lastNumControllers      (0)
        , m_isActive                (bfalse)
        , m_typeChanged             (bfalse)
        , m_isControllersArrayFilled(bfalse)
    {
        
    }

    UIMenuControllers::~UIMenuControllers()
    {
        
    }

    void UIMenuControllers::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        u32 maxControllerTypes = getTemplate()->getControllerVectorSize();
        ITF_WARNING(GetActor(), maxControllerTypes == 5, "UIMenuController couldn't load all Controller Layouts (%d/%d)", getTemplate()->m_ControllerLayouts.size(), 5);

        // init controllers
        u32 maxControllers = 5; //getTemplate()->getControllerCount();
        if( maxControllers > 0 )
        {
            m_controllers.resize( maxControllers );
            for(u32 i = 0; i < maxControllers; ++i)
            {
                m_controllers[i].m_Connected = 0;
            }
        }

        m_players.resize(GAMEMANAGER->getMaxLocalPlayers());

        setCanValidate(bfalse);
    }

    void UIMenuControllers::onBecomeActive()
    {
        resetInitState(btrue);
        Super::onBecomeActive();
       
        m_lastNumControllers = 0;
        m_isActive = btrue;
    }

    void UIMenuControllers::onBecomeInactive()
    {
        Super::onBecomeInactive();

        m_isActive = bfalse;

        for(u32 i = 0; i < m_players.size(); ++i)
        {
            m_players[i].m_Connected = bfalse;
        }

        for(u32 i = 0; i < m_controllers.size(); ++i)
        {
            m_controllers[i].m_Connected = 0;
        }
    }

    void UIMenuControllers::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        if(!m_isActive)
        {
            return;
        }

        if(!m_isControllersArrayFilled)
        {
            //if ( UIMenu * menu = UI_MENUMANAGER->getUIMenu(MenuController_ID) )
            {
                StringID uiComponentController = StringID::InvalidId;
                for(u32 i = 0; i < m_controllers.size(); i++)
                {
                    // TEMP FOR NOW:
                    StringID uiComponentId;
                    switch( i/*getTemplate()->getControllerName(i)*/ )
                    {
                    case 0: 
                        uiComponentId = MenuController_DRC;	
                        #ifdef ITF_CAFE_DRC
                        m_controllers[i].m_padType = InputAdapter::Pad_CafeDRC; 
                        #else
                        m_controllers[i].m_padType = InputAdapter::Pad_X360; 
                        #endif // ITF_CAFE_DRC
                        break;

                    case 1:
                        uiComponentId = MenuController_Nunchuk;
                        m_controllers[i].m_padType = InputAdapter::Pad_WiiNunchuk; 
                        break;
                    case 2:
                        uiComponentId = MenuController_SideWay;
                        m_controllers[i].m_padType = InputAdapter::Pad_WiiSideWay; 
                        break;
                    case 3:
                        uiComponentId = MenuController_URCC;
                        m_controllers[i].m_padType = InputAdapter::Pad_CafeURCC;	 
                        break;
                    case 4:
                        uiComponentId = MenuController_Classic;
                        m_controllers[i].m_padType = InputAdapter::Pad_WiiClassic; 
                        break;			
                    }


                    if (UIComponent* component = getChildComponent( uiComponentId /*getTemplate()->getControllerName(i) */))
                    {
                        m_controllers[i].m_ActorRef		= component->GetActor()->getRef();
                        m_controllers[i].m_isLoading	= bfalse;

                        
                    }
                }

                m_isControllersArrayFilled = btrue;
            }			
        }

        // poll for new controllers
        if(m_connectedControllers > 0)
        {
            UIMenuControllers_Template::ControllerLayout curLayout = getTemplate()->m_ControllerLayouts[m_connectedControllers -1];
            u32 connectedControllerPosition = 0;
            for(u32 i = 0; i < m_controllers.size(); ++i)
            {
                if(m_controllers[i].m_isLoading)
                {
                    Actor* curAct = m_controllers[i].m_ActorRef.getActor();
                    if(curAct && !curAct->isAsyncLoading())
                    {
                        // loaded screen sizes are for default resolution, need to be scaled apropriately
                        Vec2d newPos = curLayout.m_position[connectedControllerPosition] * m_actor->getScale();
                        Vec2d newScale = curLayout.m_scale[connectedControllerPosition] * m_actor->getScale();

                        /* * oldResolution ; newPos = newPos * m_actor->getScale() * Vec2d(g_pixelBy2DUnit, -g_pixelBy2DUnit);*/

                        curAct->setLocalInitialPos2D(newPos);
                        curAct->set2DPos(newPos);

                        curAct->setDepth(f32(getAbsoluteDepth()) + 1.0f);

                        curAct->setLocalInitialScale(newScale);
                        curAct->setScale(newScale);

                        curAct->createParentBind(bfalse, m_actor, Bind::Root, i, btrue, bfalse, bfalse, bfalse);

                        m_controllers[i].m_isLoading = bfalse;

                        UIComponent* uiComponent = curAct->GetComponent<UIComponent>();
                        if(uiComponent)
                        {
                            uiComponent->resetInitState(btrue);
                            uiComponent->setIsDisplay(btrue);
                        }

                        connectedControllerPosition++;
                    }
                }
            }
        }

        checkForControllers();

        // if something changed, reposition the actors on screen
        if(m_lastNumControllers != m_connectedControllers || m_typeChanged)
        {
            positionControllers();
            m_lastNumControllers = m_connectedControllers;
            m_typeChanged = bfalse;
        }
    }



    void UIMenuControllers::positionControllers()
    {
        if(m_connectedControllers > 0)
        {
            despawnControllerActors();
        }
        
        checkForControllers();

        if(m_connectedControllers == 0)
            return;
        
        for(u32 i = 0; i < m_controllers.size(); ++i)
        {
            if(!m_controllers[i].m_Connected)
                continue;

            m_controllers[i].m_isLoading = btrue;;
        }
    }

    void UIMenuControllers::despawnControllerActors()
    {
        for(u32 i = 0; i < m_controllers.size(); ++i)
        {			
            Actor* curAct = m_controllers[i].m_ActorRef.getActor();
            if(curAct && !curAct->isAsyncLoading())
            {
                UIComponent* uiComponent = curAct->GetComponent<UIComponent>();
                if(uiComponent)
                {
                    uiComponent->setIsDisplay(bfalse);
                }
            }
        }		
    }

    void UIMenuControllers::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
    }

    void UIMenuControllers::onStartDestroy( bbool _hotReload )
{
        Super::onStartDestroy(_hotReload);

        despawnControllerActors();
    }

    void UIMenuControllers::checkForControllers()
    {
        m_connectedControllers = 0;
        
#ifdef ITF_CAFE
        for(u32 i = 0; i < m_players.size(); ++i)
        {
            if(Player* curPlayer = GAMEMANAGER->getPlayer(i))
            {
                // check for inactive players: RO2-18524
                if(!curPlayer->getActive())
                    continue;

                u32 padId = curPlayer->getControllerId();

                // Auto rayman or fake?
                if(padId == U32_INVALID || padId == 999)
                    continue; 

                bbool curState = INPUT_ADAPTER->isPadConnected(padId);

                if ( m_players[i].m_Connected != curState )
                {
                    m_players[i].m_Connected = curState;
                    
                    // Connected? 
                    u32 id = getInternalControllerIdFromControllerId( padId );
                    if( id != U32_INVALID )
                    {
                        if( curState )
                            m_controllers[id].m_Connected ++;
                        else
                            m_controllers[id].m_Connected --;	

                        m_typeChanged = btrue;
                    }				
                } else if ( m_players[i].m_padType != INPUT_ADAPTER->getPadType(padId) )
                {

                    u32 oldid = getInternalControllerIdFromControllerId( 0, m_players[i].m_padType );
                    if( oldid != U32_INVALID )
                    {
                        m_controllers[oldid].m_Connected --;	
                    }

                    u32 newid = getInternalControllerIdFromControllerId( 0, INPUT_ADAPTER->getPadType(padId) );
                    if( newid != U32_INVALID )
                    {
                        m_controllers[newid].m_Connected ++;	
                    }

                    m_typeChanged = btrue;
                }

                m_players[i].m_padType = INPUT_ADAPTER->getPadType(padId);
            }
        }

        for(u32 i = 0; i < m_controllers.size(); ++i)
        {
            if(!m_controllers[i].m_Connected)
                continue;

            m_connectedControllers++;
        }
#else
        for(u32 i = 0; i < m_controllers.size(); ++i)
        {
            m_controllers[i].m_Connected = btrue;

            m_connectedControllers++;
            
        }
#endif
    }

    u32 UIMenuControllers::getInternalControllerIdFromControllerId(u32 _controllerId, InputAdapter::PadType _padType)
    {
        InputAdapter::PadType padType = _padType == InputAdapter::Pad_Invalid ? INPUT_ADAPTER->getPadType(_controllerId) : _padType;
#ifdef ITF_CAFE
        if(padType == InputAdapter::Pad_WiiClassic && ( INPUT_ADAPTER_CAFE->isURCC(_controllerId) || INPUT_ADAPTER_CAFE->wasURCC(_controllerId) ) )
            padType =  InputAdapter::Pad_CafeURCC;		
#endif

        for( u32 i = 0; i < m_controllers.size(); i++)
        {
            if(m_controllers[i].m_padType == padType)
            {				
                return i;
            }
        }
        return U32_INVALID;
    }




    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(UIMenuControllers_Template)
    BEGIN_SERIALIZATION_CHILD(UIMenuControllers_Template)
        SERIALIZE_PROPERTY_NAME("ControllerLayouts"); 
        SERIALIZE_CONTAINER_OBJECT("controllerLayout", m_ControllerLayouts)

        SERIALIZE_CONTAINER("controllerActorPaths", m_ControllerActorPaths)
        SERIALIZE_CONTAINER("controllerActorNames", m_ControllerActorNames)
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(UIMenuControllers_Template, ControllerLayout)
        SERIALIZE_MEMBER("numControllers", m_numControllers);
        SERIALIZE_CONTAINER("positions", m_position);
        SERIALIZE_CONTAINER("scales", m_scale);
    END_SERIALIZATION()

    UIMenuControllers_Template::UIMenuControllers_Template()
    {  

    }

    
    UIMenuControllers_Template::~UIMenuControllers_Template()
    {    
    }

    bbool UIMenuControllers_Template::onTemplateLoaded( bbool _hotReload )
    {
        return Super::onTemplateLoaded(_hotReload);
    }


}
