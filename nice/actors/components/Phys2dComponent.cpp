#include "precompiled_engine.h"
#ifdef USE_BOX2D 

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

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifdef ITF_SUPPORT_EDITOR
#ifndef _ITF_TOUCHSCREENADAPTER_H_
#include "engine/AdaptersInterfaces/TouchScreenAdapter.h"
#endif  //_ITF_TOUCHSCREENADAPTER_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_
#endif //ITF_SUPPORT_EDITOR

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_NGON2D_H_
#include "Core/math/Ngon2d.h"
#endif // _ITF_NGON2D_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_GAMEMATERIALSMANAGER_H_
#include "gameplay/managers/GameMaterialManager.h"
#endif //_ITF_GAMEMATERIALSMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Phys2dComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Phys2dComponent_Template)
        SERIALIZE_CONTAINER("AnimPolylineNames", m_polylines)
        if (m_polylines.size() == 0)
        {
            SERIALIZE_OBJECT_WITH_FACTORY("shape", m_shape, GAMEINTERFACE->getShapesFactory())
        }
        SERIALIZE_MEMBER("density", m_density)
        SERIALIZE_MEMBER("friction", m_friction)
        SERIALIZE_MEMBER("restitution", m_restitution)
        SERIALIZE_MEMBER("linearDamping", m_linearDamping)
        SERIALIZE_MEMBER("angularDamping", m_angularDamping)
        SERIALIZE_MEMBER("gravityMupliplier", m_gravityMultiplier)
		SERIALIZE_BOOL("fixedRotation", m_fixedRotation)
		SERIALIZE_BOOL("bullet", m_isBullet)
		SERIALIZE_BOOL("sensor", m_isSensor)
        SERIALIZE_BOOL("applyScaleToJoints", m_applyScaleToJoints)
        SERIALIZE_MEMBER("gameMaterial", m_gameMaterial)
    END_SERIALIZATION()


#define Default_density             1.f
#define Default_friction            0.3f
#define Default_restitution         0.f
#define Default_linearDamping       0.f
#define Default_angularDamping      0.f
#define Default_gravityMupliplier   1.f
#define Default_fixedRotation       bfalse
#define Default_applyScaleToJoints  bfalse


#define JOINT_CRC StringID("joint")//ITF_GET_STRINGID_CRC(joint,640962688)

