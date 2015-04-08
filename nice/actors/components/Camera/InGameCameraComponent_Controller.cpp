#include "precompiled_engine.h"

#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/InGameCameraComponent.h"
#endif //_ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_CAMERASUBJECTCOMPONENT_H_
#include "gameplay/Components/Misc/CameraSubjectComponent.h"
#endif //_ITF_CAMERASUBJECTCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#define MAX_CONTROLLERS 50

namespace ITF
{
    void setSpeedAverageAbs( const Vec2d& _speed, Vec2d& _average, const Vec2d& _weight)
    {
        _average.x() = f32_Abs(_speed.x()) *_weight.x() +_average.x() *(1.f -_weight.x());
        _average.y() = f32_Abs(_speed.y()) *_weight.y() +_average.y() *(1.f -_weight.y());
    }

    void setSpeedAverageReal( const Vec2d& _speed, Vec2d& _average, const Vec2d& _weight)
    {
        _average.x() = _speed.x() *_weight.x() +_average.x() *(1.f -_weight.x());
        _average.y() = _speed.y() *_weight.y() +_average.y() *(1.f -_weight.y());
    }

    void InGameCameraComponent::Controller::teleport(Vec3d _pos3D)
    {
        _pos3D.z() += m_component->getTemplate()->getCMInit().m_cameraLookAtOffset.z() ;

#ifdef ITF_SUPPORT_CHEAT
        _pos3D.z() += m_component->getCameraControllerManager()->getCheatZoom();
#endif //ITF_SUPPORT_CHEAT

        Vec2d pos2D(_pos3D.x(), _pos3D.y());

        m_subjectStance.resetSubjectStance();
        resetCameraTargetDecentering();

        m_subjectJustTeleported = btrue;
        m_subjectAABB.moveCenter(pos2D);
        m_subjectAABBCenterLast = pos2D;
        m_subjectLockedPosition_blend = 0.f;        

        m_zoneOffsetFromSubject = Vec2d::Zero;
        m_zoneSpeedByDepth = Vec2d::Zero;
        m_zoneSpeedReal = Vec2d::Zero;
        m_zoneAABBCenterLast = pos2D;
        m_zoneAABB.moveCenter(pos2D);

        m_cameraBaseMovingSpeed = Vec3d::Zero;
        m_cameraBaseMovingLockedDestY = bfalse;
        m_cameraBasePos = _pos3D;
        m_cameraBasePosDest = _pos3D;
        m_cameraTargetPos = _pos3D;  

        m_cameraBaseMovingBlend = Vec2d::Zero;    
        m_cameraBaseMovingSpeedAverageAbsOfTarget = Vec2d::Zero;
        m_cameraBaseMovingSpeedAverageOfTarget = Vec2d::Zero;
        m_cameraBaseMovingBlendYMinInSticked = 0.f;

        m_rayCastCollisionDownFound = bfalse;      

        m_screenBaseAABB.moveCenter(pos2D);
        m_screenTargetAABB.moveCenter(pos2D);        
    }

    void InGameCameraComponent::setControllersWeight(f32 _dt)
    {
        const u32 controllerCount = m_controllers.size();
        for ( u32 i=0; i<controllerCount; i++)
        {
            Controller& controller = m_controllers[i];

            if ( controller.isActive() )
            {
                if ( getCameraControllerManager()->isInGameCameraJustReseted() )
                {
                    controller.m_weightTime = 1.f;
                    controller.m_weight = 1.f;
                }
                else if ( controller.m_weightTime <1.f)
                {
                    controller.m_weightTime += _dt *controller.m_weightRegisterSpeed;
                    controller.m_weightTime = Min( controller.m_weightTime, 1.f);
                    controller.m_weight = m_curveLogistic.getValue(controller.m_weightTime);
                }
            }
            else
            {
                controller.m_weight = 0.0f;
                if ( controller.m_weightTime >= 0.f && !controller.m_isPaused )
                {
                    controller.m_weightTime -= _dt *m_subjectUnregisterSpeed;
                    controller.m_weight = m_curveLogistic.getValue(controller.m_weightTime);
                }
            }  
        }
    }

    void InGameCameraComponent::updateSubjectGlobalDatas( const Controller& _controller)
    {
        if ( m_subjectGlobalAABB.isValid() )
        {
            m_subjectGlobalAABB.grow(_controller.m_subjectAABB);
        }
        else
        {
            m_subjectGlobalAABB = _controller.m_subjectAABB;
        }

        m_subjectGlobalDepthMin = Min( m_subjectGlobalDepthMin, _controller.m_subjectPosReal.z());
    }

    void InGameCameraComponent::setControllersSubject()
    {
        m_mainController = NULL;
        m_controllerActiveCountLast = m_controllerActiveCount;
        m_controllerActiveCount = 0;
        m_subjectGlobalAABB.invalidate();
        m_subjectGlobalDepthMin = F32_INFINITY;
        m_isLeaderController = bfalse;
        f32 sujectDepthMax = -F32_INFINITY;
        bbool isPlayer = bfalse;
        
        ITF_VECTOR<Controller>::iterator it = m_controllers.begin();
        while( it != m_controllers.end() )
        {
            Controller& controller = *it;

            //  remove dead controllers 
            if ( !controller.isActive() && ( controller.m_weightTime < 0.f || getCameraControllerManager()->isInGameCameraJustReseted() ) )
            {
                it = m_controllers.erase(it);
            }
            else
            {
                if ( controller.isActive() )
                {                                        
                    if ( Actor* pActor = AIUtils::getActor(controller.m_subjectStance.m_objectRef) )
                    {
                        m_controllerActiveCount ++;

                        if ( controller.m_subjectStance.m_isPlayer )
                            isPlayer = btrue;

                        controller.setSubject(pActor);
                        controller.m_wasMainController = controller.m_isMainController;
                        controller.m_isMainController = bfalse;

                        sujectDepthMax = Max( sujectDepthMax, controller.m_subjectPosReal.z());

                        if ( !m_isLeaderController )
                        {
                            //  set subject global aabb
                            if ( controller.m_subjectStance.m_leadCamera )
                            {
                                if ( m_cameraModifier ) // after teleport with leader outside modifier
                                {
                                    m_subjectGlobalAABB = controller.m_subjectAABB;
                                    m_subjectGlobalDepthMin = controller.m_subjectPosReal.z();
                                }
                                else
                                {
                                    updateSubjectGlobalDatas(controller);
                                }

                                m_mainController = &controller;
                                m_isLeaderController = btrue;
                            }
                            else
                            {
                                updateSubjectGlobalDatas(controller);
                            }
                        }

                        it++;
                    }        
                    else
                    {
                        if ( m_controllers.size() == MAX_CONTROLLERS )
                        {
                            it = m_controllers.erase(it);
                        }
                        else
                        {
                            controller.m_subjectStance.m_objectRef.invalidate();
                            controller.m_weightDest = 0.f;
                            it++;
                        }
                    }
                }
                else if ( controller.m_isPaused )
                {
                    it++;
                    updateSubjectGlobalDatas(controller);
                }
                else
                {
                    it++;
                }
            }
        }

        if ( m_controllerActiveCount )
        {
            if ( !m_CMDestFinal.m_ignoreZ && isPlayer )
                m_subjectPosZ = sujectDepthMax;
        }
    }

