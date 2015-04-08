#include "precompiled_engine.h"
#ifdef USE_BOX2D 

#ifndef _ITF_ANIMMESHVERTEXPHYS2DCOMPONENT_H_
#include "engine/actors/components/AnimMeshVertexPhys2dComponent.h"
#endif //_ITF_ANIMMESHVERTEXPHYS2DCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_ANIMMESHVERTEXCOMPONENT_H_
#include "engine/actors/components/AnimMeshVertexComponent.h"
#endif //_ITF_ANIMMESHVERTEXCOMPONENT_H_

#ifndef _ITF_PHYS2D_H_
#include "engine/Phys2d/Phys2d.h"
#endif //_ITF_PHYS2D_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AnimMeshVertexPhys2dComponent_Template)

    BEGIN_SERIALIZATION_CHILD(AnimMeshVertexPhys2dComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("shape", m_shape, GAMEINTERFACE->getShapesFactory())
        SERIALIZE_MEMBER("density", m_density)
        SERIALIZE_MEMBER("friction", m_friction)
        SERIALIZE_MEMBER("restitution", m_restitution)
        SERIALIZE_MEMBER("linearDamping", m_linearDamping)
        SERIALIZE_MEMBER("angularDamping", m_angularDamping)
        SERIALIZE_MEMBER("gravityMupliplier", m_gravityMultiplier)
        SERIALIZE_BOOL("fixedRotation", m_fixedRotation)
    END_SERIALIZATION()

#define Default_density             1.f
#define Default_friction            0.3f
#define Default_restitution         0.f
#define Default_linearDamping       0.f
#define Default_angularDamping      0.f
#define Default_gravityMupliplier   1.f
#define Default_fixedRotation       bfalse


    AnimMeshVertexPhys2dComponent_Template::AnimMeshVertexPhys2dComponent_Template()
        : Super()
        , m_shape(NULL)
        , m_density(Default_density)
        , m_friction(Default_friction)
        , m_restitution(Default_linearDamping)
        , m_linearDamping(Default_angularDamping)
        , m_angularDamping(Default_angularDamping)
        , m_gravityMultiplier(Default_gravityMupliplier)
        , m_fixedRotation(Default_fixedRotation)
    {
    }

    AnimMeshVertexPhys2dComponent_Template::~AnimMeshVertexPhys2dComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(AnimMeshVertexPhys2dComponent)

    BEGIN_SERIALIZATION_CHILD(AnimMeshVertexPhys2dComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(AnimMeshVertexPhys2dComponent)
        VALIDATE_COMPONENT_PARAM("animMeshVertex", m_amvComponent, "AnimMeshVertexPhys2dComponent works with AnimMeshVertexComponent");
    END_VALIDATE_COMPONENT()

    AnimMeshVertexPhys2dComponent::AnimMeshVertexPhys2dComponent()
        : Super()
        , m_amvComponent(NULL)
        , m_world(NULL)
    {
    }

    AnimMeshVertexPhys2dComponent::~AnimMeshVertexPhys2dComponent()
    {
    }


    void AnimMeshVertexPhys2dComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_amvComponent = GetActor()->GetComponent<AnimMeshVertexComponent>();
    }

    void AnimMeshVertexPhys2dComponent::onResourceReady()
    {
        Super::onResourceReady();

        computeBodies();
    }

    void AnimMeshVertexPhys2dComponent::onUnloadResources()
    {
        Super::onUnloadResources();

        clearBodies();
    }

    void AnimMeshVertexPhys2dComponent::activateBodies(bool _activate)
    {
        for (ITF_VECTOR<b2Body *>::const_iterator bodyIter = m_bodyList.begin();
            bodyIter != m_bodyList.end(); bodyIter++)
        {
            (*bodyIter)->SetActive(_activate);
        }
    }


    void AnimMeshVertexPhys2dComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if (m_bodyList.size() == 0)
            computeBodies();

        updateBodies();
        activateBodies(true);
    }

    void AnimMeshVertexPhys2dComponent::onBecomeInactive()
    {
        activateBodies(false);

        Super::onBecomeInactive();
    }

    void AnimMeshVertexPhys2dComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);
        
        updateAMV();
    }

    void AnimMeshVertexPhys2dComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);
    }

    void AnimMeshVertexPhys2dComponent::clearBodies()
    {
        if (!m_world)
            return;

        for (ITF_VECTOR<b2Body*>::iterator bIter = m_bodyList.begin();
            bIter != m_bodyList.end(); ++bIter)
        {
            m_world->DestroyBody(*bIter);
        }

        m_bodyList.clear();
    }