#define MIN_PHYS_VALUE 0.01f

    Phys2dComponent_Template::Phys2dComponent_Template()
        : Super()
        , m_shape(NULL)
        , m_density(Default_density)
        , m_friction(Default_friction)
        , m_restitution(Default_linearDamping)
        , m_linearDamping(Default_angularDamping)
        , m_angularDamping(Default_angularDamping)
        , m_gravityMultiplier(Default_gravityMupliplier)
        , m_fixedRotation(Default_fixedRotation)
		, m_isBullet(bfalse)
		, m_isSensor(bfalse)
        , m_applyScaleToJoints(Default_applyScaleToJoints)
        , m_gameMaterialTemplate(NULL)
    {
    }

    Phys2dComponent_Template::~Phys2dComponent_Template()
    {
        SF_DEL(m_shape);
    }

    bbool Phys2dComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        if ( !m_gameMaterial.isEmpty() )
        {
            m_gameMaterialTemplate = GAMEMATERIAL_MANAGER->requestGameMaterial(m_gameMaterial);
            if ( !m_gameMaterialTemplate )
                return bfalse;
        }
        return btrue;
    }


    //-----------------------------------------
    BEGIN_SERIALIZATION(JointInfo)
        SERIALIZE_BOOL("startActive", m_isActive);
        SERIALIZE_MEMBER("childPath", m_childPath);
        SERIALIZE_MEMBER("localPosA", m_localPosA);
        SERIALIZE_MEMBER("localPosB", m_localPosB);
        SERIALIZE_BOOL("invertAB", m_invertAB);
        SERIALIZE_OBJECT_WITH_FACTORY("joint", m_jointDetail, PHYS2D->getJointDetailsFactory());
    END_SERIALIZATION()

    JointInfo::JointInfo()
        : m_jointDetail(NULL)
        , m_isActive(btrue)
        , m_localPosA(Vec2d::Zero)
        , m_localPosB(Vec2d::Zero)
        , m_invertAB(bfalse)
        , m_manualyDestroyed(bfalse)
    {
        setObjectType(BaseObject::eEditorDummy);
    }

    JointInfo::JointInfo(const ObjectPath & _objPath)
        : m_jointDetail(NULL)
        , m_isActive(btrue)
        , m_childPath(_objPath)
        , m_localPosA(Vec2d::Zero)
        , m_localPosB(Vec2d::Zero)
        , m_invertAB(bfalse)
		, m_manualyDestroyed(bfalse)
    {
        setObjectType(BaseObject::eEditorDummy);
    }


    JointInfo::~JointInfo()
    {
    }

    Pickable * JointInfo::getChild(Pickable * _parentPick) const
    {
        Pickable * child = NULL;
        if ( m_childPath.getIsAbsolute() )
        {
            child = SceneObjectPathUtils::getObjectFromAbsolutePath(m_childPath);
        }
        else
        {
            child = SceneObjectPathUtils::getObjectFromRelativePath(_parentPick, m_childPath);
        }
        return child;
    }


    JointInstance::JointInstance()
        : m_joint(NULL)
        , m_parent(NULL)
        , m_child(NULL)
        , m_parentLocalPos(0.f, 0.f)
        , m_childLocalPos(0.f, 0.f)
    {

    }

    void JointInstance::deleteJoint()
    {
        ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

        if (!m_joint || !m_parent)
            return;

        m_parent->GetWorld()->DestroyJoint(m_joint);
        m_joint      = NULL;
        m_parent    = NULL;
    }

    bbool JointInstance::createJoint(JointInfo * _jointInfo, Pickable * _parentPick, b2Body * _parentBody, bbool _applyScale)
    {
        deleteJoint();

        if (!_jointInfo->m_isActive || !_jointInfo->m_jointDetail || _jointInfo->m_manualyDestroyed)
            return bfalse;

        Vec2d parentScale = _applyScale ? _parentPick->getScale() : Vec2d::One;
        if (_parentPick->getIsFlipped())
            parentScale.x() *= -1.f;

        Pickable * childPick = _jointInfo->getChild(_parentPick);
        m_parent = _parentBody;

        m_parentLocalPos = Phys2d::Vec2dToB2Vec2(_jointInfo->m_localPosA * parentScale);
        if (!m_parent || !childPick)
            return bfalse;

        Vec2d childScale = _applyScale ? childPick->getScale() : Vec2d::One;
        if (childPick->getIsFlipped())
            childScale.x() *= -1.f;

        m_child = NULL;
        if (Actor *actor = DYNAMIC_CAST(childPick, Actor))
        {
            Phys2dComponent * physComp = actor->GetComponent<Phys2dComponent>();
            if (!physComp)
                return bfalse;

            m_child = physComp->getBody();
        } else if (Frise * frise = DYNAMIC_CAST(childPick, Frise))
        {
            m_child = frise->getBody();
        }

        if (!m_child || m_child->GetWorld() != _parentBody->GetWorld())
            return bfalse;

        m_childLocalPos = Phys2d::Vec2dToB2Vec2(_jointInfo->m_localPosB * childScale);
        if (_jointInfo->m_invertAB)
            m_joint = _jointInfo->m_jointDetail->createInternalJoint(m_child, m_childLocalPos, m_parent, m_parentLocalPos);
        else
            m_joint = _jointInfo->m_jointDetail->createInternalJoint(m_parent, m_parentLocalPos, m_child, m_childLocalPos);

        if (m_joint)
            m_joint->SetUserData(_jointInfo->m_invertAB?(void *)1:(void *)0);

        return m_joint != NULL;
    }


    bbool JointInstance::updateJoint(JointInfo * _jointInfo, bbool _applyScale)
    {
        if (!m_joint || !m_parent || !m_child)
            return bfalse;

        Pickable * parentPick = (Pickable *)m_parent->GetUserData();
        Pickable * childPick = (Pickable *)m_child->GetUserData();
        b2Body   * parentBody = m_parent;

        if (!parentPick || !childPick)
            return bfalse;

        return createJoint(_jointInfo, parentPick, parentBody, _applyScale);
    }




    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Phys2dComponent)

    BEGIN_SERIALIZATION_CHILD(Phys2dComponent)

        BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit_SaveReadOnly)
            f32 mass = m_body ? m_body->GetMass() : 0.f;
            SERIALIZE_MEMBER("mass", mass)
        END_CONDITION_BLOCK()

        SERIALIZE_ENUM_BEGIN("bodyType", m_bodyType)
            SERIALIZE_ENUM_VAR(b2_staticBody);
            SERIALIZE_ENUM_VAR(b2_kinematicBody);
            SERIALIZE_ENUM_VAR(b2_dynamicBody);
        SERIALIZE_ENUM_END();
        
        SERIALIZE_CONTAINER("AnimPolylineNames", m_polylines)
        SERIALIZE_MEMBER("density", m_density)
        SERIALIZE_MEMBER("friction", m_friction)
        SERIALIZE_MEMBER("restitution", m_restitution)
        SERIALIZE_MEMBER("linearDamping", m_linearDamping)
        SERIALIZE_MEMBER("angularDamping", m_angularDamping)
        SERIALIZE_MEMBER("gravityMupliplier", m_gravityMultiplier)
        SERIALIZE_BOOL("fixedRotation", m_fixedRotation)
        
        SERIALIZE_MEMBER("categoryBits", m_categoryBits)
        SERIALIZE_MEMBER("maskBits", m_maskBits)

        SERIALIZE_BOOL("enablePhysic", m_enablePhysicInit)
        BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit)
            SERIALIZE_BOOL("enablePhysicCurr", m_enablePhysic)
        END_CONDITION_BLOCK()
        SERIALIZE_BOOL("startAwake", m_startAwake)
        SERIALIZE_MEMBER("bullet", m_isBullet)
		SERIALIZE_BOOL("sensor", m_isSensor)
        SERIALIZE_OBJECT("shape", m_editableShape)
        SERIALIZE_BOOL("shapeCentered", m_editableShapeCentered)

        SERIALIZE_BOOL("applyScaleToJoints", m_applyScaleToJoints)
        SERIALIZE_CONTAINER_OBJECT("jointList", m_jointInfoList)
        
        SERIALIZE_BOOL("showShape", m_shapeShow);
		SERIALIZE_MEMBER("shapeColor", m_shapeColor)
    END_SERIALIZATION()

     BEGIN_VALIDATE_COMPONENT(Phys2dComponent)
        VALIDATE_COMPONENT_PARAM("", m_error.isEmpty(), m_error.cStr());
     END_VALIDATE_COMPONENT()

    Phys2dComponent::Phys2dComponent()
        : Super()
        , m_linkComponent(NULL)
        , m_world(NULL)
        , m_forcedWorld(NULL)
        , m_body(NULL)
        , m_bodyType(b2_dynamicBody)
        , m_density(Default_density)
        , m_friction(Default_friction)
        , m_restitution(Default_restitution)
        , m_linearDamping(Default_angularDamping)
        , m_angularDamping(Default_angularDamping)
        , m_gravityMultiplier(Default_gravityMupliplier)
        , m_fixedRotation(Default_fixedRotation)
        , m_categoryBits(0x1)
        , m_maskBits(0xFFFF)
		, m_linkRotationToActor(btrue)
        , m_enablePhysicInit(btrue)
        , m_enablePhysic(btrue)
        , m_startAwake(btrue)
        , m_isAwake(bfalse)
        , m_needUpdateJoints(bfalse)
        , m_isBullet(U32_INVALID)
		, m_isSensor(bfalse)
        , m_shapeShow(bfalse)
        , m_shapeColor(Color::white())
        , m_editableShapeCentered(bfalse)
        , m_applyScaleToJoints(Default_applyScaleToJoints)
		, m_physScale(Vec2d(1.0f,1.0f))

#ifdef ITF_SUPPORT_EDITOR
        , m_linkEdition(bfalse)
