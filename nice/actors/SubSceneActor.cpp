#include "precompiled_engine.h"

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseObjectFactory.h"
#endif //_ITF_BASEOBJECTFACTORY_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(SubSceneActor)

    BEGIN_SERIALIZATION_CHILD(SubSceneActor)

        SERIALIZE_MEMBER("RELATIVEPATH", m_subScenePath);
        SERIALIZE_MEMBER("EMBED_SCENE", m_isInternal);
        SERIALIZE_MEMBER("IS_SINGLE_PIECE", m_isSinglePiece);
        SERIALIZE_MEMBER("ZFORCED", m_zLock);
        SERIALIZE_MEMBER("DIRECT_PICKING", m_directPicking);

        SERIALIZE_ENUM_BEGIN("viewType",m_viewType);
            SERIALIZE_ENUM_VAR(VIEWTYPE_MAIN);
            SERIALIZE_ENUM_VAR(VIEWTYPE_REMOTE);
            SERIALIZE_ENUM_VAR(VIEWTYPE_ALL);
            SERIALIZE_ENUM_VAR(VIEWTYPE_MAINONLY);
            SERIALIZE_ENUM_VAR(VIEWTYPE_REMOTEONLY);
            SERIALIZE_ENUM_VAR(VIEWTYPE_REMOTEASMAIN_ONLY);
        SERIALIZE_ENUM_END();

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            if(m_isInternal)
            {
                SERIALIZE_OBJECT("SCENE", m_subScene);
            }
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerialize_Editor) // used for undo/redo
            SERIALIZE_OBJECT("parentBind", m_pParentBind_Initial);
            SERIALIZE_MEMBER("USERFRIENDLY", m_userFriendly);
        END_CONDITION_BLOCK()

    END_SERIALIZATION()

    SubSceneActor::SubSceneActor() 
        : m_isInternal(bfalse)
        , m_subScene(NULL)
        , m_viewType(VIEWTYPE_ALL)
#ifdef ITF_SUPPORT_EDITOR
        , m_prevViewType(VIEWTYPE_ALL)
        , m_subScenePathChanged(bfalse)
