#include "precompiled_engine.h"

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "gameplay/managers/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    FixedArray<CameraControllerManager*,2> CameraControllerManager::s_CCManagerInstance;   
    static const f32 s_autoRampUpCoeffIncr = 0.015f;

    CameraControllerManager::CameraControllerManager()
        : m_camSubjectList(10)
        , m_camModifierList(50)
        , m_camControllerList(10)
        , m_camSubjectList_Active(10)        
        , m_rampUpCoeff(0.f)
        , m_pauseMode(bfalse)
        , m_ignoreShake(bfalse)
        , m_ignoreShakeFromCageBehaviour(bfalse)
        , m_shakeCountdown(0.0f)
        , m_shake()
        , m_shakeConfig(NULL)
        , m_defaultDepth(13.f)
#ifdef ITF_SUPPORT_DEBUGFEATURE
        , m_debugDrawInfo(bfalse)
        , m_debugDrawScreen4By3(None)
        , m_shakeDebugDraw(bfalse)
        , m_debugDrawModifiers(bfalse)
#endif //ITF_SUPPORT_DEBUGFEATURE
        , m_isInGameCameraJustReseted(bfalse)
        , m_isCameraJustTeleported(bfalse)
        , m_screenRatio(1.f)
        , m_wantedScreenRatio(1.f)

#ifdef ITF_SUPPORT_CHEAT
        , m_isCheatDezoomMax(bfalse)
        , m_cheatZoom(0.f)