#endif
    {
    }


    Phys2dComponent::~Phys2dComponent()
    {
#ifdef ITF_SUPPORT_EDITOR
        clearMeshFromBodyFixtures();
#endif //ITF_SUPPORT_EDITOR
    }

    void Phys2dComponent::onStartDestroy( bbool _hotReload )
    {
        Super::onStartDestroy(_hotReload);

#ifdef ITF_SUPPORT_EDITOR
        if (EVENTMANAGER)
            EVENTMANAGER->unregisterEvent(EditorEventLink_CRC, this);
#endif //ITF_SUPPORT_EDITOR


        for (ITF_VECTOR<JointInfo>::iterator jointIt = m_jointInfoList.begin();
            jointIt != m_jointInfoList.end(); jointIt++)
        {
            if ((*jointIt).m_jointDetail)
            {
                // no need to delete joints -> will be deleted by bidy destruction
                // (*jointIt).m_jointDetail->deleteJoint();
                SF_DEL((*jointIt).m_jointDetail);
            }
        }
        m_jointInfoList.clear();

        clearBodies();
    }


    b2World * Phys2dComponent::getPhys2dWorld()
    {
        if (m_forcedWorld)
            return m_forcedWorld;

        return PHYS2D->getWorld(GetActor()->getScene(), GetActor()->getDepth());
    }

    void Phys2dComponent::setPhys2dWorld(b2World * _world)
    {
        m_forcedWorld = _world;

        if (m_world)
        {
            computeBodies();
            updateBodies();
            updateJointList();
        }
    }


    void Phys2dComponent::clearSourceJoints()
    {
        if (m_body)
        {
            b2World * world = m_body->GetWorld();
            ITF_VECTOR<b2Joint *> jointToDelete;
            for (b2JointEdge * jointEdge = m_body->GetJointList();
                jointEdge != NULL; jointEdge = jointEdge->next)
            {
                bbool jointInverted = jointEdge->joint->GetUserData()?btrue:bfalse;
                if ((!jointInverted && jointEdge->joint->GetBodyA() == m_body) ||
                    ( jointInverted && jointEdge->joint->GetBodyB() == m_body))
                {
                    jointToDelete.push_back(jointEdge->joint);
                }
            }

            for (ITF_VECTOR<b2Joint *>::iterator jointIt = jointToDelete.begin();
                jointIt != jointToDelete.end(); ++jointIt)
            {
                world->DestroyJoint(*jointIt);
            }
        }

        m_jointInstanceList.clear();

#ifdef ITF_SUPPORT_EDITOR
       clearCurrentDrag();
#endif //ITF_SUPPORT_EDITOR
    }

    void Phys2dComponent::clearBodies()
    {
        if (!m_world)
            return;

        if (m_body)
        {
            m_world->DestroyBody(m_body);
            m_body = NULL;
        }

        m_fixtureList.clear();
        m_world = NULL;
    }

    void Phys2dComponent::updateBodies()
    {
        if (m_body)
        {
            m_body->SetTransform(b2Vec2(m_actor->get2DPos().x(), m_actor->get2DPos().y()), m_actor->getAngle());
        }
    }


    void Phys2dComponent::fillFixtureDef(b2FixtureDef & _fixtureDef)
    {
        if (m_density == Default_density)
            m_density = getTemplate()->getDensity();
        _fixtureDef.density             = m_density;

        if (m_friction == Default_friction)
            m_friction = getTemplate()->getFriction();
        _fixtureDef.friction            = m_friction;

        if (m_restitution == Default_restitution)
            m_restitution = getTemplate()->getRestitution();
        _fixtureDef.restitution         = m_restitution;
        _fixtureDef.filter.categoryBits = m_categoryBits;
        _fixtureDef.filter.maskBits     = m_maskBits;

		if( ! m_isSensor )
			m_isSensor = getTemplate()->isSensor();
		_fixtureDef.isSensor = m_isSensor != bfalse;
    }

    bbool Phys2dComponent::isFixtureListValid()
    {
        if (m_fixtureList.size() == 0)
            return bfalse;

        for (ITF_VECTOR<b2Fixture *>::iterator fixtIter = m_fixtureList.begin();
            fixtIter != m_fixtureList.end(); ++fixtIter)
        {
            if (!(*fixtIter))
                return bfalse;
        }
        return btrue;
    }


    bbool isOnSegment(const Vec2d & p, const Vec2d & p1, const Vec2d & p2)
    {
        Vec2d v1 = p1-p;
        Vec2d v2 = p2-p;

        return f32_Abs(v1.cross(v2)) < MTH_BIG_EPSILON && v1.dot(v2) <= 0.f;

    }

    void Phys2dComponent::createFixtureList(b2FixtureDef * _fixtureDef)
    {
        if (!_fixtureDef || !_fixtureDef->shape)
            return;

        if (_fixtureDef->shape->GetType() != b2Shape::e_polygon)
        {
            m_fixtureList.push_back(m_body->CreateFixture(_fixtureDef));
            return;
        }

        b2PolygonShape * polyShape = (b2PolygonShape *)_fixtureDef->shape;
        if (polyShape->GetVertexCount() <= 3)
        {
            m_fixtureList.push_back(m_body->CreateFixture(_fixtureDef));
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
            m_fixtureList.push_back(m_body->CreateFixture(_fixtureDef));
        }

    }

    bbool Phys2dComponent::fixtureListTestPoint(const b2Vec2 & _b2worldPos)
    {
        for (ITF_VECTOR<b2Fixture *>::iterator fixtIter = m_fixtureList.begin();
            fixtIter != m_fixtureList.end(); ++fixtIter)
        {
            if ((*fixtIter)->TestPoint(_b2worldPos))
                return btrue;
        }
        return bfalse;
    }



    void Phys2dComponent::computeBodies()
    {
        bbool   restoreStatus   = bfalse;
        bool    wasAwake        = false;
        bool    wasActive       = false;
        if (m_body)
        {
            restoreStatus = btrue;
            wasAwake  = m_body->IsAwake();
            wasActive = m_body->IsActive();
        }

        clearBodies();
        computeBodiesIntern();
        
        if (!isFixtureListValid())
        {
            clearBodies();
            return;
        }

        if (restoreStatus)
        {
            m_body->SetAwake(wasAwake);
            m_body->SetActive(wasActive);
        }

#ifdef ITF_SUPPORT_EDITOR
        createMeshFromBodyFixtures();
#endif
    }

    void Phys2dComponent::computeBodiesIntern()
    {
#if !defined(ITF_DISABLE_WARNING)
        m_error.clear();
#endif

        b2BodyDef bodyDef;
        bodyDef.type        = m_bodyType;
        bodyDef.position.Set(m_actor->get2DPos().x(), m_actor->get2DPos().y());
        bodyDef.angle       = m_actor->getAngle();
        bodyDef.active      = bfalse;
        //bodyDef.awake       = bfalse;
        m_isAwake           = m_startAwake;

        if (m_linearDamping == Default_linearDamping)
            m_linearDamping = getTemplate()->getLinearDamping();
        bodyDef.linearDamping = m_linearDamping;

        if (m_angularDamping == Default_angularDamping)
            m_angularDamping = getTemplate()->getAngularDamping();
        bodyDef.angularDamping = m_angularDamping;

        if (m_gravityMultiplier == Default_gravityMupliplier)
            m_gravityMultiplier = getTemplate()->getGravityMultiplier();
        bodyDef.gravityScale = m_gravityMultiplier;
        
        if (m_isBullet == U32_INVALID)
            m_isBullet = getTemplate()->isBullet();
        bodyDef.bullet = m_isBullet != bfalse;

        if (m_applyScaleToJoints == Default_applyScaleToJoints)
            m_applyScaleToJoints = getTemplate()->applyScaleToJoints();

        bodyDef.fixedRotation = m_fixedRotation || getTemplate()->isFixedRotation();
        
        m_world = getPhys2dWorld();
        if (!m_world)
            return;

        m_body = m_world->CreateBody(&bodyDef);

        if (!m_body)
            return;

        m_body->SetUserData(GetActor());
        if(m_polylines.size())
        {
            for (ITF_VECTOR<StringID>::const_iterator polylineNameIt = m_polylines.begin(); polylineNameIt != m_polylines.end(); polylineNameIt++)
            {
                if(polylineNameIt->isValid())
                {
                    b2FixtureDef fixtureDef;
                    fillFixtureDef(fixtureDef);

                    b2PolygonShape dynamicPolygon;
                    b2Vec2 *pbList = NULL;

                    AnimLightComponent* animLight = m_actor->GetComponent<AnimLightComponent>();
                    if (animLight)
                    {
                        u32 count;
                        Vec2d* vertex = animLight->getCurrentPolylinePointBuffer(*polylineNameIt, &count);

                        if (!vertex || count <= 2)
                        {
#if !defined(ITF_DISABLE_WARNING)
                            m_error = "Phys2dComponent::computeBodies : Polyline needs at least 3 points!";
#endif
                            return;
                        }

                        if (count > b2_maxPolygonVertices)
                        {
#if !defined(ITF_DISABLE_WARNING)
                            m_error = "Phys2dComponent::computeBodies : Too many vertex at the polyline!";
#endif
                            return;
                        }

                        pbList = newAlloc(mId_Phys, b2Vec2[count]);
                        for (u32 i=0; i<count; i++)
                        {
                            Vec2d pos = (vertex[i] - m_actor->get2DPos()).Rotate(-m_actor->getAngle());
                            if (m_actor->getIsFlipped())
                            {
                                pos.x() = -pos.x();
                            }

                            pbList[count - i - 1].Set(pos.x(), pos.y());
                        }

                        ITF_MEMCOPY(dynamicPolygon.m_vertices, pbList, count * sizeof(b2Vec2));
                        dynamicPolygon.m_count = count;
                        fixtureDef.shape = &dynamicPolygon;
                    }

                    createFixtureList(&fixtureDef);
                    SF_DEL_ARRAY(pbList);
                }
            }
        }
        else
        {
            const PhysShape * shape = m_editableShape.getShape() ? m_editableShape.getShape() : getTemplate()->getShape();
            Vec2d offset = getEditableShapeGlobalOffset(bfalse);

            if (!shape)
                return;

            b2FixtureDef fixtureDef;
            fillFixtureDef(fixtureDef);

            b2CircleShape  dynamicCircle;
            b2PolygonShape dynamicPolygon;
            b2Vec2 *pbList = NULL;

            Vec2d scale = GetActor()->getScale() * m_physScale;
            if (GetActor()->getIsFlipped())
                scale.x() *= -1.f;
			Vec2d absScale = Vec2d(f32_Abs(scale.x()), f32_Abs(scale.y()));

            if (const PhysShapeCircle  *physCircle = DYNAMIC_CAST(shape, PhysShapeCircle))
            {
                dynamicCircle.m_radius = physCircle->getRadius() * absScale.x();
                if (dynamicCircle.m_radius < MIN_PHYS_VALUE)
                    dynamicCircle.m_radius = MIN_PHYS_VALUE;
                dynamicCircle.m_p.Set(offset.x(), offset.y());
                fixtureDef.shape = &dynamicCircle;
            } else if (const PhysShapePolygon  *physPolygon = DYNAMIC_CAST(shape, PhysShapePolygon))
            {
                const ITF_VECTOR<Vec2d> & pointList = physPolygon->getVertex();
                u32 nbPoints = pointList.size();

                if (nbPoints < 2)
                {
                    if (const PhysShapeBox *physBox = DYNAMIC_CAST(shape, PhysShapeBox))
                    {
                        dynamicPolygon.SetAsBox(f32_Max(MIN_PHYS_VALUE, physBox->getExtent().x() * absScale.x()),
                                                f32_Max(MIN_PHYS_VALUE, physBox->getExtent().y() * absScale.y()));
                        dynamicPolygon.m_centroid = dynamicPolygon.m_centroid + b2Vec2(offset.x(), offset.y());
                    } else
                        return;
                } else
                {
                    if (nbPoints >  b2_maxPolygonVertices)
                        return;

                    pbList = newAlloc(mId_Phys, b2Vec2[pointList.size()]);
                    bbool inverseOrder = nbPoints > 2 ? (pointList[2] - pointList[0]).cross(pointList[1] - pointList[0]) > 0.f != GetActor()->getIsFlipped() : bfalse;
                    for (u32 i=0; i<nbPoints; i++)
                    {
                        Vec2d pos = pointList[i];
                        pos *= scale;

                        if (inverseOrder)
                            pbList[nbPoints - i - 1].Set(pos.x() + offset.x(), pos.y() + offset.y());
                        else
                            pbList[i].Set(pos.x() + offset.x(), pos.y() + offset.y());
                    }
                    ITF_MEMCOPY(dynamicPolygon.m_vertices, pbList, pointList.size() * sizeof(b2Vec2));
                    dynamicPolygon.m_count = pointList.size();
                }
                fixtureDef.shape = &dynamicPolygon;
            } else
            {
                return;
            }
            createFixtureList(&fixtureDef);
            SF_DEL_ARRAY(pbList);
        }

    }

    void Phys2dComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
        
        m_editableShape.initialize(m_actor);
        m_linkComponent = m_actor->GetComponent<LinkComponent>();

