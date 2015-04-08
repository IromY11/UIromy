#include "precompiled_engine.h"
#ifdef USE_BOX2D

#ifndef _ITF_PHYS2DBREAKABLEBONECOMPONENT_H_
#include "engine/actors/components/Phys2dBreakableBoneComponent.h"
#endif //_ITF_PHYS2DBREAKABLEBONECOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_RENDERSIMPLEANIMCOMPONENT_H_
#include "gameplay/Components/Display/RenderSimpleAnimComponent.h"
#endif //_ITF_RENDERSIMPLEANIMCOMPONENT_H_

#ifndef _ITF_PHYS2D_H_
#include "engine/Phys2d/Phys2d.h"
#endif //_ITF_PHYS2D_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
#define SERIALIZE_BODYTYPE(name,val) \
    SERIALIZE_ENUM_BEGIN(name,val); \
    SERIALIZE_ENUM_VAR(b2_staticBody); \
    SERIALIZE_ENUM_VAR(b2_kinematicBody); \
    SERIALIZE_ENUM_VAR(b2_dynamicBody); \
    SERIALIZE_ENUM_END();


    IMPLEMENT_OBJECT_RTTI(sPieceTemplate)
    BEGIN_SERIALIZATION(sPieceTemplate)
        SERIALIZE_MEMBER("BoneName", m_boneName)
        SERIALIZE_MEMBER("PolylineName", m_polyName)
        SERIALIZE_BODYTYPE("BodyType", m_bodyType)
    END_SERIALIZATION()  

    IMPLEMENT_OBJECT_RTTI(Phys2dBreakableBoneComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Phys2dBreakableBoneComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("PiecesInfo", m_pieces) 
        SERIALIZE_BOOL("hasBackPieces", m_hasBackPieces)
    END_SERIALIZATION()

    Phys2dBreakableBoneComponent_Template::Phys2dBreakableBoneComponent_Template()
    : Super()
    , m_hasBackPieces(bfalse)
    {
    }

    Phys2dBreakableBoneComponent_Template::~Phys2dBreakableBoneComponent_Template()
    {
    }

    ITF::StringID Phys2dBreakableBoneComponent_Template::getBoneName( u32 _index ) const
    {
        ITF_ASSERT_CRASH(_index < m_pieces.size(), "Index out of bounds");

        return m_pieces[_index].m_boneName;
    }

    ITF::StringID Phys2dBreakableBoneComponent_Template::getPolyName( u32 _index ) const
    {
        ITF_ASSERT_CRASH(_index < m_pieces.size(), "Index out of bounds");

        return m_pieces[_index].m_polyName;
    }

    b2BodyType Phys2dBreakableBoneComponent_Template::getBodyType( u32 _index ) const
    {
        ITF_ASSERT_CRASH(_index < m_pieces.size(), "Index out of bounds");

        return m_pieces[_index].m_bodyType;
    }
    

    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Phys2dBreakableBoneComponent)

    BEGIN_SERIALIZATION_CHILD(Phys2dBreakableBoneComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Phys2dBreakableBoneComponent)
        VALIDATE_COMPONENT_PARAM("Component's Dependence", m_animComponent, "AngleAnimatedComponent requires an AnimLightComponent");
        VALIDATE_COMPONENT_PARAM("Component's Dependence", m_renderComponent, "RenderSimpleAnimComponent requires a Phys2dComponent");
        VALIDATE_COMPONENT_PARAM("Component's Dependence", m_phys2dComponent, "AngleAnimatedComponent requires a Phys2dComponent");
    END_VALIDATE_COMPONENT()

    Phys2dBreakableBoneComponent::Phys2dBreakableBoneComponent()
    : Super()
    , m_break(bfalse)
    , m_broke(bfalse)
    , m_initVelocity(0.0f,0.0f)
    , m_initAngularVelocity(0.0f)
    , m_resitance(1.0f)
    , m_currBehaveDelay(0.0f)
    {
    }


    Phys2dBreakableBoneComponent::~Phys2dBreakableBoneComponent()
    {
    }

    void Phys2dBreakableBoneComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_renderComponent = m_actor->GetComponent<RenderSimpleAnimComponent>();
        m_phys2dComponent = m_actor->GetComponent<Phys2dComponent>();
        if(m_phys2dComponent)
            m_phys2dComponent->registerListener(this);

        m_currBehaveDelay = getTemplate()->getBehaveDelay();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,AnimGameplayEvent_CRC, this);
    }

    void Phys2dBreakableBoneComponent::onResourceReady()
    {
        createPieces();
        m_resitance = getTemplate()->getBreakingImpulse();
    }

    void Phys2dBreakableBoneComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if ((m_break || m_broke) && m_pieces.size() == 0)
            computeBodies(LOGICDT);

        for (ITF_VECTOR<sPiece>::iterator it = m_pieces.begin(); it != m_pieces.end(); it++)
        {
            if(it->m_body)
            {
                it->m_body->SetAwake(it->m_isAwake != bfalse);
                it->m_body->SetActive(btrue);
            }
        }
    }

    void Phys2dBreakableBoneComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
        for (ITF_VECTOR<sPiece>::iterator it = m_pieces.begin(); it != m_pieces.end(); it++)
        {
            if(it->m_body)
            {
                it->m_isAwake = it->m_body->IsAwake();
                it->m_body->SetActive(bfalse);
            }
        }
    }

    void Phys2dBreakableBoneComponent::onStartDestroy( bbool _hotReload )
    {
        Super::onStartDestroy(_hotReload);

        clearBodies();
    }

    void Phys2dBreakableBoneComponent::clearBodies()
    {
        if (!m_world)
            return;

        for (ITF_VECTOR<sPiece>::iterator it = m_pieces.begin(); it != m_pieces.end(); it++)
        {
            if(it->m_body)
            {
                m_world->DestroyBody(it->m_body);
            }
        }
        m_pieces.clear();

        m_world = NULL;
    }

    void Phys2dBreakableBoneComponent::updateBodies()
    {
        for (ITF_VECTOR<sPiece>::iterator it = m_pieces.begin(); it != m_pieces.end(); it++)
        {
            if(it->m_body)
            {
                Vec2d pos;
                f32 angle;

                if(m_animComponent->getBonePos(it->m_boneID, pos) && m_animComponent->getBoneAngle(it->m_boneID, angle))
                    it->m_body->SetTransform(Phys2d::Vec2dToB2Vec2(pos), angle);
            }
        }
    }

    void Phys2dBreakableBoneComponent::computeBodies(f32 _dt)
    {
        m_phys2dComponent->setActive(bfalse);
        m_animComponent->setForceSynchro(btrue);

        
        b2Vec2 center = Phys2d::Vec2dToB2Vec2(m_actor->get2DPos());
        b2Body* body = m_phys2dComponent->getBody();
        if(body)
        {
            center = body->GetWorldCenter();
        }

        //SafeArray<AnimBoneDyn> & boneDynList = m_animComponent->getAnimMeshScene()->m_AnimInfo.getCurrentBoneList();
        u32 index = 0;
        for (ITF_VECTOR<sPiece>::iterator it = m_pieces.begin(); it != m_pieces.end(); it++, index++)
        {
            b2BodyDef bodyDef;
            bodyDef.type        = getTemplate()->getBodyType(index); //b2_dynamicBody/*m_phys2dComponent->getBodyType()*/;
            Vec2d pos;
            f32 angle;
            if(m_animComponent->getBonePos(it->m_boneID, pos) && m_animComponent->getBoneAngle(it->m_boneID, angle))
            {
                bodyDef.position = Phys2d::Vec2dToB2Vec2(pos);
                bodyDef.angle = angle;
            }

            bodyDef.active      = btrue;
            bodyDef.linearDamping = m_phys2dComponent->getLinearDamping();
            bodyDef.angularDamping = m_phys2dComponent->getAngularDamping();
            bodyDef.gravityScale = m_phys2dComponent->getGravityMultiplier();
            bodyDef.fixedRotation = m_phys2dComponent->isFixedRotation();
            bodyDef.bullet = m_phys2dComponent->isBullet();

            m_world = m_phys2dComponent->getPhys2dWorld();
            if (!m_world)
                return;

            it->m_body = m_world->CreateBody(&bodyDef);

            if (!it->m_body)
                return;

            it->m_body->SetUserData(GetActor());
            u32 count;
            Vec2d* vertex = m_animComponent->getCurrentPolylinePointBuffer(getTemplate()->getPolyName(index), &count);

            ITF_WARNING(m_actor, count <=  b2_maxPolygonVertices, "Phys2dComponent::computeBodies : Too many vertex at the polyline!");
            ITF_WARNING(m_actor, count >  2, "Phys2dComponent::computeBodies : Polyline needs at least 3 points!");
            if (count <= 2 || count >  b2_maxPolygonVertices)
            {
                clearBodies();
                return;
            }

            b2FixtureDef fixtureDef;
            if (getTemplate()->hasBackPieces())
            {
                fixtureDef.density = m_phys2dComponent->getDensity() * 0.5f;
            }
            else
            {
                fixtureDef.density = m_phys2dComponent->getDensity();
            }
            
            fixtureDef.friction = m_phys2dComponent->getFriction();
            fixtureDef.restitution = m_phys2dComponent->getRestitution();

            b2PolygonShape dynamicPolygon;
            b2Vec2 *pbList = NULL;
            pbList = newAlloc(mId_Phys, b2Vec2[count]);

            
            f32 radius = 0.0f;
            for (u32 i=0; i<count; i++)
            {
                const Vec2d posVertex = (vertex[i] - pos).Rotate(-bodyDef.angle);
                pbList[count - i - 1].Set(posVertex.x(), posVertex.y());
                f32 distance = posVertex.norm();
                if (distance > radius)
                {
                    radius = distance;
                }
            }
            it->m_radius = radius;
            ITF_MEMCOPY(dynamicPolygon.m_vertices, pbList, count * sizeof(b2Vec2));
            dynamicPolygon.m_count = count;
            fixtureDef.shape = &dynamicPolygon;
        

            fixtureDef.filter.categoryBits = m_phys2dComponent->getCategoryBits();
            fixtureDef.filter.maskBits     = m_phys2dComponent->getMaskBits();

            //it->m_body->CreateFixture(&fixtureDef);
            createFixtureList(it->m_body, &fixtureDef);

            SF_DEL_ARRAY(pbList);

            if (it->m_previousInfo.m_angle == F32_INVALID || it->m_currentInfo.m_angle == F32_INVALID )
            {
                b2Vec2 pieceCenter = it->m_body->GetWorldCenter();
                b2Vec2 velocity = m_initVelocity + b2Cross(m_initAngularVelocity, pieceCenter - center);
                it->m_body->SetAngularVelocity(m_initAngularVelocity);
                it->m_body->SetLinearVelocity(velocity);
            }
            else
            {
                b2Vec2 b2Speed = Phys2d::Vec2dToB2Vec2((it->m_currentInfo.m_pos - it->m_previousInfo.m_pos)/_dt);
                f32 angleSpeed = (it->m_currentInfo.m_angle - it->m_previousInfo.m_angle)/_dt;
                it->m_body->SetAngularVelocity(angleSpeed);
                it->m_body->SetLinearVelocity(b2Speed);
            }
        }
    }

    


    void Phys2dBreakableBoneComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

        if (m_break)
        {
            b2Body* body = m_phys2dComponent->getBody();
            if(body && body->IsActive())
            {
                m_initVelocity = body->GetLinearVelocity();
                Vec2d speed = Phys2d::b2Vec2ToVec2d(body->GetLinearVelocity());
                m_initAngularVelocity = body->GetAngularVelocity();
            }
            saveBonesPosAndAngle();
            computeBodies(_dt);

            m_broke = btrue;
            m_break = bfalse;
        }  

        if (m_broke == false)
        {
            b2Body* body = m_phys2dComponent->getBody();
            if (body)
            {
                m_initVelocity = body->GetLinearVelocity();
                m_initAngularVelocity = body->GetAngularVelocity();
                m_animComponent->setIsBoneModification(btrue);
            }
            saveBonesPosAndAngle();
        }
        else
        {
            updateBones();
            if (getTemplate()->getBehaveAfterBreak() == eDestroyAfterBreak)
            {
                m_currBehaveDelay -= _dt;
                if(m_currBehaveDelay <= 0.0f)
                    AIUtils::destroyActor(m_actor);
            }
            else if (getTemplate()->getBehaveAfterBreak() == eRebuildAfterBreak)
            {
                m_currBehaveDelay -= _dt;
                if(m_currBehaveDelay <= 0.0f)
                {
                    rebuild();
                }
            }
            
        }

    }

    void Phys2dBreakableBoneComponent::createPieces()
    {
        u32 count = getTemplate()->getPiecesCount();
        m_pieces.resize(count);
        for (u32 index = 0; index < count; index++)
        {
            m_pieces[index].m_boneID = m_animComponent->getBoneIndex(getTemplate()->getBoneName(index));
            m_pieces[index].m_body = NULL;

            // Init Pos & Angle
            m_pieces[index].m_previousInfo.m_pos = Vec2d::Zero;
            m_pieces[index].m_previousInfo.m_angle = F32_INVALID;
            m_pieces[index].m_currentInfo.m_pos = Vec2d::Zero;
            m_pieces[index].m_currentInfo.m_angle = F32_INVALID;
        }
    }

    void Phys2dBreakableBoneComponent::saveBonesPosAndAngle()
    {
        u32 count = getTemplate()->getPiecesCount();
        for (u32 index = 0; index < count; index++)
        {
            u32     boneID = m_pieces[index].m_boneID;
            Vec2d   pos;
            f32     angle;
            if(m_animComponent->getBonePos(boneID, pos) && m_animComponent->getBoneAngle(boneID, angle))
            {
                // Backup Pos & Angle
                m_pieces[index].m_previousInfo.m_pos = m_pieces[index].m_currentInfo.m_pos;
                m_pieces[index].m_previousInfo.m_angle = m_pieces[index].m_currentInfo.m_angle;

                // Set Current
                m_pieces[index].m_currentInfo.m_pos = pos;
                m_pieces[index].m_currentInfo.m_angle = angle;
            }
        }
    }

    void Phys2dBreakableBoneComponent::updateBones()
    {
        Vec2d pos;
        f32 angle;
        for (ITF_VECTOR<sPiece>::iterator it = m_pieces.begin(); it != m_pieces.end(); it++)
        {
            if(it->m_body) 
            {
                pos = Phys2d::b2Vec2ToVec2d(it->m_body->GetPosition());
                angle = it->m_body->GetAngle() - m_actor->getAngle();
                SafeArray<AnimBoneDyn> & boneDynList = m_animComponent->getAnimMeshScene()->m_AnimInfo.getCurrentBoneList();
                
                m_actor->growAABB(pos-Vec2d::One*it->m_radius);
                m_actor->growAABB(pos+Vec2d::One*it->m_radius);

                m_actor->transformWorldPosToLocal(pos);
                boneDynList[it->m_boneID].m_Pos = pos;
                boneDynList[it->m_boneID].m_XAxe = Vec2d::Right.Rotate(angle) * boneDynList[it->m_boneID].m_XAxeLenght;
            }
        }
        m_animComponent->setIsBoneModification(btrue);
    }

    void Phys2dBreakableBoneComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        if (AnimGameplayEvent* animEvent = DYNAMIC_CAST(_event, AnimGameplayEvent))
        {
            if (animEvent->getName() == MRK_BREAK)
            {
                if (m_broke)
                {
                    return;
                }

                m_break = true;
            }
            else if (animEvent->getName() == MRK_REBUILD)
            {
                if(m_pieces.size())
                {
                    clearBodies();

                    m_phys2dComponent->setActive(btrue);
                }
            }
        }
    }


