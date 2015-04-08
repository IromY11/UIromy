#include "precompiled_engine.h"

#ifndef _ITF_CAMERA3DCOMPONENT_H_
#include "engine/actors/components/Camera/Camera3dComponent.h"
#endif //_ITF_CAMERA3DCOMPONENT_H_

#ifndef _ITF_MESH3DCOMPONENT_H_
#include "engine/actors/components/mesh3Dcomponent.h"
#endif //_ITF_MESH3DCOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_
namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Camera3dComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Camera3dComponent_Template)
        SERIALIZE_MEMBER("camBone", m_camBone)
    END_SERIALIZATION()

    //---------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Camera3dComponent)

    BEGIN_SERIALIZATION_CHILD(Camera3dComponent)

        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate)
            SERIALIZE_MEMBER("fadeInSmoothA", m_fadeInSmooth)        
            SERIALIZE_MEMBER("fadeOutSmoothA", m_fadeOutSmooth) 
        END_CONDITION_BLOCK()

        SERIALIZE_ViewMode("viewMode", m_viewMode)
        SERIALIZE_MEMBER("fadeInSmooth", m_fadeInSmooth)        
        SERIALIZE_MEMBER("fadeOutSmooth", m_fadeOutSmooth)        
        SERIALIZE_MEMBER("deltaFogZ", m_deltaFogZ)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Camera3dComponent)
        VALIDATE_COMPONENT_PARAM("", m_meshComponent, "Camera3dComponent requires a Mesh3DComponent");
        VALIDATE_COMPONENT_PARAM("", m_boneIndex != U32_INVALID, "bone '%s' not found", getTemplate()->m_camBone.getDebugString());
    END_VALIDATE_COMPONENT()

    void Camera3dComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_meshComponent = m_actor->GetComponent<Mesh3DComponent>();

        m_actor->registerEvent(EventTrigger_CRC, this);
    }

    void Camera3dComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        if (m_viewMode & ViewMode_Main)
        {
            m_mainCameraManager = CameraControllerManager::getManagerFromId(CAMID_MAIN);

            if (m_mainCameraManager)
            {
                m_mainCameraManager->registerCameraController(this);
            }
        }

#ifdef ITF_USE_REMOTEVIEW
        if (m_viewMode & ViewMode_Remote)
        {
            m_remoteCameraManager = CameraControllerManager::getManagerFromId(CAMID_REMOTE);

            if (m_remoteCameraManager)
            {
                m_remoteCameraManager->registerCameraController(this);
            }
        }
#endif // ITF_USE_REMOTEVIEW
    }

#ifdef ITF_SUPPORT_EDITOR
    void Camera3dComponent::onPostPropertyChange()
    {
        m_actor->setHotReloadAfterModif();
    }
#endif // ITF_SUPPORT_EDITOR

    void Camera3dComponent::onStartDestroy( bbool _hotReload )
    {
        Super::onStartDestroy(_hotReload);

        if (m_isActive)
        {
            setAsMainCamera(bfalse);
        }

        if (m_mainCameraManager)
        {
            m_mainCameraManager->unregisterCameraController(this);
        }

        if (m_remoteCameraManager)
        {
            m_remoteCameraManager->unregisterCameraController(this);
        }
    }

    void Camera3dComponent::onResourceReady()
    {
        Super::onResourceReady();

        if ( m_meshComponent )
        {
            if ( Skeleton3D* skl3d = m_meshComponent->getSkeleton3D() )
            {
                m_boneIndex = (u32)skl3d->getBoneIndexByName(getTemplate()->m_camBone);
            }
        }
    }

    void Camera3dComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if ( EventTrigger* trigger = DYNAMIC_CAST(_event, EventTrigger) )
        {
            // toggle
            setAsMainCamera(!m_isActive);
        }
    }

    void Camera3dComponent::setAsMainCamera( const bbool _value )
    {
        ITF_ASSERT(m_isActive != _value);
        m_isActive = _value;

        if (m_isActive)
        {
            if (m_mainCameraManager)
            {
                m_mainCameraManager->setMainCameraController(this, m_fadeInSmooth);
            }

            if (m_remoteCameraManager)
            {
                m_remoteCameraManager->setMainCameraController(this, m_fadeInSmooth);
            }
            m_prevCamPos = m_camPos;
        }
        else
        {
            if (m_mainCameraManager)
            {
                m_mainCameraManager->removeMainCameraController(this, m_fadeOutSmooth);
            }

            if (m_remoteCameraManager)
            {
                m_remoteCameraManager->removeMainCameraController(this, m_fadeOutSmooth);
            }
        }
    }

    void Camera3dComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        Matrix44 matrix;
        if ( m_meshComponent->getBoneGlobalMatrix(m_boneIndex, matrix) )
        {
            m_camPos = matrix.T();
            m_camLookAtDir = matrix.I();
            m_camUpDir = matrix.J();
        }

        f32 normPos = (m_camPos-m_prevCamPos).sqrnorm();
        if ( normPos > 100.f)
        {
            CameraControllerManager::preApplyCameraPos( CAMID_MAIN|CAMID_REMOTE, m_camPos );
        }
        m_prevCamPos = m_camPos;
    }

    void Camera3dComponent::getCameraData( CameraData& _data ) const
    {
        _data.m_pos = m_camPos;
        _data.m_lookAtDir = m_camLookAtDir;
        _data.m_upDir = m_camUpDir;
        _data.m_deltaFogZ = m_deltaFogZ;
    }

}