#ifdef ITF_SUPPORT_EDITOR    
        if (m_linkComponent)
        {
            m_linkComponent->addTagType(JOINT_CRC, Color::zero());
        }
#endif
#if !defined(ITF_DISABLE_WARNING)
        m_error.clear();
#endif
        if (m_polylines.size() == 0 && getTemplate()->getPolylinesNameSize() != 0)
        {
            m_polylines = getTemplate()->getPolylinesNames();
        }

        m_enablePhysic = m_enablePhysicInit;

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventTeleport_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,Phys2dJointEvent_Activate_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,Phys2dJointEvent_Update_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,Phys2dEvent_Modify_CRC,this);
    }

    void Phys2dComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

#ifdef ITF_SUPPORT_EDITOR
        if (EVENTMANAGER)
            EVENTMANAGER->registerEvent(EditorEventLink_CRC, this);
#endif //ITF_SUPPORT_EDITOR
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,PhysEventJointDestroyed_CRC,this);
    }

    void Phys2dComponent::onResourceReady()
    {
        Super::onResourceReady();
        
        m_editableShape.onResourceReady();

        if (getWorld() && getWorld()->isActive())
            computeBodies();
        updateBodies();

#ifdef ITF_PROJECT_POP
        if(m_actor->GetComponent<StickToPolylinePhysComponent>() != NULL)
			setActive(bfalse);
#endif
    }


    void Phys2dComponent::onSceneInactive()
    {
        Super::onSceneInactive();

        clearBodies();
    }

    void Phys2dComponent::onBecomeActive()
    {
        Super::onBecomeActive();

		if (!m_body)
			computeBodies();

        if (m_body)
        {
            updateJointList();
            
            m_body->SetActive(m_enablePhysic);
            m_body->SetAwake(m_isAwake);
        }

#ifdef ITF_PROJECT_POP
        if(m_actor->GetComponent<StickToPolylinePhysComponent>() != NULL)
			setActive(bfalse);
#endif

        m_editableShape.onBecomeActive(this);


#if !defined(ITF_DISABLE_WARNING)
        if (!m_body && m_error.isEmpty())
            GetActor()->setDataError("Invalid body object !");
#endif //ITF_DISABLE_WARNING
    }

    void Phys2dComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
        if (m_body)
        {
            m_isAwake = m_body->IsAwake();
            m_body->SetActive(bfalse);
        }

        m_editableShape.onBecomeInactive(this);
    }

    Vec2d Phys2dComponent::getEditableShapeGlobalOffset(bbool _rotate) const
    {
        Vec2d offset = m_editableShape.getShape() ? m_editableShape.getLocalOffset() : Vec2d::Zero;
        offset *= GetActor()->getScale();
        if (_rotate)
            offset = offset.Rotate(m_body->GetAngle());

        return offset;
    }

