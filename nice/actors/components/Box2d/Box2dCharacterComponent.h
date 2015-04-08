#ifndef _ITF_BOX2DCHARACTERCOMPONENT_H_
#define _ITF_BOX2DCHARACTERCOMPONENT_H_

#define Box2dCharacterComponent_CRC ITF_GET_STRINGID_CRC(Box2dCharacterComponent,3797331934)
#define Box2dCharacterComponent_Template_CRC ITF_GET_STRINGID_CRC(Box2dCharacterComponent_Template,98231260)

#ifndef _ITF_PHYS2DCOMPONENT_H_
#include "engine/actors/components/Phys2dComponent.h"
#endif //_ITF_PHYS2DCOMPONENT_H_


#ifdef USE_BOX2D

#include "Box2d/Box2d.h"

namespace ITF
{
    class Box2dCharacterComponent : public Phys2dComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Box2dCharacterComponent, Phys2dComponent, Box2dCharacterComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:
        Box2dCharacterComponent();
        virtual ~Box2dCharacterComponent();

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        Update( f32 _dt );
        
        virtual void        beginContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact );
        virtual void        endContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact );
        virtual void        preSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2Manifold* _oldManifold );
        virtual void        postSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2ContactImpulse* _impulse );

        ITF_INLINE  bbool   hasGround(){return m_ground;}
        void                ApplyForceToCenter( Vec2d _force, const bbool _groundParalel );

    protected:
        virtual void        computeBodies();

    private:
        friend class Phys2dContactListener;
        friend class Phys2dJoint_Plugin;
        ITF_INLINE const class Box2dCharacterComponent_Template* getTemplate() const;
        
        void updateGround();

        //Shape
        f32             m_height;
        f32             m_radius;
        b2CircleShape   m_roofShape;
        b2CircleShape   m_groundShape;
        b2PolygonShape  m_bodyShape;


        //Ground
        b2Fixture*      m_groundFixture;
        bbool           m_ground;
        Vec2d           m_groundNormal;
        f32             m_groundNormalLimit;
    };


    //---------------------------------------------------------------------------------------------------

    class Box2dCharacterComponent_Template : public Phys2dComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Box2dCharacterComponent_Template, Phys2dComponent_Template, Box2dCharacterComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(Box2dCharacterComponent)
        DECLARE_SERIALIZE()

    public:

        Box2dCharacterComponent_Template();
        virtual ~Box2dCharacterComponent_Template();

    private:

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Box2dCharacterComponent_Template* Box2dCharacterComponent::getTemplate() const
    {
        return static_cast<const Box2dCharacterComponent_Template*>(m_template);
    }
}

#endif // USE_BOX2D

#endif // _ITF_BOX2DPOLYLINECOMPONENT_H_