#endif //ITF_SUPPORT_CHEAT
        
        , m_updateCamSubjectList_Active(bfalse)
        , m_updateCamControllers_SubjectList(bfalse)
        , m_id(CAMID_MAIN)
        , m_mainPriority(1)
        , m_autoRampUpCoeff_IsActive(bfalse)        
        , m_autoRampUpCoeff_LengthSqr(0.f)
        , m_setZoneNeutralDest(bfalse)
    {
        EVENTMANAGER_REGISTER_EVENT_LISTENER(EventRegisterCameraSubject_CRC,this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(EventCameraShake_CRC,this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(EventCameraIgnoreShake_CRC,this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(EventSingletonConfigChanged_CRC,this);

        m_cameraSequence.m_camSubjectList.reserve(10);

#ifdef DEVELOPER_JAY_CAMERA
#ifndef ITF_FINAL
        m_debugDrawInfo = btrue;
        m_debugDrawModifiers = btrue;
#endif 
#endif
    }

    CameraControllerManager::~CameraControllerManager()
    {
        if (TEMPLATEDATABASE && ( m_shakeConfig != NULL ) )
        {
            TEMPLATEDATABASE->releaseTemplate( m_shakeConfig->getFile() );
        }

        EVENTMANAGER->unregisterEvent(EventRegisterCameraSubject_CRC, this);
        EVENTMANAGER->unregisterEvent(EventCameraShake_CRC, this);
        EVENTMANAGER->unregisterEvent(EventCameraIgnoreShake_CRC, this);
        EVENTMANAGER->unregisterEvent(EventSingletonConfigChanged_CRC, this);
    }  

    void CameraControllerManager::init()
    {
        // load camera shake config file
        const Path& configFile = GAMEMANAGER->getCameraShakeConfig();

        if ( !configFile.isEmpty() )
        {
            m_shakeConfig = TEMPLATEDATABASE->requestTemplate<CameraShakeConfig_Template>(configFile);
            if (!m_shakeConfig)
            {
                ITF_FATAL_ERROR("Couldn't load camera shake config: %s", configFile.toString8().cStr());
            }
        }

        m_bindedObjects.clear();
        m_bindCameraMoveDelta.set( 0.f,0.f,0.f );
        m_bindLastCameraPos.set( 0.f,0.f,0.f );
        
        Camera * cam = getCamera();
        m_camData.m_pos = cam->getPos();
        ITF_ASSERT_CRASH( m_camData.m_pos.isValid(), "Invalid position");
    }

    Camera * CameraControllerManager::getCamera() const
    {
        Camera * cam = View::getGameplayView()->getCamera();

#ifdef ITF_USE_REMOTEVIEW
        if (this == CameraControllerManager::getManagerFromId(CAMID_REMOTE))
            cam = View::getRemoteView()->getCamera();
#endif // ITF_USE_REMOTEVIEW

        return cam;
    }

    void CameraControllerManager::setTeleported()
    {
        m_isCameraJustTeleported = btrue;
        resetInGameCamera();
    
        // notify the controllers
        const u32 camControllerCount = m_camControllerList.size();   
        for (u32 i = 0; i < camControllerCount; i++)
        {
            CameraControllerData& data = m_camControllerList[i];
            if (data.m_controller)
            {
                data.m_controller->teleport(m_camData.m_pos);
                data.m_destinationWeight = data.m_finalWeight;
                data.m_weight = data.m_finalWeight;
                data.m_prevSpeed = Vec3d::Zero;
                data.m_prevCamData.m_pos = m_camData.m_pos;
            }
            else
                data.m_weight = 0.f;
        }

        // notify the modifiers
        for(u32 i= 0; i < m_camModifierList.size(); ++i)
        {
            m_camModifierList[i]->resetExtension();
        }

        // reset mix
        m_rampUpCoeff = 1.f; 

        // reset sequence
        stopCameraSequence(1.f);
       
        // reset shake
        resetShake();

        activeAutoRampUpCoeff(bfalse);
    }

    void CameraControllerManager::teleport(const Vec3d& _newPos)
    {
        ITF_ASSERT_CRASH( _newPos.isValid(), "Invalid position");

        setIgnoreShakeCamFromCageBehaviour(bfalse);
        updateCameraControllerSubjectActiveList();        

        m_camData = CameraData(); // reset current camera data
        m_camData.m_pos = _newPos; // init position, must be done before function setTeleported()

        setTeleported(); // must be called before function predictCameraDataAfterTeleport() --> teleport position will change

        // predict position with camera modifiers settings
        predictCameraDataAfterTeleport( m_camData );
        
        // apply predict position to current view immediately, will update view frustrum
        applyCameraData();

        // As we telported the camera, we'll skip next rendering to avoid popping objects
        ITF::SYSTEM_ADAPTER->setSkipNextRendering(btrue);
    }

    void CameraControllerManager::preApplyCameraPos(const Vec3d& _newPos)
    {
        ITF_ASSERT_CRASH( _newPos.isValid(), "Invalid position");
        m_camData.m_pos = _newPos;
        
        // apply predict position to current view immediately, will update view frustrum
        applyCameraData();
    }

    void CameraControllerManager::updateScreenRatio()
    {
        m_screenRatio =  View::getGameplayView()->getRatio();
        m_wantedScreenRatio =  View::getGameplayView()->getWantedRatio();

#ifdef ITF_SUPPORT_DEBUGFEATURE
        if ( isDebugDrawScreen4By3() )
        {
            m_screenRatio = Min( m_screenRatio, 1.333333f);
        }
#endif
    }

    void CameraControllerManager::updateControllers(f32 _dt)
    {
        // update or remove camera controllers
        for (i32 i = 0; i < (i32)m_camControllerList.size(); i++)
        {
            const CameraControllerData& data = m_camControllerList[i];

            if (data.m_controller)
            {
                if ( isCameraControllerActive(data.m_controller))
                {
                    data.m_controller->onManagerUpdate(_dt);
                }
            }
            else if ( !cameraControllerHasInfluence(data) )
            {
                m_camControllerList.eraseNoOrder(i);
                i--;
            }
        }
    }

    void CameraControllerManager::activeAutoRampUpCoeff( bbool _isActive )
    {                
        m_autoRampUpCoeff_LengthSqr = 0.f;
        m_autoRampUpCoeff_IsActive  = _isActive && m_rampUpCoeff <1.f;
    }
    
    void CameraControllerManager::updateAutoRampUpCoeff( const CameraData& _data )
    {                
        if ( m_autoRampUpCoeff_IsActive )
        {
            const f32 curLengthSqr = ( _data.m_pos - m_camData.m_pos ).sqrnorm();

            if ( m_autoRampUpCoeff_LengthSqr > 0.f && curLengthSqr > m_autoRampUpCoeff_LengthSqr +MTH_EPSILON )
            {                
                m_rampUpCoeff = f32_Min( 1.f, m_rampUpCoeff +s_autoRampUpCoeffIncr ); 
            }

            m_autoRampUpCoeff_LengthSqr = curLengthSqr;
        }
    }

    void CameraControllerManager::setCameraData( f32 _dt )
     {
        CameraData mainCamData;        
        const Vec3d prevCamPosInGame = m_camData.m_pos;   
        const f32 oneOverDt = _dt > 0.f ? 1.f / _dt : 0.f;

        // mix the cameras (components)
        if ( m_camControllerList.size() )
        {
            updateCameraSequenceWeight();

            m_camData.m_pos         = m_cameraSequence.m_camData.m_pos * m_cameraSequence.m_weight;
            m_camData.m_focale      = m_cameraSequence.m_camData.m_focale * m_cameraSequence.m_weight;
            m_camData.m_depth       = m_cameraSequence.m_camData.m_depth * m_cameraSequence.m_weight;
            m_camData.m_lookAtDir   = m_cameraSequence.m_camData.m_lookAtDir * m_cameraSequence.m_weight;
            m_camData.m_upDir       = m_cameraSequence.m_camData.m_upDir * m_cameraSequence.m_weight;
            m_camData.m_deltaFogZ   = m_cameraSequence.m_camData.m_deltaFogZ * m_cameraSequence.m_weight;
			m_camData.m_UpDnAngle	= m_cameraSequence.m_camData.m_UpDnAngle;
            m_camData.m_offsetHVS   = m_cameraSequence.m_camData.m_offsetHVS * m_cameraSequence.m_weight;
			m_camData.m_horizontalVersusVertical = m_cameraSequence.m_camData.m_horizontalVersusVertical * m_cameraSequence.m_weight;
            m_camData.m_flagMirror  = 0;

            f32 totalWeight = m_cameraSequence.m_weight;                

            for (u32 i = 0; i < m_camControllerList.size(); i++)
            {
                CameraControllerData& data = m_camControllerList[i];

                data.m_destinationWeight = data.m_destinationWeight + (data.m_finalWeight - data.m_destinationWeight) * m_rampUpCoeff;
                data.m_weight = data.m_weight + (data.m_destinationWeight - data.m_weight) * m_rampUpCoeff;
                totalWeight += data.m_weight;
                
                CameraData curCamData; // to reset all values for each controller

                if (data.m_controller)
                {
                    data.m_controller->getCameraData(curCamData);

                    if ( data.hasPrevData() )
                        data.m_prevSpeed = (curCamData.m_pos - data.m_prevCamData.m_pos) * oneOverDt;
                    else
                        data.m_prevSpeed = Vec3d::Zero;

                    data.m_prevCamData = curCamData;      

                    if ( isMainCameraControllerData(data) )
                    {
                        mainCamData = curCamData;
                    }
                }
                else // cam was de-registered but still has influence: let it drift
                {
                    ITF_ASSERT_CRASH(data.hasPrevData(), "Cam registered and unregistered in same trame");

                    data.m_prevCamData.m_pos += data.m_prevSpeed * _dt;
                    curCamData = data.m_prevCamData;
                }

                ITF_ASSERT_CRASH( curCamData.m_pos.isValid(), "Invalid position");

                m_camData.m_pos						 += curCamData.m_pos * data.m_weight;
                m_camData.m_lookAtDir				 += curCamData.m_lookAtDir * data.m_weight;
                m_camData.m_upDir					 += curCamData.m_upDir * data.m_weight;
                m_camData.m_focale					 += curCamData.m_focale * data.m_weight;
                m_camData.m_depth					 += curCamData.m_depth * data.m_weight;
                m_camData.m_deltaFogZ				 += curCamData.m_deltaFogZ * data.m_weight;
                m_camData.m_horizontalVersusVertical += curCamData.m_horizontalVersusVertical * data.m_weight;
                m_camData.m_offsetHVS                += curCamData.m_offsetHVS * data.m_weight;
				m_camData.m_UpDnAngle				 += curCamData.m_UpDnAngle;

                m_camData.m_flagMirror  |= curCamData.m_flagMirror;
            }

            if ( totalWeight > 0.f )
            {
                const f32 oneOverWeight = 1.0f/totalWeight;

                m_camData.m_pos						 *= oneOverWeight;
                m_camData.m_focale					 *= oneOverWeight;
                m_camData.m_depth					 *= oneOverWeight;
                m_camData.m_deltaFogZ				 *= oneOverWeight;
                m_camData.m_lookAtDir				 *= oneOverWeight;
                m_camData.m_upDir					 *= oneOverWeight;
                m_camData.m_horizontalVersusVertical *= oneOverWeight;
                m_camData.m_offsetHVS				 *= oneOverWeight;
            }
            else
            {
                // reset
                m_camData = CameraData();
            }
        }

        updateAutoRampUpCoeff( mainCamData );     

        m_camSpeedInGame = (m_camData.m_pos - prevCamPosInGame) * oneOverDt;
    }

    void CameraControllerManager::onEndUpdateProcess(f32 _dt)
    {
        updateScreenRatio();
        updateCameraControllerSubjectActiveList();        

        if ( m_isCameraJustTeleported || ( !GAMEMANAGER->isInPause() && !m_pauseMode 
            && (!CHEATMANAGER || (!CHEATMANAGER->getPause() || CHEATMANAGER->getPauseStep()))) )
        {
            updateControllers(_dt);
            setCameraData( _dt );                        
            updateShake(_dt);
            applyCameraData();   
            updateBindedObjects();
            resetTempValues();
        }

        resetControllerSubject();
        updateScreenAABB();  

#ifdef ITF_SUPPORT_DEBUGFEATURE
        drawDebug();
#endif //ITF_SUPPORT_DEBUGFEATURE
    }

    void CameraControllerManager::resetTempValues()
    {
        m_isInGameCameraJustReseted = bfalse;
        m_isCameraJustTeleported = bfalse;
        m_updateCamControllers_SubjectList = bfalse;
    }

    void CameraControllerManager::resetControllerSubject()
    {
        for ( u32 i=0; i<m_camSubjectList.size(); i++)
        {
            CameraControllerSubject& subject = m_camSubjectList[i];

            subject.m_subjectPerform.reset();
            subject.m_refOld = ITF_INVALID_OBJREF;
        }
    }

    void CameraControllerManager::updateShake(f32 _dt)
    {
        if ( m_shakeCountdown > 0.f )
        {
            // offset camera position
            const f32 t = (m_shake.getDuration() - m_shakeCountdown);
            m_camData.m_pos.x() += m_shake.getShakeX().getValue(t);
            m_camData.m_pos.y() += m_shake.getShakeY().getValue(t);
            m_camData.m_pos.z() += m_shake.getShakeZ().getValue(t);

            // countdown, reset to shake when done
            m_shakeCountdown -= _dt;
            if (m_shakeCountdown < 0.0f)
            {
                resetShake();
            }
        }
    }

    void CameraControllerManager::updateBindedObjects()
    {
        if ( m_bindLastCameraPos.IsEqual(Vec3d::Zero) )
        {
            m_bindLastCameraPos = m_camData.m_pos;
        }
        m_bindCameraMoveDelta = m_camData.m_pos - m_bindLastCameraPos;

        for( u32 i = 0; i < m_bindedObjects.size(); ++i )
        {
            BindedObject & bindedObj = m_bindedObjects[i];
            BaseObject * obj = bindedObj.m_objectRef.getObject();
            if ( obj != NULL )
            {
                if ( Pickable * pickable = DYNAMIC_CAST(obj,Pickable) )
                {
                    if(!m_isCameraJustTeleported)
                    {
                        Vec3d gameplayDelta = pickable->getPos() - bindedObj.m_lastComputedPos;
                        bindedObj.m_offset += gameplayDelta;
                    }
                    Vec3d newPos = m_camData.m_pos + bindedObj.m_offset;

                    if ( bindedObj.m_updateInitPos )
                    {
                        Vec3d initPos = m_camData.m_pos + bindedObj.m_initialOffset;
                        if(bindedObj.m_ignoreZ)
                            initPos.setZ(pickable->getWorldInitialZ());
                        pickable->setWorldInitialPos( initPos, btrue);
                    }

                    if(bindedObj.m_ignoreZ)
                        newPos.setZ(pickable->getDepth());

                    pickable->setPos( newPos );
                    bindedObj.m_lastComputedPos = newPos;

                    if(pickable->getObjectType() == BaseObject::eActor)
                    {
                        static_cast<Actor*>(pickable)->updateWorldCoordinatesForBoundChildren();
                    }       
                }
                else
                {
                    ITF_ASSERT_MSG( 0, "Camera Binded objects must be a pickable object !" );
                }
            }
            else
            {
                m_bindedObjects.eraseNoOrder( i );
                i--;
            }
        }
        m_bindLastCameraPos = m_camData.m_pos;
    }

    void CameraControllerManager::applyCameraData() const
    {
		Vec3d tv_axis;
        Camera* cam = getCamera();

		cam->setPosition( m_camData.m_pos );

		//Vec3d::rotate(&tv_axis, &m_camData.m_lookAtDir, m_camData.m_UpDnAngle.ToRadians(), &Vec3d::XAxis);
		tv_axis.rotate(&m_camData.m_lookAtDir, m_camData.m_UpDnAngle.ToRadians(), &Vec3d::XAxis);
		
        cam->setLookAtDir( tv_axis );

		//Vec3d::rotate(&tv_axis, &m_camData.m_upDir, m_camData.m_UpDnAngle.ToRadians(), &Vec3d::XAxis);
		tv_axis.rotate(&m_camData.m_upDir, m_camData.m_UpDnAngle.ToRadians(), &Vec3d::XAxis);
		cam->setUpDir( m_camData.m_upDir );

		cam->setDeltaFogZ( m_camData.m_deltaFogZ );
        cam->setFocale( m_camData.m_focale * GAMEMANAGER->getCameraZoom());
        cam->setFlag( m_camData.m_flagMirror );

		cam->setHorizontalVersusVertical(m_camData.m_horizontalVersusVertical);
		cam->setOffsetHVS(m_camData.m_offsetHVS);

        cam->apply(); 
    }

    void CameraControllerManager::onEvent( Event* _event )
    {
        if ( EventRegisterCameraSubject* eventRegister = DYNAMIC_CAST(_event,EventRegisterCameraSubject) )
        {
            processEventRegisterSubject(eventRegister);
        }
        else if ( EventCameraShake* shake = DYNAMIC_CAST(_event,EventCameraShake) )
        {
            addShake(shake->getName());
        }
        else if ( EventCameraIgnoreShake* ignoreShake = DYNAMIC_CAST(_event,EventCameraIgnoreShake) )
        {
            m_ignoreShake = ignoreShake->getIgnoreShake();
        }
#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE
        else if (EventSingletonConfigChanged* configChanged = DYNAMIC_CAST(_event,EventSingletonConfigChanged))
        {
            // reload camera shake config file
            const Path& configFile = GAMEMANAGER->getCameraShakeConfig();
            if (configChanged->getPath() == configFile)
            {
                m_shakeConfig = TEMPLATEDATABASE->reloadTemplate<CameraShakeConfig_Template>(configFile);
                if (!m_shakeConfig)
                {
                    ITF_FATAL_ERROR("Couldn't reload camera shake config: %s", configFile.toString8().cStr());
                }
            }
        }
#endif //ITF_SUPPORT_HOTRELOAD_TEMPLATE

    }

    bbool CameraControllerManager::hasMask( u32 _mask ) const
    {
        return ( (getId()&_mask) != 0 );
    }

    CameraControllerManager* CameraControllerManager::getManagerFromId( u32 _id )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pInstance = s_CCManagerInstance[i];

            if ( pInstance->getId() == _id )
            {
                return pInstance;
            }
        }

        return NULL;
    }

    u32 CameraControllerManager::getManagerIndexFromId( u32 _id )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pInstance = s_CCManagerInstance[i];

            if ( pInstance->getId() == _id )
            {
                return i;
            }
        }

        return U32_INVALID;
    }
 
    void CameraControllerManager::registerSubject( u32 _mask, ObjectRef _subject, f32 _delayToRegister /*= 0.f*/, bbool _isPlayer /*= btrue*/, bbool _doTeleport /*= btrue*/, bbool _leadCamera /*= bfalse*/, bbool _resetIfAlone /*= btrue*/ )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            //if ( pCCManager->hasMask(_mask))
            {
                pCCManager->registerSubject( _subject, _delayToRegister, _isPlayer, _doTeleport, _leadCamera, _resetIfAlone );
            }
        }
    }

    void CameraControllerManager::unregisterSubject( u32 _mask, ObjectRef _subject)
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->unregisterSubject(_subject);
            }
        }
    }

    u32 CameraControllerManager::getIsSubjectRegisteredMask( ObjectRef _subject )
    {
        u32 resMask = 0;

        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->isSubjectRegistered( _subject ) )
            {
                resMask = resMask | pCCManager->getId();
            }
        }
        return resMask;
    }

    void CameraControllerManager::swapSubjects( u32 _mask, ObjectRef _subjectRefFrom, ObjectRef _subjectRefTo )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->swapSubjects( _subjectRefFrom, _subjectRefTo );
            }
        }
    }
    
    void CameraControllerManager::destroy()
    { 
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            SF_DEL(s_CCManagerInstance[i]);
        }

        s_CCManagerInstance.clear();
    }

    void CameraControllerManager::addShake( u32 _mask, const StringID& _name )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->addShake(_name);
            }
        }
    }


	bbool CameraControllerManager::getShakeConfig( u32 _mask, const StringID& _name, CameraShake &_shake )
	{
		for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
		{
			CameraControllerManager* pCCManager = s_CCManagerInstance[i];

			if ( pCCManager->hasMask( _mask) )
			{
				return pCCManager->getShakeConfig(_name, _shake);
			}
		}
		return bfalse;
	}


    void CameraControllerManager::resetShake( u32 _mask )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->resetShake();
            }
        }
    }

    void CameraControllerManager::init( u32 _mask )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->init();
            }
        }
    }

    void CameraControllerManager::onSwitchToWorld( u32 _mask, World* _pWorld)
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];
            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->onSwitchToWorld(_pWorld);
            }
        }
    }

    void CameraControllerManager::teleport( u32 _mask, const Vec3d& _pos )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];
            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->teleport( _pos );
            }
        }
    }

    void CameraControllerManager::preApplyCameraPos( u32 _mask, const Vec3d& _pos )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];
            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->preApplyCameraPos( _pos );
            }
        }
    }

    void CameraControllerManager::pauseMode( u32 _mask, bbool _pause)
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];
            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->pauseMode( _pause);
            }
        }
    }

    void CameraControllerManager::unregisterNPCFromCamera( u32 _mask )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];
            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->unregisterNPCFromCamera();
            }
        }
    }

    void CameraControllerManager::unregisterAllSubjects( u32 _mask )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];
            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->unregisterAllSubjects();
            }
        }
    }

    void CameraControllerManager::setSubjectJustJumped( u32 _mask, ObjectRef _subject )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];
            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->setSubjectJustJumped(_subject);
            }
        }
    }

    void CameraControllerManager::setSubjectJustReceivedPunchUp( u32 _mask, ObjectRef _subject )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];
            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->setSubjectJustReceivedPunchUp(_subject);
            }
        }
    }

    void CameraControllerManager::setSubjectJustBounceJumped( u32 _mask, ObjectRef _subject )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];
            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->setSubjectJustBounceJumped(_subject);
            }
        }
    }

    void CameraControllerManager::onEndUpdateProcess( u32 _mask, f32 _dt)
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];
            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->onEndUpdateProcess(_dt);
            }
        }
	}

	bbool CameraControllerManager::getShakeConfig(const StringID& _name, CameraShake &_shake)
	{
		if ( m_ignoreShake || m_ignoreShakeFromCageBehaviour )
		{
			return bfalse;
		}

		// warn and give up if name is invalid
		if (!_name.isValid())
		{
			ITF_WARNING(NULL, bfalse, "Invalid camera shake ID");
			return bfalse;
		}

		// camera shake config *has to* be loaded
		if (!m_shakeConfig)
		{
			ITF_ASSERT_MSG(m_shakeConfig, "Camera shake config missing");
			return bfalse;
		}

		// search for shake
		ITF_VECTOR<CameraShake>::const_iterator it; 
		for (it = m_shakeConfig->getShakes().begin();
			it != m_shakeConfig->getShakes().end();
			++it)
		{
			if (it->getName() == _name)
			{
				break;
			}
		}

		// warn and give up if not found
		if (it == m_shakeConfig->getShakes().end())
		{
			ITF_WARNING(NULL, bfalse, "Camera shake %s undefined", _name.getDebugString());
			return bfalse;
		}

		// give up if not setup properly (something went wrong with serialization)
		if (!it->isSetup())
		{
			return bfalse;
		}

		// copy shake, apply master intensity & init countdown timer
		_shake = *it;

		return btrue;
	}

    void CameraControllerManager::addShake(const StringID& _name)
    {
        // copy shake, apply master intensity & init countdown timer
		if( getShakeConfig(_name, m_shake) )
			m_shakeCountdown = m_shake.getDuration();
    }

    void CameraControllerManager::onMapLoaded()
    {
        // flush unregistered controllers
        for (u32 i = 0; i < m_camControllerList.size();)
        {
            CameraControllerData& data = m_camControllerList[i];

            if (!data.m_controller)
            {
                m_camControllerList.eraseNoOrder(i);
            }
            else
            {
                i++;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////    
    // Camera Controllers
    //////////////////////////////////////////////////////////////////////////

    const ICameraController* CameraControllerManager::getInGameCamera() const
    {
        const u32 camControllerCount = m_camControllerList.size();
        for (u32 i = 0; i < camControllerCount; i++)
        {
            const ICameraController* cam = m_camControllerList[i].m_controller;

            if ( cam && cam->isInGameCamera() )
            {
                return cam;
            }
        }

        return NULL;
    }

    void CameraControllerManager::registerCameraController( ICameraController* _cam )
    {
        ITF_ASSERT_MSG( _cam, "Invalid camera");

        // don't register twice
        const u32 camControllerCount = m_camControllerList.size();
        for (u32 i = 0; i < camControllerCount; i++)
        {
            if (m_camControllerList[i].m_controller == _cam)
            {
                return;
            }
        }

        CameraControllerData data(_cam);
        m_camControllerList.push_back( data );
    }

    void CameraControllerManager::unregisterCameraController( ICameraController* _cam )
    {
        ITF_ASSERT_MSG( _cam, "Invalid camera");

        const u32 camControllerCount = m_camControllerList.size();
        for (u32 i = 0; i < camControllerCount; i++)
        {
            CameraControllerData& controller = m_camControllerList[i];

            if ( controller.m_controller == _cam )
            {
                ITF_ASSERT_MSG ( getMainCameraControllerData() != &controller, "You are unregistering the main camera");

                if ( !controller.hasPrevData() )
                {
                    // Register and unregister in the SAME frame
                    m_camControllerList.eraseNoOrder(i);
                }
                else
                {
                    controller.m_controller = NULL;
                    controller.m_finalWeight = 0.f;
                }
                
                return;
            }
        }
    }

    bbool CameraControllerManager::cameraControllerHasInfluence( const ICameraController* _cam ) const
    {        
        for (u32 i = 0; i < m_camControllerList.size(); i++)
        {
            const CameraControllerData& data = m_camControllerList[i];

            if ( data.m_controller == _cam )
            {
                return cameraControllerHasInfluence(data);
            }
        }

        return bfalse;
    }

    bbool CameraControllerManager::isMainCameraController( const ICameraController* _cam )
    {
        return _cam == getMainCameraController();
    }

    // this function must be called outside this class
    void CameraControllerManager::setMainCameraController( ICameraController* _mainCam, f32 _blendIn )
    {        
        activeAutoRampUpCoeff(bfalse);

        ITF_ASSERT_MSG( _mainCam, "Invalid camera");
        ITF_ASSERT_MSG( isCameraControllerActive( _mainCam ), "Inactive camera");

        CameraControllerData* pMainData = NULL;
        const u32 camControllerCount = m_camControllerList.size();
        m_cameraSequence.deactivate();

        for (u32 i = 0; i < camControllerCount; i++)
        {
            CameraControllerData& data = m_camControllerList[i];
            data.m_finalWeight = 0.f; // reset the final weight for all controllers

            if ( data.m_controller == _mainCam )
            {
                pMainData = &data;
            }
        }

        if ( pMainData )
        {           
            pMainData->m_finalWeight = 1.f;
            pMainData->m_mainPriority = m_mainPriority;

            m_rampUpCoeff = _blendIn;            
            m_mainPriority++;

            return;
        }

        ITF_ASSERT_MSG( 0, "This camera controller is not registered");

        restorePreviousMainCameraController( NULL, m_rampUpCoeff);
    }

    void CameraControllerManager::restorePreviousMainCameraController( const CameraControllerData* _pCurrentMainData, f32 _blendOut )
    {        
        CameraControllerData* pMainData = NULL;
        u32 bestMainPriority = 0;   // if all controllers have a priority of 0 (= removed), then no main will be found

        const u32 camControllerCount = m_camControllerList.size();
        for ( u32 i = 0; i < camControllerCount; i++ )
        {
            CameraControllerData& data = m_camControllerList[i];
            data.m_finalWeight = 0.f;
            
            if ( &data != _pCurrentMainData && data.m_controller && data.m_mainPriority > bestMainPriority && isCameraControllerActive( data.m_controller ) )
            {
                pMainData = &data;
                bestMainPriority = data.m_mainPriority;
            }
        }

        if ( pMainData )
        {
            pMainData->m_finalWeight = 1.f;

            m_rampUpCoeff = _blendOut;            
        }
        
        activeAutoRampUpCoeff( btrue );
    }

    void CameraControllerManager::removeMainCameraController( ICameraController* _mainCam, f32 _blendOut )
    {
        CameraControllerData* data = getCameraControllerData(_mainCam);
        ITF_ASSERT_MSG(data, "this controller is not registered!");
        if (data)
        {
            // I want to stay registered but I can't be selected as main anymore
            data->m_mainPriority = 0;
        }

        const CameraControllerData* pCurrentMainData = getMainCameraControllerData();

        if ( pCurrentMainData && pCurrentMainData->m_controller == _mainCam )
        {
            restorePreviousMainCameraController( pCurrentMainData, _blendOut );
        }
    }

    CameraControllerData* CameraControllerManager::getMainCameraControllerData()
    {
        const u32 camControllerCount = m_camControllerList.size();
        for (u32 i = 0; i < camControllerCount; i++)
        {
            CameraControllerData& data = m_camControllerList[i];

            if ( isMainCameraControllerData(data) )
            {
                return &data;
            }
        }

        return NULL;
    }

    ICameraController* CameraControllerManager::getMainCameraController() 
    {
        if ( CameraControllerData* pData = getMainCameraControllerData() )
        {
            ITF_ASSERT_MSG( pData->m_controller, "the main controller is not supposed to be null!");

            return pData->m_controller;
        }

        return NULL;
    }


    //////////////////////////////////////////////////////////////////////////
    // Camera Modifiers
    //////////////////////////////////////////////////////////////////////////

    bbool CameraControllerManager::isCamModifierRegistered(CameraModifierComponent* _camModifier ) const
    {
        return (m_camModifierList.find(_camModifier) != -1);
    }

    bbool CameraControllerManager::isCamModifierComponentActive( const CameraModifierComponent* _camModifier ) const
    {
        if ( const Actor * actor = _camModifier->GetActor() )
        {
            if ( const World * world = actor->getWorld() )
            {
                return world->getAllowUpdate();
            }
        }       

        return bfalse;
    }

    void CameraControllerManager::unregisterCamModifierComponent( CameraModifierComponent* _camModifier )
    {
        const i32 index = m_camModifierList.find( _camModifier);

        if ( index != -1 )
        {
            m_camModifierList.eraseNoOrder( index );            
        }
    }

    void CameraControllerManager::registerCamModifierComponent( CameraModifierComponent* _camModifier )
    {
        i32 index = m_camModifierList.find( _camModifier);

        if( index == -1 )
        {
            m_camModifierList.push_back( _camModifier );            
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Subjects
    //////////////////////////////////////////////////////////////////////////
    void CameraControllerManager::removeInvalidSubjects()
    {
        for ( u32 i=0; i<m_camSubjectList.size();)
        {
            const Actor* actor = (Actor*) GETOBJECT(m_camSubjectList[i].m_ref);

            if ( !actor )
            {
                m_camSubjectList.eraseNoOrder(i); 
                m_updateCamSubjectList_Active = btrue;
            }
            else
            {
                i++;
            }
        }        
    }

    i32 CameraControllerManager::getSubjectIndex( ObjectRef _subject ) const
    {
        const u32 subjectCount = m_camSubjectList.size();
        for(u32 i=0; i<subjectCount; i++)
        {
            if( m_camSubjectList[i].m_ref==_subject )
            {
                return i;
            }
        }

        return -1;
    }

    void CameraControllerManager::resetInGameCamera()
    { 
        m_isInGameCameraJustReseted = btrue;
    }

    void CameraControllerManager::registerSubject( ObjectRef _subjectRef, f32 _delayToRegister, bbool _isPlayer, bbool _doTeleport, bbool _leadCamera, bbool _resetIfAlone )
    {
        removeInvalidSubjects();

        if( getSubjectIndex(_subjectRef ) == -1 )
        {
            CameraControllerSubject newSubject;
            newSubject.m_ref = _subjectRef;
            newSubject.m_delayToRegister = _delayToRegister;
            newSubject.m_isPlayer = _isPlayer;
            newSubject.m_leadCamera = _leadCamera;

            m_camSubjectList.push_back( newSubject );

            if ( m_camSubjectList.size() == 1 )
            {
                if ( _resetIfAlone )
                {
                    resetInGameCamera();
                }

                if ( _doTeleport )
                {
                    if ( const Actor* actor = (Actor*) GETOBJECT(_subjectRef) )
                    {
                        teleport( actor->getPos() );
                    }   
                }
            }             

            m_updateCamSubjectList_Active = btrue;
        }
    }

    void CameraControllerManager::unregisterAllSubjects()
    {
        m_camSubjectList.clear(); 
        m_updateCamSubjectList_Active = btrue; 
    }

    void CameraControllerManager::unregisterSubject( ObjectRef _subject )
    {
        const i32 index = getSubjectIndex( _subject );

        if( index >= 0 )
        {
            m_camSubjectList.eraseNoOrder(index); 
            m_updateCamSubjectList_Active = btrue; 
        }      

        removeInvalidSubjects();
    }

    void CameraControllerManager::updateRegisteredSubjectStatus( u32 _mask, ObjectRef _ref, bbool _leadCamera )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask(_mask))
            {
                pCCManager->updateRegisteredSubjectStatus( _ref, _leadCamera );
            }
        }
    }

    void CameraControllerManager::updateRegisteredSubjectStatus(ObjectRef _ref, bbool _leadCamera)
    {
        const u32 subjectCount = m_camSubjectList.size();

        for (u32 i=0; i<subjectCount; i++)
        {
            CameraControllerSubject& subject = m_camSubjectList[i];

            if( subject.m_ref == _ref )
            {
                subject.m_leadCamera = _leadCamera;
                m_updateCamSubjectList_Active = btrue;
                break;
            }
        }
    }

    void CameraControllerManager::swapSubjects( ObjectRef _subjectRefFrom, ObjectRef _subjectRefTo )
    {
        i32 index = getSubjectIndex( _subjectRefFrom );

        if( index >= 0 )
        {
            CameraControllerSubject& subject = m_camSubjectList[index];
            subject.m_ref = _subjectRefTo;
            subject.m_refOld = _subjectRefFrom;

            m_updateCamSubjectList_Active = btrue;
        }
        else
        {
            ITF_ASSERT_MSG(1, "Subject was not registered");
            registerSubject( _subjectRefTo );
        }        
    }

    u32 CameraControllerManager::getSubjectListCount()
    {
        return m_camSubjectList.size();
    }

    u32 CameraControllerManager::getSubjectPlayerListCount()
    {
        u32 subjectListCount = m_camSubjectList.size();
        u32 playerCount = 0;

        for ( u32 i = 0; i < subjectListCount; i++ )
        {
            if ( m_camSubjectList[i].m_isPlayer )
            {
                playerCount++;
            }
        }

        return playerCount;
    }

    void CameraControllerManager::getSubjectByIndex( u32 _index, CameraControllerSubject& _subject)
    {
        if( _index >= getSubjectListCount() )
        {
            _subject.m_ref = NULL;
            ITF_ASSERT_MSG(1, "Subject Camera not found in list");
            return;
        }

        _subject = m_camSubjectList[_index];    
    }

    void CameraControllerManager::setSubjectJustJumped( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            m_camSubjectList[index].m_subjectPerform.m_justJumped = btrue;
        }
    }

    bbool CameraControllerManager::isSubjectJustJumped( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            return m_camSubjectList[index].m_subjectPerform.m_justJumped;
        }

        return bfalse;
    }

    void CameraControllerManager::setSubjectJustBounceJumped( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            m_camSubjectList[index].m_subjectPerform.m_justBounceJumped = btrue;
        }
    }

    bbool CameraControllerManager::isSubjectJustBounceJumped( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            return m_camSubjectList[index].m_subjectPerform.m_justBounceJumped;
        }

        return bfalse;
    }

    SubjectPerform CameraControllerManager::getSubjectPerform( ObjectRef _subject )
    {
        SubjectPerform perform;

        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            perform = m_camSubjectList[index].m_subjectPerform;
        }

        return perform;
    }

    void CameraControllerManager::setSubjectJustReceivedPunchUp( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            m_camSubjectList[index].m_subjectPerform.m_justReceivedPunchUp = btrue;
        }
    }

    bbool CameraControllerManager::hasLeadCameraSubject() const
    {
        const u32 subjectCount = m_camSubjectList.size();
        for (u32 i=0; i<subjectCount; ++i)
        {
            if (m_camSubjectList[i].m_leadCamera)
            {
                return btrue;
            }
        }
        return bfalse;
    }

    //////////////////////////////////////////////////////////////////////////
    // Cheat
    //////////////////////////////////////////////////////////////////////////
    void CameraControllerManager::backToGame()
    {
#ifdef ITF_SUPPORT_CHEAT
        resetCheatZoom();
        m_isCheatDezoomMax = bfalse;
#endif //ITF_SUPPORT_CHEAT

#ifdef ITF_SUPPORT_DEBUGFEATURE
        m_debugDrawInfo = bfalse;
        m_debugDrawModifiers = bfalse;
#endif //ITF_SUPPORT_DEBUGFEATURE
    }