#ifdef ITF_SUPPORT_EDITOR
    void Phys2dBreakableBoneComponent::drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        if (!(_flags & DrawEditFlag_Collisions))
            return;

        for (ITF_VECTOR<sPiece>::const_iterator it = m_pieces.begin(); it != m_pieces.end(); it++)
        {
            if(it->m_body)
            {
                for (b2Fixture* fixturePtr = it->m_body->GetFixtureList(); fixturePtr; fixturePtr = fixturePtr->GetNext())
                {
                    b2Shape* shape = fixturePtr->GetShape();
                    DebugDraw::shapeB2(Phys2d::b2Vec2ToVec2d(it->m_body->GetPosition()), m_actor->getDepth(), it->m_body->GetAngle(), shape, Color::yellow());
                }
            }
        }
    }
#endif

    void Phys2dBreakableBoneComponent::hitObject(Vec2d &_axis)
    {
        Super::hitObject(_axis);

        StringID hitID = getTemplate()->getDefaultAnimHit();
        if (getTemplate()->getDefaultAnimHitAngle()->size())
        {
            StringID bestStringID = StringID::Invalid;
            Angle    bestAngle = Angle::Angle360;
            Angle    curAngle = (-_axis).getAngle();

            const ITF_VECTOR<sBreakAnim>* hitAnim = getTemplate()->getDefaultAnimHitAngle();

            for (ITF_VECTOR<sBreakAnim>::const_iterator it = hitAnim->begin(); it !=hitAnim->end(); it++)
            {
                Angle newAngle = Angle( f32_Abs( Angle::GetShortestDif(curAngle, it->m_angle).Radians() ) );

                if (newAngle < bestAngle )
                {
                    bestAngle = newAngle;
                    bestStringID = it->m_animBreak;
                }
            }
            if (bestStringID.isValid())
                m_animComponent->setAnim(bestStringID, U32_INVALID, bfalse, getTemplate()->getAnimHitPriority());
        }             
        else if (hitID.isValid())
            m_animComponent->setAnim(hitID, U32_INVALID, bfalse, getTemplate()->getAnimHitPriority());
    }

	void Phys2dBreakableBoneComponent::breakObject(Vec2d &_axis)
	{
		Super::breakObject(_axis);

        StringID breakID = getTemplate()->getDefaultAnimBreak();
        if (getTemplate()->getDefaultAnimBreakAngle()->size())
        {
            StringID bestStringID = StringID::Invalid;
            Angle    bestAngle = Angle::Angle360;
            Angle    curAngle = (-_axis).getAngle();

            const ITF_VECTOR<sBreakAnim>* breakAnim = getTemplate()->getDefaultAnimBreakAngle();
            
            for (ITF_VECTOR</*Phys2dBreakableComponent_Template::*/sBreakAnim>::const_iterator it = breakAnim->begin(); it != breakAnim->end(); it++)
            {
                Angle newAngle = Angle( f32_Abs( Angle::GetShortestDif(curAngle, it->m_angle).Radians() ) );
            
                if (newAngle < bestAngle )
                {
                   bestAngle = newAngle;
                   bestStringID = it->m_animBreak;
                }
            }
            if (bestStringID.isValid())
                m_animComponent->setAnim(bestStringID, U32_INVALID, bfalse, getTemplate()->getAnimBreakPriority());
        }             
        else if (breakID.isValid())
            m_animComponent->setAnim(breakID, U32_INVALID, bfalse, getTemplate()->getAnimBreakPriority());
	}

    void Phys2dBreakableBoneComponent::rebuild()
    {
        m_broke = bfalse;
        m_break = bfalse;
// 
//          m_animComponent->setForceSynchro(bfalse);
//          m_animComponent->setAnim(getTemplate()->getDefaultAnimRebuild());

//         m_actor->forceReload();
    }

    bbool Phys2dBreakableBoneComponent::isOnSegment(const Vec2d & p, const Vec2d & p1, const Vec2d & p2)
    {
        Vec2d v1 = p1-p;
        Vec2d v2 = p2-p;

        return f32_Abs(v1.cross(v2)) < MTH_BIG_EPSILON && v1.dot(v2) <= 0.f;

    }

    void Phys2dBreakableBoneComponent::createFixtureList( b2Body* _body, b2FixtureDef* _fixtureDef )
    {
        if (!_fixtureDef || !_fixtureDef->shape)
            return;

        if (_fixtureDef->shape->GetType() != b2Shape::e_polygon)
        {
            _body->CreateFixture(_fixtureDef);
            return;
        }

        b2PolygonShape * polyShape = (b2PolygonShape *)_fixtureDef->shape;
        if (polyShape->GetVertexCount() <= 3)
        {
            _body->CreateFixture(_fixtureDef);
            return;
        }

        // 
        ITF_VECTOR< ITF_VECTOR<Vec2d> >  finalList;
        ITF_VECTOR< ITF_VECTOR<Vec2d> >  queue;

        queue.emplace_back();
        ITF_VECTOR<Vec2d> & first = queue.back();
        for (i32 i=0; i<polyShape->GetVertexCount(); i++)
            first.push_back(Phys2d::b2Vec2ToVec2d(polyShape->GetVertex(i)));

        while (queue.size())
        {
            ITF_VECTOR<Vec2d> & vec = queue.front();
            u32 vecSize = vec.size();
            bbool isConvex = btrue;

            for (u32 i=0; i<vecSize; i++)
            {
                u32 i1 = i;
                u32 i2 = (i + 1) % vecSize;
                u32 i3 = (i + 2) % vecSize;

                Vec2d & p1 = vec[i1];
                Vec2d & p2 = vec[i2];
                Vec2d & p3 = vec[i3];

                f32 det = (p3 - p2).cross(p1 - p2);
                if (det < 0.f)
                {
                    isConvex   = bfalse;
                    f32 minLen = F32_INFINITY;
                    u32 h = U32_INVALID;
                    u32 k = U32_INVALID;
                    Vec2d hitV;

                    for (u32 j=0; j<vecSize; j++)
                    {
                        if (j == i1 || j == i2)
                            continue;

                        u32 j1 = j;
                        u32 j2 = (j + 1) % vecSize;

                        Vec2d & v1 = vec[j1];
                        Vec2d & v2 = vec[j2];

                        Vec2d p1v1 = v1 - p1;
                        Vec2d p1p2 = p2 - p1;
                        Vec2d v1v2 = v2 - v1;


                        f32 div = v1v2.cross(p1p2);
                        if (f32_Abs(div) < MTH_EPSILON)
                            continue;

                        f32 a = v1v2.cross(p1v1) / div;
                        Vec2d p = p1 + p1p2 * a;
                        // p2 in p1p and p in v1v2
                        if (isOnSegment(p2, p1, p) & isOnSegment(p, v1, v2))
                        {
                            Vec2d d = p2 - p;
                            f32 len = d.sqrnorm();
                            if (len < minLen)
                            {
                                h = j1;
                                k = j2;
                                hitV = p;
                                minLen = len;
                            }
                        }
                    }

                    ITF_VECTOR<Vec2d> vec1;
                    ITF_VECTOR<Vec2d> vec2;

                    if (h == U32_INVALID || k == U32_INVALID)
                        return;

                    u32 j1 = h;
                    u32 j2 = k;
                    Vec2d v1 = vec[j1];
                    Vec2d v2 = vec[j2];

                    if (!hitV.IsEqual(v2, MTH_EPSILON))
                        vec1.push_back(hitV);
                    if (!hitV.IsEqual(v1, MTH_EPSILON))
                        vec2.push_back(hitV);

                    h = U32_INVALID;
                    k = i1;

                    for (;;)
                    {
                        if (k != j2) 
                            vec1.push_back(vec[k]);
                        else
                        {
                            if (h == U32_INVALID)
                                return;
                            if(!isOnSegment(v2, vec[h], p1))
                                vec1.push_back(vec[k]);
                            break;
                        }

                        h = k;
                        k = (k + vecSize - 1) % vecSize;
                    }

                    std::reverse(vec1.begin(), vec1.end());

                    h = U32_INVALID;
                    k = i2;
                    for (;;)
                    {
                        if(k != j1)
                            vec2.push_back(vec[k]);
                        else
                        {
                            if (h == U32_INVALID)
                                return;
                            if(!isOnSegment(v1, vec[h], p2))
                                vec2.push_back(vec[k]);
                            break;
                        }

                        h = k;
                        k = (k + 1) % vecSize;
                    }

                    queue.push_back(vec1);
                    queue.push_back(vec2);
                    queue.erase(queue.begin());

                    break;
                }
            }

            if (isConvex)
            {
                finalList.push_back(queue.front());
                queue.erase(queue.begin());
            }
        }

        for (ITF_VECTOR< ITF_VECTOR<Vec2d> >::iterator lstIter = finalList.begin();
            lstIter != finalList.end(); lstIter++)
        {
            polyShape->Set((b2Vec2*)&(*lstIter)[0], lstIter->size());
            _body->CreateFixture(_fixtureDef);
        }
    }

}

#endif
