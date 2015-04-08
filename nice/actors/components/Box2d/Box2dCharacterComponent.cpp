#include "precompiled_engine.h"
#ifdef USE_BOX2D

#ifndef _ITF_BOX2DCHARACTERCOMPONENT_H_
#include "engine/actors/components/Box2d/Box2dCharacterComponent.h"
#endif //_ITF_BOX2DCHARACTERCOMPONENT_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_PHYS2D_H_
#include "engine/Phys2d/Phys2d.h"
#endif //_ITF_PHYS2D_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Box2dCharacterComponent_Template)

        BEGIN_SERIALIZATION_CHILD(Box2dCharacterComponent_Template)
        END_SERIALIZATION()

        Box2dCharacterComponent_Template::Box2dCharacterComponent_Template()
        : Super()
    {
    }

    Box2dCharacterComponent_Template::~Box2dCharacterComponent_Template()
    {
    }

    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Box2dCharacterComponent)

        BEGIN_SERIALIZATION_CHILD(Box2dCharacterComponent)
        SERIALIZE_MEMBER("CharacterHeight", m_height)
        SERIALIZE_MEMBER("CharacterRadius", m_radius)
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Box2dCharacterComponent)
        END_VALIDATE_COMPONENT()

        Box2dCharacterComponent::Box2dCharacterComponent()
        : Super()
        , m_height(1.8f)
        , m_radius(0.5f)
        , m_groundFixture(NULL)
        , m_ground(bfalse)
        , m_groundNormal(Vec2d::Zero)
        , m_groundNormalLimit(0.5f)
    {
    }


    Box2dCharacterComponent::~Box2dCharacterComponent()
    {
    }

    void Box2dCharacterComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_groundShape.m_radius = m_radius;
        m_groundShape.m_p.Set(0.0f, m_radius);
        m_roofShape.m_radius = m_radius;
        m_roofShape.m_p.Set(0.0f, m_height - m_radius);
        m_bodyShape.SetAsBox(m_radius, (m_height - (m_radius*2.0f)) * 0.5f, b2Vec2(0.0f, m_height*0.5f), 0.0f);

        if (m_body)
        {
            computeBodies();
        }
        
    }

    void Box2dCharacterComponent::computeBodies()
    {
        m_fixedRotation = btrue;

        Super::computeBodies();

        if (m_body)
        {
            //Clean all the fixtures
            for (b2Fixture* fixturePtr = m_body->GetFixtureList(); fixturePtr; fixturePtr = fixturePtr->GetNext())
            {
                m_body->DestroyFixture(fixturePtr);
            }

            b2FixtureDef fixtureDef;

            fixtureDef.density = m_density;
            fixtureDef.friction = m_friction;
            fixtureDef.restitution = m_restitution;
            fixtureDef.filter.categoryBits = m_categoryBits;
            fixtureDef.filter.maskBits     = m_maskBits;

            fixtureDef.shape = &m_groundShape;
            m_groundFixture = m_body->CreateFixture(&fixtureDef);
            fixtureDef.shape = &m_bodyShape;
            createFixtureList(&fixtureDef);
            fixtureDef.shape = &m_roofShape;
            m_body->CreateFixture(&fixtureDef);
        }
    }

    void Box2dCharacterComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        AnimatedComponent* anim = m_actor->GetComponent<AnimatedComponent>();
        if (anim)
        {
            anim->setInput(ITF_GET_STRINGID_CRC(INAIR, 1518932154), !hasGround());
            anim->setInput(ITF_GET_STRINGID_CRC(SPEEDXONPOLYWITHSTICKX, 543295311), f32_Abs(m_body->GetLinearVelocity().x));

            if (hasGround())
            {
                anim->setAnim(ITF_GET_STRINGID_CRC(IDLE, 1633200266), U32_INVALID);
            }
            else
            {
                anim->setAnim(ITF_GET_STRINGID_CRC(JUMPFALL, 3848453645), U32_INVALID);
            }
        }
        if(f32_Abs(m_body->GetLinearVelocity().x) > MTH_EPSILON)
            m_actor->setIsFlipped(m_body->GetLinearVelocity().x < 0.0f);

        updateGround();
    }

    void Box2dCharacterComponent::updateGround()
    {
        m_ground = bfalse;
        m_groundNormal = Vec2d::Zero;

        for (b2ContactEdge* contactEdge = m_body->GetContactList(); contactEdge; contactEdge = contactEdge->next)
        {
            b2Contact* contact = contactEdge->contact;
            if(contact->IsTouching())
            {
                bbool contactValid = bfalse;
                if (m_groundFixture == contact->GetFixtureA())
                {
                    contactValid = btrue;
                }
                else if (m_groundFixture == contact->GetFixtureB())
                {
                    contactValid = btrue;
                }

                if(contactValid)
                {
                    b2WorldManifold worldManifold;
                    contact->GetWorldManifold(&worldManifold);
                    if(worldManifold.normal.y > m_groundNormalLimit)
                    {
                        m_ground = btrue;
                        m_groundNormal = Phys2d::b2Vec2ToVec2d(worldManifold.normal);

                        //TODO: check if colide with 2 edges
                    }
                }
            }
        }
        
    }

    void Box2dCharacterComponent::beginContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact )
    {
        Super::beginContact(_pickableA, _pickableB, _contact);

        if(_contact->IsTouching())
        {
        }
    }

    void Box2dCharacterComponent::endContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact )
    {
        Super::endContact(_pickableA, _pickableB, _contact);
    }

    void Box2dCharacterComponent::preSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2Manifold* _oldManifold )
    {
        Super::preSolve(_pickableA, _pickableB, _contact, _oldManifold);
    }

    void Box2dCharacterComponent::postSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2ContactImpulse* _impulse )
    {
        Super::postSolve(_pickableA, _pickableB, _contact, _impulse);
    }

    void Box2dCharacterComponent::ApplyForceToCenter( Vec2d _force, const bbool _groundParalel )
    {
#ifdef ITF_SUPPORT_DEBUGFEATURE
        DebugDraw::arrow2D(m_actor->get2DPos(), m_actor->get2DPos() + _force, m_actor->getDepth(), Color::yellow(), 1.0f);
#endif //ITF_SUPPORT_DEBUGFEATURE

        if(_groundParalel && hasGround())
        {
            f32 angle = m_groundNormal.getAngle() - MTH_PIBY2;
            _force = _force.Rotate(angle);
        }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        DebugDraw::arrow2D(m_actor->get2DPos(), m_actor->get2DPos() + _force, m_actor->getDepth(), Color::green(), 1.0f);
#endif //ITF_SUPPORT_DEBUGFEATURE

        m_body->ApplyForceToCenter(Phys2d::Vec2dToB2Vec2(_force), true);

    }

}

#endif