#ifdef ITF_SUPPORT_CHEAT
    void CameraControllerManager::setCheatZoom(f32 _increment)
    {
        m_cheatZoom += _increment;
        if (m_defaultDepth + m_cheatZoom < 0)
            m_cheatZoom = -m_defaultDepth;
    }

    void CameraControllerManager::resetCheatZoom()
    {
        m_cheatZoom = 0.0f;
    }

    void CameraControllerManager::switchCheatDezoomMaxWithPad()
    { 
        static f64 timeLast = 0.f;

        f64 thisTime = SYSTEM_ADAPTER->getTime();
        f64 deltaTime = thisTime -timeLast;
        timeLast = thisTime;

        if (deltaTime > 0.3f)
            return;

        switchCheatDezoomMax();
    }

    void CameraControllerManager::switchCheatDezoomMax()
    { 
        m_isCheatDezoomMax = !m_isCheatDezoomMax; 
        resetCheatZoom();
    }
#endif //ITF_SUPPORT_CHEAT

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void CameraControllerManager::drawDebug()
    {
        if ( m_id != CAMID_MAIN )
            return;

        if ( m_debugDrawModifiers )
        {
            //  cameras modifiers components
            const u32 camModifiersCount = m_camModifierList.size();
            for(u32 i=0; i<camModifiersCount; i++)
            {
                const CameraModifierComponent * camMod = m_camModifierList[i];

                if ( isCamModifierComponentActive(camMod) )
                {
                    camMod->drawDebug();
                }                
            }
        }

        if ( m_debugDrawInfo )
        {
            String8 txt;

            if ( !isCameraSequenceFinished() )
            {                
                txt.setTextFormat("Camera sequence current weight : %f", m_cameraSequence.m_weight );
                GFX_ADAPTER->drawDBGText(txt);
                txt.setTextFormat("Camera sequence final weight : %f", m_cameraSequence.m_finalWeight );
                GFX_ADAPTER->drawDBGText(txt);
                txt.setTextFormat("Camera sequence position : ( %f, %f, %f )", m_cameraSequence.m_camData.m_pos.x(), m_cameraSequence.m_camData.m_pos.y(), m_cameraSequence.m_camData.m_pos.z() );
                GFX_ADAPTER->drawDBGText(txt);
                txt.setTextFormat("");
                GFX_ADAPTER->drawDBGText(txt);
            }

            //  cameras components
            const u32 camControllerCount = m_camControllerList.size();
            for(u32 i=0; i<camControllerCount; i++)
            {
                const CameraControllerData& data = m_camControllerList[i];
                if ( data.m_controller && isCameraControllerActive( data.m_controller ) )
                {
                    data.m_controller->drawDebug(data);
                }
            }
            GFX_ADAPTER->drawDBGAABB( m_screenAABB, Color::orange(), 0.f, 1.f, m_camData.m_pos.z() - m_camData.m_depth);

            // final position            
            txt.setTextFormat("Camera Final Position: (%f, %f, %f)", m_camData.m_pos.x(), m_camData.m_pos.y(), m_camData.m_pos.z());
            GFX_ADAPTER->drawDBGText(txt);
        }

        if ( isDebugDrawScreen4By3() )
        {           
            f32 height = (f32)GFX_ADAPTER->getScreenHeight();
            f32 deltaScreenBy2 = ( (f32)GFX_ADAPTER->getScreenWidth() - height *1.333333f ) *0.5f;

            if ( deltaScreenBy2 > 0.f )
            {
                u32 color;

                switch ( m_debugDrawScreen4By3 )
                {
                case Transparent:
                    color = ITFCOLOR_FROM_ARGB(2130706432u);
                    break;

                case Opaque:
                    color = ITFCOLOR_FROM_ARGB(4278190080u);
                    break;

                default:
                    color = ITFCOLOR_FROM_ARGB(2130706432u);
                    break;
                }

                const AABB& screenAABB = GFX_ADAPTER->getScreenAABB();
                GFX_ADAPTER->drawDBG2dBox(screenAABB.getMin(), deltaScreenBy2, height, color, color, color, color, 0);
                GFX_ADAPTER->drawDBG2dBox(screenAABB.getMax(), -deltaScreenBy2, -height, color, color, color, color, 0);
            }
        }
        if (m_shakeDebugDraw)
        {
            // camera shake curves
            if (m_shakeCountdown>0.0f)
            {
                ICameraController* controller = getMainCameraController();
                Actor* controllerActor = controller ? controller->getControllerActor() : NULL;
                ObjectRef controllerActorRef = controllerActor ? controllerActor->getRef() : ObjectRef::InvalidRef;
                DEBUG_DRAW_CURVE_OFS(m_shake.getShakeX(), controllerActorRef, -3.0f, 0.0f, 0.0f);
                DEBUG_DRAW_CURVE_OFS(m_shake.getShakeY(), controllerActorRef, 0.0f, 0.0f, 0.0f);
                DEBUG_DRAW_CURVE_OFS(m_shake.getShakeZ(), controllerActorRef, 3.0f, 0.0f, 0.0f);
                f32 t = (m_shake.getDuration() - m_shakeCountdown);
                m_shake.getShakeX().getValue(t);
                m_shake.getShakeY().getValue(t);
                m_shake.getShakeZ().getValue(t);
            }
        }       
    }