#ifdef ITF_SUPPORT_EDITOR
    Pickable *      Phys2dComponent::s_currentDrag = NULL;
    b2Body   *      Phys2dComponent::s_gound = NULL;
    b2MouseJoint *  Phys2dComponent::s_joint = NULL;

    void Phys2dComponent::clearCurrentDrag()
    {
        if (s_currentDrag != m_actor)
            return;

        s_gound->GetWorld()->DestroyBody(s_gound);
        s_gound = NULL;

        // joint destroyed when destroying if needed
        s_joint = NULL;

        s_currentDrag = NULL;
    }

#endif

    void Phys2dComponent::updateAABB()
    {
        if (!m_body)
            return;

        AABB completeAABB;
        completeAABB.invalidate();
        const b2Transform & bodyTransform = m_body->GetTransform();

        for (b2Fixture* fixturePtr = m_body->GetFixtureList(); fixturePtr; fixturePtr = fixturePtr->GetNext())
        {
            b2Shape * shape = fixturePtr->GetShape();
            if (!shape)
                continue;

            i32 childCount = shape->GetChildCount();
            for (i32 i=0; i<childCount; i++)
            {
                b2AABB aabb;
                shape->ComputeAABB(&aabb, bodyTransform, i);

                completeAABB.grow(Phys2d::b2Vec2ToVec2d(aabb.lowerBound));
                completeAABB.grow(Phys2d::b2Vec2ToVec2d(aabb.upperBound));
            }
        }

        m_actor->growAABB(completeAABB);
    }

    void Phys2dComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

        if (!m_body || !m_body->IsActive())
            return;

        if (m_needUpdateJoints)
            updateJointList();

        m_actor->set2DPos(Vec2d(m_body->GetPosition().x, m_body->GetPosition().y));
		if (!m_body->IsFixedRotation() && m_linkRotationToActor)
			m_actor->setAngle(m_body->GetAngle());

        updateAABB();

        m_editableShape.update();
        updateJointDestruction();


#ifdef ITF_SUPPORT_EDITOR
        u32 controllerId = TOUCHSCREEN_ADAPTER->getControllerId();
        TouchDataArray touchDataArray;
        if ( TOUCHSURFACESMANAGER.getTouchData( controllerId, touchDataArray ) && touchDataArray.size() > 0)
        {
            Vec2d  worldPos     = touchDataArray[0].getCurrentWorldPos(m_actor->getDepth()).truncateTo2D();
            b2Vec2 b2worldPos   = b2Vec2(worldPos.x(), worldPos.y());
            switch (touchDataArray[0].state)
            {
            case TouchData::Begin:
                {
					if (!EDITOR->physicsEditionMode())
						break;

                    if (s_currentDrag)
                        break;

                    if (fixtureListTestPoint(b2worldPos))
                    {
                        s_currentDrag = m_actor;

                        b2World * world = getPhys2dWorld();
                        b2BodyDef bodyDef;
                        s_gound = world->CreateBody(&bodyDef);

                        b2MouseJointDef jointDef;
                        jointDef.bodyA        = s_gound;
                        jointDef.bodyB        = m_body;
                        jointDef.dampingRatio = 0.2f;
                        jointDef.frequencyHz  = 60.f;
                        jointDef.maxForce     = (float) (1000.0f * m_body->GetMass());
                        jointDef.target       = b2worldPos;

                        s_joint = (b2MouseJoint *)world->CreateJoint(&jointDef);
                        m_body->SetAwake(btrue);
                    }
                }
                break;
            case TouchData::Moving:
                if (s_currentDrag == m_actor)
                {
                    s_joint->SetTarget(b2worldPos);
                }
                break;

            case TouchData::End:
                clearCurrentDrag();
                break;
            }
        }
#endif //ITF_SUPPORT_EDITOR
    }

#ifdef ITF_SUPPORT_EDITOR
    void Phys2dComponent::batchPrimitives( const ITF_VECTOR<class View*>& _views )
    {
        if (!m_shapeShow || !m_body)
            return;

        Matrix44 mat;
        mat.setRotationZ(m_body->GetAngle());
        mat.setTranslation(Phys2d::b2Vec2ToVec2d(m_body->GetPosition()).to3d(GetActor()->getDepth()));

        m_shapeMesh.setMatrix(mat);
        m_shapeMesh.getCommonParam()->m_colorFactor = m_shapeColor;

        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_shapeMesh, m_actor->getDepth(), GetActor()->getRef());

    }
#endif


    void Phys2dComponent::updateJointDestruction()
    {
#ifdef ITF_SUPPORT_EDITOR
        if (m_linkEdition)
            return;
#endif //ITF_SUPPORT_EDITOR        if (m_linkEdition)

        ITF_VECTOR<JointInstance>::iterator jointInstanceIt = m_jointInstanceList.begin();
        f32 invTimeStep = f32_Inv(PHYS2D->getTimeStep());

        for (ITF_VECTOR<JointInfo>::iterator jointIt = m_jointInfoList.begin();
            jointIt != m_jointInfoList.end(); jointIt++, jointInstanceIt++)
        {
            if (jointIt->m_manualyDestroyed ||
                !jointIt->m_jointDetail ||
                (jointIt->m_jointDetail->m_maxForce < MTH_EPSILON && jointIt->m_jointDetail->m_maxTorque < MTH_EPSILON) ||
                !jointInstanceIt->getJoint())
                continue;

            b2Joint * joint = jointInstanceIt->getJoint();
            
            if ((jointIt->m_jointDetail->m_maxForce     >= MTH_EPSILON &&
                 Phys2d::b2Vec2ToVec2d(joint->GetReactionForce(invTimeStep)).sqrnorm()  > jointIt->m_jointDetail->m_maxForce * jointIt->m_jointDetail->m_maxForce) ||
                (jointIt->m_jointDetail->m_maxTorque    >= MTH_EPSILON && 
                 joint->GetReactionTorque(invTimeStep)  > jointIt->m_jointDetail->m_maxTorque))
            {
                jointIt->m_manualyDestroyed = btrue;
                jointInstanceIt->deleteJoint();
            }
        }
    }


