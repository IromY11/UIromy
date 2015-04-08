#ifndef _ITF_CAMERA3DCOMPONENT_H_
#define _ITF_CAMERA3DCOMPONENT_H_

#define Camera3dComponent_CRC ITF_GET_STRINGID_CRC(Camera3dComponent,2418634720)
#define Camera3dComponent_Template_CRC ITF_GET_STRINGID_CRC(Camera3dComponent_Template,1690647398)

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_ICAMERACONTROLLER_H_
#include "engine/display/ICameraController.h"
#endif //_ITF_ICAMERACONTROLLER_H_

namespace ITF
{
    class CameraControllerManager;
    class Mesh3DComponent;

    class Camera3dComponent : public ActorComponent
                            , public ICameraController
    {
        DECLARE_OBJECT_CHILD_RTTI(Camera3dComponent, ActorComponent, Camera3dComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Camera3dComponent()
            : Super()
            , m_viewMode(ViewMode_Both)
            , m_fadeInSmooth(0.05f)            
            , m_fadeOutSmooth(0.05f)            
            , m_deltaFogZ(13.f)

            , m_mainCameraManager(NULL)
            , m_remoteCameraManager(NULL)
            , m_meshComponent(NULL)
            , m_boneIndex(U32_INVALID)

            , m_isActive(bfalse)
            , m_camPos(Vec3d::Zero)
            , m_prevCamPos(Vec3d::Zero)
            , m_camLookAtDir(-Vec3d::ZAxis)
            , m_camUpDir(Vec3d::YAxis)
        {
        }
        virtual ~Camera3dComponent() {}

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onFinalizeLoad();
        virtual void onStartDestroy(bbool _hotReload);
        virtual void        onResourceReady();
        virtual void        onEvent( Event* _event);
        virtual void        Update( f32 _dt );

        // ICameraController
        virtual void        getCameraData( CameraData& _data ) const;
        virtual Actor*      getControllerActor() const { return m_actor; }
        // end ICameraController

#ifdef ITF_SUPPORT_EDITOR
        virtual void        onPostPropertyChange();
#endif // ITF_SUPPORT_EDITOR


    private:
        ITF_INLINE const class Camera3dComponent_Template* getTemplate() const;

        enum ViewMode
        {
        	ViewMode_Main   = 0x001,
        	ViewMode_Remote = 0x002,
        	ViewMode_Both = ViewMode_Main|ViewMode_Remote,
            ENUM_FORCE_SIZE_32(ViewMode)
        };
        
        #define SERIALIZE_ViewMode(name, val) \
        SERIALIZE_ENUM_BEGIN(name, val); \
            SERIALIZE_ENUM_VAR(ViewMode_Main); \
            SERIALIZE_ENUM_VAR(ViewMode_Remote); \
            SERIALIZE_ENUM_VAR(ViewMode_Both); \
        SERIALIZE_ENUM_END();


        // instance-serialized
        ViewMode m_viewMode;
        f32 m_fadeInSmooth;        
        f32 m_fadeOutSmooth;        
        f32 m_deltaFogZ;

        // init
        CameraControllerManager* m_mainCameraManager;
        CameraControllerManager* m_remoteCameraManager;
        Mesh3DComponent* m_meshComponent;
        u32 m_boneIndex;

        // runtime
        bbool m_isActive;
        Vec3d m_camPos;
        Vec3d m_camLookAtDir;
        Vec3d m_camUpDir;

        Vec3d m_prevCamPos;

        void setAsMainCamera( const bbool _value );
   };


    //---------------------------------------------------------------------------------------------------

    class Camera3dComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Camera3dComponent_Template, ActorComponent_Template, Camera3dComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(Camera3dComponent)
        DECLARE_SERIALIZE()

    public:

        Camera3dComponent_Template()
            : Super()
        {
        }
        virtual ~Camera3dComponent_Template() {}

        StringID m_camBone;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Camera3dComponent_Template* Camera3dComponent::getTemplate() const
    {
        return static_cast<const Camera3dComponent_Template*>(m_template);
    }
}

#endif // _ITF_CAMERA3DCOMPONENT_H_
