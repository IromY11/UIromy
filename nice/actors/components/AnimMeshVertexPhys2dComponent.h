#ifndef _ITF_ANIMMESHVERTEXPHYS2DCOMPONENT_H_
#define _ITF_ANIMMESHVERTEXPHYS2DCOMPONENT_H_

#define AnimMeshVertexPhys2dComponent_CRC ITF_GET_STRINGID_CRC(AnimMeshVertexPhys2dComponent,1266560452)
#define AnimMeshVertexPhys2dComponent_Template_CRC ITF_GET_STRINGID_CRC(AnimMeshVertexPhys2dComponent_Template,3045460468)

#ifdef USE_BOX2D 

#include "Box2d/Box2d.h"

namespace ITF
{
    class AnimMeshVertexPhys2dComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimMeshVertexPhys2dComponent, ActorComponent, AnimMeshVertexPhys2dComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        AnimMeshVertexPhys2dComponent();
        virtual ~AnimMeshVertexPhys2dComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onResourceReady();
        virtual void        onUnloadResources();
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue );
#endif

        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event);

                void        clearBodies();
                void        computeBodies();
                void        updateAMV();
                void        updateBodies();
                void        activateBodies(bool _activate);

    private:
        ITF_INLINE const class AnimMeshVertexPhys2dComponent_Template* getTemplate() const;

        class AnimMeshVertexComponent   * m_amvComponent;
        b2World                         *m_world;
        ITF_VECTOR<b2Body *>            m_bodyList;
    };


    //---------------------------------------------------------------------------------------------------

    class AnimMeshVertexPhys2dComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimMeshVertexPhys2dComponent_Template, ActorComponent_Template, AnimMeshVertexPhys2dComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(AnimMeshVertexPhys2dComponent)
        DECLARE_SERIALIZE()

    public:

        AnimMeshVertexPhys2dComponent_Template();
        virtual ~AnimMeshVertexPhys2dComponent_Template();

        const PhysShape *   getShape() const { return m_shape; }
        f32                 getDensity() const { return m_density; }
        f32                 getFriction() const { return m_friction; }
        f32                 getRestitution() const { return m_restitution; }

        f32                 getLinearDamping() const { return m_linearDamping; }
        f32                 getAngularDamping() const { return m_angularDamping; }
        bbool               isFixedRotation() const { return m_fixedRotation; }
        f32                 getGravityMultiplier() const { return m_gravityMultiplier; }

    private:
        PhysShape * m_shape;

        f32                 m_density;
        f32                 m_friction;
        f32                 m_restitution;
        f32                 m_linearDamping;
        f32                 m_angularDamping;
        f32                 m_gravityMultiplier;
        bbool               m_fixedRotation;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const AnimMeshVertexPhys2dComponent_Template* AnimMeshVertexPhys2dComponent::getTemplate() const
    {
        return static_cast<const AnimMeshVertexPhys2dComponent_Template*>(m_template);
    }
}

#endif //USE_BOX2D 

#endif // _ITF_ANIMMESHVERTEXPHYS2DCOMPONENT_H_