#endif // ITF_SUPPORT_EDITOR
        , m_isSinglePiece(bfalse)
        , m_zLock(bfalse)
        , m_directPicking(btrue)
        , m_initialAABB3d(AABB3d::createRaw())
    {
    }

    SubSceneActor::~SubSceneActor()
    {
        SF_DEL(m_subScene);
    }

    void SubSceneActor::onDestroy( bbool _hotReload )
    {
        Super::onDestroy(_hotReload);

        if ( m_subScene )
        {
            m_subScene->destroy();
            SF_DEL(m_subScene);
        }
    }

    void SubSceneActor::onFinalizeLoad( bbool _hotReload )
    {
        Super::onFinalizeLoad(_hotReload);
        
#ifndef ITF_FINAL
        validate(); // a subsceneactor does not depends on any resource so validate right now
#endif // ITF_FINAL

        if ( m_subScene && !m_subScene->isFinalizedLoad()) // editor issue
        {
            m_subScene->onFinalizeLoad();
        }

        rebuildAABB();
    }

    void SubSceneActor::onFinalizeLoadDelayed()
    {
        Super::onFinalizeLoad(bfalse);

        if ( m_subScene && !m_subScene->isFinalizedLoad()) // editor issue
        {
            m_subScene->onFinalizeLoadDelayed();
        }
    }

    void SubSceneActor::onStartDestroy( bbool _hotReload )
    {
        Super::onStartDestroy(_hotReload);

        if ( m_subScene )
        {
            m_subScene->onStartDestroy();
        }
    }

    void SubSceneActor::onStartDestroyDelayed()
    {
        Super::onStartDestroy(bfalse);

        if ( m_subScene )
        {
            m_subScene->onStartDestroyDelayed();
        }
    }
    void SubSceneActor::onSceneActive()
    {
        Super::onSceneActive();

        if(m_subScene)
            m_subScene->onSceneActive();

        loadResources();
    }

    void SubSceneActor::onSceneActiveDelayed()
    {
        Super::onSceneActive();

        loadResources();
    }

    void SubSceneActor::onSceneInactive()
        {
        Super::onSceneInactive();

        if(m_subScene)
            m_subScene->onSceneInactive();

        unloadResources();
        }

    void SubSceneActor::onSceneInactiveDelayed()
    {
        Super::onSceneInactive();

        if(m_subScene)
            m_subScene->onSceneInactiveDelayed();

        unloadResources();
    }

    void SubSceneActor::update( f32 _deltaTime )
    {
        Actor::update(_deltaTime);
        
        if(isSinglePiece())
            rebuildAABB();
            
        growAABB3d(m_initialAABB3d);
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SubSceneActor::onLoaded(HotReloadType _hotReload)
    {
        Super::onLoaded(_hotReload);

#ifdef ITF_SUPPORT_EDITOR
        removeTag("actor");
        addTag("SubSceneActor");
#endif

        setUpdateGroup(WorldUpdate::UpdateGroup_Environment);

        Scene* pSubScene = getSubScene();
        if ( pSubScene )
        {
            setLoadedSubScene(pSubScene);
            pSubScene->replaceReinitDataByCurrent();
        }

        rebuildAABB();
    }
    
    void SubSceneActor::hotReload( ActorHotReload& _reloadInfo, HotReloadType _hotReloadType )
    {
        updateWorldCoordinatesFromBoundParent();
        updateWorldCoordinatesForBoundChildren();
        initPause();
    }
    
    void SubSceneActor::finalizeReload( bbool _wasActiveInScene, HotReloadType _hotReload /*= HotReloadType_None*/ )
    {
    }

    void SubSceneActor::setLoadedSubScene(Scene* _subScene)
    {
        ITF_ASSERT(_subScene);

        m_subScenePath = _subScene->getPath();
        m_subScene = _subScene;

        _subScene->setSubSceneActorHolder(this);
            
        if(World* world = getWorld())
        {
            world->addScene(_subScene);
        }

        Scene* ownScene = getScene();
        if( ownScene )
        {
            if ( isOnLoadedDone() )
            {
                if ( !_subScene->isOnLoaded() )
                {
                    _subScene->onLoaded();
                }
            }

            if ( isOnFinalizeLoadedDone() )
            {
                if ( !_subScene->isFinalizedLoad() )
                {
                    _subScene->onFinalizeLoad();
                }
            }
            
            if ( getIsSceneActive() != _subScene->isActive())
            {
                if(getIsSceneActive())
                    _subScene->onSceneActive();
                else
                {
                    _subScene->setContentInactive();
                    _subScene->onSceneInactive();
                }
            }
        }
    }
    
    Scene* SubSceneActor::getSubScene()const
    {
        return m_subScene;
    }

    void SubSceneActor::onPosChanged( const Vec3d& _prevPos )
    {
        Super::onPosChanged(_prevPos);

        const Vec3d deltaPos = getPos() - _prevPos;

        offsetSubScene(deltaPos);
    }

    void SubSceneActor::onDepthChanged( f32 _prevDepth )
    {
        Super::onDepthChanged(_prevDepth);

        f32 deltaZ = getPos().z() - _prevDepth;
        Vec3d deltaPos(0.f,0.f,deltaZ);

        offsetSubScene(deltaPos);
    }

    void SubSceneActor::onAngleChanged( f32 _prevAngle )
    {
        Super::onAngleChanged(_prevAngle);

        const f32 deltaAngle = getAngle() - _prevAngle;

        rotateSubScene(deltaAngle, NULL);
    }

    void SubSceneActor::onAnglePosChanged(f32 _prevAngle, const Vec3d& _prevPos)
    {
        Super::onAnglePosChanged(_prevAngle, _prevPos);
        
        const Vec3d deltaPos = getPos() - _prevPos;
        const f32 deltaAngle = getAngle() - _prevAngle;

        rotateAndOffsetSubScene(deltaAngle, deltaPos);
    }

    void SubSceneActor::onScaleChanged( const Vec2d & _prevScale )
    {
        Super::onScaleChanged(_prevScale);

        Vec2d deltaScale = getScale() - _prevScale;

        scaleSubScene(deltaScale);
    }

    void SubSceneActor::onFlipChanged()
    {
        Super::onFlipChanged();

        flipSubScene();
    }

    void SubSceneActor::rotateAndOffsetSubScene(f32 _deltaAngle, const Vec3d& _deltaPos)
    {
        if (Scene* pScene = getSubScene())
        {
            // Process scene's elements to place them relatively to the new origin
            const PickableList &pickables = pScene->getPickableList();
            const Vec3d rotateCenter = getPos() - _deltaPos;
          
            for(u32 i = 0; i < pickables.size(); ++i)
            {
                Pickable* pickable = pickables[i];
#ifdef ITF_SUPPORT_EDITOR
                if( pickable->getObjectType() == BaseObject::eFrise && ((Frise*)pickable)->hasMetaFriezeOwner() )
                    continue;
#endif // ITF_SUPPORT_EDITOR

                Vec3d rotatedPos = pickable->getPos() - rotateCenter;                
                Vec3d_Rotate(&rotatedPos, _deltaAngle);
                rotatedPos += rotateCenter;

                pickable->setAngleAndPos(pickable->getAngle() + _deltaAngle,rotatedPos + _deltaPos);
            }
        }

    }
    
    void SubSceneActor::offsetSubScene( const Vec3d& _delta)
    {
        //if( _delta.sqrnorm() < MTH_EPSILON ) // epsilon too big
        if(Vec3d::Zero == _delta)
            return;

        if (Scene* pScene = getSubScene())
        {
            // Process scene's elements to place them relatively to the new origin
            const PickableList &pickables = pScene->getPickableList();

            for(u32 i = 0; i < pickables.size(); ++i)
            {
                Pickable* pickable = pickables[i];
#ifdef ITF_SUPPORT_EDITOR
                if( pickable->getObjectType() == BaseObject::eFrise && ((Frise*)pickable)->hasMetaFriezeOwner() )
                    continue;
#endif // ITF_SUPPORT_EDITOR

                    pickable->setPos(pickable->getPos() + _delta);
                }
            }
        }

    void SubSceneActor::rotateSubScene( f32 _deltaAngle, Vec3d* _pOrigin /*= NULL*/ )
    {
        //if(f32_Abs(_deltaAngle) < MTH_EPSILON) // epsilon too big
        if(0.0f == _deltaAngle)
            return;

        if(Scene* pScene = getSubScene())
        {
            Vec3d pos = getPos();
            if(!_pOrigin)
            {
                _pOrigin = &pos;
            }

            const PickableList& content = pScene->getPickableList();
            for(u32 i = 0; i < content.size(); ++i)
            {
                Pickable* pickable = content[i];
#ifdef ITF_SUPPORT_EDITOR
                if( pickable->getObjectType() == BaseObject::eFrise && ((Frise*)pickable)->hasMetaFriezeOwner() )
                    continue;
#endif // ITF_SUPPORT_EDITOR

                const Vec3d& pos3D = pickable->getPos();

                Vec3d rotatedPos = pos3D - *_pOrigin;                
                Vec3d_Rotate(&rotatedPos, _deltaAngle);
                rotatedPos += *_pOrigin;

                pickable->setAngleAndPos(pickable->getAngle() + _deltaAngle, rotatedPos);
                }
            }
        }

    void SubSceneActor::scaleSubScene( const Vec2d& _deltaScale )
    {
        if(Vec2d::Zero == _deltaScale)
            return;

        if(Scene* scene = getSubScene())
        {
            const Vec2d prevScale = getScale() - _deltaScale;
            
            static const f32 minCoefToScaleBackWithoutTooMuchPrecisionLoss = 0.00001f; // assuming scale it never < 0.0f
            Vec2d coefScale = Vec2d(minCoefToScaleBackWithoutTooMuchPrecisionLoss, minCoefToScaleBackWithoutTooMuchPrecisionLoss);
            if(prevScale.x() != 0.0f)
                coefScale.x() = getScale().x() / prevScale.x();
            if(prevScale.y() != 0.0f)
                coefScale.y() = getScale().y() / prevScale.y();

            const PickableList& content = scene->getPickableList();
            for(u32 i = 0; i < content.size(); ++i)
            {
                Pickable* pickable = content[i];
#ifdef ITF_SUPPORT_EDITOR
                if( pickable->getObjectType() == BaseObject::eFrise && ((Frise*)pickable)->hasMetaFriezeOwner() )
                    continue;
#endif // ITF_SUPPORT_EDITOR

                if( pickable->getObjectType() == BaseObject::eActor )
                    if( Bind* bind = ((Actor*)pickable)->getParentBind())
                        if ( bind->m_useParentScale )
                            continue;

                const Vec2d myScale = pickable->getScale() * coefScale;
                pickable->setScale(myScale);

                Vec2d pickPos = pickable->get2DPos();
                transformWorldPosToLocal(pickPos);
                pickPos *= coefScale;
                transformLocalPosToWorld(pickPos);

                pickable->set2DPos(pickPos);
            }
        }
    }

    void SubSceneActor::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        // relay events to children
        if ( EventTeleport* pTeleportEvent = DYNAMIC_CAST(_event,EventTeleport) )
        {
            EventTeleport childTeleport = *pTeleportEvent;

            if ( m_subScene )
            {
                const PickableList& children = m_subScene->getPickableList();
                const u32 childrenCount = children.size();

                for (u32 i = 0; i < childrenCount; i++)
                {
                    Pickable* p = children[i];

                    if ( Actor* actor = DYNAMIC_CAST(p,Actor) )
                    {
                        Vec3d pos = actor->getBoundWorldInitialPos(btrue);
                        f32 angle = actor->getBoundWorldInitialAngle(btrue);

                        childTeleport.setPos(pos);
                        childTeleport.setAngle(angle);
                    }
                    else
                    {
                        Vec2d pickablePos = p->getLocalInitialPos().truncateTo2D();
                        f32 pickableZ = p->getLocalInitialZ();
                        f32 pickableRot = p->getLocalInitialRot();

                        if( getIsFlipped() )
                        {
                            pickablePos.x() = -pickablePos.x();
                            pickableRot = -pickableRot;
                        }

                        pickablePos.x() *= getScale().x();
                        pickablePos.y() *= getScale().y();

                        pickablePos = pickablePos.Rotate(getAngle()) + getPos().truncateTo2D();
                        pickableZ += getPos().z();
                        pickableRot += getAngle();

                        childTeleport.setPos(pickablePos.to3d(pickableZ));
                        childTeleport.setAngle(pickableRot);
                    }

                    p->onEvent(&childTeleport);
                }
            }
        }
        else
        {
            if ( m_subScene )
            {
                const PickableList& children = m_subScene->getPickableList();
                const u32 childrenCount = children.size();

                for (u32 i = 0; i < childrenCount; i++)
                {
                    children[i]->onEvent(_event);
                }
            }
        }
    }

    void SubSceneActor::setZLock( bbool _b )
    {
        m_zLock = _b;
    }

    void SubSceneActor::updateCurrentWorldDataContentFromLocal()
    {
        if(m_subScene)
        {
            const PickableList& content = m_subScene->getPickableList();
            for(u32 iObj = 0; iObj < content.size(); ++iObj)
            {
                Pickable* pickable = content[iObj];

                pickable->setPos(pickable->getWorldInitialPos());
                pickable->setScale(pickable->getWorldInitialScale());
                pickable->setAngle(pickable->getWorldInitialRot());
                pickable->setIsFlipped(pickable->getWorldInitialFlip());
            }
        }
    }

    void SubSceneActor::deleteSpawnedActors()
    {
        if(m_subScene)
        {
            m_subScene->deleteSpawnedActors();
        }
    }

    void SubSceneActor::flipSubScene()
    {
        if(m_subScene)
        {
            const PickableList& pickables = m_subScene->getPickableList();
            for(u32 iObj = 0; iObj < pickables.size(); ++iObj )
            {
                Pickable* pickable = pickables[iObj];
#ifdef ITF_SUPPORT_EDITOR
                if( pickable->getObjectType() == BaseObject::eFrise && ((Frise*)pickable)->hasMetaFriezeOwner() )
                    continue;
#endif // ITF_SUPPORT_EDITOR

                Vec3d newLocalPos = pickable->getLocalPos();
                newLocalPos.x() = -newLocalPos.x();
                pickable->setLocalPos(newLocalPos);
                pickable->setLocalAngle(-pickable->getLocalAngle());
                pickable->setIsFlipped(!pickable->getIsFlipped());
            }
        }
    }

    void SubSceneActor::rebuildAABB()
    {
        AABB3d absAABB3d(getPos(), getPos());
        
        if(m_subScene)
    {
            const u32 uPickableCount = m_subScene->getPickableList().size();
            for(u32 i = 0; i < uPickableCount; ++i)
            {
                const Pickable* pickable = m_subScene->getPickableList()[i];
                
                absAABB3d.grow(pickable->getAABB3d());
            }
        }

            setAABB3d(absAABB3d);
            m_initialAABB3d = getAABB3d();
        }

    bbool SubSceneActor::isSinglePieceByHierarchy() const
    {
        if(Scene* scene = getScene())
        {
            SubSceneActor* holder = scene->getSubSceneActorHolder();

            while(holder)
            {
                if(holder->isSinglePiece())
                    return btrue;

                holder = holder->getScene()->getSubSceneActorHolder();
            }
        }
        
        return bfalse;
    }

    void SubSceneActor::setViewMaskForPickable( Pickable* _pickable, bbool _force )
    {
        if ( DYNAMIC_CAST(_pickable,SubSceneActor) != NULL )
        {
            return;
        }

        WorldUpdateElement* element = _pickable->getWorldUpdateElement();

        if (_force || element->getViewMask() == View::MASKID_ALL)
        {
            View::MaskIds maskToSet = View::MASKID_ALL;

            switch(m_viewType)
            {
                case VIEWTYPE_MAIN:     maskToSet = View::MASKID_MAIN; break;
                case VIEWTYPE_REMOTE:   maskToSet = View::MASKID_REMOTE; break;
                case VIEWTYPE_MAINONLY: maskToSet = View::MASKID_MAINONLY; break;
                case VIEWTYPE_REMOTEONLY: maskToSet = View::MASKID_REMOTEONLY; break;
                case VIEWTYPE_REMOTEASMAIN_ONLY: maskToSet = View::MASKID_REMOTE_ASMAIN; break;                    
            }
            element->setViewMask(static_cast<u32>(maskToSet));
        }
    }

