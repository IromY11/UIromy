#include "precompiled_engine.h"

#ifndef _ITF_BASECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/BaseCameraComponent.h"
#endif //_ITF_BASECAMERACOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(BaseCameraComponent)

    BEGIN_SERIALIZATION_ABSTRACT_CHILD(BaseCameraComponent)
    SERIALIZE_MEMBER("remote",m_remote);
    END_SERIALIZATION()


    IMPLEMENT_OBJECT_RTTI(BaseCameraComponent_Template)
    BEGIN_SERIALIZATION_ABSTRACT_CHILD(BaseCameraComponent_Template)
        SERIALIZE_MEMBER("startAsMainCam", m_startAsMainCamera);        
        SERIALIZE_MEMBER("rampUpCoeff", m_rampUpCoeff);
    END_SERIALIZATION()

    BaseCameraComponent_Template::BaseCameraComponent_Template()
        : m_startAsMainCamera(bfalse)        
        , m_rampUpCoeff(0.03f)
    {
    }
    BaseCameraComponent_Template::~BaseCameraComponent_Template()
    {
    }

    BaseCameraComponent::BaseCameraComponent()
        : Super()
        , m_cameraControllerManager(NULL)
        , m_remote(bfalse)
    {
    }

    BaseCameraComponent::~BaseCameraComponent()
    {
    }

    void BaseCameraComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();
#ifdef ITF_USE_REMOTEVIEW
        m_cameraControllerManager = m_remote?CameraControllerManager::getManagerFromId(CAMID_REMOTE):CameraControllerManager::getManagerFromId(CAMID_MAIN);
#else
        m_cameraControllerManager = m_remote?NULL:CameraControllerManager::getManagerFromId(CAMID_MAIN);
#endif // ITF_USE_REMOTEVIEW

        if (m_cameraControllerManager)
        {
            m_cameraControllerManager->registerCameraController( this );
        }
    }

    void BaseCameraComponent::onStartDestroy( bbool _hotReload )
{
        Super::onStartDestroy(_hotReload);

        if ( m_cameraControllerManager)
        {
            m_cameraControllerManager->unregisterCameraController( this );
        }
    }

    void BaseCameraComponent::onSceneActive()
    {
        Super::onSceneActive();
        if ( m_cameraControllerManager )
        {
            if (getTemplate()->getStartAsMainCamera())
            {
                m_cameraControllerManager->setMainCameraController(this, 1.f);
            }
        }
    }

    void BaseCameraComponent::onSceneInactive()
    {
        Super::onSceneInactive();
        if ( m_cameraControllerManager)
        {
            m_cameraControllerManager->removeMainCameraController(this, 1.f);
        }
    }

    void BaseCameraComponent::setAsMainCamera( bbool _value )
    {
        if (m_cameraControllerManager)
        {
            if (_value)
            {
                m_cameraControllerManager->setMainCameraController(this, getTemplate()->getRampUpCoeff());
            }
            else
            {
                m_cameraControllerManager->removeMainCameraController(this, getTemplate()->getRampUpCoeff());
            }
        }
    }

} // namespace ITF