#ifdef ITF_SUPPORT_EDITOR
    void Phys2dComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        if (m_body)
        {
            m_editableShape.onEditorMove();
            updateBodies();


            if (m_enablePhysic)
                m_body->SetAwake(true);

#ifdef ITF_PROJECT_POP
			if(m_actor->GetComponent<StickToPolylinePhysComponent>() != NULL)
				setActive(bfalse);
#endif
        }
    }

    void Phys2dComponent::onEditorCreated( class Actor* _original )
    {
        updateBodies();
    }


    void Phys2dComponent::drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        if (!(_flags & DrawEditFlag_Collisions))
            return;

        if (m_needUpdateJoints)
            return;

        if (m_body && m_body->IsActive())
        {            
            for (b2Fixture* fixturePtr = m_body->GetFixtureList(); fixturePtr; fixturePtr = fixturePtr->GetNext())
            {
                b2Shape* shape = fixturePtr->GetShape();
                DebugDraw::shapeB2(Phys2d::b2Vec2ToVec2d(m_body->GetPosition()), m_actor->getDepth(), m_body->GetAngle(), shape, Color::yellow());
            }
        }

        
        if (m_jointInstanceList.size() == m_jointInfoList.size())
        {
            ITF_VECTOR<JointInstance>::const_iterator jointInstanceIt = m_jointInstanceList.begin();
            ITF_VECTOR<JointInfo>::const_iterator jointIt = m_jointInfoList.begin();
            for (;jointIt != m_jointInfoList.end(); jointIt++, jointInstanceIt++)
            {
                const JointInfo       & joint         = *jointIt;
                const JointInstance   & jointInstance = *jointInstanceIt;
                
                if (!joint.m_jointDetail || !jointInstance.getJoint())
                    continue;

                Pickable   * child   = joint.getChild(m_actor);
                b2Joint    * joint2d = jointInstance.getJoint();
                if (joint.m_invertAB)
                {
                    joint.m_jointDetail->debugDraw(  child,   Phys2d::b2Vec2ToVec2d(joint2d->GetAnchorA()).to3d(child->getDepth()),
                        m_actor, Phys2d::b2Vec2ToVec2d(joint2d->GetAnchorB()).to3d(m_actor->getDepth()), bfalse);
                } else
                {
                    joint.m_jointDetail->debugDraw(  m_actor, Phys2d::b2Vec2ToVec2d(joint2d->GetAnchorA()).to3d(m_actor->getDepth()),
                        child,   Phys2d::b2Vec2ToVec2d(joint2d->GetAnchorB()).to3d(child->getDepth()), bfalse);
                }
            }
        }
    }


    void Phys2dComponent::clearMeshFromBodyFixtures()
    {
        for (u32 i = 0; i<m_shapeMesh.getNbMeshElement(); i++)
        {
            GFX_ADAPTER->removeIndexBuffer( m_shapeMesh.getMeshElement(i).m_indexBuffer );
        }
        m_shapeMesh.clearMeshElementList();
        m_shapeMesh.removeVertexBuffer(0);
    }


    void Phys2dComponent::createMeshFromBodyFixtures()
    {
        clearMeshFromBodyFixtures();
        if (!m_shapeShow || !m_body)
            return;

        u16 nbVertex = 0;
        u16 indexTab[64];
        u16 circleFaces = 16;

        m_shapeMat.setTexture(TEXSET_ID_DIFFUSE, GFX_ADAPTER->getWhiteOpaqueTexture());

        for (b2Fixture * fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            b2Shape * shape = fixture->GetShape();
            u32 nbIndex;

            if (shape->GetType() == b2Shape::e_polygon)
            {
                b2PolygonShape* s = (b2PolygonShape*)shape;
                nbIndex = FillNgon((Vec2d*)(&s->GetVertex(0)), s->GetVertexCount(),indexTab) * 3;

                for (u32 idx=0; idx<nbIndex; idx++)
                {
                    indexTab[idx] += nbVertex;
                }

                nbVertex += (u16)s->GetVertexCount();
            } else if (shape->GetType() == b2Shape::e_circle)
            {
                nbIndex = circleFaces * 3;
                u32 idx = 0;
                u16 ptidx = 1;

                for (; ptidx<circleFaces; ptidx++)
                {
                    indexTab[idx++] = 0 + nbVertex;
                    indexTab[idx++] = ptidx + nbVertex;
                    indexTab[idx++] = ptidx + 1 + nbVertex;
                }
                indexTab[idx++] = 0 + nbVertex;
                indexTab[idx++] = ptidx + nbVertex;
                indexTab[idx++] = 1 + nbVertex;

                nbVertex += circleFaces + 1;
            } else
                continue;

            ITF_IndexBuffer * indexBuffer = GFX_ADAPTER->createIndexBuffer(  nbIndex, bfalse);
            u16        *idxBuff;
            indexBuffer->Lock((void **) &idxBuff);
            ITF_Memcpy(idxBuff, indexTab, nbIndex * sizeof(u16));
            indexBuffer->Unlock();

            if (m_shapeMesh.getNbMaterial() == 0)
                m_shapeMesh.addElementAndMaterial(m_shapeMat);
            else
                m_shapeMesh.addElement(0);
            m_shapeMesh.getMeshElement().m_indexBuffer = indexBuffer;
            m_shapeMesh.getMeshElement().m_count       = nbIndex;

        }

        if (!nbVertex)
            return;

        m_shapeMesh.createVertexBuffer(nbVertex, VertexFormat_PCT, sizeof(VertexPCT), vbLockType_static);
        VertexPCT    *pdata;
        m_shapeMesh.LockVertexBuffer((void **) &pdata);
        for (b2Fixture * fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            b2Shape * shape = fixture->GetShape();

            if (shape->GetType() == b2Shape::e_polygon)
            {
                b2PolygonShape* s = (b2PolygonShape*)shape;
                for (i32 vertexIdx = 0; vertexIdx < s->GetVertexCount(); ++vertexIdx, ++pdata)
                {
                    Vec2d pt = Phys2d::b2Vec2ToVec2d(s->GetVertex(vertexIdx));
                    pdata->setData(pt, pt, U32_INVALID);
                }
            } else if (shape->GetType() == b2Shape::e_circle)
            {
                b2CircleShape* s = (b2CircleShape*)shape;
                pdata->setData(Vec2d::Zero, Vec2d::Zero, U32_INVALID);
                pdata++;
                for (u32 ptIdx = 0; ptIdx  <= circleFaces; ptIdx++, pdata++)
                {
                    Vec2d pt = Vec2d::Right.Rotate(ptIdx * MTH_2PI / circleFaces) * s->m_radius;
                    pdata->setData(pt, pt, U32_INVALID);
                }
            }
        }
        m_shapeMesh.UnlockVertexBuffer();
    }