    void InGameCameraComponent::removeControllers()
    {       
        const SafeArray<CameraControllerSubject>& subjectList = getCameraControllerManager()->getCameraSubjectList();

        const u32 controllersListCount = m_controllers.size();
        for ( u32 controllerId=0; controllerId<controllersListCount; controllerId++)
        {
            Controller& controller = m_controllers[controllerId];

            if ( !controller.isActive() )
                continue;

            bbool subjectRegistered = bfalse;

            for ( u32 subjectListId=0; subjectListId<subjectList.size(); subjectListId++)
            {
                const CameraControllerSubject& subject = subjectList[subjectListId];

                if ( subject.m_ref == controller.m_subjectStance.m_objectRef )
                {
                    subjectRegistered = btrue;

                    // update status, may has changed
                    controller.m_subjectStance.m_leadCamera = subject.m_leadCamera;

                    break;
                }
                else if ( subject.m_refOld == controller.m_subjectStance.m_objectRef )
                {
                    // switch subject
                    controller.m_subjectStance.m_objectRef = subject.m_ref;

                    subjectRegistered = btrue;
                    break;
                }
            }

            if ( !subjectRegistered )
            {
                if ( isPlayingPageSequence(controller.m_subjectStance.m_objectRef) )
                {
                    controller.m_isPaused = btrue;
                }
                else if ( controller.m_subjectStance.m_isPlayer && isPlayingShooter(controller.m_subjectStance.m_objectRef) )
                {
                    controller.m_isPaused = btrue;
                }

                controller.m_subjectStance.m_objectRef.invalidate();
                controller.m_weightDest = 0.f;
            }
        }
    }

    void InGameCameraComponent::addControllers(bbool _teleportIfAlone /*= bfalse*/)
    {        
        const SafeArray<CameraControllerSubject>& subjectList = getCameraControllerManager()->getCameraSubjectList();

        for ( u32 i=0; i<subjectList.size(); i++)
        {
            if ( m_controllers.size() == MAX_CONTROLLERS )
            {
                break;
            }

            const CameraControllerSubject& subject = subjectList[i];

            bbool subjectAlreadyRegistered = bfalse;
            const u32 controllersListCount = m_controllers.size();
            for ( u32 j=0; j<controllersListCount; j++)
            {
                if ( subject.m_ref == m_controllers[j].m_subjectStance.m_objectRef )
                {
                    subjectAlreadyRegistered = btrue;  
                    break;
                }
            }

            // add new subject
            if ( !subjectAlreadyRegistered )
            {
                const Actor* actor = (Actor*)GETOBJECT( subject.m_ref );

                if ( actor )
                {
                    Controller newController(this);
                    newController.m_subjectStance.m_objectRef = subject.m_ref;
                    newController.m_subjectStance.m_isPlayer = subject.m_isPlayer;
                    newController.m_subjectStance.m_leadCamera = subject.m_leadCamera;
                    newController.m_registerStartPos = m_cameraTargetAveragePos;
                    newController.m_depthRef = m_subjectPosZ;

                    if ( getCameraControllerManager()->isInGameCameraJustReseted() )
                    {
                        newController.m_weightRegisterSpeed = F32_INFINITY;
                        newController.m_weight = 1.f;
                    }
                    else
                    {
                        if ( subject.m_delayToRegister >0.f )
                            newController.m_weightRegisterSpeed = 1.f /subject.m_delayToRegister;
                        else
                            newController.m_weightRegisterSpeed = m_subjectRegisterSpeed;
                    }

                    newController.teleport(actor->getPos());
                    m_controllers.push_back(newController);

                    if ( _teleportIfAlone && getCameraControllerManager()->isInGameCameraJustReseted() && m_controllers.size() == 1 )
                    {
                        const f32 prevDepth = m_depth;
                        const f32 prevSubjectPosZ = m_subjectPosZ;   

                        teleport(actor->getPos());

                        if ( getCameraControllerManager()->isCameraJustTeleported() ) // to restore teleport z value
                        {
                            m_depth = prevDepth;
                            m_subjectPosZ = prevSubjectPosZ;   
                        }                        
                    }
                }                
            }
        }

        ITF_ASSERT_MSG(m_controllers.size()<MAX_CONTROLLERS, "Too many subjects registered then unregistered in camera");
    }

    void InGameCameraComponent::setControllersPosition()
    {    
        const u32 controllerListCount = m_controllers.size();
        for ( u32 i=0; i<controllerListCount; i++)
        {
            Controller& controller = m_controllers[i];

            if ( controller.isActive() )
            {
                controller.update();
            }
        }
    }

    void InGameCameraComponent::Controller::update()
    {
        setZone();
        setScreenBase();
        initRayCastCollisionDown();
        setCameraBase();
        setCameraTarget();
    } 

