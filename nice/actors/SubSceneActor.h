#ifndef _ITF_SUBSCENEACTOR_H_
#define _ITF_SUBSCENEACTOR_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/Actor.h"
#endif //_ITF_ACTOR_H_

namespace ITF
{
    class Scene;

#define SubSceneActor_CRC ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)
    class SubSceneActor : public Actor
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(SubSceneActor,Actor,1336151817);
        DECLARE_SERIALIZE()

        SubSceneActor();
        ~SubSceneActor();

        virtual void    onLoaded(HotReloadType _hotReload); // from actor/pickable        
        virtual void    hotReload( ActorHotReload& _reloadInfo, HotReloadType _hotReloadType );
        virtual void    finalizeReload( bbool _wasActiveInScene, HotReloadType _hotReload );
        virtual void    onFinalizeLoad( bbool _hotReload );
        virtual void    onFinalizeLoadDelayed();
        virtual void    onDestroy( bbool _hotReload );
        virtual void    onStartDestroy( bbool _hotReload );
        virtual void    onStartDestroyDelayed();

        virtual void    onSceneActive();
        virtual void    onSceneActiveDelayed();
        virtual void    onSceneInactive();
        virtual void    onSceneInactiveDelayed();

        virtual void    update( f32 _deltaTime );

        void            rebuildAABB();

        void            setLoadedSubScene(Scene* _subScene);
        Scene*          getSubScene()const;
        const Path&     getSubScenePath()const                             { return m_subScenePath; }
        void            setSubScenePath(const Path& _path)                 { m_subScenePath = _path; }

        bbool           isSceneEmbeded()const                           { return m_isInternal;}
        void            setIsSceneEmbeded(bbool _b)                     { m_isInternal = _b; }

        void            deleteSpawnedActors();

        virtual void    onEvent(Event* _event);
        
        bbool           isSinglePiece() const                           { return m_isSinglePiece; }
        void            setSinglePiece(bbool _set)                      { m_isSinglePiece = _set; }
        
        bbool           isSinglePieceByHierarchy() const;
        
        virtual bbool   isZForced() const                               { return m_zLock; }
        void            setZLock(bbool _b);

        void            updateCurrentWorldDataContentFromLocal();

        void            flipSubScene();

        void            setViewMaskForPickable( Pickable* _pickable, bbool _force = bfalse );
        
#ifndef ITF_FINAL
        virtual bbool   validate();
#endif // !ITF_FINAL
        
        void            applySinglePieceToContent(bbool _singlePiece)const;

#ifdef ITF_SUPPORT_EDITOR
        virtual void    onEditorMove(bbool _modifyInitialPos = btrue);
        virtual void    onEditorCreated( Pickable* _original = NULL );  // the actor has just been created in the editor (drag'n'drop from a lua, ctrl+drag clone, or copy-paste clone)        
        void            setDirectPicking(bbool _b)                          { m_directPicking = _b; }
        bbool           getDirectPicking()const                             { return m_directPicking; }
        
        void            setSubScenePathChanged(bbool _b)                    { m_subScenePathChanged = _b; }
        bbool           getSubScenePathChanged()const                       { return m_subScenePathChanged; }

        virtual bbool   hasSelectedChild() const;
#endif // ITF_SUPPORT_EDITOR

    private:

        enum ViewType
        {
            VIEWTYPE_MAIN,
            VIEWTYPE_REMOTE,
            VIEWTYPE_ALL,
            VIEWTYPE_MAINONLY,
            VIEWTYPE_REMOTEONLY,
            VIEWTYPE_REMOTEASMAIN_ONLY,
        };


        virtual void    onAnglePosChanged(f32 _prevAngle, const Vec3d& _prevPos);
        virtual void    onPosChanged( const Vec3d& _prevPos );
        virtual void    onDepthChanged( f32 _prevDepth );
        virtual void    onAngleChanged(f32 _prevAngle);
        virtual void    onScaleChanged(const Vec2d& _prevScale);
        virtual void    onFlipChanged();

        void            offsetSubScene(const Vec3d& _delta);
        void            rotateSubScene(f32 _deltaAngle, Vec3d* _pOrigin);
        void            scaleSubScene( const Vec2d& _scale);
        void            rotateAndOffsetSubScene(f32 _deltaAngle, const Vec3d& _delta);

        void            setViewType( ViewType _viewType ) { m_viewType = _viewType; }

        bbool           m_isInternal;
        bbool           m_isSinglePiece;
        Path            m_subScenePath;
        Scene*          m_subScene;
        ViewType        m_viewType;

#ifdef ITF_SUPPORT_EDITOR
        void            onPrePropertyChange();
        void            onPostPropertyChange();

        ViewType        m_prevViewType;
        bbool           m_subScenePathChanged;
#endif // ITF_SUPPORT_EDITOR

        bbool           m_zLock;
        bbool           m_directPicking;
        
        AABB3d          m_initialAABB3d;
    };

}

#endif
