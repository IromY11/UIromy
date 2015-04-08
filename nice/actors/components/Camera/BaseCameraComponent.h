#ifndef _ITF_BASECAMERACOMPONENT_H_
#define _ITF_BASECAMERACOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_ICAMERACONTROLLER_H_
#include "engine/display/ICameraController.h"
#endif //_ITF_ICAMERACONTROLLER_H_

namespace ITF
{
    class CameraControllerManager;

    class BaseCameraComponent : public ActorComponent
                              , public ICameraController
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(BaseCameraComponent, ActorComponent,1900041299)
        DECLARE_SERIALIZE()

    public:

        BaseCameraComponent();
        virtual ~BaseCameraComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onFinalizeLoad();
        virtual void onStartDestroy(bbool _hotReload);

        virtual void        onSceneActive();
        virtual void        onSceneInactive();

        // ICameraController
        virtual Actor*      getControllerActor() const { return m_actor; }
        // end ICameraController

        void                setAsMainCamera( bbool _value );

    protected:
        CameraControllerManager* getCameraControllerManager() const { return m_cameraControllerManager; } 

    private:
        const class BaseCameraComponent_Template* getTemplate() const;

        CameraControllerManager* m_cameraControllerManager;
        bbool               m_remote;

    };

    class BaseCameraComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(BaseCameraComponent_Template,ActorComponent_Template,1745559923)
        DECLARE_SERIALIZE()

    public:

        BaseCameraComponent_Template();
        ~BaseCameraComponent_Template();

        bbool               getStartAsMainCamera() const { return m_startAsMainCamera; }        
        f32                 getRampUpCoeff() const { return m_rampUpCoeff; }

    private:
        bbool               m_startAsMainCamera;        
        f32                 m_rampUpCoeff;
    };

    ITF_INLINE const BaseCameraComponent_Template* BaseCameraComponent::getTemplate() const
    {
        return static_cast<const BaseCameraComponent_Template*>(m_template);
    }
}

#endif // _ITF_BASECAMERACOMPONENT_H_

