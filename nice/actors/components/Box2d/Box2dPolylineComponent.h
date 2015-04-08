#ifndef _ITF_BOX2DPOLYLINECOMPONENT_H_
#define _ITF_BOX2DPOLYLINECOMPONENT_H_

#define Box2dPolylineComponent_CRC ITF_GET_STRINGID_CRC(Box2dPolylineComponent,1021303259)
#define Box2dPolylineComponent_Template_CRC ITF_GET_STRINGID_CRC(Box2dPolylineComponent_Template,3859574989)

#ifndef _ITF_PHYS2DCOMPONENT_H_
#include "engine/actors/components/Phys2dComponent.h"
#endif //_ITF_PHYS2DCOMPONENT_H_


#ifdef USE_BOX2D

#include "Box2d/Box2d.h"

namespace ITF
{
    namespace AIUtils
    {
        class SRayCastContactInfo;
    }

    class Box2dPolylineComponent : public Phys2dComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Box2dPolylineComponent, Phys2dComponent, Box2dPolylineComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:
        Box2dPolylineComponent();
        virtual ~Box2dPolylineComponent();

        virtual void            onActorLoaded( Pickable::HotReloadType _hotReload );

        virtual void            beginContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact );
        virtual void            endContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact );
        virtual void            preSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2Manifold* _oldManifold );
        virtual void            postSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2ContactImpulse* _impulse );

        bbool                   hasGround();
        void                    ApplyForceToCenter( Vec2d _force, const bbool _groundParalel );
        
    private:
        friend class Phys2dContactListener;
        friend class Phys2dJoint_Plugin;
        ITF_INLINE const class Box2dPolylineComponent_Template* getTemplate() const;

        Frise*  getFrieze( Pickable* _pickableA, Pickable* _pickableB );

        void    addGroundInfo( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact );
        void    removeGroundInfo( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact );
        Angle   m_groundAngleLimit;
        f32     m_groundNormalLimit;

        ITF_VECTOR<AIUtils::SRayCastContactInfo*> m_groundInfo;


    };


    //---------------------------------------------------------------------------------------------------

    class Box2dPolylineComponent_Template : public Phys2dComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Box2dPolylineComponent_Template, Phys2dComponent_Template, Box2dPolylineComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(Box2dPolylineComponent)
        DECLARE_SERIALIZE()

    public:

        Box2dPolylineComponent_Template();
        virtual ~Box2dPolylineComponent_Template();

    private:
        
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Box2dPolylineComponent_Template* Box2dPolylineComponent::getTemplate() const
    {
        return static_cast<const Box2dPolylineComponent_Template*>(m_template);
    }
}

#endif // USE_BOX2D

#endif // _ITF_BOX2DPOLYLINECOMPONENT_H_


