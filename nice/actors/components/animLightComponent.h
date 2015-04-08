
#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#define _ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_ANIMCOMMONINTERFACE_H_
#include "engine/actors/components/AnimCommonInterface.h"
#endif // _ITF_ANIMCOMMONINTERFACE_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef _ITF_SUBANIMFRAMEINFO_H_
#include "engine/animation/SubAnimFrameInfo.h"
#endif //_ITF_SUBANIMFRAMEINFO_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_GRIDFLUID_H_
#include "engine/display/GridFluid/GridFluid.h"
#endif // _ITF_GRIDFLUID_H_

#define MAX_EVENTPROCESSCONTAINER_EVENTS    10

namespace ITF
{
    class AnimScene;
    class AnimBoneDyn;
    class AnimMeshScene;

    static const f32 g_pixelBy2DUnit = 64.0f;

    struct BoneMapping
    {
        DECLARE_SERIALIZE()
        StringID m_inputBone;
        StringID m_outputBone;
    };

    struct AnimLightFrameInfo
    {
        DECLARE_SERIALIZE()

        AnimLightFrameInfo() : m_weight(1.f), m_usePatches(btrue) {}
        AnimLightFrameInfo( const SubAnimFrameInfo& _subAnimFrameInfo, f32 _weight, bbool _usePatches ) : m_subAnimFrameInfo(_subAnimFrameInfo), m_weight(_weight), m_usePatches(_usePatches) {}

        SubAnimFrameInfo        m_subAnimFrameInfo;
        f32                     m_weight;
        bbool                   m_usePatches;
    };

    typedef ITF_VECTOR<AnimLightFrameInfo> AnimLightFrameInfoArray;

    #define AnimLightComponent_CRC ITF_GET_STRINGID_CRC(AnimLightComponent,2800021434)
    class AnimLightComponent : public GraphicComponent, public IAnimCommonInterface
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimLightComponent,GraphicComponent,2800021434);

    public:
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        AnimLightComponent();
        ~AnimLightComponent();

        ITF_INLINE const class AnimLightComponent_Template* getTemplate() const;

        virtual bbool                   needsUpdate() const { return btrue; }
        ITF_INLINE virtual bbool        needsDraw() const;
        ITF_INLINE virtual bbool        needsDraw2D() const;

        virtual void                    Update( f32 _deltaTime );
        void                            forceSynchronousFrame();
        virtual void                    batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void                    batchPrimitives2D( const ITF_VECTOR <class View*>& _views );
        virtual void                    onEvent( Event* _event );
   
		virtual void					SerializePersistent( CSerializerObject* serializer, u32 flags );
		virtual void					SerializeAnim( CSerializerObject* serializer, const char* name, StringID &animName, u32 flags );

        bbool                           getInactive() const { return f32_Abs(m_animPlayRate) < MTH_EPSILON; }
        void				            setInactive(const bbool _b)				
        { 
            if (_b) 
                m_animPlayRate = 0.f;
            else
                m_animPlayRate = 1.f;
        }

        bbool				getInvisible()							{ return m_Invisible;	}
        void				setInvisible(const bbool _b)			{ m_Invisible = _b;	}