    //////////////////////////////////////////////////////////////////////////
    /// Screen Target
    void InGameCameraComponent::Controller::setScreenTarget()
    {
        const f32 depth = m_cameraTargetPos.z() -m_component->m_subjectPosZ;
        Vec2d screenTargetLongBy2;
        screenTargetLongBy2.y() = f32_Abs( tan(m_component->m_CMCur.m_focale *0.5f) *depth);
        screenTargetLongBy2.x() = screenTargetLongBy2.y() *m_component->getCameraControllerManager()->m_screenRatio;

        m_screenTargetAABB.setMin( m_cameraTargetPos.truncateTo2D() -screenTargetLongBy2 );
        m_screenTargetAABB.setMax( m_cameraTargetPos.truncateTo2D() +screenTargetLongBy2 );
    }

    //////////////////////////////////////////////////////////////////////////
    /// Screen Base
    void InGameCameraComponent::Controller::setScreenBaseAABB()
    {
        m_screenBaseAABB.setMin( m_cameraBasePos.truncateTo2D() -m_component->m_screenRealLongBy2 );
        m_screenBaseAABB.setMax( m_cameraBasePos.truncateTo2D() +m_component->m_screenRealLongBy2 );
    }

    void InGameCameraComponent::Controller::setControllerScreenRealLimit()
    {
        m_screenRealLimitUp = m_component->m_screenRealAABB.getMin().y() +m_component->m_screenRealAABB.getHeight() *m_component->m_CMCur.m_camModifierTemplate->m_screenLimitUpScale;
        m_screenRealLimitUpDist = m_screenRealLimitUp -m_component->m_screenRealAABB.getMin().y();
        m_screenRealLimitUpTargetDist = m_subjectAABB.getCenter().y() -m_component->m_screenRealAABB.getMin().y();

        if ( m_screenRealLimitUpDist > 0.f )
        {
            m_screenRealLimitUpRatio = 1.f - m_screenRealLimitUpTargetDist /m_screenRealLimitUpDist;
            m_screenRealLimitUpRatio = Clamp( m_screenRealLimitUpRatio, 0.f, 1.f);

            m_screenRealLimitUpSpeedMax = m_component->m_CMCur.m_camModifierTemplate->m_screenLimitUpSpeedMax *m_screenRealLimitUpDist *m_screenRealLimitUpRatio;
        }
    }

    void InGameCameraComponent::Controller::setScreenBase()
    {
        setScreenBaseAABB();
        setControllerScreenRealLimit();
    }

    //////////////////////////////////////////////////////////////////////////
    /// Zone
    void InGameCameraComponent::Controller::setZoneSpeed()
    {
        if ( m_subjectStance.m_isLockedPosition )
            m_zoneSpeedReal = m_subjectStance.m_lockedPosition -m_subjectStance.m_lockedPositionLast;
        else if ( m_subjectLockedPosition_blend >0.f )
            m_zoneSpeedReal = Vec2d::Zero;
        else
            m_zoneSpeedReal = m_zoneAABB.getCenter() -m_zoneAABBCenterLast;

        m_zoneSpeedReal *= 60.f;
        m_zoneSpeedByDepth = m_zoneSpeedReal *m_component->m_depthCoeff;
    }

    void InGameCameraComponent::Controller::setZone()
    {    
        m_zoneAABBCenterLast = m_zoneAABB.getCenter(); 

        setZoneAABB();
        setZoneOffsetX();
        setZoneOffsetY();

        // set center
        m_zoneAABB.moveCenter( m_subjectAABB.getCenter() +m_zoneOffsetFromSubject);

        // set speed
        setZoneSpeed();                
    }

    void InGameCameraComponent::Controller::setZoneAABB()
    {
        Vec2d offset;
        offset.x() = (m_subjectAABB.getWidth() *0.5f)  +m_component->m_subjectZoneNeutral.x();
        offset.y() = (m_subjectAABB.getHeight() *0.5f) +m_component->m_subjectZoneNeutral.y();

        m_zoneAABB.setMin( m_zoneAABBCenterLast -offset);
        m_zoneAABB.setMax( m_zoneAABBCenterLast +offset);
    }

    f32 InGameCameraComponent::Controller::getZoneOffsetXMax()
    {
        return ( m_zoneAABB.getWidth() -m_subjectAABB.getWidth() ) *0.5f;
    }

    f32 InGameCameraComponent::Controller::getZoneOffsetYMax()
    {
        return ( m_zoneAABB.getHeight() -m_subjectAABB.getHeight() ) *0.5f;
    }

    void InGameCameraComponent::Controller::setZoneOffsetX()
    {
        if ( m_subjectAABB.getWidth() >= m_zoneAABB.getWidth() )
        {
            m_zoneOffsetFromSubject.x() = 0.f;
        }
        else if ( m_subjectAABB.getMin().x() < m_zoneAABB.getMin().x() )
        {
            m_zoneOffsetFromSubject.x() = getZoneOffsetXMax();
        }
        else if ( m_subjectAABB.getMax().x() > m_zoneAABB.getMax().x() )
        {
            m_zoneOffsetFromSubject.x() = -getZoneOffsetXMax();
        }
        else
        {
            m_zoneOffsetFromSubject.x() = m_zoneAABB.getCenter().x() -m_subjectAABB.getCenter().x();
        }
    }

