#include "precompiled_engine.h"
#ifdef USE_BOX2D

#ifndef _ITF_BOX2DPOLYLINECOMPONENT_H_
#include "engine/actors/components/Box2d/Box2dPolylineComponent.h"
#endif //_ITF_BOX2DPOLYLINECOMPONENT_H_

#ifndef _ITF_PHYS2D_H_
#include "engine/Phys2d/Phys2d.h"
#endif //_ITF_PHYS2D_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Box2dPolylineComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Box2dPolylineComponent_Template)
    END_SERIALIZATION()

    Box2dPolylineComponent_Template::Box2dPolylineComponent_Template()
        : Super()
    {
    }

    Box2dPolylineComponent_Template::~Box2dPolylineComponent_Template()
    {
    }

    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Box2dPolylineComponent)

    BEGIN_SERIALIZATION_CHILD(Box2dPolylineComponent)
        SERIALIZE_MEMBER("GroundAngleLimit", m_groundAngleLimit)
        
        BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
            SERIALIZE_CONTAINER_OBJECT("GroundInfo", m_groundInfo);
        END_CONDITION_BLOCK()

    END_SERIALIZATION()

     BEGIN_VALIDATE_COMPONENT(Box2dPolylineComponent)
     END_VALIDATE_COMPONENT()

    Box2dPolylineComponent::Box2dPolylineComponent()
        : Super()
        , m_groundAngleLimit(bfalse, MTH_PIBY6)
        , m_groundNormalLimit(MTH_Cos60)
        , m_groundInfo(NULL)
    {
    }


    Box2dPolylineComponent::~Box2dPolylineComponent()
    {
        for (u32 index = 0; index < m_groundInfo.size(); index++)
        {
            SF_DEL(m_groundInfo[index]);
        }
    }

    void Box2dPolylineComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_groundNormalLimit = f32_Sin(m_groundAngleLimit.Radians());
    }

    void Box2dPolylineComponent::beginContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact )
    {
        Super::beginContact(_pickableA, _pickableB, _contact);

        if(_contact->IsTouching())
        {
//            if (Vec2d::Up.dot(Phys2d::b2Vec2ToVec2d(_contact->GetManifold()->localNormal)) < m_groundNormalLimit)
            {
                addGroundInfo(_pickableA, _pickableB, _contact);
            }
        }
    }

    void Box2dPolylineComponent::endContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact )
    {
        Super::endContact(_pickableA, _pickableB, _contact);

        if(!_contact->IsTouching())
        {
//            if (Vec2d::Up.dot(Phys2d::b2Vec2ToVec2d(_contact->GetManifold()->localNormal)) < m_groundNormalLimit)
            {
                removeGroundInfo(_pickableA, _pickableB, _contact);
            }
        }
    }

    void Box2dPolylineComponent::preSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2Manifold* _oldManifold )
    {
        Super::preSolve(_pickableA, _pickableB, _contact, _oldManifold);
    }

    void Box2dPolylineComponent::postSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2ContactImpulse* _impulse )
    {
        Super::postSolve(_pickableA, _pickableB, _contact, _impulse);
    }

    Frise* Box2dPolylineComponent::getFrieze( Pickable* _pickableA, Pickable* _pickableB )
    {
        if ( _pickableA->getObjectType() == BaseObject::eFrise )
        {
            return (Frise*)(_pickableA);
        }
        else if ( _pickableB->getObjectType() == BaseObject::eFrise )
        {
            return (Frise*)(_pickableB);
        }
        
        return NULL;
    }

    void Box2dPolylineComponent::addGroundInfo( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact )
    {
        if(Frise* frieze = getFrieze(_pickableA, _pickableB))
        {
            //TODO: need to have a link between the edge in Box2d and the original freeze edge
            Vec2d start = Vec2d::Zero;
            b2WorldManifold worldManifold;
            _contact->GetWorldManifold( &worldManifold );
            u32 numPoints = _contact->GetManifold()->pointCount;
            for (u32 index = 0; index < numPoints; index++)
            {
                start += Phys2d::b2Vec2ToVec2d(worldManifold.points[index]);
            }
            start /= numPoints;
            start += Phys2d::b2Vec2ToVec2d(worldManifold.normal);

#ifdef ITF_SUPPORT_DEBUGFEATURE
            DebugDraw::circle(start, m_actor->getDepth(), 0.1f, Color::yellow(), 1.0f);
            DebugDraw::arrow2D(start, start + Phys2d::b2Vec2ToVec2d(-worldManifold.normal), m_actor->getDepth(), Color::yellow(), 1.0f);
            DebugDraw::circle(start+ Phys2d::b2Vec2ToVec2d(-worldManifold.normal), m_actor->getDepth(), 0.1f, Color::red(), 1.0f);
#endif //ITF_SUPPORT_DEBUGFEATURE

            AIUtils::SRayCastContactInfo* contactInfo = new AIUtils::SRayCastContactInfo();
        
            AIUtils::RayCast_GetClosestCollision(start, Phys2d::b2Vec2ToVec2d(worldManifold.normal)*(-1.5f), frieze->getDepth(), *contactInfo);
            if (contactInfo->isValid() && contactInfo->getCollisionNormal().y() > m_groundNormalLimit)
            {
                bbool edgeFound = bfalse;
                for (ITF_VECTOR<AIUtils::SRayCastContactInfo*>::iterator it = m_groundInfo.begin(); it != m_groundInfo.end(); it++)
                {
                    PolyLine* poly = (*it)->getCollisionPoly();
                    if(poly->getOwnerFrise() == frieze && contactInfo->getCollisionEdgeIndex() == (*it)->getCollisionEdgeIndex())
                    {
                        edgeFound = btrue;

                        if (contactInfo->getCollisionNormal().y() > (*it)->getCollisionNormal().y())
                        {
                            AIUtils::SRayCastContactInfo* oldInfo = (*it);
                            (*it) = contactInfo;
                            SF_DEL(oldInfo);
                        }
                        else
                        {
                            SF_DEL(contactInfo);
                        }

                        break;
                    }
                }

                if(!edgeFound)
                    m_groundInfo.push_back(contactInfo);
            }
        }
    }

    void Box2dPolylineComponent::removeGroundInfo( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact )
    {
        if(Frise* frieze = getFrieze(_pickableA, _pickableB))
        {
            //TODO: need to have a link between the edge in Box2d and the original freeze edge
            for (ITF_VECTOR<AIUtils::SRayCastContactInfo*>::iterator it = m_groundInfo.begin(); it != m_groundInfo.end(); it++)
            {
                PolyLine* poly = (*it)->getCollisionPoly();
                if(poly->getOwnerFrise() == frieze)
                {
                    m_groundInfo.erase(it);
                    break;
                }
            }
        }
    }

    ITF::bbool Box2dPolylineComponent::hasGround()
    {
        return (m_groundInfo.size() > 0);
    }

    void Box2dPolylineComponent::ApplyForceToCenter( Vec2d _force, const bbool _groundParalel )
    {
#ifdef ITF_SUPPORT_DEBUGFEATURE
        DebugDraw::arrow2D(m_actor->get2DPos(), m_actor->get2DPos() + _force, m_actor->getDepth(), Color::yellow(), 1.0f);
#endif //ITF_SUPPORT_DEBUGFEATURE

        if(_groundParalel && hasGround())
        {
            Vec2d normal(0.0f, 0.0f);
            for (ITF_VECTOR<AIUtils::SRayCastContactInfo*>::iterator it = m_groundInfo.begin(); it != m_groundInfo.end(); it++)
            {
                normal += (*it)->getCollisionNormal();
            }
            normal /= m_groundInfo.size();

            f32 angle = normal.getAngle() - MTH_PIBY2;
            _force.Rotate(angle);
        }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        DebugDraw::arrow2D(m_actor->get2DPos(), m_actor->get2DPos() + _force, m_actor->getDepth(), Color::green(), 1.0f);
#endif //ITF_SUPPORT_DEBUGFEATURE

        m_body->ApplyForceToCenter(Phys2d::Vec2dToB2Vec2(_force), true);
    }

}

#endif