#endif

    void Phys2dComponent::onEvent( Event* _event)
    {
		if(EventTeleport * teleportEvent = DYNAMIC_CAST(_event,EventTeleport))
		{
			if (m_body)
			{
#ifdef ITF_SUPPORT_EDITOR
				m_editableShape.onEditorMove();
#endif //ITF_SUPPORT_EDITOR
				if ( teleportEvent->getReset() )
				{
					getBody()->SetLinearVelocity(b2Vec2_zero);
					getBody()->SetAngularVelocity(0.0f);
				}
				updateBodies();
			}
		} else if (Phys2dJointEvent_Activate * activateEvent = DYNAMIC_CAST(_event,Phys2dJointEvent_Activate))
        {
            ITF_VECTOR<JointInfo *> joints = getJointInfoByType(activateEvent->getType());
            for (ITF_VECTOR<JointInfo *>::iterator it = joints.begin(); it != joints.end(); it++)
            {
                if(*it)
                {
                    (*it)->m_isActive = activateEvent->activate();
                    updateJointList();
                }
            }
            
        } else if (Phys2dJointEvent_Update * activateEvent = DYNAMIC_CAST(_event,Phys2dJointEvent_Update))
        {
            JointDetails * jointDetails = activateEvent->getJointDetail();
            if (jointDetails)
            {
                ITF_VECTOR<JointInfo *> joints = getJointInfoByType(jointDetails->getType());
                for (ITF_VECTOR<JointInfo *>::iterator it = joints.begin(); it != joints.end(); it++)
                {
                    if(*it && (*it)->m_jointDetail && jointDetails->getType() == (*it)->m_jointDetail->getType())
                    {
                        BinaryClone(jointDetails, (*it)->m_jointDetail);
                        updateJointList();
                    }
                }
            }
        } else if (Phys2dEvent_Modify * modifyEvent = DYNAMIC_CAST(_event,Phys2dEvent_Modify))
        {
            bbool somethingDone = bfalse;
            
            if (modifyEvent->changeBodyActivationStatus())
            {
                somethingDone = btrue;
                m_body->SetActive(modifyEvent->getBodyActivationStatus());
            }

            if (m_body && modifyEvent->changeDensity())
            {
                somethingDone = btrue;
                for (b2Fixture * fix = m_body->GetFixtureList(); fix != NULL; fix = fix->GetNext())
                    fix->SetDensity(modifyEvent->getDensity());
            }

            if (modifyEvent->changeRestitution())
            {
                somethingDone = btrue;
                for (b2Fixture * fix = m_body->GetFixtureList(); fix != NULL; fix = fix->GetNext())
                    fix->SetRestitution(modifyEvent->getRestitution());
            }

            if (modifyEvent->changeLinearDampling())
            {
                somethingDone = btrue;
                m_body->SetLinearDamping(modifyEvent->getLinearDampling());
            }

            if (modifyEvent->changeAngularDampling())
            {
                somethingDone = btrue;
                m_body->SetAngularDamping(modifyEvent->getAngularDampling());
            }

            if (modifyEvent->changeGravityMultiplier())
            {
                somethingDone = btrue;
                m_body->SetGravityScale(modifyEvent->getGravityMultiplier());
            }

            if (modifyEvent->changeType())
            {
                somethingDone = btrue;
                m_body->SetType((b2BodyType)modifyEvent->getType());
            }

            if (modifyEvent->changeFixedRotation())
            {
                somethingDone = btrue;
                m_body->SetFixedRotation(modifyEvent->getFixedRotation());
            }

            ITF_WARNING(GetActor(), somethingDone, "Phys2dEvent_Modify called without any modification");
        }
		else if( Phys2dEvent_EnablePhysic * enable =  DYNAMIC_CAST(_event, Phys2dEvent_EnablePhysic) )
		{
			setPhysicEnabled(enable->isPhysicEnabled());
		}
        Super::onEvent(_event);

#ifdef ITF_SUPPORT_EDITOR
        if (EditorEventLink * event = DYNAMIC_CAST(_event, EditorEventLink))
        {
            if (event->getSender() == GetActor()->getRef())
            {
                updateJointList();
            }
        }
#endif

        if (PhysEventJointDestroyed * event = DYNAMIC_CAST(_event, PhysEventJointDestroyed))
        {
            setJointDeleted(event->getJoint());
        }
    }

	void Phys2dComponent::teleportTo(const Vec3d& _pos)
	{
		ActorComponent::teleportTo(_pos);

		if (m_body)
		{
			m_body->SetTransform(Phys2d::Vec2dToB2Vec2(_pos.truncateTo2D()), m_body->GetAngle());
			m_body->SetLinearVelocity(b2Vec2_zero);
			m_body->SetAngularVelocity(0.f);
		}
	}

    void Phys2dComponent::teleportTo(const Vec3d& _pos, f32 _angle)
    {
        ActorComponent::teleportTo(_pos);

        if (m_body)
        {
            m_body->SetTransform(Phys2d::Vec2dToB2Vec2(_pos.truncateTo2D()), _angle);
            m_body->SetLinearVelocity(b2Vec2_zero);
            m_body->SetAngularVelocity(0.f);
        }
    }

    void Phys2dComponent::registerListener(Phys2dComponentListener * _listener)
    {
        int idx = m_listernerList.find(_listener);
        if (idx < 0)
		{
            m_listernerList.push_back(_listener);
		}
    }

    void Phys2dComponent::unregisterListener(Phys2dComponentListener * _listener)
    {
        int idx = m_listernerList.find(_listener);
        if (idx >= 0)
            m_listernerList.erase(m_listernerList.begin() + idx);
    }

    void Phys2dComponent::unregisterAllListener()
    {
        m_listernerList.clear();
    }

#ifdef ITF_SUPPORT_EDITOR
    void Phys2dComponent::centerActorAndEditableShape()
    {
        PhysShapePolygon  *physPolygon = SAFE_DYNAMIC_CAST(m_editableShape.getShape(), PhysShapePolygon);
        if (!physPolygon)
            return;

        Vec2d center(Vec2d::Zero);
        const ITF_VECTOR<Vec2d> & pointList = physPolygon->getVertex();
        ITF_VECTOR<Vec2d>::const_iterator ptIter = pointList.begin();
        for( ; ptIter != pointList.end(); ++ptIter)
        {
            center += *ptIter;
        }
        center /= (f32)pointList.size();

        if (center.IsEqual(Vec2d::Zero, MTH_EPSILON))
            return;

        Vec2d centerActor = center;
        GetActor()->transformLocalVectorToWorld(centerActor);
        GetActor()->set2DPos(GetActor()->get2DPos() + centerActor);

        ITF_VECTOR<Vec2d> newList;
        newList.resize(pointList.size());

        ptIter = pointList.begin();
        for( ITF_VECTOR<Vec2d>::iterator newPtIter = newList.begin();
            newPtIter != newList.end(); ++newPtIter, ++ptIter)
        {
            *newPtIter = *ptIter - center;
        }
        physPolygon->setPoints(newList);
        
        m_editableShape.setLocalOffset(Vec2d::Zero);
        m_editableShape.initialize(m_actor);
    }
    
    void Phys2dComponent::onPostPropertyChange()
    {
        if (m_editableShape.getShape() && m_editableShapeCentered)
            centerActorAndEditableShape();
        
        computeBodies();
        updateBodies();
        // update always called in objectChange
        // updateJointList();

        if (m_body)
        {
            m_body->SetAwake(m_isAwake);
            m_body->SetActive(m_enablePhysic);

#ifdef ITF_PROJECT_POP
            if(m_actor->GetComponent<StickToPolylinePhysComponent>() != NULL)
				setActive(bfalse);
#endif
        }
    }