#ifndef ITF_FINAL
    bbool SubSceneActor::validate()
    {
        bbool validated = Super::validate();
        
        if(validated)
        {
            if(getSubScene() == NULL)
            {
                String8 errMsg;
                errMsg.setTextFormat("Subscene not loaded: %s", getSubScenePath().toString8().cStr());
                setDataError(errMsg);
                
                validated = bfalse;
            }
        }
            
        return validated;
    }
#endif // !ITF_FINAL

    void SubSceneActor::applySinglePieceToContent(bbool _singlePiece) const
    {
        if(Scene* subscene = getSubScene())
        {
            const PickableList& content = subscene->getPickableList();
            for(u32 i = 0; i < content.size(); ++i)
            {
                Pickable* pickable = content[i];
                 
                WORLD_MANAGER->unregisterForUpdate(pickable);
                WORLD_MANAGER->registerForUpdate(pickable, _singlePiece);
            }
            
            const ITF_VECTOR <SubSceneActor*>& ssaList = subscene->getSubSceneActors();
            for(u32 i = 0; i < ssaList.size(); ++i)
            {
                SubSceneActor* pSSA = ssaList[i];
                
                if(pSSA->isSinglePiece() && !_singlePiece)
                    continue;
                    
                pSSA->applySinglePieceToContent(_singlePiece);
            }
        }
    }