    void InGameCameraComponent::Controller::setZoneOffsetY()
    {
        if ( m_subjectAABB.getHeight() >= m_zoneAABB.getHeight() )
        {
            m_zoneOffsetFromSubject.y() = 0.f;
        }
        else if ( m_subjectAABB.getMin().y() < m_zoneAABB.getMin().y() )
        {
            m_zoneOffsetFromSubject.y() = getZoneOffsetYMax();
        }
        else if ( m_subjectAABB.getMax().y() > m_zoneAABB.getMax().y() )
        {
            m_zoneOffsetFromSubject.y() = -getZoneOffsetYMax();
        }
        else
        {
            m_zoneOffsetFromSubject.y() = m_zoneAABB.getCenter().y() -m_subjectAABB.getCenter().y();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /// Camera Target
    void InGameCameraComponent::Controller::resetCameraTargetDecentering()
    {
        m_cameraTargetDecentering = Vec2d::Zero;
        m_cameraTargetDecenteringDir = Vec2d::Zero;
        m_cameraTargetDecenteringDest = Vec2d::Zero;
        m_cameraTargetDecenteringSpeedAverageRealOfTarget = Vec2d::Zero;
        m_cameraTargetDecenteringSpeedAverageAbsOfTarget = Vec2d::Zero;
        m_cameraTargetDecenteringBlend = Vec2d::Zero;
        m_cameraTargetDecenteringInertie = Vec2d::Zero;
        m_cameraTargetDecenteringDestSpeed = Vec2d::Zero;
        m_cameraTargetDecenteringSpeed = Vec2d::Zero;
        m_cameraTargetDecenteringCoeffLast = Vec2d::Zero;
        m_cameraTargetDecenteringDirFromSubjectStance = Vec2d::Zero;
    }

    f32 InGameCameraComponent::Controller::getCameraTargetDecenteringDestinationX()
    {      
        if ( m_component->m_controllerActiveCount >1 || m_subjectStance.m_isLockedPosition )
            return 0.f;

        if ( m_component->m_cameraModifier && !m_component->m_cameraModifier->getCM().m_useDecentering )
            return 0.f;

        f32 decenteringCoeff = 0.f;

        if ( m_subjectStance.m_joystickMoveNormalized.x() == 0.f 
            && getSign(m_cameraTargetDecenteringDir.x()) == getSign(m_cameraTargetDecentering.x())
            && !m_subjectStance.m_justStick && !m_component->getCameraControllerManager()->isInGameCameraJustReseted() )
        {
            decenteringCoeff = m_cameraTargetDecenteringCoeffLast.x(); 
        }
        else
        {
            f32 coeff = 0.f;
            const f32 speedDelta = m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringSpeedMax.x() -m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringSpeedMin.x();

            if ( speedDelta > 0.f )
            {
                coeff = ( m_cameraTargetDecenteringSpeedAverageAbsOfTarget.x() -m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringSpeedMin.x() ) /speedDelta;
                coeff = Clamp( coeff, 0.f, 1.f);
            }

            f32 decenteringDelta = m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringOffsetAtSpeedMax.x() -m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringOffsetAtSpeedMin.x();

            decenteringCoeff = m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringOffsetAtSpeedMin.x() +coeff*decenteringDelta;
            m_cameraTargetDecenteringCoeffLast.x() = decenteringCoeff;
        }

        // decentering by depth
        f32 depthCoeff = 0.f;
        f32 depthDelta = m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringDepthMax -m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringDepthMin;
        if ( depthDelta >0.f )
        {
            depthCoeff = 1.f -( m_component->m_depth -m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringDepthMin) /depthDelta;
            depthCoeff = Clamp( depthCoeff, 0.f, 1.f);
        }

        f32 destination = decenteringCoeff *m_component->m_screenRealLongBy2.x() *m_cameraTargetDecenteringDir.x() *depthCoeff;
        return destination;
    }

    void InGameCameraComponent::Controller::setCameraTargetDecenteringDirectionX()
    {
        if ( m_subjectStance.m_joystickMoveNormalized.x() != 0.f && m_subjectStance.m_hang )
        {
            m_cameraTargetDecenteringDirFromSubjectStance.x() = m_subjectStance.m_joystickMoveNormalized.x();
        }
        else
        {            
            if ( m_subjectStance.m_performUTurn )
            {
                if ( !m_subjectStance.m_performUTurnLast )
                {
                    m_cameraTargetDecenteringDirFromSubjectStance.x() = -m_subjectStance.m_lookDir.x(); 
                }                   
            }
            else if ( m_subjectStance.m_wallSlide )
            {
                m_cameraTargetDecenteringDirFromSubjectStance.x() = -m_subjectStance.m_lookDir.x(); 
            }
            else if ( m_subjectStance.m_isLockedPosition)
            {
                m_cameraTargetDecenteringDirFromSubjectStance.x() = 0.f;
            }
            else
            {
                m_cameraTargetDecenteringDirFromSubjectStance.x() = m_subjectStance.m_lookDir.x();
            }
        }

        m_cameraTargetDecenteringDir.x() = m_cameraTargetDecenteringDirFromSubjectStance.x() *m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringSubjectLookDirWeight.x() +m_cameraTargetDecenteringSpeedAverageRealOfTarget.x();
        m_cameraTargetDecenteringDir.x() = Clamp( m_cameraTargetDecenteringDir.x(), -1.f, 1.f );
    }    

    void InGameCameraComponent::Controller::setCameraTargetDecenteringXBlendAndInertie()
    {
        f32 coeff = 0.f;
        const f32 speedDelta = m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringSpeedMax.x() -m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringSpeedMin.x();

        if ( speedDelta > 0.f )
        {
            coeff = ( m_cameraTargetDecenteringSpeedAverageAbsOfTarget.x() -m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringSpeedMin.x() ) /speedDelta;
            coeff = Clamp( coeff, 0.f, 1.f);
        }

        //  blend
        f32 blendDelta = m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringBlendAtSpeedMax.x() -m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringBlendAtSpeedMin.x();
        m_cameraTargetDecenteringBlend.x() = m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringBlendAtSpeedMin.x() +coeff*blendDelta;
        m_cameraTargetDecenteringBlend.x() = Clamp( m_cameraTargetDecenteringBlend.x(), 0.f, 1.f);

        // inertie
        f32 inertieDelta = m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringInertieAtSpeedMax.x() -m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringInertieAtSpeedMin.x();
        m_cameraTargetDecenteringInertie.x() = m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringInertieAtSpeedMin.x() +coeff*inertieDelta;
        m_cameraTargetDecenteringInertie.x() = Clamp( m_cameraTargetDecenteringInertie.x(), 0.f, 1.f);
    }

    void InGameCameraComponent::Controller::setCameraTargetDecenteringX()
    {
        // set blend and inertie
        setCameraTargetDecenteringXBlendAndInertie();

        // set destination
        setCameraTargetDecenteringDirectionX();

        // set current destination
        f32 speed = ( getCameraTargetDecenteringDestinationX() -m_cameraTargetDecenteringDest.x() ) *m_cameraTargetDecenteringBlend.x();
        m_cameraTargetDecenteringDestSpeed.x() = speed + m_cameraTargetDecenteringDestSpeed.x() *m_cameraTargetDecenteringInertie.x(); 
        m_cameraTargetDecenteringDest.x() += m_cameraTargetDecenteringDestSpeed.x();

        // set current 
        speed = ( m_cameraTargetDecenteringDest.x() -m_cameraTargetDecentering.x() ) *m_cameraTargetDecenteringBlend.x();
        m_cameraTargetDecenteringSpeed.x() = 0.0f + m_cameraTargetDecenteringSpeed.x() *m_cameraTargetDecenteringInertie.x();
        //m_cameraTargetDecenteringSpeed.x() = speed + m_cameraTargetDecenteringSpeed.x() *m_cameraTargetDecenteringInertie.x(); //SEB : Ugly hack : enleve l'anticipation du scroll
        m_cameraTargetDecentering.x() += m_cameraTargetDecenteringSpeed.x();        
    }

    void InGameCameraComponent::Controller::setCameraTargetDecentering()
    {
        if ( m_component->getCameraControllerManager()->isInGameCameraJustReseted())
        {
            resetCameraTargetDecentering();
            setCameraTargetDecenteringX();

            m_cameraTargetDecenteringDest.x() = getCameraTargetDecenteringDestinationX();
            m_cameraTargetDecentering = m_cameraTargetDecenteringDest;
        }
        else
        {
            setSpeedAverageAbs( m_zoneSpeedReal, m_cameraTargetDecenteringSpeedAverageAbsOfTarget, m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringSmooth);
            setSpeedAverageReal( m_zoneSpeedReal, m_cameraTargetDecenteringSpeedAverageRealOfTarget, m_component->m_CMCur.m_camModifierTemplate->m_cameraDecenteringSmooth);

            setCameraTargetDecenteringX();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /// Camera Base
    void InGameCameraComponent::Controller::setCameraBaseBlendX()
    {
        f32 coeff = 0.f;
        const f32 speedDelta = m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingSpeedMax.x() -m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingSpeedMin.x();

        if ( speedDelta > 0.f )
        {
            coeff = ( m_cameraBaseMovingSpeedAverageAbsOfTarget.x() -m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingSpeedMin.x() ) /speedDelta;
            coeff = Clamp( coeff, 0.f, 1.f);
        }

        const f32 blendDelta = m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingBlendAtSpeedMax.x() -m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingBlendAtSpeedMin.x();

        m_cameraBaseMovingBlend.x() = m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingBlendAtSpeedMin.x() +coeff*blendDelta;
        m_cameraBaseMovingBlend.x() = Clamp( m_cameraBaseMovingBlend.x(), 0.f, 1.f);
    }

    void InGameCameraComponent::Controller::setCameraBasePositionX()
    {
        setCameraBaseBlendX();

        m_cameraBaseMovingSpeed.x() = ( m_zoneAABB.getCenter().x() -m_cameraBasePos.x() ) *m_cameraBaseMovingBlend.x();

        m_cameraBasePos.x() += m_cameraBaseMovingSpeed.x();       
    }

    void InGameCameraComponent::Controller::setCameraBaseBlendY()
    {
        f32 coeff = 0.f;
        const f32 speedDelta = m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingSpeedMax.y() -m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingSpeedMin.y();

        if ( speedDelta > 0.f )
        {
            coeff = ( m_cameraBaseMovingSpeedAverageAbsOfTarget.y() -m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingSpeedMin.y() ) /speedDelta;
            coeff = Clamp( coeff, 0.f, 1.f);
        }

        const f32 blendDelta = m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingBlendAtSpeedMax.y() -m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingBlendAtSpeedMin.y();

        m_cameraBaseMovingBlend.y() = m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingBlendAtSpeedMin.y() +coeff*blendDelta;
        m_cameraBaseMovingBlend.y() = Clamp( m_cameraBaseMovingBlend.y(), 0.f, 1.f);
    }
   
    void InGameCameraComponent::Controller::increazeCameraMovingBaseSpeedAverageAbsOfTarget()
    {
        if ( m_subjectStance.m_stick && m_component->m_screenRealLongBy2.y() > 0 )
        {
            const f32 positionDelta = f32_Abs(m_zoneAABB.getCenter().y() -m_cameraBasePos.y());
            const f32 coeff = Clamp( positionDelta /m_component->m_screenRealLongBy2.y(), 0.f, 1.f);

            m_cameraBaseMovingBlendYMinInSticked = m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingBlendAtSpeedMin.y() +(m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingBlendAtSpeedMax.y() -m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingBlendAtSpeedMin.y()) *coeff;

            if ( m_cameraBaseMovingBlend.y() < m_cameraBaseMovingBlendYMinInSticked  )
            {
                m_cameraBaseMovingSpeedAverageAbsOfTarget.y() += m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingBlendAccelerationJustSticked *m_component->m_depthCoeff;
            }
        }
    }

    void InGameCameraComponent::Controller::setCameraBasePositionYDest()
    {  
        const SubjectPerform subjectPerform = m_component->getCameraControllerManager()->getSubjectPerform( m_subjectStance.m_objectRef);

        if ( subjectPerform.m_justBounceJumped || subjectPerform.m_justReceivedPunchUp )
            m_component->m_resetDezoomTime = btrue;

        if ( ( m_subjectStance.m_wallSlide || m_subjectStance.m_wallSlideLast ) && subjectPerform.m_justJumped && isRayCastCollisionUp() )
        {
            if ( m_cameraBaseMovingLockedDestY )
            {
                m_cameraBaseMovingLockedDestY = bfalse;

                // remove speed average abs
                m_cameraBaseMovingSpeedAverageAbsOfTarget.y() = 0.f;
            }
        }
        else if ( m_subjectStance.m_justUnstick || ( subjectPerform.m_justJumped && m_subjectStance.m_jumpOnAirTimerAllowedLast ) )
        {
            m_cameraBaseMovingLockedDestY = btrue;
            m_cameraBasePosDest.y() = m_zoneAABBCenterLast.y();           
        }
        else if ( !m_cameraBaseMovingLockedDestY && ( subjectPerform.m_justBounceJumped || subjectPerform.m_justReceivedPunchUp ))
        {
            m_cameraBaseMovingLockedDestY = btrue;
            m_cameraBasePosDest.y() = m_zoneAABB.getCenter().y();
        }
        else if ( m_subjectStance.m_justStick || subjectPerform.m_justJumped )
        {
            if ( m_cameraBaseMovingLockedDestY )
            {
                m_cameraBaseMovingLockedDestY = bfalse;

                // remove speed average abs
                m_cameraBaseMovingSpeedAverageAbsOfTarget.y() = 0.f;                
            }                        
        }

        if ( m_cameraBaseMovingLockedDestY )
        {
            if ( m_zoneAABB.getCenter().y() < m_cameraBasePosDest.y() )
            {
                m_cameraBaseMovingLockedDestY = bfalse; 
                m_cameraBasePosDest.y() = m_zoneAABB.getCenter().y();
                m_cameraBaseMovingSpeedAverageAbsOfTarget.y() *= 0.5f; 
            }
            else if ( m_subjectSpeed.y() > m_screenRealLimitUpSpeedMax || ( m_subjectStance.m_windForce.y() >0.f && m_subjectSpeed.y() >0.f) )
            {
                m_cameraBaseMovingLockedDestY = bfalse; 
                m_cameraBasePosDest.y() = m_zoneAABB.getCenter().y();

                // remove speed average abs
                m_cameraBaseMovingSpeedAverageAbsOfTarget.y() = 0.f; 

                if ( m_subjectSpeed.y() > m_screenRealLimitUpSpeedMax )
                {
                    m_component->m_isScreenLimitCrossed = btrue;
                }
            }
        }        
        else
        {
            m_cameraBasePosDest.y() = m_zoneAABB.getCenter().y();

            increazeCameraMovingBaseSpeedAverageAbsOfTarget();
        }        
    }

    void InGameCameraComponent::Controller::setCameraBasePositionY()
    {    
        setCameraBasePositionYDest();

        if ( !m_subjectStance.m_stick && m_zoneSpeedReal.y() < 0.f )
        {
            if ( isRayCastCollisionDown() )
            {
                m_cameraBaseMovingSpeedAverageAbsOfTarget.y() = Min( m_cameraBaseMovingSpeedAverageAbsOfTarget.y(), f32_Abs(m_rayCastDown.y()) );               
            }
        }        

        setCameraBaseBlendY();

        m_cameraBaseMovingSpeed.y() = ( m_cameraBasePosDest.y() -m_cameraBasePos.y() ) *m_cameraBaseMovingBlend.y();
        m_cameraBasePos.y() += m_cameraBaseMovingSpeed.y(); 
    }

    void InGameCameraComponent::Controller::setCameraBasePositionZ()
    {
        m_cameraBasePos.z() = (m_ignoreZ ? m_backupPreviousZ : m_component->m_subjectPosZ) + m_component->m_CMCur.m_cameraLookAtOffset.z();

#ifdef ITF_SUPPORT_CHEAT
        m_cameraBasePos.z() += m_component->getCameraControllerManager()->getCheatZoom();
#endif //ITF_SUPPORT_CHEAT
    }

    void InGameCameraComponent::Controller::setCameraBasePosition()
    {
        setCameraBasePositionX();
        setCameraBasePositionY();
        setCameraBasePositionZ();
    }

    void InGameCameraComponent::Controller::setCameraBase()
    {
        const Vec2d smooth(1.0f,1.0f);// = m_subjectStance.m_leadCamera ? m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingSmooth *m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingSmoothCoeffForLeader : m_component->m_CMCur.m_camModifierTemplate->m_cameraMovingSmooth; 

        setSpeedAverageAbs( m_zoneSpeedByDepth, m_cameraBaseMovingSpeedAverageAbsOfTarget, smooth );
        setSpeedAverageReal( m_zoneSpeedByDepth, m_cameraBaseMovingSpeedAverageOfTarget, smooth );
        setCameraBasePosition();        
    }

    //////////////////////////////////////////////////////////////////////////
    /// Camera Target
    void InGameCameraComponent::Controller::setCameraTargetPos()
    {
        m_cameraTargetPos.x() = m_cameraBasePos.x() +m_cameraTargetDecentering.x() +m_component->m_CMCur.m_cameraLookAtOffset.x();
        m_cameraTargetPos.y() = m_cameraBasePos.y() +m_cameraTargetDecentering.y() +m_component->m_CMCur.m_cameraLookAtOffset.y() +m_component->m_CMCur.m_directionLookAtOffset;
        m_cameraTargetPos.z() = m_cameraBasePos.z();

        if ( m_weight < 1.f )
        {
            m_cameraTargetPos.x() = m_component->m_cameraTargetAveragePos.x() +(m_cameraTargetPos.x() -m_component->m_cameraTargetAveragePos.x()) *m_weight;
            m_cameraTargetPos.y() = m_component->m_cameraTargetAveragePos.y() +(m_cameraTargetPos.y() -m_component->m_cameraTargetAveragePos.y()) *m_weight;
        }
    }

    void InGameCameraComponent::Controller::setCameraTarget()
    {        
        setCameraTargetDecentering();
        setCameraTargetPos();
        setScreenTarget();
    }

    //////////////////////////////////////////////////////////////////////////
    /// Ray Cast
    void InGameCameraComponent::Controller::initRayCastCollisionDown()
    {
        m_rayCastDown = Vec2d::Zero;
        m_rayCastCollisionDownFound = bfalse;
        m_rayCastDownLengthMax = m_component->m_CMCur.m_camModifierTemplate->m_rayCastScaleMax * m_component->m_screenRealAABB.getHeight();
    }

    bbool InGameCameraComponent::Controller::isRayCastCollisionDown()
    {
        if ( m_subjectSpeed.y() >=0.f || m_component->m_CMCur.m_camModifierTemplate->m_rayCastScaleMax <= 0.f )
            return bfalse;

        // init rayCast
        m_rayCastDown = m_subjectSpeed;

        // limit rayCast        
        if ( m_rayCastDown.sqrnorm() > m_rayCastDownLengthMax *m_rayCastDownLengthMax)
        {
            m_rayCastDown.setLength(m_rayCastDownLengthMax);
        }

        // test rayCast with environment
        PhysRayCastContactsContainer contacts;
        const Vec2d p0 = m_subjectAABB.getCenter();
        const Vec2d p1 = p0 +m_rayCastDown;

        if ( !PHYSWORLD->rayCastEnvironment( p0, p1, ECOLLISIONFILTER_ENVIRONMENT, m_subjectPosReal.z(), contacts) )
        {
            return bfalse;
        }

        // get nearest collision
        f32 coeff = F32_INFINITY;
        for ( u32 i=0; i<contacts.size(); i++ )
        {
            coeff = Min( coeff, contacts[i].m_t0 );
        }

        m_rayCastDown = ( p1 -p0) *coeff;
        m_rayCastCollisionDownFound = btrue;

        return btrue;
    }

    bbool InGameCameraComponent::Controller::isRayCastCollisionUp()
    {
        if ( m_subjectSpeed.y() <=0.f )
            return bfalse;

        // test rayCast with environment
        PhysRayCastContactsContainer contacts;
        const Vec2d p0 = m_subjectAABB.getCenter();
        const Vec2d p1 = p0 +m_subjectSpeed;
       
        if ( !PHYSWORLD->rayCastEnvironment( p0, p1, ECOLLISIONFILTER_ENVIRONMENT, m_subjectPosReal.z(), contacts) )
        {
            return bfalse;
        }

#ifdef DEVELOPER_JAY_CAMERA
        GFX_ADAPTER->drawDBGLine(p0, p1, Color::violet());
#endif

        return btrue;       
    }

    //////////////////////////////////////////////////////////////////////////
    // Subject
    void InGameCameraComponent::Controller::setSubjectSpeed( Vec2d _pos )
    {
        const Vec2d speedDest = ( _pos -m_subjectAABBCenterLast) *60.f;
        if ( m_subjectStance.m_justStick || m_subjectLockedPosition_blend>0.f )
        {
            m_subjectSpeed = Vec2d::Zero;
        }
        else
        {
            m_subjectSpeed = speedDest;
        }

        m_subjectAABBCenterLast = _pos;
    }

    void InGameCameraComponent::Controller::setSubjectAABB(const Actor* _pActor)
    {
        const CameraSubjectComponent* subjectComp = _pActor->GetComponent<CameraSubjectComponent>();
        if ( subjectComp )
        {
            m_subjectAABB = subjectComp->getAABB();
        }
        else
        {
            const Vec2d pos = _pActor->get2DPos();
            const Vec2d offset(0.5f,0.5f);
            m_subjectAABB.setMin( pos -offset);
            m_subjectAABB.setMax( pos +offset);
        }

        const Vec2d centerAABB = m_subjectAABB.getCenter();
        m_subjectPosReal.x() = centerAABB.x();
        m_subjectPosReal.y() = centerAABB.y();
        m_subjectPosReal.z() = m_depthRef + ( _pActor->getDepth() -m_depthRef ) *m_weight;        
    }

    void InGameCameraComponent::Controller::setSubjectLockedPosition()
    {
        const Vec2d subjectPosReal = m_subjectAABB.getCenter();

        if ( m_subjectStance.m_isLockedPosition)
        {
            if ( !m_subjectStance.m_wasLockedPosition )
            {
                m_subjectStance.m_lockedPositionLast = m_subjectStance.m_lockedPosition;

                if ( m_component->getCameraControllerManager()->isInGameCameraJustReseted())
                    m_subjectLockedPosition_blend = 1.f;        
            }

            m_subjectLockedPosition_blend += m_component->m_CMCur.m_camModifierTemplate->m_lockPositionBlendOnEnter;
        }
        else
            m_subjectLockedPosition_blend -= m_component->m_CMCur.m_camModifierTemplate->m_lockPositionBlendOnExit;

        m_subjectLockedPosition_blend = Clamp( m_subjectLockedPosition_blend, 0.f, 1.f);

        if ( m_subjectLockedPosition_blend > 0.f)
        {
            const f32 blendCurveValue = m_component->m_curveLogistic.getValue(m_subjectLockedPosition_blend);
            const Vec2d subjectPosFinal = subjectPosReal +( m_subjectStance.m_lockedPosition -subjectPosReal) *blendCurveValue;
            m_subjectAABB.moveCenter(subjectPosFinal);
        }
    }

    void InGameCameraComponent::Controller::setSubject(const Actor* _pActor)
    {
        const SubjectPerform subjectPerform = m_component->getCameraControllerManager()->getSubjectPerform( m_subjectStance.m_objectRef);
        const bbool subjectStance_PrevStick = m_subjectStance.m_stick;        

        m_subjectStance.initSubjectStance();
        m_component->setSubjectStance( &m_subjectStance, m_component->m_screenTargetAverageAABB );

        const f32 previousRealDepth = m_subjectPosReal.z();

        setSubjectAABB(_pActor);
        setSubjectLockedPosition();
        m_component->projectAABB(m_subjectAABB, m_subjectPosReal.z(), m_component->m_subjectPosZ );
        setSubjectSpeed(m_subjectAABB.getCenter());

        // set stick 
        if ( f32_Abs( previousRealDepth -m_subjectPosReal.z() ) > 0.05f && !m_subjectJustTeleported )        
            m_subjectStance.m_stick = btrue;        

        // stick from bounce
        if ( subjectPerform.m_justBounceJumped )
            m_subjectStance.m_stick = btrue;

        // set just sticked
        if ( m_subjectStance.m_stick && !subjectStance_PrevStick )
            m_subjectStance.m_justStick = btrue;

        // set just unsticked
        if ( !m_subjectStance.m_stick && subjectStance_PrevStick )
            m_subjectStance.m_justUnstick = btrue; 

        m_subjectJustTeleported = bfalse;
    }

    void InGameCameraComponent::SubjectStance::initSubjectStance()
    {
        m_performUTurnLast = m_performUTurn;
        m_jumpOnAirTimerAllowedLast = m_jumpOnAirTimerAllowed;
        m_wallSlideLast = m_wallSlide;
        m_lockedPositionLast = m_lockedPosition;
        m_wasLockedPosition = m_isLockedPosition;

        m_stick = bfalse;
        m_lookDir = Vec2d::Zero;
        m_joystickMoveNormalized = Vec2d::Zero;
        m_justUnstick = bfalse;
        m_justStick = bfalse;       
        m_performUTurn = bfalse;
        m_hang = bfalse;
        m_wallSlide = bfalse;
        m_jumpOnAirTimerAllowed = bfalse;
        m_isLockedPosition = bfalse;
        m_windForce = Vec2d::Zero;        
    }

    void InGameCameraComponent::SubjectStance::resetSubjectStance()
    {
        m_performUTurn = bfalse;
        m_isLockedPosition = bfalse;
        initSubjectStance();
    }

    void InGameCameraComponent::setMainActiveController(f32 _deltaTime)
    {
        const Actor* pLeader = getMainActivePlayer();

        u32 onLeftCount = 0;
        u32 onRightCount = 0;
        f32 bestPosOnLeft = F32_INFINITY;
        f32 bestPosOnRight = -F32_INFINITY;

        Controller* bestControllerOnLeft = nullptr;
        Controller* bestControllerOnRight = nullptr;
        Controller* prevBestController = nullptr;

        for ( u32 i=0; i<m_controllers.size(); i++)
        {
            Controller& controller = m_controllers[i];            
            const f32 joyMoveX = controller.m_subjectStance.m_joystickMoveNormalized.x();            

            if ( controller.m_wasMainController )
            {
                prevBestController = &controller;
            }

            if ( pLeader == AIUtils::getActor(controller.m_subjectStance.m_objectRef) )
            {
                if ( joyMoveX != 0.f)
                    m_mainActiveControllerWithoutJoyMoveTime = 0.f;
                else
                    m_mainActiveControllerWithoutJoyMoveTime += _deltaTime;

                if ( m_mainActiveControllerWithoutJoyMoveTime < 1.f )
                {
                    m_mainController = &controller;
                    return;
                }                
            }
            else if ( joyMoveX != 0.f )
            {
                if ( joyMoveX > 0.f )
                    onRightCount++;
                else
                    onLeftCount++;

                const f32 posX = controller.m_subjectPosReal.x();

                if ( posX > bestPosOnRight )
                {
                    bestControllerOnRight = &controller;
                    bestPosOnRight = posX;
                }

                if ( posX < bestPosOnLeft )
                {
                    bestControllerOnLeft = &controller;
                    bestPosOnLeft = posX;
                }
            }
        }

        if ( onLeftCount != onRightCount )
        {
            m_mainController = onLeftCount > onRightCount ? bestControllerOnLeft : bestControllerOnRight;
        }
        else
        {            
            m_useVisibilityFromMainActiveController = bfalse;

            if ( prevBestController )
            {
                m_mainController = prevBestController;              
            }        
            else
            {
                m_mainController = &m_controllers[0];
            }
        }
    }

    bbool InGameCameraComponent::setMainController(f32 _deltaTime)
    {       
        const bbool prevUseVisibilityFromMainActivePlayer = m_useVisibilityFromMainActiveController;
        m_resetVisibilityFromMainActiveController = bfalse;
        m_useVisibilityFromMainActiveController = btrue;        

        // main controller defined by leader camera
        if ( m_mainController )
        {
            m_mainController->m_isMainController = btrue;      

            return btrue;
        }       

        // get modifier aabb
        AABB modifierDirectionAABB(-Vec2d::Infinity, Vec2d::Infinity);

        if ( m_cameraModifier )
        {
            if ( m_cameraModifier->getCMTemplate().m_isMainDRCPlayer )
            {
                setMainActiveController(_deltaTime);
                m_resetVisibilityFromMainActiveController = prevUseVisibilityFromMainActivePlayer != m_useVisibilityFromMainActiveController;
                m_mainController->m_isMainController = btrue;

                return btrue;
            }

            modifierDirectionAABB = m_cameraModifier->getModifierAABBMax();
        }

        f32 bestDotValue = -F32_INFINITY;
        const bbool isConstraintExtended = m_constraintModifier ? m_constraintModifier->isConstraintExtended() : bfalse;
        const u32 controllerListCount = m_controllers.size();

        if ( !isConstraintExtended )
        {            
            for (u32 i=0; i<controllerListCount; i++)
            {
                Controller& controller = m_controllers[i];

                if ( !controller.isActive() )
                    continue;            

                if ( controller.m_subjectAABB.checkOverlap(modifierDirectionAABB) || controller.m_wasMainController )
                {
                    f32 dotSensMap = controller.m_subjectAABB.getCenter().dot(m_CMCur.m_cameraModifierDirectionNormalized);
                    if ( dotSensMap > bestDotValue )
                    {
                        bestDotValue = dotSensMap;  
                        m_mainController = &controller;
                    }
                }
            }
        }

        // out of modifierDirectionAABB or in constraint extended
        if ( !m_mainController )
        {
            bestDotValue = -F32_INFINITY;

            for (u32 i=0; i<controllerListCount; i++)
            {
                Controller& controller = m_controllers[i];

                if ( !controller.isActive() )
                    continue; 

                const f32 dotSensMap = controller.m_subjectAABB.getCenter().dot(m_CMCur.m_cameraModifierDirectionNormalized);
                if ( dotSensMap > bestDotValue )
                {
                    bestDotValue = dotSensMap;  
                    m_mainController = &controller;
                }
            }
        }
       
        if ( !m_mainController )
        {
            ITF_ASSERT_CRASH(0, "TRY TO REPRO THIS CRASH PLEASE! Player has an invalid position. Thanks");
            return bfalse;
        }

        m_mainController->m_isMainController = btrue;      
        return btrue;
    }

    Actor* InGameCameraComponent::getMainSubject() const
    {
        if ( m_mainController )
        {
            return AIUtils::getActor(m_mainController->m_subjectStance.m_objectRef);
        }

        return NULL; 
    }


}// namespace ITF