#endif //ITF_SUPPORT_EDITOR


    ITF_VECTOR<JointInfo *> Phys2dComponent::getJointInfoByType(JointDetailsType _type)
    {
        ITF_VECTOR<JointInfo *> returnJoints;

        for (ITF_VECTOR<JointInfo>::iterator jointIt = m_jointInfoList.begin(); jointIt != m_jointInfoList.end(); jointIt++)
        {
            if (_type == JointDetails_None || (jointIt->m_jointDetail && jointIt->m_jointDetail->getType() == _type))
                returnJoints.push_back(&(*jointIt));
        }

        return returnJoints;
    }

    void Phys2dComponent::updateJointList()
    {
        if (!m_linkComponent)
            return;
        
#ifdef ITF_SUPPORT_EDITOR
        if (m_linkEdition)
            return;
#endif //ITF_SUPPORT_EDITOR


        m_needUpdateJoints  = bfalse;
        clearSourceJoints();

        const LinkComponent::ChildrenList & childrenList = m_linkComponent->getChildren();

        // remove destroyed links
        ITF_VECTOR<i32> newLinks;
        newLinks.resize(m_linkComponent->getChildren().size());
        for (LinkComponent::ChildrenList::const_iterator it = childrenList.begin();
            it != childrenList.end(); ++it)
        {
            if ((*it).hasTag(JOINT_CRC))
            {
                i32         value = 0;
                String8     valueStr;
                (*it).getTagValue( JOINT_CRC, valueStr);
                if (!valueStr.isEmpty())
                    value = atoi(valueStr.cStr());
                if (value <= 0)
                    value = 1;
                if (value > 5)
                    value = 5;
                newLinks[it - childrenList.begin()] = value;

            } else
            {
                newLinks[it - childrenList.begin()] = 0;
            }
        }
        
        for (ITF_VECTOR<JointInfo>::iterator jointIt = m_jointInfoList.begin();jointIt != m_jointInfoList.end(); )
        {
            bbool jointFound = bfalse;
            for (LinkComponent::ChildrenList::const_iterator it = childrenList.begin();
                it != childrenList.end(); ++it)
            {
                if (newLinks[it - childrenList.begin()] <= 0)
                    continue;

                bbool isPath = it->getPath() == (*jointIt).m_childPath;
#ifdef ITF_SUPPORT_EDITOR
                isPath = isPath || (it->getPreviousPath() == (*jointIt).m_childPath);
#endif //ITF_SUPPORT_EDITOR

                if (!isPath)
                    continue;

#ifdef ITF_SUPPORT_EDITOR
                if ((it->getPreviousPath() == (*jointIt).m_childPath))
                    (*jointIt).m_childPath = it->getPath();
#endif //ITF_SUPPORT_EDITOR

                jointFound = btrue;
                newLinks[it - childrenList.begin()]--;
                break;
            }

            if (!jointFound)
            {
                if ((*jointIt).m_jointDetail)
                {
                    SF_DEL((*jointIt).m_jointDetail);
                }
                jointIt = m_jointInfoList.erase(jointIt);
            } else
            {
                jointIt++;
            }
        }

        // create new links
        for (LinkComponent::ChildrenList::const_iterator it = childrenList.begin();
            it != childrenList.end(); ++it)
        {
            for (;newLinks[it - childrenList.begin()] > 0; newLinks[it - childrenList.begin()]--)
            {
                m_jointInfoList.push_back(JointInfo((*it).getPath()));
            }
        }

        // compute instance data
        m_jointInstanceList.resize(m_jointInfoList.size());
        ITF_VECTOR<JointInstance>::iterator jointInstanceIt = m_jointInstanceList.begin();
        for (ITF_VECTOR<JointInfo>::iterator jointIt = m_jointInfoList.begin();
            jointIt != m_jointInfoList.end(); jointIt++, jointInstanceIt++)
        {
            jointInstanceIt->createJoint(&(*jointIt), GetActor(), getBody(), m_applyScaleToJoints);
        }
    }
    void Phys2dComponent::setActive( bbool _active )
    {
        m_enablePhysic = _active;
        if (m_body)
        {
            teleportTo(m_actor->getPos(), m_actor->getAngle());
            m_body->SetActive(_active);
        }
    }

    ITF::bbool Phys2dComponent::isActive()
    {
        if (m_body)
        {
            return m_body->IsActive();
        }

        return bfalse;
    }

    

    void Phys2dComponent::setJointDeleted(b2Joint * _joint)
    {
        if (m_needUpdateJoints)
            return;

        for (ITF_VECTOR<JointInstance>::iterator jointIt = m_jointInstanceList.begin();
            jointIt != m_jointInstanceList.end(); jointIt++)
        {
            JointInstance & jointInfo = *jointIt;
            
            if (jointInfo.getJoint() == _joint)
            {
                m_needUpdateJoints = btrue;
            }
        }
    }

    void Phys2dComponent::beginContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact )
    {
        for (ITF_VECTOR<Phys2dComponentListener*>::iterator it = m_listernerList.begin(); it != m_listernerList.end(); it++)
            (*it)->beginContact(_pickableA, _pickableB, _contact);
    }

    void Phys2dComponent::endContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact )
    {
        for (ITF_VECTOR<Phys2dComponentListener*>::iterator it = m_listernerList.begin(); it != m_listernerList.end(); it++)
            (*it)->endContact(_pickableA, _pickableB, _contact);
    }

    void Phys2dComponent::preSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2Manifold* _oldManifold )
    {
        for (ITF_VECTOR<Phys2dComponentListener*>::iterator it = m_listernerList.begin(); it != m_listernerList.end(); it++)
            (*it)->preSolve(_pickableA, _pickableB, _contact, _oldManifold);
    }

    void Phys2dComponent::postSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2ContactImpulse* _impulse )
    {
        for (ITF_VECTOR<Phys2dComponentListener*>::iterator it = m_listernerList.begin(); it != m_listernerList.end(); it++)
            (*it)->postSolve(_pickableA, _pickableB, _contact, _impulse);
    }

    void Phys2dComponent::polylineAdd( StringID _polylineID, bbool _recomputeBody )
    {
        if (m_polylines.find(_polylineID) == -1)
        {
            m_polylines.push_back(_polylineID);
            if(_recomputeBody)
            {
                computeBodies();
                updateBodies();
            }
        }
    }

    void Phys2dComponent::polylinesRemove( StringID _polylineID, bbool _recomputeBody )
    {
        u32 index = m_polylines.find(_polylineID);
        if (index != -1)
        {
            m_polylines.removeAt(index);
            if(_recomputeBody)
            {
                computeBodies();
                updateBodies();
            }
        }
    }
}

#endif
