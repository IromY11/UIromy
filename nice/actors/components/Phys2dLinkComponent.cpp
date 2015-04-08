#include "precompiled_engine.h"
#ifdef USE_BOX2D

#ifndef _ITF_PHYS2DLINKCOMPONENT_H_
#include "engine/actors/components/Phys2dLinkComponent.h"
#endif //_ITF_PHYS2DLINKCOMPONENT_H_

#ifndef _ITF_PHYS2DCOMPONENT_H_
#include "engine/actors/components/Phys2dComponent.h"
#endif //_ITF_PHYS2DCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHYS2D_H_
#include "engine/Phys2d/Phys2d.h"
#endif //_ITF_PHYS2D_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

namespace ITF
{

    void Phys2dLinkInfo::deleteLink(b2World * _world)
    {
        m_linkedActor = NULL;
        m_useJoint    = btrue;
        if (!m_joint)
            return;

        _world->DestroyBody(m_ground);
        m_ground = NULL;

        // no need to call destroy on joint -> destroyed with m_gound
        m_joint  = NULL;
    }

    bbool Phys2dLinkInfo::computeLocalData(Pickable *_parent)
    {
        if (!m_linkedActor)
            return bfalse;

        m_localAngle = m_linkedActor->getAngle() - _parent->getAngle();
        m_localPos   = m_linkedActor->get2DPos() - _parent->get2DPos();
        m_localPos   = m_localPos.Rotate(-_parent->getAngle());
        m_localPos.x() /= _parent->getScale().x();
        m_localPos.y() /= _parent->getScale().y();
        return btrue;
    }

    bbool Phys2dLinkInfo::getGlobalPosAndAngle(Pickable *_parent, Vec2d & _pos, f32 & _angle)
    {
        if (!m_linkedActor)
            return bfalse;

        _pos      = m_localPos;
        _pos.x() *= _parent->getScale().x();
        _pos.y() *= _parent->getScale().y();
        _pos      = _pos.Rotate(_parent->getAngle());
        _pos     += _parent->get2DPos();

        _angle    = _parent->getAngle() + m_localAngle;
        return btrue;
    }




    IMPLEMENT_OBJECT_RTTI(Phys2dLinkComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Phys2dLinkComponent_Template)
        SERIALIZE_MEMBER("dampingRatio", m_dampingRatio);
        SERIALIZE_MEMBER("frequencyHz", m_frequencyHz);
        SERIALIZE_MEMBER("maxForce", m_maxForce);
    END_SERIALIZATION()

    Phys2dLinkComponent_Template::Phys2dLinkComponent_Template()
        : Super()
        , m_dampingRatio(0.2f)
        , m_frequencyHz(60.f)
        , m_maxForce(1000.0f)
    {
    }

    Phys2dLinkComponent_Template::~Phys2dLinkComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Phys2dLinkComponent)

    BEGIN_SERIALIZATION_CHILD(Phys2dLinkComponent)
        SERIALIZE_BOOL("SnapChildren", m_snapChildren)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Phys2dLinkComponent)
    END_VALIDATE_COMPONENT()

    Phys2dLinkComponent::Phys2dLinkComponent()
        : Super()
        , m_world2d(NULL)
        , m_linkComponent(NULL)
        , m_needUpdateJoints(bfalse)
        , m_snapChildren(bfalse)
    {
    }

    Phys2dLinkComponent::~Phys2dLinkComponent()
    {

    }


    void Phys2dLinkComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();
    }

    void Phys2dLinkComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

#ifdef ITF_SUPPORT_EDITOR
        if (EVENTMANAGER)
            EVENTMANAGER->registerEvent(EditorEventLink_CRC, this);