#define MIN_PHYS_VALUE 0.01f

    void AnimMeshVertexPhys2dComponent::computeBodies()
    {
        clearBodies();
        
        const PhysShape * shape = getTemplate()->getShape();
        if (!shape)
            return;

        m_world = PHYS2D->getWorld(GetActor()->getScene(), GetActor()->getDepth());
        if (!m_world)
            return;

        b2BodyDef bodyDef;
        bodyDef.type        = b2_dynamicBody;
        bodyDef.active      = false;

        bodyDef.linearDamping   = getTemplate()->getLinearDamping();
        bodyDef.angularDamping  = getTemplate()->getAngularDamping();
        bodyDef.gravityScale    = getTemplate()->getGravityMultiplier();
        bodyDef.fixedRotation   = getTemplate()->isFixedRotation() != bfalse;

        b2FixtureDef fixtureDef;
        fixtureDef.density      = getTemplate()->getDensity();
        fixtureDef.friction     = getTemplate()->getFriction();
        fixtureDef.restitution  = getTemplate()->getRestitution();


        b2CircleShape   dynamicCircle;
        b2PolygonShape  dynamicPolygon;

        b2Vec2                  *pbList = NULL;
        bbool                   inverseOrder = bfalse;
        u32                     nbPoints = 0;
        const ITF_VECTOR<Vec2d> *pointListPtr = NULL;

        Vec2d scale = GetActor()->getScale();
        if (GetActor()->getIsFlipped())
            scale.x() *= -1.f;

        u32 physType = U32_INVALID;
        if (const PhysShapeCircle  *physCircle = DYNAMIC_CAST(shape, PhysShapeCircle))
        {
            fixtureDef.shape = &dynamicCircle;
            physType         = 0;
        } else if (const PhysShapePolygon  *physPolygon = DYNAMIC_CAST(shape, PhysShapePolygon))
        {
            pointListPtr = &physPolygon->getVertex();
            nbPoints = (*pointListPtr).size();
            if (nbPoints < 2)
            {
                if (const PhysShapeBox *physBox = DYNAMIC_CAST(shape, PhysShapeBox))
                {
                    fixtureDef.shape = &dynamicPolygon;
                    physType         = 1;
                } else
                    return;
            } else
            {
                if (nbPoints >  b2_maxPolygonVertices)
                    return;

                pbList            = newAlloc(mId_Phys, b2Vec2[(*pointListPtr).size()]);
                inverseOrder     = nbPoints > 2 ? ((*pointListPtr)[2] - (*pointListPtr)[0]).cross((*pointListPtr)[1] - (*pointListPtr)[0]) > 0.f != GetActor()->getIsFlipped() : bfalse;
                fixtureDef.shape = &dynamicPolygon;
                physType         = 2;
            }
            
        }


        Transform3d actorTransform = m_amvComponent->getActorTransform();
        Transform3d amvTransform;

        const SingleAnimDataRuntimeList & amvList = m_amvComponent->getAMVList();
        for (SingleAnimDataRuntimeList::const_iterator amvIter = amvList.begin();
            amvIter != amvList.end(); ++amvIter)
        {
            const SingleAnimDataRuntime & amv = *amvIter;
            amvTransform = actorTransform *  amv.getTransform3d();

            bodyDef.position = Phys2d::Vec2dToB2Vec2(amvTransform.get2DPos());
            bodyDef.angle    = amvTransform.getAngle();

            b2Body * body = m_world->CreateBody(&bodyDef);

            if (!body)
            {
                clearBodies();
                return;
            }

             m_bodyList.push_back(body);

            switch (physType)
            {
            case 0:
                dynamicCircle.m_radius = ((PhysShapeCircle  *)shape)->getRadius() * f32_Abs(scale.x() * amvTransform.getScale().x());
                break;
            case 1:
                dynamicPolygon.SetAsBox(f32_Max(MIN_PHYS_VALUE, f32_Abs(((PhysShapeBox *)shape)->getExtent().x() * scale.x() * amvTransform.getScale().x())),
                                        f32_Max(MIN_PHYS_VALUE, f32_Abs(((PhysShapeBox *)shape)->getExtent().y() * scale.y() * amvTransform.getScale().y())));
                break;
            case 2:
                for (u32 i=0; i<nbPoints; i++)
                {
                    Vec2d pos = (*pointListPtr)[i];
                    pos *= scale * amvTransform.getScale();

                    if (inverseOrder)
                        pbList[nbPoints - i - 1].Set(pos.x(), pos.y());
                    else
                        pbList[i].Set(pos.x(), pos.y());
                }
                dynamicPolygon.Set(pbList, (*pointListPtr).size());
                break;
            }
            if (!body->CreateFixture(&fixtureDef))
            {
                clearBodies();
                return;
            }
        }
        SF_DEL_ARRAY(pbList);
    }


    void AnimMeshVertexPhys2dComponent::updateAMV()
    {
        SingleAnimDataRuntimeList & amvList = m_amvComponent->getAMVList();
        if (amvList.size() != m_bodyList.size())
            return;

        Transform3d actorTransform = m_amvComponent->getActorTransform();

        ITF_VECTOR<b2Body *>::const_iterator bodyIter = m_bodyList.begin();
        for (SingleAnimDataRuntimeList::iterator amvIter = amvList.begin();
            amvIter != amvList.end(); amvIter++, bodyIter++)
        {
            SingleAnimDataRuntime & amv = *amvIter;
            
            Vec2d   scale = amv.getTransform3d().getScale();
            f32     z     = amv.getTransform3d().getZ();
            Vec2d   pos   = actorTransform.inverseTransformPos(Phys2d::b2Vec2ToVec2d((*bodyIter)->GetPosition()).to3d()).truncateTo2D();
            f32     angle = actorTransform.inverseTransformAngle((*bodyIter)->GetAngle());

            amv.setTransform3dFrom(pos.to3d(z), angle, scale, bfalse);
        }
    }

    void AnimMeshVertexPhys2dComponent::updateBodies()
    {
        const SingleAnimDataRuntimeList & amvList = m_amvComponent->getAMVList();
        if (amvList.size() != m_bodyList.size())
            return;

        Transform3d actorTransform = m_amvComponent->getActorTransform();
        Transform3d amvTransform;

        ITF_VECTOR<b2Body *>::const_iterator bodyIter = m_bodyList.begin();
        for (SingleAnimDataRuntimeList::const_iterator amvIter = amvList.begin();
            amvIter != amvList.end(); amvIter++, bodyIter++)
        {
            const SingleAnimDataRuntime & amv = *amvIter;
            amvTransform = actorTransform *  amv.getTransform3d();

            (*bodyIter)->SetTransform(Phys2d::Vec2dToB2Vec2(amvTransform.get2DPos()), amvTransform.getAngle());
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void AnimMeshVertexPhys2dComponent::drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        if (!(_flags & DrawEditFlag_Collisions))
            return;

        for (ITF_VECTOR<b2Body *>::const_iterator bodyIter = m_bodyList.begin();
            bodyIter != m_bodyList.end(); bodyIter++)
        {
            for (b2Fixture* fixturePtr = (*bodyIter)->GetFixtureList(); fixturePtr; fixturePtr = fixturePtr->GetNext())
            {
                b2Shape* shape = fixturePtr->GetShape();
                DebugDraw::shapeB2(Phys2d::b2Vec2ToVec2d((*bodyIter)->GetPosition()), m_actor->getDepth(), (*bodyIter)->GetAngle(), shape, Color::yellow());
            }
        }
    }


    void AnimMeshVertexPhys2dComponent::onEditorMove(bbool _modifyInitialPos)
    {
         Super::onEditorMove(_modifyInitialPos);

         computeBodies();
         activateBodies(true);
    }
#endif

}


#endif //USE_BOX2D