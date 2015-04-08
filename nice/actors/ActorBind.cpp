#include "precompiled_engine.h"

#ifndef _ITF_ACTOR_BIND_H_
#include "engine/actors/ActorBind.h"
#endif // _ITF_ACTOR_BIND_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMATIONBONES_H_
#include "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_PROCEDURALBONE_H_
#include "engine/actors/ProceduralBone.h"
#endif //_ITF_PROCEDURALBONE_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

// #ifdef ITF_SUPPORT_EDITOR
// #ifndef _ITF_UNDOREDOMANAGER_H_
// #include "editor/UndoRedoManager.h"
// #endif //_ITF_UNDOREDOMANAGER_H_
// #endif

namespace ITF
{
    BEGIN_SERIALIZATION(Bind)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data | ESerialize_Editor)
            SERIALIZE_MEMBER("parentPath",          m_parent);
            SERIALIZE_MEMBER("offsetPos",           m_initialPositionOffset);
            SERIALIZE_MEMBER("offsetAngle",         m_initialAngleOffset);
            SERIALIZE_ENUM_BEGIN("type",            m_type);
                SERIALIZE_ENUM_VAR(Root);
                SERIALIZE_ENUM_VAR(BoneName);
                SERIALIZE_ENUM_VAR(ProceduralBoneName);
            SERIALIZE_ENUM_END();
            SERIALIZE_MEMBER("typeData",            m_typeData);
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable | ESerialize_Editor)
            SERIALIZE_MEMBER("useParentFlip",       m_useParentFlip);
            SERIALIZE_MEMBER("useParentScale",      m_useParentScale);
            SERIALIZE_MEMBER("useParentAlpha",      m_useParentAlpha);
            SERIALIZE_MEMBER("removeWithParent",    m_removeWithParent);
        END_CONDITION_BLOCK()
        
        SERIALIZE_FUNCTION(postLoadInstance, ESerialize_Data_Load | ESerialize_Editor_Load);
    END_SERIALIZATION()

    Bind::Bind()
    : m_parent()
    , m_useParentScale(bfalse)
    , m_useParentFlip(bfalse)
    , m_useParentAlpha(bfalse)
    , m_removeWithParent(bfalse)
    , m_type(Root)
    , m_typeData(0)
    , m_runtimeParent()
    , m_initialPositionOffset(Vec3d::Zero)
    , m_positionOffset(Vec3d::Zero)
    , m_initialAngleOffset(0.0f)
    , m_angleOffset(0.0f)
    , m_runtimeDisabled(bfalse)
    , m_temporaryBind( bfalse )
    {
    }

    void Bind::onSceneActive(Pickable* _owner)
    {
        WorldUpdateElement* wue = _owner->getWorldUpdateElement();
        
        Pickable* pParent = static_cast<Pickable*>(m_runtimeParent.getObject());

        if ( pParent )
        {
            if(WorldUpdateElement* parentWue = pParent->getWorldUpdateElement())
            {
                if( wue->getParent() != parentWue )
                {
                    WORLD_MANAGER->getWorldUpdate().bindElement(parentWue, wue);
                }
            }
        }
    }

    void Bind::onSceneInactive(Pickable* _owner)
    {
        WorldUpdateElement* wue = _owner->getWorldUpdateElement();
        
        if(WorldUpdateElement* parentWue = wue->getParent())
        {
            Pickable* pParent = static_cast<Pickable*>(m_runtimeParent.getObject());

            if ( pParent )
            {
                ITF_ASSERT_CRASH(parentWue == pParent->getWorldUpdateElement(), "Wrong parent");
                WORLD_MANAGER->getWorldUpdate().unbindElement(parentWue, wue);
            }
        }
    }

    bbool Bind::relativePathGoingUp( const ObjectPath& _relativePath )
    {
        ITF_ASSERT(!_relativePath.getIsAbsolute());

        if(_relativePath.getNumLevels() > 0)
        {
            return _relativePath.getLevel(0).m_parent;
        }
        
        return bfalse;
    }

    const ProceduralBone* getProceduralBone( const Actor* _actor, const StringID _id )
    {
        return ProceduralBone::getBone(_actor, _id);
    }

    BindHandler::BindHandler( )
        : m_owner(NULL)
    {
    }

    void BindHandler::setOwner( Actor* _owner )
    {
        ITF_ASSERT_CRASH(_owner, "Owner can't be NULL");

        m_owner = _owner;
    }

    bbool BindHandler::hasChild(Actor* _child) const
    {
        return (m_children.find(_child->getRef()) >= 0);
    }

    void BindHandler::addChild( Actor* _child, bbool _updatePosRecursive /*= bfalse*/ )
    {
        ITF_ASSERT_MSG(m_owner!=NULL,"There is no owner set");

        if ( !m_owner )
        {
            return;
        }

        ObjectRef childRef = _child->getRef();

        ITF_ASSERT_CRASH(!hasChild(_child), "Can't add twice the same child");

        m_children.push_back(childRef); // if more than 2 priorities, a scan if needed here

        Bind* childBindToParent = _child->getParentBind();
        ITF_ASSERT_CRASH(childBindToParent, "Wrong child");
        childBindToParent->m_runtimeParent = m_owner->getRef();

        if ( childBindToParent->m_type == Bind::BoneName && childBindToParent->m_typeData != U32_INVALID )
        {
            AnimLightComponent* animLight = m_owner->GetComponent<AnimLightComponent>();

            if ( animLight )
            {
                animLight->setDisableVisiblePrediction(btrue);
            }
        }

        // Add object bind to m_owner so children's update done after parent
        if(m_owner->getIsSceneActive() && _child->getIsSceneActive())
            WORLD_MANAGER->getWorldUpdate().bindElement(m_owner->getWorldUpdateElement(), _child->getWorldUpdateElement());

        updateWorldCoordinates(_child, childBindToParent, _updatePosRecursive);
        computeInitialTransform(_child);
    }

    void BindHandler::removeChild( Actor* _child )
    {
        ObjectRef childRef = _child->getRef();

        i32 index = m_children.find(childRef);
        ITF_ASSERT_CRASH(index >= 0, "Can't delete unknown child");
        m_children.removeAtUnordered(index); // keep order because children are ordered by priority
                

        ITF_ASSERT_CRASH(_child->getParentBind(), "Wrong child");
        _child->getParentBind()->m_runtimeParent.invalidate();
        
        if(m_owner->getIsSceneActive() && _child->getIsSceneActive())
            WORLD_MANAGER->getWorldUpdate().unbindElement(m_owner->getWorldUpdateElement(), _child->getWorldUpdateElement());
    }

    void BindHandler::onSceneActive()
    {
        WorldUpdateElement* wue = m_owner->getWorldUpdateElement();
        u32 childCount = m_children.size();
        for(u32 iChild = 0; iChild < childCount; ++iChild)
        {
            Pickable* pGenericChild = static_cast<Pickable*>(m_children[iChild].getObject());
            ITF_ASSERT_MSG(pGenericChild!=m_owner, "An actor is bound to itself");
            
            if ( pGenericChild == m_owner )
            {
                Actor* childActor = pGenericChild->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                if ( childActor )
                {
                    removeChild(childActor);
                }
                else
                {
                    m_children.removeAtUnordered(iChild);
                }
                m_owner->setParentBind(NULL);
                childCount--;
                continue;
            }

            ITF_ASSERT_CRASH(pGenericChild, "A child was deleted but not removed from parent bind list");
            if(WorldUpdateElement* childWue = pGenericChild->getWorldUpdateElement())
                if(childWue->getParent() != wue)
                    WORLD_MANAGER->getWorldUpdate().bindElement(wue, childWue);
        }
    }

    void BindHandler::onSceneInactive()
    {
        WorldUpdateElement* wue = m_owner->getWorldUpdateElement();
        const u32 childCount = m_children.size();
        for(u32 iChild = 0; iChild < childCount; ++iChild)
        {
            Pickable* pGenericChild = static_cast<Pickable*>(m_children[iChild].getObject());
            ITF_ASSERT_CRASH(pGenericChild, "A child was deleted but not removed from parent bind list");
            
            if(WorldUpdateElement* childWue = pGenericChild->getWorldUpdateElement())
                if(childWue->getParent() == wue)
                    WORLD_MANAGER->getWorldUpdate().unbindElement(wue, childWue);
        }
    }

    void BindHandler::update( )
    {
        ITF_ASSERT_CRASH(m_owner, "Owner can't be NULL");

        AABB3d ownerAABB = m_owner->getAABB3d();

        const u32 childCount = m_children.size();
        for(u32 iChild = 0; iChild < childCount; ++iChild)
        {
            Actor* child = static_cast<Actor*>(m_children[iChild].getObject());
            ITF_ASSERT_CRASH(child, "A child was deleted but not removed from parent bind list");

            Bind* pBind = child->getParentBind();
            ITF_ASSERT_CRASH(pBind, "Invalid child, not bind information");
            
            if (pBind->isRuntimeDisabled())
            {
                continue;
            }
                
            updateWorldCoordinates(child, pBind);

            ownerAABB.grow(child->getAABB3d());
        }

        // Include children's AABB inside owner's AABB
        m_owner->setAABB3d(ownerAABB);
    }

    void BindHandler::growAABB()
    {
        ITF_ASSERT_CRASH(m_owner, "Owner can't be NULL");

        AABB3d ownerAABB = m_owner->getAABB3d();

        const u32 childCount = m_children.size();
        for(u32 iChild = 0; iChild < childCount; ++iChild)
        {
            Actor* child = static_cast<Actor*>(m_children[iChild].getObject());
            ITF_ASSERT_CRASH(child, "A child was deleted but not removed from parent bind list");

            Bind* pBind = child->getParentBind();
            ITF_ASSERT_CRASH(pBind, "Invalid child, not bind information");

            if (pBind->isRuntimeDisabled())
            {
                continue;
            }

            ownerAABB.grow(child->getAABB3d());
        }

        // Include children's AABB inside owner's AABB
        m_owner->setAABB3d(ownerAABB);
    }

    static f32 getParentAlpha(const Bind* _child)
    {
        f32 result = 1.0f;
        if(const Actor* parent = static_cast<Actor*>(_child->m_runtimeParent.getObject()))
        {
            const AnimLightComponent* animLightComponent = parent->GetComponent<AnimLightComponent>();

            switch(_child->m_type)
            {
            case Bind::Root:
            case Bind::ProceduralBoneName:
                if(animLightComponent)
                    result = animLightComponent->getAlpha();
                else if ( const GraphicComponent* fallbackgraphicComponent = parent->GetComponent<GraphicComponent>() )
                    result = fallbackgraphicComponent->getAlpha();
                break;

            case Bind::BoneName:
                if ( animLightComponent )
                {
                    if(animLightComponent->isLoaded() && animLightComponent->getNumPlayingSubAnims())
                    {
                        const StringID boneName(_child->m_typeData);
                        i32 idx = animLightComponent->getBoneIndex(boneName);

                        if(idx != U32_INVALID)
                        {
                            animLightComponent->getBoneAlpha(idx, result);
                            }
                        }
                    }
                break;

            default:
                ITF_ASSERT_MSG(0, "Not supported yet");
                break;
            }
        }

        return result;
    }

    void BindHandler::updateWorldCoordinates( Actor* _childActor, Bind* _childBind, bbool _updatePosRecursive /*= bfalse*/ ) const
    {
        Vec3d pos;
        f32 angle;

        // Do not update child if they are not both 2D or 3D. For 2D actors, scale is tweaked depending on the screen resolution so it would 
        // move its 3D children when resizing the screen.
        if(m_owner->getIs2D() != _childActor->getIs2D())
            return;

        const bbool bApply = computeWorldCoordinates(
            _childBind, _childBind->getPosOffset(), _childBind->getAngleOffset(),
            pos, angle);

        if(bApply)
        {
            if(_childBind->m_useParentFlip)
                _childActor->setIsFlipped(m_owner->getIsFlipped());

            if(_childBind->m_useParentScale)
                _childActor->setScale(m_owner->getScale());

            if(_childBind->m_useParentAlpha)
            {
                EventShow setAlphaEvent;
                setAlphaEvent.setAlpha(getParentAlpha(_childBind));
                _childActor->onEvent(&setAlphaEvent);
            }

			// PreUpdate for on the components
			{
				const Actor::ComponentsVector & components = _childActor->GetAllComponents();

				Actor::ComponentsVector::const_iterator it = components.begin();
				while ( it != components.end() )
				{
					ActorComponent* pComponent = *it;

					pComponent->preUpdatePos( pos );
                    
                    it++;
				}

				_childActor->setAngleAndPos(angle,pos);
            }
        }

        if (_updatePosRecursive)
        {
            const BindHandler & handler = _childActor->getChildrenBindHandler();
            const u32 childCount = handler.m_children.size();
            for(u32 iChild = 0; iChild < childCount; ++iChild)
            {
                Actor* child = static_cast<Actor*>(handler.m_children[iChild].getObject());
                ITF_ASSERT_CRASH(child, "A child was deleted but not removed from parent bind list");

                handler.updateWorldCoordinates(child, child->getParentBind(), _updatePosRecursive);
            }
        }
    }

    void BindHandler::replaceReinitDataByCurrent()
    {
        // replaceReinitDataByCurrent may remove some children, so we are working on a copy. 
        ITF_VECTOR<ObjectRef> childrenCopy;
        childrenCopy.reserve(m_children.size());
        for(ITF_VECTOR<ObjectRef>::iterator it = m_children.begin() ; it != m_children.end() ; ++it)
        {
            childrenCopy.push_back(*it);
        }

        for(u32 iChild = 0; iChild < childrenCopy.size(); ++iChild)
        {
            ObjectRef childRef = childrenCopy[iChild];
            if(m_children.find(childRef) == -1)
                continue;

            Actor* child = static_cast<Actor*>(childRef.getObject());
            ITF_ASSERT_CRASH(child, "A child was deleted but not removed from parent bind list");

            child->replaceReinitDataByCurrent();
        }
    }

    void BindHandler::clear()
    {
        ITF_VECTOR<Actor*> children;
        for(u32 iChild = 0; iChild < m_children.size(); ++iChild)
        {
            if(Actor* pChild = static_cast<Actor*>(m_children[iChild].getObject()))
            {
                children.push_back(pChild);
            }
        }

        for(u32 iChild = 0; iChild < children.size(); ++iChild)
            removeChild(children[iChild]);

        m_children.clear();
    }

    void BindHandler::requestChildrenDestruction()
    {
        for(u32 iChild = 0; iChild < m_children.size(); ++iChild)
        {
            if(Actor* pChild = static_cast<Actor*>(m_children[iChild].getObject()))
            {
                if(Bind* pParentBind = pChild->getParentBind())
                {
                    if (!pParentBind->isRuntimeDisabled() && pParentBind->m_removeWithParent)
                    {
                        pChild->requestDestruction();
                        iChild--; // unbindFromParent() called from requestDestruction() modify m_children.size()
                    }
                }
            }
        }
    }

    bbool BindHandler::getTransform( const Bind* _childBind, Vec3d& _pos, f32& _angle ) const
    {
        _pos = Vec3d::Zero;
        _angle = 0.f;

        switch(_childBind->m_type)
        {
        case Bind::Root:
            {
                _pos = m_owner->getPos();                
                _angle = m_owner->getAngle();
            }
            return btrue;

        case Bind::BoneName:
            {
                if ( AnimLightComponent* pAnimLightComponent = m_owner->GetComponent<AnimLightComponent>() )
                {
                    if(pAnimLightComponent->isLoaded() && pAnimLightComponent->getNumPlayingSubAnims())
                    {
                        const StringID boneName(_childBind->m_typeData);
                        i32 idx = pAnimLightComponent->getBoneIndex(boneName);

                        if(idx != U32_INVALID)
                        {
                            if (pAnimLightComponent->getBonePos(idx, _pos) && pAnimLightComponent->getBoneAngle(idx, _angle))
                                return btrue;
                            }
                        }
                    }
                }
            break;

        case Bind::ProceduralBoneName:
            {
                const ProceduralBone* proceduralBone = getProceduralBone(m_owner, StringID(_childBind->m_typeData));
                if (proceduralBone)
                {
                    _pos = proceduralBone->getTransform().getPos();
                    _angle = proceduralBone->getTransform().getAngle();
                    return btrue;
                }
            }
            break;

        default:
            ITF_ASSERT_MSG(0, "Not supported yet");
            break;
        }

        return bfalse;
    }

    static void computeWorldAngle( f32 _angle, f32 _parentAngle, f32& _result )
    {
        _result = _parentAngle + _angle;
    }

    static void computeWorldPosition( const Vec3d& _position, const Vec3d& _parentPos, f32 _parentAngle, const Vec2d& _parentScale, bbool _flip, Vec3d& _result )
    {
        _result = _position;
        if(_flip)
            _result.x() = -_result.x();

        _result.x() *= _parentScale.x();
        _result.y() *= _parentScale.y();

        Vec3d_Rotate(&_result, _parentAngle);
        _result += _parentPos;
    }

    bbool BindHandler::computeWorldCoordinates( const Bind* _childBind,
        const Vec3d& _localPos, const f32 _localAngle,  // input
        Vec3d& _worldPos, f32& _worldAngle ) const            // output
    {
        Vec3d localPos = _localPos;

        switch(_childBind->m_type)
        {
        case Bind::Root:
            {
                const bbool bFlip = _childBind->m_useParentFlip && m_owner->getIsFlipped();

                computeWorldAngle(_localAngle, m_owner->getAngle(), _worldAngle);
                computeWorldPosition( localPos, m_owner->getPos(), m_owner->getAngle(), m_owner->getScale(), bFlip, _worldPos );
            }
            return btrue;

        case Bind::BoneName:
            {
                if ( AnimLightComponent* pAnimLightComponent = m_owner->GetComponent<AnimLightComponent>() )
                {
                    if(pAnimLightComponent->isLoaded() && pAnimLightComponent->getNumPlayingSubAnims())
                    {
                        const StringID boneName(_childBind->m_typeData);
                        i32 idx = pAnimLightComponent->getBoneIndex(boneName);

                        if(idx != U32_INVALID)
                        {
                            Vec2d pos;
                            f32   angle;
                            if ( pAnimLightComponent->getBonePos(idx, pos) && pAnimLightComponent->getBoneAngle(idx, angle) )
                            {
                                const bbool bFlip   = _childBind->m_useParentFlip && m_owner->getIsFlipped();
                                const f32 fZ        = m_owner->getDepth();
                                f32 localAngle = _localAngle;
                               
                                if ( bFlip )
                                {
                                    localPos = Vec3d( -localPos.x(), -localPos.y(), localPos.z() );
                                    localAngle = MTH_PI - localAngle;
                                }
                                computeWorldAngle( localAngle, angle, _worldAngle);
                                computeWorldPosition( localPos, pos.to3d(fZ), angle, m_owner->getScale(), bFlip, _worldPos);
                                return btrue;
                            }
                        }
                    }
                }
            }
            return bfalse;

        case Bind::ProceduralBoneName:
            {
                const ProceduralBone* proceduralBone = getProceduralBone(m_owner, StringID(_childBind->m_typeData));
                if (proceduralBone)
                {
                    _worldAngle = proceduralBone->getTransform().transformAngle(_localAngle);
                    _worldPos = proceduralBone->getTransform().transformPos(localPos);
                    return btrue;
                }
            }
            return bfalse;

        default:
            ITF_ASSERT_MSG(0, "Not supported yet");
            break;
        }

        return bfalse;
    }

    bbool BindHandler::computeLocalCoordinates( const Bind* _childBind, const Vec3d& _childWorldPos, const f32 _childWorldAngle, Vec3d& _localPos, f32& _localAngle ) const
    {
        switch (_childBind->m_type)
        {
        case Bind::ProceduralBoneName:
            {
                const ProceduralBone* proceduralBone = getProceduralBone(m_owner, StringID(_childBind->m_typeData));
                if (proceduralBone)
                {
                    _localPos = proceduralBone->getTransform().inverseTransformPos(_childWorldPos);
                    _localAngle = proceduralBone->getTransform().inverseTransformAngle(_childWorldAngle);
                    return btrue;
                }
            }
            return bfalse;
        }

        Vec3d parentWorldPos;
        f32 parentWorldAngle;
        if(getTransform( _childBind, parentWorldPos, parentWorldAngle ))
        {
            _localAngle = _childWorldAngle - parentWorldAngle;
            _localPos   = _childWorldPos - parentWorldPos;
            Vec3d_Rotate(&_localPos, -parentWorldAngle);
            _localPos.x() /= m_owner->getScale().x();
            _localPos.y() /= m_owner->getScale().y();
            
            if(_childBind->m_useParentFlip && m_owner->getIsFlipped())
                _localPos.x() = -_localPos.x();

            return btrue;

// WIP 
#if 0
            switch(_childBind->m_type)
            {
            case ActorBind::Root:
                {
                    if(_childBind->m_useParentFlip && m_owner->isFlipped())
                        _localPos.x() = -_localPos.x();

                    return btrue;
                }
                break;

            case ActorBind::BoneName:
                {
                    if ( AnimLightComponent* pAnimLightComponent = m_owner->GetComponent<AnimLightComponent>() )
                    {
                        if(pAnimLightComponent->isLoaded() && pAnimLightComponent->getNumPlayingSubAnims())
                        {
                            const StringID boneName(_childBind->m_typeData);
                            i32 idx = pAnimLightComponent->getBoneIndex(boneName);

                            if(idx != U32_INVALID)
                            {
                                if ( AnimBoneDyn* bone = pAnimLightComponent->getBone(idx) )
                                {
                                    if(_childBind->m_useParentFlip && pAnimLightComponent->getFlipped())
                                    {
                                        _localAngle = parentWorldAngle - _childWorldAngle;
                                        _localPos.x() = -_localPos.x();
                                    }
                                    return btrue;
                                }
                            }
                        }
                    }

                }
                break;

            default:
                ITF_ASSERT_MSG(0, "Not supported yet");
                break;
            }
#endif // 0
        }
        return bfalse;
    }

    static bbool getInitialTransform(const Actor* _owner, const Bind* _bind, Transform3d& _t)
    {
        switch(_bind->m_type)
        {
        case Bind::Root:
            {
                const bbool flip = _bind->m_useParentFlip && _owner->getWorldInitialFlip();
                _t.setFrom(_owner->getBoundWorldInitialPos(bfalse), _owner->getBoundWorldInitialAngle(bfalse), _owner->getWorldInitialScale(), flip);
            }
            return btrue;

        case Bind::BoneName:
            {
                if ( AnimLightComponent* pAnimLightComponent = _owner->GetComponent<AnimLightComponent>() )
                {
                    if(pAnimLightComponent->isLoaded() && pAnimLightComponent->getNumPlayingSubAnims())
                    {
                        const StringID boneName(_bind->m_typeData);
                        i32 idx = pAnimLightComponent->getBoneIndex(boneName);

                        if(idx != U32_INVALID)
                        {
                            Vec2d pos;
                            f32   angle;
                            if ( pAnimLightComponent->getBonePos(idx, pos) && pAnimLightComponent->getBoneAngle(idx, angle))
                            {
                                _t.setFrom(pos.to3d(_owner->getDepth()), angle, _owner->getScale(), _owner->getIsFlipped());
                                return btrue;
                            }
                        }
                    }
                }
            }
            break;

        case Bind::ProceduralBoneName:
            {
                const ProceduralBone* proceduralBone = getProceduralBone(_owner, StringID(_bind->m_typeData));
                if (proceduralBone)
                {
                    _t = proceduralBone->getTransform();
                    return btrue;
                }
            }
            break;

        default:
            ITF_ASSERT_MSG(0, "Not supported yet");
            break;
        }

        return bfalse;
    }
    
    bbool BindHandler::computeWorldInitialCoordinates( const Bind* _childBind, const Vec3d& _localPos, const f32 _localAngle, Vec3d& _worldPos, f32& _worldAngle ) const
    {
        Vec3d localPos = _localPos;

        Transform3d transform;
        if(getInitialTransform(m_owner, _childBind, transform))
        {
            _worldPos = transform.transformPos(localPos);
            _worldAngle = transform.transformAngle(_localAngle);
            
            return btrue;
        }
        return bfalse;
    }

    bbool BindHandler::computeLocalInitialCoordinates( const Bind* _childBind, const Vec3d& _worldPos, const f32 _worldAngle, Vec3d& _localInitPos, f32& _localInitAngle ) const
    {
        Transform3d transform;
        if(getInitialTransform(m_owner, _childBind, transform))
        {
            _localInitPos = transform.inverseTransformPos(_worldPos);
            _localInitAngle = transform.inverseTransformAngle(_worldAngle);
            
            return btrue;
        }
        return bfalse;
    }

    void BindHandler::computeInitialTransform( Actor* _bindedChild ) const
    {
        Bind* bind = _bindedChild->getParentBind();
        ITF_ASSERT_CRASH(bind, "Invalid child, not bind information");
        
        Transform3d initialTransform;
        if(getInitialTransform(m_owner, bind, initialTransform))
        {
            if(bind->m_useParentScale)
                _bindedChild->setWorldInitialScale(m_owner->getWorldInitialScale(), bfalse);

            if(bind->m_useParentFlip)
                _bindedChild->setWorldInitialFlip(m_owner->getWorldInitialFlip(), bfalse);

            _bindedChild->setWorldInitialRot(initialTransform.transformAngle(bind->getInitialAngleOffset()), bfalse);

            Vec3d offset = bind->getInitialPosOffset();
            _bindedChild->setWorldInitialPos(initialTransform.transformPos(offset), bfalse);
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void BindHandler::onEditorMove(bbool _modifyInitialPos)
    {
        const u32 childCount = m_children.size();
        for(u32 iChild = 0; iChild < childCount; ++iChild)
        {
            Actor* child = static_cast<Actor*>(m_children[iChild].getObject());
            ITF_ASSERT_CRASH(child, "A child was deleted but not removed from parent bind list");
            
            Bind* bind = child->getParentBind();
            ITF_ASSERT_CRASH(bind, "Invalid child, not bind information");
                
            if(_modifyInitialPos)
                computeInitialTransform(child);

            updateWorldCoordinates(child, bind);

            child->onEditorMove(_modifyInitialPos);

            m_owner->growAABB3d(child->getAABB3d());
        }
    }

#endif // ITF_SUPPORT_EDITOR

} //namespace ITF