#endif //ITF_SUPPORT_DEBUGFEATURE

    void CameraControllerManager::onSwitchToWorld(World* _pWorld)
    {
        m_updateCamSubjectList_Active = btrue;        

        restorePreviousMainCameraController( NULL, 1.f);
    }

    bbool CameraControllerManager::isCameraControllerActive( ICameraController* _controller)
    {
        if (_controller->isAlwaysActive())
            return btrue;

        if ( Actor* pActor = _controller->getControllerActor() )
        {
            World* world = pActor->getWorld();

            if( world && world->isActive())
                return btrue;
        }

        return bfalse;
    }

    void CameraControllerManager::updateCameraControllerSubjectActiveList()
    {
        if ( !m_updateCamSubjectList_Active)
            return;

        m_updateCamSubjectList_Active = bfalse;
        m_updateCamControllers_SubjectList = btrue;
        m_camSubjectList_Active.clear();

        for(u32 i= 0; i < m_camSubjectList.size(); ++i)
        {
            const CameraControllerSubject& ccs = m_camSubjectList[i];

            if (Actor* pActor = static_cast<Actor*>(ccs.m_ref.getObject()))
            {
                if ( ccs.m_isPlayer )
                {
                    m_camSubjectList_Active.push_back(ccs);
                }
                else
                {
                    World* world = pActor->getWorld();
                    if ( world && world->isActive())
                    {
                        m_camSubjectList_Active.push_back(ccs);
                    }
                }
            }
        }
    }

    void CameraControllerManager::predictCameraDataAfterTeleport( CameraData& _camData, bbool _useLookAtOffset /*= bfalse*/ ) const
    {        
        const CameraModifierComponent* camModComponent = getCameraModifierComponent( AABB(_camData.m_pos), _camData.m_pos.z(), btrue );        

        if ( camModComponent )
        {         
			_camData.m_depth    = camModComponent->getUpdateData().m_cameraLookAtOffset.z();
			if(_useLookAtOffset)
			{
				_camData.m_pos.x() += camModComponent->getUpdateData().m_cameraLookAtOffset.x();
				_camData.m_pos.y() += camModComponent->getUpdateData().m_cameraLookAtOffset.y();
			}
            _camData.m_pos.z() += _camData.m_depth;  
            _camData.m_focale   = camModComponent->getUpdateData().m_focale;

            if ( camModComponent->isConstraint() )
            {
                AABB constraintAABB( camModComponent->getModifierAABBMax() );
                const CamModifier& camMod = camModComponent->getCM();

                if ( !camMod.m_constraintLeftIsActive )            
                    constraintAABB.setMinX( -F32_INFINITY );

                if ( !camMod.m_constraintRightIsActive )            
                    constraintAABB.setMaxX( F32_INFINITY );

                if ( !camMod.m_constraintBottomIsActive )            
                    constraintAABB.setMinY( -F32_INFINITY );

                if ( !camMod.m_constraintTopIsActive )            
                    constraintAABB.setMaxY( F32_INFINITY );            

                getPredictPositionInConstraintAABB( constraintAABB, _camData.m_depth, _camData.m_pos, _camData.m_focale ); 
            }           
        }       
        else
        {            
            _camData.m_pos.z() += _camData.m_depth; // apply default depth
        }
    }

    void CameraControllerManager::getPredictPositionInConstraintAABB( const AABB& _constraintAABB, const f32 _depth, Vec3d& _posWanted, f32 _focale ) const
    {
        AABB screenAABB;
        getAABBFromDepthAndPos( screenAABB, _depth, _posWanted, _focale );

        f32 depthMaxX = F32_INFINITY;
        f32 depthMaxY = F32_INFINITY;

        const f32 tanFocalBy2 = f32_Abs(tan(_focale * 0.5f));
        ITF_ASSERT_CRASH( tanFocalBy2 > 0.f, "Invalid focale");
        ITF_ASSERT_CRASH( m_screenRatio > 0.f, "Invalid screen ratio" );

        // axe Z
        if ( screenAABB.getWidth() > _constraintAABB.getWidth() )
        {
            _posWanted.x() = _constraintAABB.getCenter().x();

            depthMaxX = _constraintAABB.getWidth() -screenAABB.getWidth();
        }
        
        if ( screenAABB.getHeight() > _constraintAABB.getHeight() )
        {
            _posWanted.y() = _constraintAABB.getCenter().y();

            depthMaxY = _constraintAABB.getHeight() -screenAABB.getHeight();
        }

        if ( depthMaxX != F32_INFINITY || depthMaxY != F32_INFINITY )
        {
            const f32 depthConstraint = f32_Min( depthMaxX /m_screenRatio, depthMaxY ) * 0.5f / tanFocalBy2;

            _posWanted.z() += depthConstraint;

            // update screen AABB with new depth
            getAABBFromDepthAndPos( screenAABB, _depth + depthConstraint, _posWanted, _focale );
        }


        // axe X
        if ( depthMaxX == F32_INFINITY  )        
        {
            if ( screenAABB.getMin().x() < _constraintAABB.getMin().x() )
            {
                _posWanted.x() += _constraintAABB.getMin().x() - screenAABB.getMin().x();
            }
            else if ( screenAABB.getMax().x() > _constraintAABB.getMax().x() )
            {
                _posWanted.x() += _constraintAABB.getMax().x() - screenAABB.getMax().x();
            }
        }

        // axe Y
        if ( depthMaxY == F32_INFINITY  )        
        {
            if ( screenAABB.getMin().y() < _constraintAABB.getMin().y() )
            {
                _posWanted.y() += _constraintAABB.getMin().y() - screenAABB.getMin().y();
            }
            else if ( screenAABB.getMax().y() > _constraintAABB.getMax().y() )
            {
                _posWanted.y() += _constraintAABB.getMax().y() - screenAABB.getMax().y();
            }
        }
    }
  
    void CameraControllerManager::getAABBFromDepth( AABB& _aabb, f32 _depth ) const
    {
        getAABBFromDepthAndPos( _aabb, _depth, m_camData.m_pos, m_camData.m_focale );
    }
    
    void CameraControllerManager::getAABBFromDepthAndPos( AABB& _aabb, f32 _depth, const Vec3d & _pos, f32 _focal ) const
    {
        ITF_ASSERT_MSG( _depth > 0.f, "Depth must must positive" );

        Vec2d screenRealLongBy2;
        screenRealLongBy2.y() = f32_Abs( tan( _focal * 0.5f ) * _depth );
        screenRealLongBy2.x() = screenRealLongBy2.y() * m_screenRatio;

        const Vec2d& camPos = _pos.truncateTo2D();
        _aabb.setMin( camPos - screenRealLongBy2 );
        _aabb.setMax( camPos + screenRealLongBy2 );
    }

    void CameraControllerManager::updateScreenAABB()
    {
        getAABBFromDepth( m_screenAABB, m_camData.m_depth );
    }

    // Project screen AABB at given z.
    // Doesn't make sense if z is behind cam (returns false)
    bbool CameraControllerManager::getAABB( const f32 _z, AABB& _aabb ) const
    {
        const f32 depth = m_camData.m_pos.z() - _z;
        if ( depth > 0.f )
        {
            getAABBFromDepth( _aabb, depth );
            return btrue;
        }

        return bfalse;
    }

    bbool CameraControllerManager::projectAABB( AABB& _aabb, f32 _curZ, f32 _destZ ) const
    {
        ITF_ASSERT_MSG( _aabb.isValid(), "Invalid AABB" );
        ITF_ASSERT_MSG( !_aabb.isZero(), "You have to initialize your AABB" );

        AABB screenAABBCurZ;
        if ( !getAABB( _curZ, screenAABBCurZ ) )
            return bfalse;

        AABB screenAABBDestZ;
        if ( !getAABB( _destZ, screenAABBDestZ ) )
            return bfalse;

        const Vec2d deltaAABB( screenAABBDestZ.getMax() - screenAABBCurZ.getMax() );

        _aabb.setMin( _aabb.getMin() - deltaAABB );
        _aabb.setMax( _aabb.getMax() + deltaAABB );

        return btrue;
    }

    void CameraControllerManager::predictViewFrustum( FrustumDesc& _frustum, const AABB3d& _aabb, f32 _screenRatio, f32 _focale , f32 _camDistance )
    {
        const Vec3d maxNear     = _aabb.getMax() - (_aabb.getMax().z() - _aabb.getMin().z()) * Vec3d::ZAxis;
        const Vec3d pos         = (_aabb.getMin() + maxNear) / 2.0f;        
        const f32 angle         = MTH_PI - (MTH_PIBY2 + _focale / 2.0f);
        const f32 distToAABB    = _camDistance ? _camDistance : tan(angle) * Max((pos.x() - _aabb.getMin().x()) / _screenRatio, pos.y() - _aabb.getMin().y());
        const Vec3d frustumPos  = pos + Vec3d::ZAxis * distToAABB;
        
        FrustumDesc::buildFrustum(_frustum, frustumPos, -Vec3d::ZAxis, Vec3d::YAxis, _focale, _screenRatio, 0.1f);
    }

    void CameraControllerManager::processEventRegisterSubject( EventRegisterCameraSubject* eventRegister )
    {
        if ( eventRegister->getActionActivator() != EventRegisterCameraSubject::ACTION_NONE )
        {
            Actor* subject = Actor::SafeDynamicCast<Actor>(eventRegister->getActivator().getObject(),Actor_CRC);
            ITF_ASSERT(subject);
            if (subject)
            {
                if ( eventRegister->getActionActivator() == EventRegisterCameraSubject::ACTION_REGISTER )
                {
                    registerSubject(subject->getRef(), eventRegister->getDelay(), bfalse, bfalse, btrue);
                }
                else
                {
                    unregisterSubject(subject->getRef());
                }
            }
        }

        if ( eventRegister->getActionChildren() != EventRegisterCameraSubject::ACTION_NONE )
        {
            if ( Actor* sender = Actor::SafeDynamicCast<Actor>(eventRegister->getSender().getObject(),Actor_CRC) )
            {
                if ( LinkComponent* linkComponent = sender->GetComponent<LinkComponent>() )
                {
                    AIUtils::LinkIterator iterator(linkComponent);

                    if ( eventRegister->getActionChildren() == EventRegisterCameraSubject::ACTION_REGISTER )
                    {
                        while ( Actor* subject = iterator.getNextActor() )
                        {
                            registerSubject(subject->getRef(), eventRegister->getDelay(), bfalse, bfalse, btrue);
                        }
                    }
                    else
                    {
                        while ( Actor* subject = iterator.getNextActor() )
                        {
                            unregisterSubject(subject->getRef());
                        }
                    }
                }
            }
        }
    }

    void CameraControllerManager::unregisterNPCFromCamera()
    {
        // clear all subjects but the players (apparently they don't re-register on checkpoint...)

        for (u32 i = 0; i < m_camSubjectList.size(); )
        {
            if (m_camSubjectList[i].m_isPlayer)
            {
                i++;
                continue;
            }

            unregisterSubject(m_camSubjectList[i].m_ref);
        }
    }

    // Camera sequence
    void CameraControllerManager::startCameraSequence( u32 _mask, const Vec3d& _pos, f32 _blendIn /*= 0.05f*/, f32 _depth /*= F32_INFINITY*/, bbool _unregisterPlayer /*= bfalse*/, f32 _focale /*= MTH_PIBY4*/ )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->startCameraSequence( _pos, _blendIn, _depth, _unregisterPlayer, _focale );
            }
        }
    }

    void CameraControllerManager::stopCameraSequence( u32 _mask, f32 _blendOut /*= 0.05f*/ )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->stopCameraSequence( _blendOut );
            }
        }
    }

    void CameraControllerManager::setCameraSequencePosition( u32 _mask, const Vec3d& _pos, f32 _depth /*= F32_INFINITY*/ )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask( _mask) )
            {
                pCCManager->setCameraSequencePosition( _pos, _depth );
            }
        }
    }

    void CameraControllerManager::startCameraSequence( const Vec3d& _pos, f32 _blend, f32 _depth, bbool _unregisterPlayer, f32 _focale )
    {        
        if ( isCameraSequenceActive() )
        {
            ITF_ASSERT_MSG( 0, "camera sequence was already activated");
            return;
        }

        m_rampUpCoeff = _blend;        

        ITF_ASSERT_CRASH( _pos.isValid(), "Invalid position");
        m_cameraSequence.m_camData.m_pos = _pos;

        if (_depth != F32_INFINITY)
        {
            m_cameraSequence.m_camData.m_depth      = _depth;
            m_cameraSequence.m_camData.m_deltaFogZ  = _depth;       
        } else
        {
            m_cameraSequence.m_camData.m_depth      = m_camData.m_depth;
            m_cameraSequence.m_camData.m_deltaFogZ  = m_camData.m_deltaFogZ;       
        }
        m_cameraSequence.m_camData.m_focale = _focale;
        m_cameraSequence.m_finalWeight = 1.f;
        m_cameraSequence.m_destinationWeight = 0.f;
        m_cameraSequence.m_weight = 0.f;

        for (u32 i = 0; i < m_camControllerList.size(); i++)
        {
            m_camControllerList[i].m_finalWeight = 0.f;
        }

        // unregister subjects
        if( _unregisterPlayer)
        {
            m_cameraSequence.m_camSubjectList = m_camSubjectList;
            m_camSubjectList.clear();
            m_updateCamSubjectList_Active = btrue;
        }
    }

    void CameraControllerManager::setCameraSequencePosition ( const Vec3d& _pos, f32 _depth )
    { 
        ITF_ASSERT_MSG( isCameraSequenceActive(), "camera sequence isn't activated");

        ITF_ASSERT_CRASH( _pos.isValid(), "Invalid position");
        m_cameraSequence.m_camData.m_pos = _pos; 

        if ( _depth != F32_INFINITY)
        {
            m_cameraSequence.m_camData.m_depth = _depth;
            m_cameraSequence.m_camData.m_deltaFogZ = _depth;
        }
    } 

    void CameraControllerManager::stopCameraSequence( f32 _blendOut )
    {
        if ( !isCameraSequenceActive() )
            return;

        m_rampUpCoeff = _blendOut;              
        m_cameraSequence.m_finalWeight = 0.f;

        // register subjects
        if ( m_cameraSequence.m_camSubjectList.size() )
        {
            m_camSubjectList = m_cameraSequence.m_camSubjectList;
            m_cameraSequence.m_camSubjectList.clear();

            m_updateCamSubjectList_Active = btrue;

            CameraSequenceTeleport();
        }

        restorePreviousMainCameraController( NULL, _blendOut );
    }

    void CameraControllerManager::CameraSequenceTeleport()
    {
        Vec3d posTeleport = m_cameraSequence.m_camData.m_pos - Vec3d::Front * m_camData.m_depth;

        // notify the controllers
        const u32 camControllerCount = m_camControllerList.size();   
        for (u32 i = 0; i < camControllerCount; i++)
        {
            CameraControllerData& data = m_camControllerList[i];
            if (data.m_controller)
                data.m_controller->teleport(posTeleport);
        }

        m_isCameraJustTeleported = btrue;
        resetInGameCamera();
    }

    void CameraControllerManager::updateCameraSequenceWeight()
    {
        m_cameraSequence.m_destinationWeight += (m_cameraSequence.m_finalWeight - m_cameraSequence.m_destinationWeight) * m_rampUpCoeff;
        m_cameraSequence.m_weight += (m_cameraSequence.m_destinationWeight - m_cameraSequence.m_weight) * m_rampUpCoeff;
    }

    const CameraModifierComponent* CameraControllerManager::getCameraModifierComponent( const AABB& _aabb, f32 _depth, bbool _withConstraint /*= bfalse*/) const
    {
        i32 priorityRef = I32_MIN;
        const CameraModifierComponent* camModRef = NULL;       
		Vec2d center = _aabb.getCenter();

        for ( u32 i=0; i<m_camModifierList.size(); i++ )
        {      
            const CameraModifierComponent* camMod = m_camModifierList[i];
            
            if ( !isCamModifierComponentActive(camMod) )
                continue;

            i32 priority = camMod->getCM().m_cameraModifierPriority;

            if ( priority >= priorityRef )
            {               
                if( camMod->getIgnoreAABB() || camMod->getModifierAABBMax().contains(center) )
                {
                    if ( !_withConstraint || camMod->isConstraint() )
                    {
                        if ( priority == priorityRef && camModRef )
                        {
                            f32 deltaZRef = f32_Abs( camModRef->GetActor()->getDepth() -_depth);
                            f32 deltaZ = f32_Abs( camMod->GetActor()->getDepth() -_depth);

                            if ( deltaZ< deltaZRef)                
                                camModRef = camMod;
                        }
                        else
                        {
                            priorityRef = priority;                     
                            camModRef = camMod;
                        } 
                    }                  
                }
            }
        }

        return camModRef;
    }

    bbool CameraControllerManager::getMapDirection( Vec2d& _mapDir, const Vec3d& _pos )
    {        
        const AABB posAABB(_pos.truncateTo2D());
        const CameraModifierComponent* camModRef = getCameraModifierComponent( posAABB, _pos.z() ); 

        if ( !camModRef )
            return bfalse;

        _mapDir = camModRef->getUpdateData().getDirection();
        ITF_ASSERT(_mapDir.isNormalized());

        return btrue;
    }

    void CameraControllerManager::getPredictPosition( const AABB& _aabb, f32 _depth, Vec3d& _predictPosition )
    {        
        ITF_ASSERT( _aabb.isValid() );

        const f32 tanFocalBy2 = f32_Abs(tan(m_camData.m_focale*0.5f));

        ITF_ASSERT_CRASH( tanFocalBy2 > 0.f, "Invalid focale");
        ITF_ASSERT_CRASH( m_screenRatio > 0.f, "Invalid screen ratio");

        _predictPosition.x() = _aabb.getMin().x() + ( _aabb.getMax().x() - _aabb.getMin().x() ) *0.5f;
        _predictPosition.y() = _aabb.getMin().y() + ( _aabb.getMax().y() - _aabb.getMin().y() ) *0.5f;
        _predictPosition.z() = _depth + f32_Max( _aabb.getWidth() /m_screenRatio, _aabb.getHeight() ) *0.5f /tanFocalBy2;
    }

    //*****************************************************************************************************************
    void CameraControllerManager::bindObject( Pickable * _pickable, bbool _updateInitPos/*=bfalse*/, bbool _ignoreZ /*= bfalse*/ )
    {
        if ( _pickable == NULL )
        {
            return;
        }

        for( u32 i = 0; i < m_bindedObjects.size(); ++i )
        {
            if ( m_bindedObjects[i].m_objectRef == _pickable->getRef() )
            {
                return;
            }
        }
        BindedObject bindedObj;
        bindedObj.m_objectRef   = _pickable->getRef();
        bindedObj.m_updateInitPos = _updateInitPos;
        bindedObj.m_offset = _pickable->getPos() - m_camData.m_pos;
        bindedObj.m_initialOffset = _pickable->getWorldInitialPos() - m_camData.m_pos;
        bindedObj.m_lastComputedPos = _pickable->getPos();
        bindedObj.m_ignoreZ = _ignoreZ;

        m_bindedObjects.push_back(bindedObj);
    }

    //*****************************************************************************************************************
    void CameraControllerManager::unbindObject( Pickable * _pickable )
    {
        if ( _pickable == NULL )
        {
            return;
        }

        for( u32 i = 0; i < m_bindedObjects.size(); ++i )
        {
            if ( m_bindedObjects[i].m_objectRef == _pickable->getRef() )
            {
                m_bindedObjects.eraseNoOrder( i );
                return;
            }
        }
    }

    //*****************************************************************************************************************
    bbool CameraControllerManager::isObjectBound( Pickable * _pickable ) const
    {
        if ( _pickable == NULL ) 
        {
            return bfalse;
        }

        for( u32 i = 0; i < m_bindedObjects.size(); ++i )
        {
            if ( m_bindedObjects[i].m_objectRef == _pickable->getRef() )
            {
                return btrue;
            }
        }
        return bfalse;
    }

    //*****************************************************************************************************************
    CameraControllerManager::BindedObject* CameraControllerManager::getBindObject( Pickable * _pickable )
    {
        for( u32 i = 0; i < m_bindedObjects.size(); ++i )
        {
            BindedObject& bindObj = m_bindedObjects[i];
            if ( bindObj.m_objectRef == _pickable->getRef() )
            {
                return &bindObj;
            }
        }
        return NULL;
    }

    //*****************************************************************************************************************
    void CameraControllerManager::getScreenCenter( Vec3d& _pos ) const
    {
        _pos.x() = m_camData.m_pos.x();
        _pos.y() = m_camData.m_pos.y();
        _pos.z() = m_camData.m_pos.z() - m_camData.m_depth;
    }

    CameraControllerData* CameraControllerManager::getCameraControllerData( const ICameraController* _cam )
    {
        const u32 count = m_camControllerList.size();

        for (u32 i = 0; i < count; i++)
        {
            CameraControllerData& data = m_camControllerList[i];

            if (data.m_controller == _cam)
            {
                return &data;
            }
        }

        return NULL;
    }

    void CameraControllerManager::setLeaderSubject( u32 _mask, ObjectRef _subject)
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask(_mask))
            {
                pCCManager->setLeaderSubject( _subject );
            }
        }
    }

    void CameraControllerManager::setLeaderSubject( ObjectRef _subject )
    {
        const i32 subjectID = getSubjectIndex(_subject );

        if( subjectID != -1 )
        {
            CameraControllerSubject& subject = m_camSubjectList[subjectID];
            subject.m_leadCamera = btrue;
        }

        ITF_ASSERT_MSG(subjectID!=-1,"Subject not registered");
    }

    void CameraControllerManager::resetInGameCamera( u32 _mask )
    {
        for ( u32 i = 0; i < s_CCManagerInstance.size(); i++ )
        {
            CameraControllerManager* pCCManager = s_CCManagerInstance[i];

            if ( pCCManager->hasMask(_mask))
            {
                pCCManager->resetInGameCamera();
            }
        }
    }

} //namespace ITF