#endif //ITF_SUPPORT_EDITOR
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,PhysEventJointDestroyed_CRC,this);
    }

    void Phys2dLinkComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        createJoints();
    }

    void Phys2dLinkComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        deleteJoints();
    }

    void Phys2dLinkComponent::onStartDestroy( bbool _hotReload )
{
        deleteJoints();

#ifdef ITF_SUPPORT_EDITOR
        if (EVENTMANAGER)
            EVENTMANAGER->unregisterEvent(EditorEventLink_CRC, this);
#endif //ITF_SUPPORT_EDITOR

    }

    void Phys2dLinkComponent::deleteJoints()
    {
        for (ITF_VECTOR<Phys2dLinkInfo>::iterator linkIt = m_linkList.begin();
            linkIt != m_linkList.end(); ++linkIt)
        {
            linkIt->deleteLink(m_world2d);
        }
        m_linkList.clear();
    }

    void Phys2dLinkComponent::createJoints()
    {
        deleteJoints();

        m_needUpdateJoints = bfalse;
        if (!m_linkComponent)
            return;

        const LinkComponent::ChildrenList & children = m_linkComponent->getChildren();
        if (children.size() == 0)
            return;

        for (LinkComponent::ChildrenList::const_iterator childrenIter = children.begin();
            childrenIter != children.end(); ++childrenIter)
        {
            Actor * actor = SAFE_DYNAMIC_CAST(m_linkComponent->getChildObject(childrenIter->getPath()), Actor);
            if (actor)
            {

				if (!actor->isActive() 
#ifndef ITF_FINAL
					&& !actor->hasDataError()
#endif //ITF_FINAL
					)
				{
					m_needUpdateJoints = btrue;
					break;
				}
				
				Phys2dComponent * physComp = actor->GetComponent<Phys2dComponent>();
                if (physComp)
                {
                    b2Body * linkedBody = physComp->getBody();
                    if (linkedBody)
                    {
                        Phys2dLinkInfo linkInfo;
                        linkInfo.m_linkedActor = actor;
                        linkInfo.m_useJoint    = linkedBody->GetType() == b2_dynamicBody;
                        
                        m_world2d = linkedBody->GetWorld();

                        if (linkInfo.m_useJoint)
                        {
                            b2BodyDef bodyDef;
                            bodyDef.userData = GetActor();
                            linkInfo.m_ground  = m_world2d->CreateBody(&bodyDef);

                            b2MouseJointDef jointDef;
                            jointDef.bodyA              = linkInfo.m_ground;
                            jointDef.bodyB              = linkedBody;
                            jointDef.collideConnected   = false;
                            jointDef.dampingRatio       = getTemplate()->getDampingRatio();
                            jointDef.frequencyHz        = getTemplate()->getFrequencyHz();
                            jointDef.maxForce           = getTemplate()->getMaxForce() * linkedBody->GetMass();
                            jointDef.target             = b2Vec2(actor->getPos().x(), actor->getPos().y());

                            linkInfo.m_joint = (b2MouseJoint *)m_world2d->CreateJoint(&jointDef);
                        } else
                        {
                            ITF_WARNING(m_actor, linkedBody->GetType() == b2_kinematicBody, "Trying to move static body !");
                        }
                        if (!m_snapChildren)
                            linkInfo.computeLocalData(m_actor);
                        m_linkList.push_back(linkInfo);
                    }
                }
            }
        }
    }

    void Phys2dLinkComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

        if (m_needUpdateJoints)
            createJoints();

        for (ITF_VECTOR<Phys2dLinkInfo>::iterator linkIt = m_linkList.begin();
            linkIt != m_linkList.end(); ++linkIt)
        {
            Phys2dLinkInfo & linkInfo = *linkIt;
            Vec2d   dstPos;
            f32     dstAngle;
            if (!linkInfo.getGlobalPosAndAngle(m_actor, dstPos, dstAngle))
                continue;

            if (!linkInfo.m_useJoint)
            {
                Phys2dComponent * physComp   = linkInfo.m_linkedActor->GetComponent<Phys2dComponent>();
                b2Body          * linkedBody = physComp ? physComp->getBody() : NULL;
                if (linkedBody)
                {
                    Vec2d deltaPos = dstPos - Phys2d::b2Vec2ToVec2d(linkedBody->GetPosition());
                    Vec2d speed    = deltaPos * f32_Inv(_dt);

                    linkedBody->SetLinearVelocity(Phys2d::Vec2dToB2Vec2(speed));
                }
            } else if (linkInfo.m_joint)
            {
                linkInfo.m_joint->SetTarget(Phys2d::Vec2dToB2Vec2(dstPos));
            }
        }
    }

    void Phys2dLinkComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);

#ifdef ITF_SUPPORT_EDITOR
        if (EditorEventLink * event = DYNAMIC_CAST(_event, EditorEventLink))
        {
            if (event->getSender() == GetActor()->getRef())
            {
                createJoints();
            }
        }
#endif

        if (PhysEventJointDestroyed * event = DYNAMIC_CAST(_event, PhysEventJointDestroyed))
        {
            setJointDeleted(event->getJoint());
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Phys2dLinkComponent::onEditorMove(bbool _modifyInitialPos /* = btrue */)
    {
        Super::onEditorMove(_modifyInitialPos);

        createJoints();
    }

    void Phys2dLinkComponent::onPostPropertyChange()
    {
        createJoints();
    }

#endif

    void Phys2dLinkComponent::setJointDeleted(b2Joint * _joint)
    {
        if (m_needUpdateJoints)
            return;

        for (ITF_VECTOR<Phys2dLinkInfo>::iterator linkIt = m_linkList.begin();
            linkIt != m_linkList.end(); ++linkIt)
        {
            if (linkIt->m_joint == _joint)
            {
                m_needUpdateJoints = btrue;
                break;
            }
        }
    }
}


#endif // USE_BOX2D