#ifdef ITF_SUPPORT_EDITOR
    void SubSceneActor::onEditorMove( bbool _modifyInitialPos /*= btrue*/ )
    {
        Super::onEditorMove(_modifyInitialPos);
        
        if(m_subScene)
        {
            const PickableList& pickables = m_subScene->getPickableList();
            for(u32 i = 0; i < pickables.size(); ++i)
                pickables[i]->onEditorMove(_modifyInitialPos);
        }
        rebuildAABB();
    }

    void SubSceneActor::onEditorCreated( Pickable* _original /*= NULL */ )
    {
        Super::onEditorCreated(_original);
        
        rebuildAABB();
    }

    void SubSceneActor::onPrePropertyChange()
    {
        Super::onPrePropertyChange();

        m_prevViewType = m_viewType;
    }

    void SubSceneActor::onPostPropertyChange()
    {
        if ( m_prevViewType != m_viewType && m_subScene && getIsSceneActive() )
        {
            ITF_VECTOR <Scene*> scenes;

            scenes.push_back(m_subScene);

            for ( u32 i = 0; i < scenes.size(); i++ )
            {
                Scene* currentScene = scenes[i];
                const PickableList& pickables = currentScene->getPickableList();
                u32 numPickables = pickables.size();

                for ( u32 pickIndex = 0; pickIndex < numPickables; pickIndex++ )
                {
                    setViewMaskForPickable(pickables[pickIndex], btrue);
                }

                const ITF_VECTOR <SubSceneActor*>& subScenes = currentScene->getSubSceneActors();
                u32 numSubScenes = subScenes.size();

                for ( u32 subSceneIndex = 0; subSceneIndex < numSubScenes; subSceneIndex++ )
                {
                    SubSceneActor* subScene = subScenes[subSceneIndex];

                    subScene->setViewType(m_viewType);

                    Scene* scene = subScene->getSubScene();

                    if ( scene )
                    {
                        scenes.push_back(scene);
                    }
                }
            }
        }

        Super::onPostPropertyChange();
    }

    bbool SubSceneActor::hasSelectedChild() const
    {
        if (isSelected())
            return btrue;
        if (m_subScene != NULL)
        {
            const PickableList &children = m_subScene->getPickableList();
            for (PickableList::const_iterator childIt = children.begin(); childIt != children.end(); ++childIt)
            {
                if ((*childIt)->hasSelectedChild())
                    return btrue;
            }
        }
        return bfalse;
    }

#endif // ITF_SUPPORT_EDITOR

}