#ifdef ITF_SUPPORT_EDITOR
        /////////////////////////////////
        //  Editors
        /////////////////////////////////
        virtual void    drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void    onEditorMove(bbool _modifyInitialPos = btrue);
        virtual void    onPrePropertyChange();
        virtual void    onPostPropertyChange();
        StringID        m_prevDefaultAnim;

        class PlayAnim_evt *validateForceEvt;
        i32                 validateForceFrame;
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE f32                  getSyncOffset() const { return m_syncOffset; }
        ITF_INLINE void                 setSyncOffset( const f32 _value ) { m_syncOffset = _value; m_subAnimSet.setGlobalSyncOffset(m_syncOffset); }

        ITF_INLINE f32                  getStartOffset() const { return m_startOffset; }
        ITF_INLINE void                 setStartOffset( const f32 _value ) { m_startOffset = _value; }

        virtual void                    onUnloadResources();
        bbool                           isLoaded() const;
        virtual void                    onResourceLoaded();

        ITF_INLINE AnimMeshScene*       getAnimMeshScene() const { return m_AnimMeshScene; }

        void                            setPlayRate( f32 _rate ) { m_animPlayRate = _rate; }

        virtual void                    onActorLoaded(Pickable::HotReloadType /*_hotReload*/) ;
        virtual void                    onActorClearComponents();
        virtual void                    onCheckpointLoaded();

        i32                             getBoneIndex (const StringID& _boneName) const;
        bbool                           getBonePos (u32 _boneIndex, Vec2d& _dest, bbool _getBoneEnd = bfalse) const;
        bbool                           getBonePos (u32 _boneIndex, Vec3d& _dest, bbool _getBoneEnd = bfalse) const;
        bbool                           getBoneAngle(u32 _boneIndex, f32& _angle, bbool _useFlip = true) const;
        bbool                           getBoneOrientation (u32 _boneIndex, Vec2d& _orientation) const;
        bbool                           getBoneScale (u32 boneIndex, Vec2d& _dest) const;
        bbool                           getBoneAlpha(u32 boneIndex, f32& _alpha) const;
        bbool                           getBoneLenght(u32 _boneIndex, f32& _lenght) const;
        AnimBoneDyn*                    getBoneForUpdate(u32 boneIndex, bbool _updateLocal);
        u32                             getBoneCount() const;

        bbool                           setBoneScale (u32 boneIndex, Vec2d& _scale);
		bbool                           setBoneAlpha(u32 boneIndex, f32& _alpha);
        
        AnimPolyline *                  getCurrentPolyline(const StringID & _polyId, bbool _useWorking = bfalse) const;
        Vec2d *                         getPolylinePoint(const StringID & _polyId);
        Vec2d *                         getCurrentPolylinePointBuffer(const StringID & _polyId, u32 * _size = NULL, AnimPolyline::PolylineType * _type = NULL);
        Vec2d *                         getCurrentFirstPolylinePoint(const StringID & _polyPointName, class AnimPolyline ** _polyline = NULL, u32 * _index = NULL);

        virtual AABB                    getVisualAABB() const;
        void                            fillAnimsAABBLocal(AABB &animAABB) const;
        AABB                            getAnimsAABB() const;

        ITF_INLINE bbool                isDrawEnabled() const { return m_drawEnabled;}
        ITF_INLINE void                 enableDraw(bbool _enable) { m_drawEnabled = _enable;}
        ITF_INLINE bbool                getDraw2D() const;
        virtual void                    updateAABB();
        AABB                            getPatchAABB() const;
        virtual void                    updateAnimTime( f32 _dt );
        virtual void                    updatePatches();
        virtual bool                    updateData(f32 _deltaTime);
        virtual bool                    postUpdateData();
        void                            updateEvents();

        bbool                           isAnimationsProcedural();
        virtual bbool					isInTransition() const { return bfalse; }	// use in animated component
        bbool                           isProcedural() const { return m_isProcedural; }
        ITF_INLINE virtual bbool        isSynchronous() const;
        void                            setIsProcedural( bbool _val ) { m_isProcedural = _val; }
        void                            setForceSynchro( bbool _val ) { m_forceSynchro = _val ; }
        bbool                           getForceSynchro() const { return m_forceSynchro; }

        bbool                           isNotifyingAnimUpdate() const;

        void                            setUpdateAnimTime( bbool _val ) { m_updateAnimTime = _val; }
        bbool                           getUpdateAnimTime() const { return m_updateAnimTime; }
        void                            setNeedRefreshTextureBank() { m_needRefreshTextureBank = btrue; }
        //////// SubAnimSet indirections
        virtual bbool                   setAnim(const StringID & _subAnim, u32 _blendFrames = U32_INVALID, bbool _forceResetTree = bfalse, u32 _priority = 0);
        virtual StringID				getLastAnimSet() const { return m_lastAnim; }
        virtual void                    setFirstAnim();
        u32                             getSubAnimCount() const                                              { return m_subAnimSet.getSubAnimCount(); }
        SubAnim*                        getSubAnim(int _index) const                                        { return m_subAnimSet.getSubAnim(_index); }
        SubAnim*                        getSubAnim(const StringID &_subAnimName) const                      { return m_subAnimSet.getSubAnim(_subAnimName); }

        void                            setCurFrame(f32 _frame, AnimEventContainer* _eventList);
        void                            setCurFrameFromStart(f32 _frame, AnimEventContainer* _evLst);
        virtual void                    resetCurTime(bbool _sendChangeEvent = bfalse);
        void                            setProceduralCursor(f32 _cursor);
        void                            setProceduralCursorForSubAnim(u32 animIndex, f32 _cursor);
        f32                             getCurTime( u32 _index = 0 ) const;
        void                            setCurTime( f32 _time, u32 _index = 0 );

        virtual bbool                   isAnimToPlayFinished() const { return !m_playAnimName.isValid() && isSubAnimFinished(); }
        bbool                           isSubAnimFinished() const;
        void                            setAnimLoop(bbool _loop);
        void                            computeVisiblePrediction();
        ITF_INLINE bbool                isVisiblePrediction() const { return m_isVisiblePrediction  && isDrawEnabled(); }
        bbool                           isBoneDataOk(i32 _boneIndex) const;
        void                            setDisableVisiblePrediction( bbool _val ) { m_disableVisiblePrediction = _val; }
        ITF_INLINE bbool                getDisableVisiblePrediction() const;

        u32                             getNumPlayingSubAnims() const                                        { return m_currentFrameSubAnims.size(); }
        virtual const SubAnim*			getCurSubAnim( u32 _index ) const                                    { return m_currentFrameSubAnims[_index].m_subAnimFrameInfo.getSubAnim(); }
        i32                             getCurSubAnimIndex( StringID _friendlyName ) const;

        void                            setColor(Color _color)  { m_currentColor = _color;}
        ITF_INLINE void                 resetColor();
        ITF_INLINE  const Color&        getColor() const {return m_currentColor;}
        ITF_INLINE  const Color&        getDestColor() const {return m_colorDst;}
        void                            processColor( f32 _dt );

        SubAnimSet *                    getSubAnimSet()                                                     { return &m_subAnimSet; }

        ITF_INLINE   i32                getVertexFormat() {return m_vertexFormat;}

        Vec3d                           getOffsettedPos() const;
        f32                             getOffsettedAngle() const;
        Vec2d                           getScale()const;

        virtual bbool                    getIsLooped() const { return m_isLooped; }
        bbool                            getUseRootRotation() const { return m_useRootRotation; }
        bbool                            getDefaultAnimFlip() const { return m_animDefaultFlip; }
        const ITF_VECTOR<AnimFrameInfo>& getAnimFrameInfo() const { return m_animFrameInfo; }

        virtual u32                     getNumAnimsPlaying() const { return getNumPlayingSubAnims(); }
        virtual const StringID&         getAnimsPlayingName(u32 _index) {return getCurSubAnim(_index)->getFriendlyName(); }
        virtual u32                     getNumAnims() const { return getSubAnimCount(); }
        virtual const StringID&         getAnimsName(u32 _index) {return getSubAnim(_index)->getFriendlyName(); }


        void							setPatchChange(const StringID & _source, const StringID & _dest);
        void							removePatchChange(const StringID & _source);
        void							clearPatchChange();
        void							updatePatchChange();


        void                            dbgGetCurAnims( u32 _index, String8 & _text) const;

        void                            setForceDirectUpdate(bbool _val, bbool _semiForced = bfalse) { if (_val) m_forceDirectUpdate++; else m_forceDirectUpdate--; ITF_ASSERT(m_forceDirectUpdate >= 0); m_semiForcedDirectUpdate = _semiForced; }
        bbool                           isForceDirectUpdate() const { return m_forceDirectUpdate > 0; }
        void                            resetSubAnimsToPlay();
        i32                             addSubAnimToPlay( const StringID & _subAnim, f32 _weight, bbool _usePatches);
        bbool                           removeSubAnimToPlay(const StringID & _subAnim);
        void                            commitSubAnimsToPlay();
        void                            setFrameForSubAnim(u32 _subAnimIndex, f32 _time, AnimEventContainer* _eventList = NULL);

        void                            processBackupRootInformation();
        u32                             getFirstRootedAnimationIndex();
        bbool                           canGetRootDelta(u32 _index = 0) const;
        Vec2d                           getRootDelta(u32 _index = 0, bbool * _ptr = NULL);
        Vec2d                           getPreviousRootDelta(f32 _dt, bbool * _ptr = NULL );
        Vec2d                           getRootDeltaCumulated( bbool _useFloatingFrame, bbool _useOriginAsStart = bfalse, u32 _animIndex = 0, u32 * _animID = NULL, f32 * _animPos = NULL, f32 * _time = NULL);

        ITF_INLINE bbool                getCanonizeTransitions() const;
        ITF_INLINE const StringID&      getDefaultAnimation() const;
        ITF_INLINE const Color&         getDefaultColor() const;

        ITF_INLINE virtual bbool        getUseBase() const;

        const StringID&					getBoneFromGeneric(const StringID& _bone) const {ITF_MAP<StringID,StringID>::const_iterator it = m_boneMap.find(_bone); return it != m_boneMap.end() ? it->second : _bone;}

        void                            forceUpdateFlip();
        ITF_INLINE const Vec2d& getScaleMultiplier() const { return m_scaleMultiplier; }
        ITF_INLINE void setScaleMultiplier(f32 _val) { m_scaleMultiplier.x() = _val; m_scaleMultiplier.y() = _val; }
        ITF_INLINE void setScaleMultiplier(const Vec2d& _val) { m_scaleMultiplier = _val; }

        ITF_INLINE void                 setRank2D(u32 rank) { m_rank2D = rank; }
        ITF_INLINE u32                  getRank2D() const { return m_rank2D; }

        ITF_INLINE const Vec2d&         getInstancePositionOffset() const {return m_instancePositionOffset;} //instance position offset is an offset which is added to the one in getTemplate()
        ITF_INLINE void                 setInstancePositionOffset(const Vec2d &_offset) {m_instancePositionOffset = _offset;}

        Vec2d                           getOffset() const;
        ITF_INLINE void                 clearSubSkeleton() { m_subSkeletonIdx = -1; }
        bbool                           setSubSkeleton(StringID _subSkeletonName);

        // IAnimCommonInterface
        virtual void                    IAnimsetAnim( const StringID& _id );
        virtual bbool                   IAnimisAnimFinished() const;
        virtual bbool                   IAnimisAnimLooped() const;
        virtual u32                     IAnimgetNumAnimsPlaying() const;
        virtual void                    IAnimdbgGetCurAnim( u32 _index, String8& _out ) const;

        ITF_INLINE bbool                getIsBoneModification() const { return m_boneModification; }
        ITF_INLINE void                 setIsBoneModification(bbool _val ) { m_boneModification = _val; }
        ITF_INLINE bbool                getIsBoneModificationLocal() const { return m_boneModificationLocal; }
        ITF_INLINE void                 setIsBoneModificationLocal(bbool _val ) { m_boneModificationLocal = _val; }
        void                            startJob(bbool _updateBone, bbool _updatePatch);

        AnimEventContainer &            getFrameEvents() { return m_frameEvents; }
        void                            setCurrentBlendFrames( u32 _blendFrames );

        void							setIgnoreEvents(const bool _ignoreEvent ) {m_ignoreEvents =  _ignoreEvent;}
        void                            setExternalScissor(f32 _minX, f32 _maxX, f32 _minY, f32 _maxY );


        GFX_GridFluidFilter             getFluidEmitterFilter() const { return m_fluidEmitterFilter; }
        void                            setFluidEmitterFilter(GFX_GridFluidFilter val);

		ITF_INLINE bbool                getEmitFluid() const { return m_emitFluid; }
		ITF_INLINE void                 setEmitFluid(ITF::bbool val) { m_emitFluid = val; }

		ITF_INLINE void                 setBasicRender(ITF::bbool val) { m_basicRender = val; }

        void                            updateMeshMatrix();

    protected:        
        void                            transformLocalAABB(AABB& _aabb) const;
        bbool                           skipBoneAndPatchUpdate();
        void                            deleteAnimMesh();
        void                            createAnimMeshScene();
        i32                             addSubAnimToPlay( const SubAnimFrameInfo& _subAnim, f32 _weight, bbool _usePatches );
        void                            resetSubAnimFrameInfoFlags();
        void                            updateSubAnimFrameInfoFlag(const SubAnimFrameInfo& _subAnimFrame);
        bbool                           getCurrentSubAnimDefaultRight(bbool _forceUseTrackInfo = bfalse);

        void                            addMeshSceneForRenderPass(const ITF_VECTOR <class View*>& _views, u32 _renderPass, f32 _depth);
        void                            addMeshSceneForRenderPass2d(const ITF_VECTOR <class View*>& _views, u32 _renderPass, f32 _depth);

        virtual void                    processAnim();
        virtual void                    processEventPlayAnim( class EventPlayAnim* _playAnimEvent );
        void                            processEventReset( class EventReset* _resetEvent );
        void                            processEventShow( class EventShow* _eventShow );

        void                            commitPlayAnim();

        bbool                           isLookingRight() const;

        AnimEventContainer              m_frameEvents;
        AnimLightFrameInfoArray         m_currentFrameSubAnims;
        ITF_VECTOR<AnimFrameInfo>       m_animFrameInfo;
        bbool                           m_samePrevAnimFrameInfo;
        SubAnimSet                      m_subAnimSet;
        Path                            m_materialShaderOverridePath;
        int                             m_vertexFormat;
        AnimMeshScene *                 m_AnimMeshScene;
        ITF_VECTOR<AnimMeshScenePass*>  m_AnimMeshSceneAllPass;
        ResourceID                      m_animID;
        f32                             m_blendDt;
        AABB                            m_visualAABB;
        Vec2d                           m_instancePositionOffset; //This offset is added to template offset
        StringID                        m_playAnimName;
        StringID						m_lastAnim;
        u32                             m_playAnimNumFrames;
        bbool							mb_forceResetAnim;
        u32                             m_lastAnimPrio;

        bbool                           m_previousLookRight;
        u32                             m_rootIndex;
        Color                           m_currentColor;
        Color                           m_colorSrc;
        Color                           m_colorDst;
        f32                             m_colorBlendTotal;
        f32                             m_colorBlendTime;
        f32                             m_currentBlendFrames;
        f32                             m_animPlayRate;

        u32                             m_loadingOK : 1,
                                        m_ignoreEvents : 1,
                                        m_drawEnabled : 1,
                                        m_isProcedural : 1,
                                        m_updateAnimTime : 1,
                                        m_isLooped : 1,
                                        m_useRootRotation : 1,
                                        m_animDefaultFlip : 1,
                                        m_useScissor : 1,
                                        m_semiForcedDirectUpdate : 1,
                                        m_isVisiblePrediction : 1,
                                        m_disableVisiblePrediction : 1, 
                                        m_forceSynchro : 1,
                                        m_boneModification : 1,
                                        m_boneModificationLocal : 1,
                                        m_polySync : 1,
                                        m_zOffsetActive : 1,
                                        m_needRefreshTextureBank : 1;

        f32                             m_syncOffset;   // instance-serialized
        f32                             m_startOffset;   // instance-serialized
        bbool                           m_useZOffset;

        class JobUpdateVisual*          m_jobUpdateVisual;

        i32                             m_forceDirectUpdate; // force the use of m_currentFrameSubAnims in animLight

        ITF_VECTOR<BoneMapping>         m_boneList;
        ITF_MAP<StringID,StringID>      m_boneMap;
        Vec2d                           m_scaleMultiplier;
        ITF_VECTOR<Vec2d>               m_globalPolylinePoint;

        VectorAnim<StringID>            m_previusPolylineList;
        VectorAnim<StringID>            m_currentPolylineList;

        u32                             m_rank2D;
        String8                         m_errorStr;

        StringID                        m_subSkeleton;
        i32                             m_subSkeletonIdx;

        StringID                        m_defaultAnim;

        bbool							m_patchChangeModified;
        ITF_MAP<StringID, StringID>		m_patchChange;

        bbool							m_emitFluid;
		bbool							m_attractFluid;
		bbool							m_basicRender;
        GFX_GridFluidFilter				m_fluidEmitterFilter;
    private:
        bbool                           fillAnimInfo();
        void						    fillBoneMap();
        bbool						    animToQuad(View* _refView);
        void                            updateScissor();

        bbool						    m_Invisible;

        previousRootInfo                m_previousRootInfo;
                                                
    };

    ITF_INLINE void AnimLightComponent::updatePatches()
    {
        m_AnimMeshScene->m_pAnimPatches.processPatchs(m_AnimMeshScene);
        m_AnimMeshScene->m_pAnimDraw.computeShaderBuffer(m_AnimMeshScene);
    }

    #define AnimLightComponent_Template_CRC ITF_GET_STRINGID_CRC(AnimLightComponent_Template,2740286289)
    class AnimLightComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimLightComponent_Template,GraphicComponent_Template,2740286289);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(AnimLightComponent);

    public:

        AnimLightComponent_Template();
        ~AnimLightComponent_Template() {}

        virtual bbool                   onTemplateLoaded( bbool _hotReload );
        virtual void                    onTemplateDelete( bbool _hotReload );
        

        
        bbool                           getUseBase() const { return m_useBase; }
        bbool                           getSmoothAnim() const { return m_smoothAnim; }
        bbool                           getCanonizeTransitions() const { return m_canonizeTransitions; }
        u32                             getDefaultBlendFrames() const { return m_defaultBlendFrames; }
        bbool                           getDraw2d() const { return m_draw2D != 0; }
        bbool                           getDraw2dSubScreen() const { return (m_draw2D == 2) ? btrue : bfalse; }
        const StringID&                 getVisualAABBanimID() const { return m_visualAABBanimID; }
        const SubAnimSet_Template*      getSubAnimTemplate() const { return &m_subAnimSet; }
        const StringID&                 getDefaultAnim() const { return m_defaultAnimation; }
        const StringID&                 getSubSkeletonID() const { return m_subSkeleton; }
        const Color&                    getDefaultColor() const { return m_defaultColor; }
        bbool                           getStartFlip() const { return m_startFlip; }
        f32                             getBackZOffset() const { return m_backZOffset; }
        f32                             getFrontZOffset() const { return m_frontZOffset; }
        f32                             getZAmplify() const { return m_zAmplify; }
        f32                             getZOrderExtract() const { return m_zOrderExtract; }
        bbool                           getIsSynchronous() const { return m_isSynchronous; }
        bbool                           getRestoreStateAtCheckpoint() const { return m_restoreStateAtCheckpoint; }
        bbool                           getDisableVisiblePrediction() const { return m_disableVisiblePrediction; }
        bbool                           getNotifyAnimUpdate() const { return m_notifyAnimUpdate; }
        Vec2d                           getScale() const { return m_scale; }
        const ITF_VECTOR<BoneMapping>&	getBoneList() const { return m_boneList; }
        const bbool	                    getUseZOffset() const { return m_useZOffset; }
        bbool                           allowRenderInTexture() const 
		{
#ifdef ITF_GLES2
			return bfalse;
#else
			return m_allowRenderInTexture; 
#endif
		}
        u32                             renderInTectureSizeX() const { return m_renderInTectureSizeX; }
        u32                             renderInTectureSizeY() const { return m_renderInTectureSizeY; }

    protected:
        bbool                           m_useBase;
        bbool                           m_smoothAnim;
        bbool                           m_canonizeTransitions;
        bbool                           m_useZOffset;
        u32                             m_defaultBlendFrames;
        u32                             m_draw2D;
        StringID                        m_visualAABBanimID;
        SubAnimSet_Template             m_subAnimSet;
        StringID                        m_defaultAnimation;
        StringID                        m_subSkeleton;
        Color                           m_defaultColor;
        bbool                           m_startFlip;
        f32                             m_backZOffset;
        f32                             m_frontZOffset;
        f32                             m_zOrderExtract;
        f32                             m_zAmplify;
        bbool                           m_isSynchronous;
        bbool                           m_restoreStateAtCheckpoint;
        bbool                           m_disableVisiblePrediction;
        bbool                           m_notifyAnimUpdate;
        
        bbool                           m_allowRenderInTexture;
        u32                             m_renderInTectureSizeX;
        u32                             m_renderInTectureSizeY;

        Vec2d                           m_scale;

        ITF_VECTOR<BoneMapping>			m_boneList;
    };



    ITF_INLINE const class AnimLightComponent_Template* AnimLightComponent::getTemplate() const { return static_cast<const AnimLightComponent_Template*>(m_template); }
    ITF_INLINE bbool AnimLightComponent::needsDraw() const { return getTemplate() && !getTemplate()->getDraw2d(); }
    ITF_INLINE bbool AnimLightComponent::needsDraw2D() const{ return getTemplate() && getTemplate()->getDraw2d(); }
    ITF_INLINE bbool AnimLightComponent::isSynchronous() const { return getTemplate()->getIsSynchronous() || isProcedural() || m_frameEvents.size() || m_forceSynchro || m_polySync; }
    ITF_INLINE bbool AnimLightComponent::isNotifyingAnimUpdate() const  { return isSynchronous() && getTemplate()->getNotifyAnimUpdate(); }
    ITF_INLINE void AnimLightComponent::resetColor() { m_currentColor = getTemplate()->getDefaultColor(); }
    ITF_INLINE const Color& AnimLightComponent::getDefaultColor() const { return getTemplate()->getDefaultColor(); }
    ITF_INLINE bbool AnimLightComponent::getUseBase() const { return getTemplate()->getUseBase(); }
    ITF_INLINE bbool AnimLightComponent::getCanonizeTransitions() const { return getTemplate()->getCanonizeTransitions(); }
    ITF_INLINE const StringID& AnimLightComponent::getDefaultAnimation() const { return getTemplate()->getDefaultAnim(); }
    ITF_INLINE bbool AnimLightComponent::getDraw2D() const {return getTemplate()->getDraw2d();}
    ITF_INLINE bbool AnimLightComponent::getDisableVisiblePrediction() const { return ( m_disableVisiblePrediction || getTemplate()->getDisableVisiblePrediction() ); }
}

#endif // _ITF_ANIMLIGHTCOMPONENT_H_

