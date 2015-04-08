#ifndef _ITF_FIXEDCAMERACOMPONENT_H_
#define _ITF_FIXEDCAMERACOMPONENT_H_

#ifndef _ITF_BASECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/BaseCameraComponent.h"
#endif //_ITF_BASECAMERACOMPONENT_H_

namespace ITF
{
    class FixedCameraComponent : public BaseCameraComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(FixedCameraComponent, BaseCameraComponent,1176318211);

    public:
        DECLARE_SERIALIZE()

        FixedCameraComponent()
            : Super()
            , m_offset(0, 0, 10)
        {
        }
        virtual ~FixedCameraComponent() {}

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onEvent( Event* _event );

        virtual void        getCameraData( CameraData& _data ) const;
        virtual void        onCameraEvent( Event* _event );

    protected:
        const class FixedCameraComponent_Template* getTemplate() const;

        Vec3d   m_offset;
    };


    class FixedCameraComponent_Template : public BaseCameraComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(FixedCameraComponent_Template,BaseCameraComponent_Template,3300170984)
        DECLARE_ACTORCOMPONENT_TEMPLATE(FixedCameraComponent)
        DECLARE_SERIALIZE()

    public:

        FixedCameraComponent_Template()
            : Super()
        {
        }
        virtual ~FixedCameraComponent_Template() {}

    };

    ITF_INLINE const FixedCameraComponent_Template* FixedCameraComponent::getTemplate() const
    {
        return static_cast<const FixedCameraComponent_Template*>(m_template);
    }
}

#endif // _ITF_FIXEDCAMERACOMPONENT_H_

