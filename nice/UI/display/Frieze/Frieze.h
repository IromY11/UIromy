#ifndef _ITF_FRISE_H_
#define _ITF_FRISE_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_FRIEZE_DRAW_H_
#include "engine/display/Frieze/FriezeDraw.h"
#endif //_ITF_FRIEZE_DRAW_H_

#ifndef ITF_ENGINE_MESH3DPRIM_H_
#include "engine/display/Primitives/Mesh3DPrim.h"
#endif //ITF_ENGINE_MESH3DPRIM_H_

#ifndef _ITF_SEEDER_H_
#include "core/Seeder.h"
#endif //_ITF_SEEDER_H_

#ifndef _ITF_GRIDFLUID_H_
#include "engine/display/GridFluid/GridFluid.h"
#endif // _ITF_GRIDFLUID_H_

#ifdef USE_BOX2D
    class b2Body;
    class b2World;
    struct b2Vec2;
#endif //USE_BOX2D

namespace ITF
{
    struct VertexAnim;
    class FriseConfig;
    class FriseTextureConfig;
    class Phys2dClient;
	class Frieze3DBuildData;
	class Frieze3DRuntimeData;
    class Mesh3D;

    //  settings de chaque zone
    struct UvLinear_InPipePatch
    {
        UvLinear_InPipePatch( 
            f32 _uvYdown, 
            f32 _uvYup, 
            f32 _uvXsign, 
            Vec2d _uvSquare_0,
            Vec2d _uvSquare_1,
            Vec2d _uvSquare_2,
            Vec2d _uvSquare_3,
            Vec2d _uvStartExt_0,
            Vec2d _uvStartExt_1,
            Vec2d _uvStartExt_2,
            Vec2d _uvStartExt_3,
            Vec2d _uvStopExt_0,
            Vec2d _uvStopExt_1,
            Vec2d _uvStopExt_2,
            Vec2d _uvStopExt_3,
            Vec2d _uvStartExtCorner_0,
            Vec2d _uvStartExtCorner_1,
            Vec2d _uvStartExtCorner_2,
            Vec2d _uvStartExtCorner_3,
            Vec2d _uvStopExtCorner_0,
            Vec2d _uvStopExtCorner_1,
            Vec2d _uvStopExtCorner_2,
            Vec2d _uvStopExtCorner_3

            )
            : m_uvYdown(_uvYdown)
            , m_uvYup(_uvYup)
            , m_uvXsign(_uvXsign)
        {
            m_uvCorner[0] = _uvSquare_0;
            m_uvCorner[1] = _uvSquare_1;
            m_uvCorner[2] = _uvSquare_2;
            m_uvCorner[3] = _uvSquare_3;

            m_uvStartExt[0] = _uvStartExt_0;
            m_uvStartExt[1] = _uvStartExt_1;
            m_uvStartExt[2] = _uvStartExt_2;
            m_uvStartExt[3] = _uvStartExt_3;

            m_uvStopExt[0] = _uvStopExt_0;
            m_uvStopExt[1] = _uvStopExt_1;
            m_uvStopExt[2] = _uvStopExt_2;
            m_uvStopExt[3] = _uvStopExt_3;

            m_uvStartExtCorner[0] = _uvStartExtCorner_0;
            m_uvStartExtCorner[1] = _uvStartExtCorner_1;
            m_uvStartExtCorner[2] = _uvStartExtCorner_2;
            m_uvStartExtCorner[3] = _uvStartExtCorner_3;

            m_uvStopExtCorner[0] = _uvStopExtCorner_0;
            m_uvStopExtCorner[1] = _uvStopExtCorner_1;
            m_uvStopExtCorner[2] = _uvStopExtCorner_2;
            m_uvStopExtCorner[3] = _uvStopExtCorner_3;
        }

        f32 m_uvYdown;
        f32 m_uvYup;
        f32 m_uvXsign;
        Vec2d m_uvCorner[4]; 
        Vec2d m_uvStartExt[4];
        Vec2d m_uvStopExt[4];

        Vec2d m_uvStartExtCorner[4];
        Vec2d m_uvStopExtCorner[4];
    };

    struct UvLinearArchi
    {
        UvLinearArchi( 
            f32 _uvYdown, 
            f32 _uvYup, 
            f32 _uvXsign, 
            Vec2d _uvSquareOut_0,
            Vec2d _uvSquareOut_1,
            Vec2d _uvSquareOut_2,
            Vec2d _uvSquareOut_3,
            Vec2d _uvVarianceOut,
            Vec2d _uvSquareIn_0,
            Vec2d _uvSquareIn_1,
            Vec2d _uvSquareIn_2,
            Vec2d _uvSquareIn_3,
            Vec2d _uvVarianceIn
            )
            : m_uvYdown(_uvYdown)
            , m_uvYup(_uvYup)
            , m_uvXsign(_uvXsign)

        {
            m_uvSquareOut[0] = _uvSquareOut_0;
            m_uvSquareOut[1] = _uvSquareOut_1;
            m_uvSquareOut[2] = _uvSquareOut_2;
            m_uvSquareOut[3] = _uvSquareOut_3;
            m_uvSquareOut[4] = _uvVarianceOut;

            m_uvSquareIn[0] = _uvSquareIn_0;
            m_uvSquareIn[1] = _uvSquareIn_1;
            m_uvSquareIn[2] = _uvSquareIn_2;
            m_uvSquareIn[3] = _uvSquareIn_3;
            m_uvSquareIn[4] = _uvVarianceIn;
        }

        f32 m_uvYdown;
        f32 m_uvYup;
        f32 m_uvXsign;
        Vec2d m_uvSquareOut[5];
        Vec2d m_uvSquareIn[5];        
    };

#define  Frise_CRC ITF_GET_STRINGID_CRC(Frise,2579114664)
    class Frise : public Pickable
    {
	friend class Frieze3DBuildData;

    private:
       
#ifdef DEVELOPER_JAY_COOK
        bbool m_hasBeenRecomputed;        
#endif // DEVELOPER_JAY_COOK

    struct RecomputeData
    {
        RecomputeData()
            : m_edgeListCount(0)
            , m_edgeListCountMax(0)
            , m_isHoleVisual(btrue)
            , m_isUvFlipY(btrue)
            , m_texRatio(1.f)
            , m_anim(bfalse)
            , m_uvXsign(1.f)
            , m_animSyncCur(0.f)
            , m_animSyncXCur(0.f)
            , m_animSyncYCur(0.f)       
            , m_alphaBorderWanted(bfalse)
            , m_heightScale(2.f)
            , m_widthScale(2.f)
            , m_zVtxUp(0.f)
            , m_zVtxDown(0.f)
            , m_edgeRunStartBuildVB(bfalse)
            , m_flexibility(0.75f)
            , m_heightScaleInv(1.f)
            , m_useFlippedUV(bfalse)
            , m_isPreComputingForCook(bfalse)
            , m_useAtlasPolygone(bfalse)
            , m_useFillGradient(bfalse)
            , m_meshPosition(0)
        { }; 

        ~RecomputeData();

        u32                  m_edgeListCount;
        u32                  m_edgeListCountMax;
        f32                  m_texRatio;		
        f32                  m_uvXsign;
        f32                  m_animSyncCur;
        f32                  m_animSyncXCur;
        f32                  m_animSyncYCur;        		
        f32                  m_heightScale;
        f32                  m_widthScale;
        f32                  m_zVtxUp;
        f32                  m_zVtxDown;        		
        f32                  m_flexibility;
        f32                  m_heightScaleInv;
        u32                  m_meshPosition;

        u32                 m_edgeRunStartBuildVB : 1,
                            m_isUvFlipY : 1,
                            m_isHoleVisual : 1,
                            m_useFlippedUV : 1,
                            m_alphaBorderWanted : 1,
                            m_anim : 1,
                            m_useAtlasPolygone : 1,
                            m_useFillGradient : 1,
                            m_isPreComputingForCook;

        ITF_MAP<StringID, Mesh3D *> m_meshCookMap;
    };

    public:

        struct FillingParams
        {
            FillingParams()
                : m_cosAngle(0.f)
                , m_sinAngle(0.f)
                , m_scaleX(0.f)
                , m_scaleY(0.f)
            {
            }
            f32 m_cosAngle;
            f32 m_sinAngle;
            f32 m_scaleX;
            f32 m_scaleY;
        };

        struct IndexList
        {
            DECLARE_SERIALIZE()
            
            ITF_VECTOR<u16> m_list;
            u32             m_idTexConfig;

            ITF_INLINE void clear()                         { m_list.clear(); }
            ITF_INLINE void push_back( const u16& _index )  { m_list.push_back( _index ); }
            ITF_INLINE void reserve( const u32& _size )     { m_list.reserve( _size ); }
            ITF_INLINE u32  capacity()                const { return m_list.capacity(); }
        };
        

        ITF_DECLARE_NEW_AND_DELETE_OPERATORS_CATEGORY(mId_Frieze);
        DECLARE_OBJECT_CHILD_RTTI(Frise, Pickable,2579114664);
        DECLARE_SERIALIZE()

        enum BuildFriezeMethode
        {
            InRoundness = 0,
            InGeneric,
            InExtremity, 
            InArchitecture,
            InFluid,
            InPipe,
            InExtremitySimple,
            InString,
            InAtlas,
            InOverlay,
            InFrame,
            InArchitectureSimple,
            InPipePatchSimple,
            InExtremityLong,
			In3D,
            BuildFriezeCount,
            ENUM_FORCE_SIZE_32(BuildFriezeMethode)
        };
    
        enum LockTexture
        {            
            LockTexture_Top = 0,
            LockTexture_Right = 1,
            LockTexture_Left = 2,
            LockTexture_Bottom = 3,
            LockTexture_None = 4,
            LockTexture_Switch = 5,
            ENUM_FORCE_SIZE_32(LockTexture)
        };

        Frise                               (   );
        virtual ~Frise                      (   );

        virtual void    onFlipChanged       (   );
        virtual void    onPosChanged        (   const Vec3d& _prevPos ) { processUpdateTranslate(_prevPos); }
        virtual void    onDepthChanged      (   f32 _prevDepth );
        virtual void    onAngleChanged      (   f32 _prevAngle )        { processUpdateTransform(); }
        virtual void    onScaleChanged      (   const Vec2d& _prevScale){ processUpdateTransform(); }


        virtual void     onAnglePosChanged(f32 _prevAngle, const Vec3d& _prevPos) {processUpdateTransform();}

        virtual void    disable();
        virtual void    enable();

        void            processUpdateTransform();
        void            processUpdateTranslate(const Vec3d& _prevPos); // only translated
        void            update              (   f32 _ellasped );
        void            batchPrimitives     ( const ITF_VECTOR <class View*>& _views );
        void            clearComputedData   (   );
        void            updateVisualPolyline(   const Transform2d& _xf );
        void            updateVisualPolyline(   const Vec2d & _deltaPos );
        void            preComputeForCook   (   );
        bbool           isCooked            (   ) const { return m_preComputedForCook; }
        
        virtual void    onStartDestroy( bbool _hotReload );
        virtual void    onDestroy( bbool _hotReload );                              // Called by the scene/actorsManager just before deleting it
        virtual void    onLoaded(HotReloadType _hotReload);
        virtual void    onFinalizeLoad( bbool _hotReload );
        virtual void    onEvent(class Event* _event);
		virtual void	onResourceLoaded();
		virtual void	onResourceUnloaded();
        
        void            postLoadCheckpointData();
        void            saveCheckpointData();
        void            preLoadChekpointData();

        virtual void    requestDestruction  (   );

        ITF_INLINE  const FriseConfig*  getConfig   (   ) const     { return static_cast<const FriseConfig*>(Super::getTemplate()); }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// add a fluid shaper. it forces the shape of the fluid
        /// @param _layerIndex
        /// @param _shaper
        void            registerFluidShaper(u32 _layerIndex, FluidShaper *_shaper);
        void            unregisterFluidShaper(u32 _layerIndex, FluidShaper *_shaper);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get wave generators that influence this frieze. Only valid for strings
        const SafeArray<ObjectRef>* getStringWaveGenerators() const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// setFluidLayerCollisionHeightMultiplier
        /// @param _layerIndex : layer index. If no such layer, do nothing
        /// @param _multiplier : the multiplier to set
        void setFluidLayerCollisionHeightMultiplier(u32 _layerIndex, f32 _multiplier);

        ///////////////////////////////////////////////////////////////////////////////////////////
        void			forceRecomputeData( bbool _isPreComputingForCook = bfalse );
        ITF_INLINE void recomputeData() {m_querriedRecomputeData = btrue;}
        bbool           isRecomputeDataQuerried()const { return m_querriedRecomputeData; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get collision data in world space. returned data is selected according to dynamic state.
        ITF_INLINE const ITF_VECTOR<PolyLine*>* getCollisionData_WorldSpace()const { return m_pCollisionData ? &m_pCollisionData->m_worldCollisionList : NULL; }
   
        const PolyLine& getVisualPoly               (   ) const { return m_visualPolyline; }
        PolyLine&       getVisualPoly               (   ) { return m_visualPolyline; }
        PolyPointList&  getVisualPolyPointListLocal (   ) { return m_visualPolyPointLocal; }
        static void     checkConnections            (   const ITF_VECTOR<Frise*>& _friezeList );
        static void     checkConnectionsDataCooked  (   const ITF_VECTOR<Frise*>& _friezeList );
#ifdef USE_BOX2D 
        b2Body *        getBody() { return m_b2dBody; }
#endif // USE_BOX2D 

#ifdef ITF_SUPPORT_EDITOR        
        void            setColorComputerTagId       (   u32 _tagid  )   {  m_colorComputerTagId = _tagid;}
        const u32       getColorComputerTagId       (   ) const {   return m_colorComputerTagId;    }
#endif // ITF_SUPPORT_EDITOR

        void            setUsePrimParamFcg( bbool _value ) { m_usePrimParamFcg = _value; }
        const bbool     getUsePrimParamFcg() const { return m_usePrimParamFcg; }

        ITF_INLINE const GFXPrimitiveParam& getGfxPrimitiveParamSerialized() const { return m_primitiveParamSerialized;}
        void            setGFXPrimitiveParamSerialized( const GFXPrimitiveParam& _param);

        ITF_INLINE const GFXPrimitiveParam& getGfxPrimitiveParamRunTime() const { return m_primitiveParamRunTime;}
        void            resetGFXPrimitiveParamRunTime(   );
        void            setDefaultDataForCheckpoint();
        void            initEventShowColors();
        void            setColorFactorRunTime       (   const Color& _color);
        void            setColorFogRunTime          (   const Color& _color );
        void            setAlpha(   f32 _fade);        
        void            setFillUVOrigin             ( const Vec2d& _pos) { m_fillUVOrigin = _pos; }
        ITF_INLINE const Vec2d& getFillUVOrigin() const { return m_fillUVOrigin; }

        virtual void    onBecomeActive();
        virtual void    onBecomeInactive();
        
        void            erasePosAt              (   u32 _index );
        void            clearPoints()   { m_pointsList.clear(); }
        f32             getEdgeScaleAtRatio(u32 _index, f32 _ratio) const { return m_pointsList.getPointScaleAt(_index) +(m_pointsList.getPointScaleAt(_index+1)-m_pointsList.getPointScaleAt(_index))*_ratio;}
        void            registerPhysic();
        void            unregisterPhysic();
        ITF_INLINE u32		getPosCount() const {return m_pointsList.getPosCount();}
        ITF_INLINE const PolyPointList&		getPointsData() const {return m_pointsList;}
        PolyPointList*  getPolyPointList() { return &m_pointsList; }
        const PolyPointList*  getPolyPointList() const { return &m_pointsList; }
        ITF_INLINE void                     setEdgeList( const PolyLineEdgeList& _edgeList ) { m_pointsList.getMutableEdgeData() = _edgeList; }
        ITF_INLINE PolyLineEdge::HoleMode  getHoleAt(   u32 _index  ) const {return getPointsData().getHoleAt(_index);}
        ITF_INLINE const Vec2d&  getPosAt           (   u32 _index  ) const { return getPointsData().getPosAt(_index);}
        ITF_INLINE const PolyLineEdge&  getEdgeAt	(   u32 _index  ) const { return getPointsData().getEdgeAt(_index);}
        ITF_INLINE bbool				isLooping( ) const { return getPointsData().isLooping();}	    
        ITF_INLINE void switchTextureAt ( u32 _index) {m_pointsList.switchTextureAt(_index); recomputeData();}
        ITF_INLINE void setHoleAt (   u32 _index, PolyLineEdge::HoleMode _setHole ) {m_pointsList.setHoleAt(_index, _setHole); recomputeData(); }
        ITF_INLINE void setPosAt (const Vec2d& _pos, u32 _i ) {m_pointsList.setPosAt(_pos, _i ); recomputeData();}        
        ITF_INLINE void addPoint (const Vec2d& _pos, i32 _at = -1 ) {m_pointsList.addPoint(_pos, _at ); recomputeData();}
        ITF_INLINE void setLoop (   bbool _loop) {m_pointsList.setLoop(_loop); recomputeData();}
        ITF_INLINE void     forceLoop (   bbool _loop) {m_pointsList.forceLoop(_loop); recomputeData();}
        ITF_INLINE void  checkLoop( ) {m_pointsList.checkLoop();}
        ITF_INLINE void switchTexturePipeExtremity() { m_switchTexturePipeExtremity = ( m_switchTexturePipeExtremity +1) %4; recomputeData(); }
        ITF_INLINE void     addEdge( const PolyLineEdge& _edge) { m_pointsList.addEdge(_edge);}
        ITF_INLINE u32  getSwitchTexturePipeExtremity() const { return m_switchTexturePipeExtremity; }
        void            setSwitchTexturePipeExtremity(u32 _value) { ITF_ASSERT(_value!=U32_INVALID); m_switchTexturePipeExtremity = _value; }

        ITF_INLINE f32      getPointScaleAt         (   u32 _index  ) const { return m_pointsList.getPointScaleAt(_index);  }
        ITF_INLINE void     setPointScaleAt         (   u32 _index, f32 _scale ) { m_pointsList.setPointScaleAt( _index, _scale); recomputeData(); }
        ITF_INLINE void     resetPointScaleAt       (   u32 _index ) { setPointScaleAt( _index, 1.f ); }
        ITF_INLINE void     resetAllPointsScale     (   ) { m_pointsList.resetAllPointsScale(); recomputeData(); }  
          
        void                randomizeAnimGlobalSync();

		bbool is3D() const { return getConfig() ? getConfig()->m_methode == In3D : bfalse; }      

        // Mesh Static
        void    clearMeshStaticData         (   );
        void    buildMesh_Static            (   );
        void    buildMesh_StaticFinal       (   );    
        void    addIndex_Static_Quad        (   IndexList& _indexList, u16& _indexVtxDown, u16& _indexVtxUp, bbool _flip );
        void    buildVB_Static_Quad         (   IndexList& _indexList, const Vec2d* _pos, const Vec2d* _uv, const u32* _color, bbool _flip, const f32* _zExtrude = NULL );
        bbool   buildVB_Static_QuadMesh     (   const Vec2d* _pos, const Vec2d* _uv, StringID & _family, i32 & _familyindex, i32 & _remainingMeshes);
        bbool   buildVB_Static_QuadMeshAlone(   const Vec2d* _pos, const Vec2d* _uv, i32 index = -1);

        void    WeldMeshDataStatic          (   IndexList& _indexList);
        void    WeldMeshDataAnim            (   IndexList& _indexList);

        f32     getAnimSpeedFactor() const { return m_animSpeedFactor; }
        void    setAnimSpeedFactor(f32 val) { m_animSpeedFactor = val; }

        f32     getAnimAmplitudeFactor() const { return m_animAmplitudeFactor; }
        void    setAnimAmplitudeFactor(f32 val) { m_animAmplitudeFactor = val; }

#ifdef ITF_SUPPORT_EDITOR
        virtual Pickable*                   clone(   Scene* _pDstScene, bbool _callOnLoaded) const;      
        virtual void                        onEditorCreated( Pickable* _original = NULL);
        virtual void                        onEditorMove( bbool _modifyInitialPos = btrue  );
        void                                onEditorTranslatePivot                      (   const Vec2d& _newPos );
        void                                onEditorCenterPivot (   );
        ITF_INLINE void                     onEditorSetFillUVOrigin( const Vec2d& _pos ) { m_fillUVOrigin = _pos; recomputeData(); }
        ITF_INLINE const bbool              OnEditorIsCenterPivot               (   ) const { return m_isCenterPivot; }
        const void                          onEditorSetCenterPivot              (   bbool _isCenterPivot ) { m_isCenterPivot = _isCenterPivot; }
        void                                onEditorRecomputeData() { forceRecomputeData(); processUpdateTransform(); }
 
        const void                          onEditorSwitchExtremityStart() { m_switchExtremityStart = !m_switchExtremityStart; forceRecomputeData(); }
        const void                          onEditorSwitchExtremityStop() { m_switchExtremityStop = !m_switchExtremityStop; forceRecomputeData(); }
        const void                          onEditorSetExtremityStart( bbool _state ) { m_switchExtremityStart = _state; }
        const void                          onEditorSetExtremityStop( bbool _state ) { m_switchExtremityStop = _state; }
        bbool                               hasLoopingMethode() const;    
        bbool                               hasSwitchExtremityMethode() const;    

        bbool                               hasMetaFriezeOwner() const { return m_metaFrieze.isValid(); }
        void                                setMetaFriezeOwner( ObjectRef _metaFriezeRef ) { m_metaFrieze = _metaFriezeRef; }
        ObjectRef                           getMetaFriezeOwner() const { return m_metaFrieze; }
        void                                updateMetaFriezeDataError();
        bbool                               isFilling() const { return m_isFilling;}
        u32                                 getAtlasIndex() const { return m_atlasIndex; }
        void                                setAtlasIndex( u32 _index) { m_atlasIndex = _index; }
        u32                                 getAtlasTessellationIndex() const { return m_atlasTessellationIndex; }
        void                                setAtlasTessellationIndex( u32 _index) { m_atlasTessellationIndex = _index; }
        bbool                               useAtlasPolygone() const { return m_useAtlasPolygoneEditor; }

        void								replaceReinitDataByCurrent();

        bbool                               m_isFilling;        
        bbool                               m_useAtlasPolygoneEditor;
#endif // ITF_SUPPORT_EDITOR

#ifndef ITF_FINAL
        virtual bbool           validate();
        virtual void            setDataError(const String8 & _str);
        virtual void            resetDataError();
#endif // ITF_FINAL
               
        ITF_INLINE ObjectRefList & getEventListeners() { return m_eventListeners; }
        ITF_INLINE void addEventListener(ObjectRef val) 
        { 
            if (m_eventListeners.find(val) == -1)
                m_eventListeners.push_back(val);
        }
        ITF_INLINE void removeEventListener(ObjectRef val) 
        { 
            i32 index = m_eventListeners.find(val);
            if (index != -1)
                m_eventListeners.removeAtUnordered(index);
        }

#ifndef ITF_STRIPPED_DATA
        GFXPrimitiveParamForced& getGfxPrimitiveParamForced() { return m_primitiveParamForced; }
#endif // ITF_STRIPPED_DATA


        Phys2dClient*   getPhys2dClient(){return m_phys2dClient;}

		ITF_Mesh* getStaticDataMesh	(  ) { return &m_pMeshStaticData->m_mesh; }


    private:
        friend class StaticMeshVertexComponent;
        friend class FBXExporter;
        f32             m_time;

        void            updateMovingCollisions();
        void            syncMovingCollisions();
        void            clearCollisionData          (   );        
        void            setVisualData   (   const FriseConfig* _config );
        void            processUpdateFluid  ( const FriseConfig* _pConfig, f32 _elapsed );
        void            registerRegion( const FriseConfig* _pConfig );
        void            unregisterRegion();
        f32             getAnimGlobalSync() { if (m_animGlobalSync == 0.f) randomizeAnimGlobalSync(); return m_animGlobalSync; }

        u32                     m_seed;
        Seeder                  m_seeder;
        u32                     m_meshPosition;
        ITF_VECTOR<i32>         m_meshManualySet;

        // Mesh3D
        struct Mesh3dInfo
        {
            Mesh3D * m_main;
            Mesh3D * m_dummy;
        };

        bbool getMeshes( const Mesh3dData * _meshData, ITF_VECTOR<Mesh3dInfo> & _meshList);
        bbool getUVMeshes( const Vec2d & _uv, const Mesh3dData * & _meshData, ITF_VECTOR<Mesh3dInfo> & _meshList);
        u32   getMeshIndex( const Mesh3dData * _meshData, StringID & _family, i32 & _lastIndexInFamily, i32 & _remainingMeshes);
        void  InitMeshData( IndexList& _indexList, const ITF_VECTOR<Mesh3dInfo> & _meshList, u32 meshIndex, u32 _start);
        void  fillMeshVertex(const Vec2d* _pos, const Vec2d* _uv, const Mesh3dData * _meshData, const ITF_VECTOR<Mesh3dInfo> & _meshList, u32 meshIndex, u32 _start, f32 _useUVLimitLeft = -1.f, f32 _useUVLimitRight = -1.f);
        Mesh3D * getMeshCooked3d(const Path & _path);

        //////////////
        // serialize  
        //////////////
        PolyPointList m_pointsList;                
#ifdef ITF_SUPPORT_EDITOR
        Path                m_editConfigName;
        bbool               m_isCenterPivot;
        bbool               m_prevUseMovingCollision;
        ObjectRef           m_metaFrieze;
        u32                 m_colorComputerTagId;    
        GFXPrimitiveParam   m_editPrimitiveParam;

        friend class FriezeEditor;

        struct Mesd3dQuadPoints
        {
            Vec2d m_pTab[4];
            
            const Mesh3dData *m_meshDataPtr;
            u32               m_index;
            u32               m_start;

            Mesd3dQuadPoints(const Vec2d * _pts, const Mesh3dData *_meshDataPtr, u32 _index, u32 _start)
                : m_meshDataPtr(_meshDataPtr), m_index(_index), m_start(_start)
            {
                m_pTab[0] = _pts[0];
                m_pTab[1] = _pts[1];
                m_pTab[2] = _pts[3];
                m_pTab[3] = _pts[2];
            }
        };
        ITF_VECTOR<Mesd3dQuadPoints> m_quadTab;
#endif


        u32         m_switchTexturePipeExtremity;
                
        bbool             m_usePrimParamFcg;
        GFXPrimitiveParam m_primitiveParamSerialized;
        GFXPrimitiveParam m_primitiveParamRunTime;

#ifndef ITF_STRIPPED_DATA
        GFXPrimitiveParamForced m_primitiveParamForced;
        void        updateForcedValues( const SerializedObjectContent_Object* _content );
#endif // !ITF_STRIPPED_DATA

		f32		m_depthOffset;
        //  uv
        f32     m_uvXoffSet;
        f32     m_uvYoffSet;
        bool    m_uvX_flip;
        bool    m_uvY_flip;
        f32     m_uvY_tile;
        
        // fill
        Vec2d   m_fillOffSetUv;
        Vec2d   m_fillUVOrigin;

        //  vertex anim        
        f32     m_animGlobalSync;

        // atlas
        u32     m_atlasIndex;
        u32     m_atlasTessellationIndex;

        f32         m_thickness;
        bbool       m_useMovingCollision;
        u32         m_moveCollisionFrame;

        LockTexture m_lockTexture;

        bbool   m_switchExtremityStart;
        bbool   m_switchExtremityStop;

        Path m_materialShaderOverridePath;
        const class GFXMaterialShader_Template * m_matShaderOverride;

        bbool           m_preComputedForCook;
        Transform2d     m_xfForCook;
        f32             m_depthForCook;
        f32             m_aabbMinZ;
        f32             m_aabbMaxZ;

        /////////////////
        // no serialize
        /////////////////

        RecomputeData*  m_pRecomputeData;

#ifdef SKIP_NODRAW_UPDATE
        f32     m_skippedDt;
#endif // SKIP_NODRAW_UPDATE

        bbool           m_disablePhysic;

        u32             m_querriedRecomputeData : 1,
                        m_physicRegistered : 1,                                          
                        m_regionRegistered;

        Angle   m_animGlobalRotCur;     
        f32     m_animSpeedFactor;
        f32     m_animAmplitudeFactor;

        struct edgeRun
        {
            edgeRun()
                : m_idEdgeStart(0)
                , m_edgeCount(0)
                , m_idTex(-1)
                , m_coeff(1.f)
                , m_idZone(0)
            {
            }

            u32 m_idEdgeStart;
            u32 m_edgeCount;
            i32 m_idTex;
            u32 m_idZone;
            f32 m_coeff;
        };

        struct MeshBuildData
        {
            DECLARE_SERIALIZE()

            ITF_VECTOR<IndexList>       m_staticIndexList;
            ITF_VECTOR<VertexPCT>       m_staticVertexList;
			ITF_VECTOR<Vec3d>			m_staticNormalList;
            
            ITF_VECTOR<IndexList>       m_animIndexList;
            ITF_VECTOR<VertexPNC3T>     m_animVertexList;

            IndexList                   m_overlayIndexList;
            ITF_VECTOR<VertexPCBT>      m_overlayVertexList;

            void clear() {  m_animIndexList.clear(); m_animVertexList.clear(); 
                            m_staticIndexList.clear(); m_staticVertexList.clear(); m_staticNormalList.clear();
                            m_overlayIndexList.clear(); m_overlayVertexList.clear(); }
        };
        MeshBuildData * m_meshBuildData;
       
        struct MeshFluidData
        {
            DECLARE_SERIALIZE()
            DataFluid       m_data;
            AABB            m_aabbLocal;
            AABB            m_aabbWorld;
        };
        MeshFluidData* m_pMeshFluidData;

        struct MeshOverlayData
        {
            DECLARE_SERIALIZE()
            ITF_Mesh        m_mesh;           
            f32             m_cosAngle;
            f32             m_sinAngle;
            AABB            m_aabbWorld;
            AABB            m_aabbLocal;
        };
        MeshOverlayData* m_pMeshOverlayData;

        struct MeshAnimData
        {
            DECLARE_SERIALIZE()
            ITF_Mesh        m_mesh;
            AABB            m_aabbWorld;
            AABB            m_aabbLocal;
        };
        MeshAnimData* m_pMeshAnimData;

        struct MeshStaticData
        {                            
            DECLARE_SERIALIZE()
            ITF_Mesh		m_mesh;
            AABB            m_aabbWorld;
            AABB            m_aabbLocal;
        };
        MeshStaticData* m_pMeshStaticData;

		// Frieze 3D
		Frieze3DBuildData* m_pFrieze3DBuildData;
		Frieze3DRuntimeData* m_pFrieze3DRuntimeData;

        struct CollisionData
        {
            DECLARE_SERIALIZE()

            //  collision
            ITF_VECTOR<PolyPointList>	m_localCollisionList;
            ITF_VECTOR<PolyLine*>		m_worldCollisionList;                      
        };
        CollisionData* m_pCollisionData;

        Phys2dClient*   m_phys2dClient;

#ifdef USE_BOX2D
        // Box 2d Data !!
        b2Body      * m_b2dBody; 
        b2World     * m_b2dWorld; 

        u32         m_bodyType;
        u16         m_categoryBits;
        u16         m_maskBits;
#endif

        void        createCollisionData();
        void        deleteCollisionData();

        // visual Polyline
        PolyPointList           m_visualPolyPointLocal;
        PolyLine                m_visualPolyline;

	public:
		// Filter bitfield.
		GFX_GridFluidFilter		m_fluidEmitterFilter;
    private:
        
        enum CollisionDouble
        {
            None = 0,
            Double,
            DoubleAndLoop,
            Count,
            ENUM_FORCE_SIZE_32(CollisionDouble)
        };

        struct collisionRun
        {
            collisionRun()
                : m_idEdgeRunStart(0)
                , m_edgeRunCount(0)
                , m_idEdgeStart(0)
                , m_edgeCount(0)
                , m_edgeCountStartOffset(0)
                , m_edgeCountStopOffset(0)
                , m_collisionOffsetLast(-1.f)
                , m_collisionOffsetNext(-1.f)
                , m_index(0)
                , m_collisionDouble(None)
            {
            }

            u32 m_idEdgeRunStart;
            u32 m_edgeRunCount;
            u32 m_idEdgeStart;
            u32 m_edgeCount;
            u32 m_edgeCountStartOffset;
            u32 m_edgeCountStopOffset;

            f32 m_collisionOffsetLast;
            f32 m_collisionOffsetNext;

            u32 m_index;
            CollisionDouble m_collisionDouble;

        };

        struct CollisionByGameMat
        {
            i32 m_startId;
            i32 m_stopId;
            Path m_gameMat;
        };

        struct edgeFrieze
        {
            edgeFrieze()
                : m_pos(Vec2d::Zero)
                , m_sight(Vec2d::Zero)
                , m_sightNormalized(Vec2d::Zero)
                , m_normal(Vec2d::Zero)
                , m_interUp(Vec2d::Zero)
                , m_interDown(Vec2d::Zero)
                , m_cornerNormal(Vec2d::Zero)
                , m_cornerAngle(0.f)
                , m_build(btrue)
                , m_snap(bfalse)
                , m_norm(0.f)
                , m_startCoeff(0.f)
                , m_normUv(0.f)
                , m_holeMode(PolyLineEdge::Hole_None)
                , m_heightStart(1.f)
                , m_heightStop(1.f)
                , m_scaleCur(1.f)
                , m_scaleInit(1.f)
                , m_switchTexture(bfalse)
                , m_stopCoeff(1.f)
                , m_idPoint(0)
            {
                m_points[0] = m_points[1] = m_points[2] = m_points[3] = Vec2d::Zero;
            }

            Vec2d       m_pos;
            Vec2d       m_sight;
            Vec2d       m_sightNormalized;
            Vec2d       m_normal;
            Vec2d       m_interUp;
            Vec2d       m_interDown;
            Vec2d       m_cornerNormal;
            f32         m_cornerAngle;
            f32         m_norm;
            f32         m_startCoeff;
            f32         m_normUv;
            PolyLineEdge::HoleMode       m_holeMode;
            f32         m_scaleCur;
            f32         m_scaleInit;
            f32         m_heightStart;
            f32         m_heightStop;
            Vec2d       m_points[4];
            f32         m_stopCoeff;
            u32         m_idPoint;
            u32         m_switchTexture : 1,
                        m_snap : 1,
                        m_build;                        
        };

        struct CornerRounder
        {
            CornerRounder()
                : m_edgeCur(NULL)
                , m_edgeLast(NULL)
                , m_edgeRun(NULL)
                , m_config(NULL)
                , m_indexVtxUp(NULL)
                , m_indexVtxDown(NULL)
                , m_uvXsign(NULL)
                , m_uvDown(NULL)
                , m_uvUp(NULL)
                , m_uvDownXOffSet(NULL)
                , m_uvUpXOffSet(NULL)
                , m_indexVtxCenter(NULL)
                , m_indexVtxCorner(NULL)
                , m_uvCorner(NULL)
                , m_zVtxCorner(0.f)
                , m_colorCorner(COLOR_ZERO)
                , m_startPos(Vec2d::Zero)
                , m_stopPos(Vec2d::Zero)
                , m_centerPos(Vec2d::Zero)
                , m_cornerPos(Vec2d::Zero)
                , m_uvXoffsetCorner(NULL)
                , m_edgeCountCur(NULL)
            {
            }

            const edgeFrieze* m_edgeCur;
            const edgeFrieze* m_edgeLast;
            const edgeRun* m_edgeRun;
            const FriseConfig* m_config;
            u16* m_indexVtxUp;
            u16* m_indexVtxDown;
            f32* m_uvXsign;
            Vec2d *m_uvDown;
            Vec2d *m_uvUp;
            f32* m_uvUpXOffSet;
            f32* m_uvDownXOffSet;

            u16* m_indexVtxCenter;
            u16* m_indexVtxCorner;
            Vec2d* m_uvCorner;
            f32* m_uvXoffsetCorner;
            f32  m_zVtxCorner;
            ColorInteger m_colorCorner;
            Vec2d m_startPos;
            Vec2d m_stopPos;
            Vec2d m_centerPos;
            Vec2d m_cornerPos;

            u32* m_edgeCountCur;
        };

#ifdef DEVELOPER_JAY_FRIEZE
        void    debugDrawInt                (   Vec2d pos2d, u32 _i, Color _color );
        void    debugDrawFloat              (   Vec2d pos2d, f32 _f, Color _color );
        void    debugDrawEdgeList           (   ITF_VECTOR<edgeFrieze>& _edgeList );
        void    debugDrawEdgeRun            (   ITF_VECTOR<edgeRun>& _edgeRunList );       
#endif
     
        void    renderFrieze                (   const ITF_VECTOR <class View*>& _views   );        
        void    buildVisualPolypointInFluid (   const ITF_VECTOR<edgeFrieze>& _edgeList  );
        void    clearVisualData         (   );                
        void    executeRecomputeData        (   bbool _isPreComputingForCook = bfalse );
        void    initDatas                   (   const FriseConfig* _pConfig, i32 _texConfigCount, bbool _isPreComputingForCook );
        i32     getFirstNoFillTextureIndex  (   const FriseConfig* config ) const;
        void    checkDataConsistency        (   const FriseConfig* _pConfig );
        void    updateAABB                  (   const Transform2d& _xf );

        // Edges In Loop
        u32     findLastBuildEdgeInLoop     (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdge ) const;

        // Edges
        bbool   skipEdgeFrieze              (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idToSkip, u32 _idToUpdate, u32 _idLast ) const;        
        void    finalizeEdgeFriezeList      (   const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList );
        bbool   updateEdgeFriezeVector      (   edgeFrieze& _edgeToUpdate, Vec2d _vector ) const;
        void    copyEdgeFromPolyline        (   ITF_VECTOR<edgeFrieze>& _edgeList );
        bbool   buildEdgeCorner             (   const FriseConfig* config, edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, f32 _cornerLongMax, f32& _dotSight ) const;
        bbool   skipWrongEdge               (   edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, f32& _dotSight ) const;    
        void    setNormEdge                 (   edgeFrieze& _edge ) const;
        bbool   skipSmallEdge               (   ITF_VECTOR<edgeFrieze>& _edgeList, edgeFrieze& _edgeLast, edgeFrieze& _edgeCur, f32 _normMin, i32 _idEdgeLastLast ) const;
        void    snapEdge                    (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeToSnap, u32 _idEdgeToUpdate ) const;
        bbool   findNextBuildEdge           (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 &_idEdge ) const;
        bbool   findPreviousBuildEdge       (   ITF_VECTOR<edgeFrieze>& _edgeList, i32 &_idEdge ) const;
        void    buildEdgeList               (   ITF_VECTOR<edgeFrieze>& _edgeList, bbool _snapEdgeLast = btrue );
        void    snapAllEdge                 (   ITF_VECTOR<edgeFrieze>& _edgeList );
        f32     getEdgeNormMin              (   const FriseConfig* _config ) const;
        f32     getEdgeRunNormMin           (   const FriseConfig* _config ) const;
        void    updateIdEdgeList            (   u32& _idCurEdge, u32& _idLastEdge, u32* _idLastEdgeValidList ) const;
        bbool   insertNewEdge               (   ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge );
        bbool   setOffsetEdge               (   edgeFrieze& curEdge, edgeFrieze& lastEdge, Vec2d& _posInit, f32 _offset, f32 longMax, f32 longMaxSqr) const;
        bbool   isEdgeWithHoleCollision     (   const edgeFrieze& _edge ) const;
        bbool   isEdgeWithHoleVisual        (   const edgeFrieze& _edge ) const;
        bbool   isEdgeLastValid_InFluid    (   const ITF_VECTOR<edgeFrieze>& _edgeList, u32 _indexCur ) const;       
        bbool   isEdgeWithHole              (   const edgeFrieze& _edge ) const;
        bbool   updateEdgeFriezePosition    (   edgeFrieze& _edge, Vec2d& _pos ) const;
        void    subtractOffsetExtremity     (   ITF_VECTOR<edgeFrieze>& _edgeList, f32 _startOffset, f32 _stopOffset ) const;
        bbool   isEdgeRunWithSwitchTexture  (   edgeRun& _edgeRun, ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        void    setScaleNewEdge             (   edgeFrieze& _edgeNew, edgeFrieze& _edgeLast, edgeFrieze& _edgeNext ) const;
        Vec2d   getPosStartOnNormalEdge     (   const edgeFrieze* _edge, f32 _offset = 1.f ) const;
        Vec2d   getPosStopOnNormalEdge      (   const edgeFrieze* _edge, f32 _offset = 1.f ) const;
        Vec2d   getPosOnCornerEdge          (   const edgeFrieze* _edge, f32 _offset = 1.f ) const;
        bbool   setScaleAverage             (   edgeFrieze& _edgeCur, edgeFrieze& _edgeLast ) const;
        bbool   isEdgeRun                   (   ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge ) const;
        bbool   getIdLastLastEdgeBuild      (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idLastEdge, u32& _idLastLastEdge ) const;
        void    buildEdgePoints             (   const FriseConfig* _config, edgeFrieze& _edgeCur) const;
        bbool   setIntersectionUp           (   const edgeFrieze& _edgeLast, edgeFrieze& _edgeCur) const;
        bbool   setIntersectionDown         (   const edgeFrieze& _edgeLast, edgeFrieze& _edgeCur) const;
        bbool   skipBadIntersection         (   const edgeFrieze& _edge, const Vec2d& _interLast, const Vec2d& _interCur) const;
        void    snapEdgesPointsWithInter    (   edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, edgeFrieze* edgeLast2, bbool _allEdgeSnaped) const;
        Vec2d   getEdgeBorderLeft           (   const edgeFrieze& _edge) const;
        Vec2d   getEdgeBorderRight          (   const edgeFrieze& _edge) const;
        void    updateEdgeHeight            (   edgeFrieze& _edgeCur, edgeFrieze& _edgeLast) const;
        bbool   isHeightEqual               (   const edgeFrieze& _edgeLast, const edgeFrieze& _edgeCur) const;
        void    snapEdgeListPointsWithInter (   const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        u32     getCountBuildEgdes          (   ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        u32     getCountSnapEgdes           (   ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        bbool   isScaleEqual                (   const edgeFrieze& _edgeLast, const edgeFrieze& _edgeCur) const;
        bbool   isSnapAllEdgeInBubble       (   ITF_VECTOR<edgeFrieze>& _edgeList) const;
        f32     getNormMinToSnap            (   ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge, f32 _normMinPoint, f32 _normMinUv) const;
        void    setPosStartOnEdgeWithHoleVisual  (   const FriseConfig* _config, const edgeFrieze& edge, Vec2d& _down, Vec2d& _up) const;
        void    setPosStopOnEdgeWithHoleVisual( const FriseConfig* _config, const edgeFrieze& edge, Vec2d& _down, Vec2d& _up, u32 _idPoint) const;
        u32     getIdEdgeNext               (   u32 _id) const;
        u32     getIdEdgeLast               (   u32 _id) const;
        bbool   buildEdgeListInSkew         (   const FriseConfig* _pConfig, ITF_VECTOR<edgeFrieze>& _edgeList ) const;
                
        // Uvs
        void    setEdgeRunCoeffUv           (   const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun ) const;
        bbool   setEdgeRunListCoeffUv       (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    setEdgeRunListNormUv        (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    orderEdgeRunListByTextureId (   ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        f32     getUvX_Start                (   );
        u32     getCornerStepNb             (   f32 _angleFan, f32 _step, f32 _scale ) const;
        void    setCornerRounderDatas       (   const FriseConfig* _config, const edgeFrieze* _edge, f32 _uvCoeff, u32& _stepNb, f32& _stepCornerAngle, f32& _uvXStep, f32& _arcBy2, f32 _uvXsign ) const;
        f32     getCornerSquareArcBy2       (   f32 _cornerAngle, f32 _uvCoeff, Vec2d _vector, f32 _uvXsign ) const;
        void    setEdgeRunNormUv            (   const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun ) const;
        u32     getIdEdgeStartWithoutSnap   (   ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun ) const;
        f32     getStepCornerCoeff          (   f32 _uvCoeff, f32 _angle ) const;
        bbool   flipEdge                    (   f32 _offset ) const;
        void    updateUvs                   (   const edgeRun& _edgeRun, const edgeFrieze* _edge, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign, f32 _uvUpXOffSet, f32 _uvDownXOffSet ) const;
        void    updateEdgeCornerUvs         (   CornerRounder& _corner, f32& _vtxUvX, f32 _uvLeft, f32 _uvRight ) const;
        f32     getCornerRounderUv          (   const FriseConfig* config, f32 _angle, f32 _uvCoeff, f32 _height, f32 _scale ) const;
        f32     getUvCornerBy2              (   const FriseConfig* _config, edgeRun& _edgeRun, edgeFrieze* _edgeCur, edgeFrieze* _edgeLast) const;
        void    setEdgeNormUv               (   edgeFrieze* _edge, f32 _ratio, edgeFrieze* _edgeLast, edgeFrieze* _edgeNext) const;
        f32     getCoeffScaleStepFanToBlend (   const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast) const;

        // In Roundness
        void    buildFrieze_InRoundness     (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );    
        void    buildVB_InRoundness_Static  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        void    buildRoundness_Static       (   const FriseConfig* config, IndexList& _indexList, struct UvsByAngle_InRoundness* _uvTab, Vec2d& _vCenter, Vec2d& _vStart, Vec2d& _vStop, f32 _angle, u32 _vtxColorCenter, u32 _vtxColorCorner );
        void    buildCollision_InRoundness  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        bbool   buildEdgeRunList_InRoundness(   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        u32     buildRoundness_getIndexUv   (   f32 _angle ) const;
        i32     getTexIdBySlope_InRoundness (   Vec2d _sight, const FriseConfig* _pConfig ) const;
        u32     getIdStartEdge_InRoundness  (   ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* _pConfig ) const;
        bbool   isEdgeRun_InRoundness       (   ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const;
        bbool   isAngleBreak_InRoundness    (   edgeFrieze& _edge ) const;

        // In Generic
        void    buildFrieze_InGeneric       (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        bbool   buildEdgeRunList_InGeneric  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    buildCollision_InGeneric    (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    buildVB_InGeneric           (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );

        // Build Fill
        f32     getFillOffSet               (   const FriseConfig* config, const edgeRun* _edgeRun ) const;
        void    buildFilling                (   const ITF_VECTOR<edgeFrieze>& _edgeList, const ITF_VECTOR<edgeRun>& _edgeRunList );
        void    buildFilling_VB             (   const FriseConfig* config, SafeArray<Vec2d>& filling, IndexList& _indexList );
        void    setFillingSafe              (   const FriseConfig* config, const SafeArray<Vec2d>& _fillingOld, SafeArray<Vec2d>& _fillingSafe ) const;
        void    buildVB_FillGradient        (   ITF_VECTOR<edgeFrieze>& _edgeList );
        void    buildFilling_Points         (   const FriseConfig* config, SafeArray<Vec2d>& filling, const ITF_VECTOR<edgeFrieze>& _edgeList, const ITF_VECTOR<edgeRun>& _edgeRunList ) const;

        // Build Outline
        void    buildOutline_EdgeRunWithCornerRounder   ( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRun, f32 _offset, f32 _stepFan, u32 _edgeCountStartOffset = 0, u32 _edgeCountStopOffset = 0, bbool _isOutlineCollision = btrue ) const;
        void    buildOutline_StartEdgeRunCornerRounder      ( const FriseConfig* config, SafeArray<Vec2d>& _outline,  const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRunLast, const edgeRun* _edgeRunCur, f32 _offsetLast, f32 _offsetCur, f32 _stepFan, bbool _isOutlineCollision = btrue ) const;        
        void    buildOutline_CornerRounder  (   SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offset, f32 _stepFan ) const;
        void    buildOutline_CornerSquare   (   SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offset ) const;
        void    buildOutline_EdgeRunWithCornerSquare ( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRun, f32 _offset, u32 _edgeCountStartOffset = 0, u32 _edgeCountStopOffset = 0, bbool _isOutlineCollision = btrue) const;
        void    buildOutline_StartEdgeRunCornerSquare ( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRunLast, const edgeRun* _edgeRunCur, f32 _offsetCur, f32 _offsetLast, bbool _isOutlineCollision = btrue) const;
        f32     getCornerEdgeNorm           (   f32 _angle, f32 _radius ) const;
        void    setPos_CornerSquareBlending (  Vec2d& _posStart, Vec2d& _posStop, Vec2d& _posSquare, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offsetCur, f32 _offsetLast ) const;
        void    buildOutline_CornerSquareBlending_startEdgeRun( SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeCur, f32 _offsetCur, f32 _offsetLast ) const;
        void    buildOutline_CornerSquareBlending_stopEdgeRun( SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeCur, f32 _offsetCur, f32 _offsetNext ) const;
        void    buildOutline_initPosCornerRounderBlending( const edgeFrieze* _edgeLast, const edgeFrieze* _edgeCur, Vec2d &_p1, Vec2d &_p2, f32 _offsetCur = 1.f, f32 _offsetLast = 1.f) const;
        void    buildOutline_CornerRounderBlending( SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeLast, const edgeFrieze* _edgeCur, f32 _offsetLast, f32 _offsetCur, f32 _stepFan, i32 _countOffset = 0, Vec2d* _posCorner = NULL) const;
        void    buildOutline_CornerSquareBlending( SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offsetCur, f32 _offsetLast ) const;
        bbool   buildOutline_isStartEdgeRunWithRounder( const FriseConfig* _config ) const;
        bbool   buildOutlineFromHole( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze* edgeCur, const edgeFrieze* edgeLast, f32 _offsetLast, f32 _offsetCur, bbool _isOutlineCollision ) const;
        bbool   buildOutlineFromHoleVisual( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze* edgeCur, const edgeFrieze* edgeLast, f32 _offsetLast, f32 _offsetCur ) const;
        bbool   buildOutlineFromHoleCollision( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze* edgeCur, const edgeFrieze* edgeLast, f32 _offsetLast, f32 _offsetCur ) const;
        void    buildOutline_PosStartOnEdgeWithHole( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio) const;
        void    buildOutline_PosStopOnEdgeWithHole( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio, u32 _idPoint) const;

        void    buildOutline_PosStopOnEdgeWithHoleCollision( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio, u32 _idPoint) const;
        void    buildOutline_PosStartOnEdgeWithHoleCollision( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio) const;


        // Mesh Common
        void    setVtxColor                 (   const FriseTextureConfig& _texConfig, u32& _vtxColorUp, u32& _vtxColorDown ) const;
        void    setVtxColorWithExtrem       (   const FriseTextureConfig& _texConfig, u32& _vtxColorUp, u32& _vtxColorDown, u32& _vtxColorUpExt, u32& _vtxColorDownExt, bbool _forceAlpha = bfalse ) const;
        void    addMeshElement              (   ITF_Mesh & _mesh, const FriseTextureConfig& _texConfig, u32 _indexTableSize ) const;        
        void    setQuadPos_StartExtremity   (   const FriseConfig* _pConfig, const edgeFrieze& _edge, Vec2d* _pos, f32 _heightMid, f32 _width, bbool _isSnaped, f32* _zExtrude ) const;
        void    setQuadPos_StopExtremity    (   const FriseConfig* _pConfig, const edgeFrieze& _edge, Vec2d* _pos, f32 _heightMid, f32 _width, bbool _isSnaped, f32* _zExtrude ) const;
        void    setQuadColor_StartExtremity (   u32* _color, u32 _vtxColorDown, u32 _vtxColorUp, u32 _vtxColorDownExtremity, u32 _vtxColorUpExtremity ) const;        
        void    setQuadColor_StopExtremity  (   u32* _color, u32 _vtxColorDown, u32 _vtxColorUp, u32 _vtxColorDownExtremity, u32 _vtxColorUpExtremity ) const;
        void    setVtxBorderLeftPosDown     (   const edgeFrieze& _edge, Vec3d& _vtxPos) const;
        void    setVtxBorderLeftPosUp       (   const edgeFrieze& _edge, Vec3d& _vtxPos) const;
        void    setVtxBorderRightPosDown    (   const edgeFrieze& _edge, Vec3d& _vtxPos) const;
        void    setVtxBorderRightPosUp      (   const edgeFrieze& _edge, Vec3d& _vtxPos) const;
        void    setVtxPosUp                 (   const Vec2d& _pos, Vec3d& _vtxPos) const;
        void    setVtxPosDown               (   const Vec2d& _pos, Vec3d& _vtxPos) const;
        void    setVtxPos                   (   const Vec2d& _pos, Vec3d& _vtxPos, f32 _z) const;
        void    setFlipQuadWithAlphaBorder  (   bbool& _flip, u32 _colorLeft, u32 _colorRight) const;
        void    setExtrudeExtremityZ        (   const FriseConfig* _pConfig );
        void    cleanIndexListArray         (   ITF_VECTOR<IndexList>& _indexListArray );

        // Mesh Anim
        void    clearMeshAnimData           (   );
        void    buildMesh_Anim              (   );   
        void    buildMesh_AnimFinal         (   );
        void    buildVB_Anim_EdgeRun        (   const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, IndexList& _indexList, u32 _vtxColorUp, u32 _vtxColorDown, u32 _vtxColorUpExtremity, u32 _vtxColorDownExtremity, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign );
        void    buildVB_Anim_CornerRounder  (   IndexList& _indexList, CornerRounder& _corner );
        void    buildVB_Anim_CornerSquare   (   IndexList& _indexList, CornerRounder& _corner );
        void    setVtxAnim                  (   const VertexAnim& _anim, VertexPNC3T& _vtx );
        void    setVtxAnimNull              (   VertexPNC3T& _vtx ) const;
        void    setVtxAnimOnEdge            (   const VertexAnim& _anim, const edgeFrieze& _edge, VertexPNC3T& _vtx );
        void    setVtxAnimOnEdge            (   const VertexAnim& _anim, Vec2d& _vector, VertexPNC3T& _vtx );
        void    setVtxAnimOnCorner          (   const VertexAnim& _anim, const edgeFrieze& _edge, VertexPNC3T& _vtx ); 
        void    copyVtxAnim                 (   VertexPNC3T& _vtxTo, VertexPNC3T& _vtxFrom ) const;
        void    addIndex_Anim_Quad          (   IndexList& _indexList, u16& _indexVtxDown, u16& _indexVtxUp, bbool _flip );
        void    buildVB_Anim_Quad           (   const VertexAnim& _anim, IndexList& _indexList, const Vec2d* _pos, const Vec2d* _uv, const u32* _color, bbool _flip, bbool _vtxAnimStart, bbool _vtxAnimStop, const f32* _zExtrude = NULL );
        void    buildVB_Anim_CornerRounderBlending( IndexList& _indexList, CornerRounder& _corner );
        void    buildVB_Anim_Corner         (   IndexList& _indexList, CornerRounder& _corner );
        void    buildVB_Anim_EdgeRunCorner  (   const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, IndexList& _indexList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32& _uvUpXOffSet, f32& _uvDownXOffSet, f32 _uvXsign );
        void    buildVB_Anim_addVtxOnStartPosEdge( const VertexAnim& _anim, const edgeFrieze& edge, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown );
        void    buildVB_Anim_addVtxOnStopPosEdge( const VertexAnim& _anim, const edgeFrieze& edge, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown );
        u32     getIndexPreviousAnimVertex  (   i32 _index ) const;

        //
        void    buildVB_Static_Corner       (   IndexList& _indexList, CornerRounder& _corner );
        void    buildVB_Static_CornerSquare (   IndexList& _indexList, Vec2d& _uvSquare, f32 _stepUvX, Vec2d& _vSquare, Vec2d& _vStop, u16 _indexVtxCenter, u16& _indexVtxSquare, f32 _angle, u32 _color );
        void    buildVB_Static_EdgeRunCorner( const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, IndexList& _indexList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32& _uvUpXOffSet, f32& _uvDownXOffSet, f32 _uvXsign );
        void    buildVB_Static_EdgeRun      (   const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, IndexList& _indexList, u32 _vtxColorUp, u32 _vtxColorDown, u32 _vtxColorUpExtremity, u32 _vtxColorDownExtremity, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign );        
        bbool   buildVB_Static_EdgeRunMesh  (   const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, IndexList& _indexList, u32 _vtxColorUp, u32 _vtxColorDown, u32 _vtxColorUpExtremity, u32 _vtxColorDownExtremity, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign );
        void    buildVB_Static_CornerRounder(   IndexList& _indexList, CornerRounder& _corner );
        void    buildVB_Static_CornerSquare (   IndexList& _indexList, CornerRounder& _corner );
        void    buildVB_Static_CornerRounderBlending( IndexList& _indexList, CornerRounder& _corner );

        // Collision
        void    initCollisionData           (   u32 _collisionWantedCount );
        f32     getCollisionOffSet          (   const FriseConfig* config, const edgeRun* _edgeRun, collisionRun& _colRun ) const;
        f32     getCollisionOffSetSimple    (   const FriseConfig* config, const edgeRun* _edgeRun ) const;
        const Path* getCollisionGameMaterial(   const FriseConfig* _friseConfig, i32 _idTex ) const;        
        bbool   isEdgeRunCollisionable      (   const FriseConfig* _friseConfig, const edgeRun& _edgeRun, const edgeFrieze& _edge ) const;
        void    buildCollision_EdgeRunList  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );        
        bbool   isEdgeRunWithCollisionHole  (   ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun, u32& _idEdgeRunStart, u32& _edgeCountStart, u32 _edgeRunListCount ) const;
        void    getIdEdgeRunStartCollision  (   const FriseConfig* _friseConfig, ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, u32& _idEdgeRunStart, u32& _idEdgeCountStart  ) const;
        void    initCollisionRun            (   const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, u32 _idEdgeRunStart, u32 _idEdgeStart, u32 _edgeCountStartOffset ) const;
        bbool   checkStartCollisionRun      (   const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, bbool& _build, u32& _idEdgeRunOld, u32 _idEdgeRun, u32 _idEdge, u32 _edgeCountStartOffset ) const;
        bbool   checkStopCollisionRun       (   const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList, collisionRun& _collisionRun, bbool& _build, u32 _edgeCountStopOffset, u32 _idEdgeRun );
        bbool   buildCollisionRunList       (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    addPointsCollision          (   const FriseConfig* _config, SafeArray<Vec2d>& collision, ITF_VECTOR<CollisionByGameMat>& _collisionList, const collisionRun& _colRun );
        void    finalizeCollisionList       (   );
        void    finalizeCollisionRun        (   const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, u32 _edgeCountStopOffset, u32 _idEdgeRun ) const;
        void    verifyCollision             (   ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList ) const;
        bbool   AddPointCollision           (   const FriseConfig* _config, PolyPointList& _pointList, const Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32& _indexPoint, const Path& _gameMat );
        void    AddPointCollisionWithGMat(   const FriseConfig* _config, PolyPointList& _pointList, const Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32& _indexPoint, const Path& _gameMat );
        bbool   skipEdgeParallelInCollisionData( const FriseConfig* _config, PolyPointList& _pointList, const Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32 _indexPoint, const Path& _gameMat ) const;
        void    buildWorldCollision         (   u32 _collisionCount );
        void    initializeMovingPolylinesAfterRecomputeData();

#ifdef USE_PHYSWORLD
        void    setWorldCollisionPhysBody   (   PolyLine* pPoly );
        void    initCollisionWorld          (   PolyLine* _pPol );
        void    deletePolylineCollisionAt   (   u32 _index );
#endif

        void    updateConnections();

#ifdef USE_BOX2D
        void    initPhys2d                  (   );
        void    addPhys2dCollision          (   PolyLine * _poly);
        void    addPhys2dCollision          (const PolyPointList & _polyList);
        void    createChainFixture          (b2Vec2 * list, i32 nbElemts, bbool isLoop);
#endif

        void    buildCollision_ExtremityWithCoord( const FriseConfig* config, SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun* _edgeRunCur, collisionRun& _colRun, f32 _offsetHeightCur, f32 _offsetHeightLast, u32 _startEdgeRun = U32_INVALID ) const;
        void    buildCollision_CornerSquare_StartEdgeRun( const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, SafeArray<Vec2d>& _outline, edgeFrieze* _edgeCur, u32 _idEdgeStart, f32 _offset, collisionRun& _collisionRun ) const;
        void    buildCollision_CornerSquare_StopEdgeRun( const FriseConfig* _config, SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeStop, f32 _offset, collisionRun& _collisionRun ) const;
        u32     buildCollisionRunListDouble   (   ITF_VECTOR<collisionRun>& _collisionRunList, const FriseConfig* _config ) const;
        void    setCollisionRunDoubleOffset (   collisionRun& _collisionRunDouble ) const;
        bbool   addPoinLastToCloseCollisionDouble( const collisionRun& _colRun, const PolyPointList& _pointList ) const;
        bbool   isParallelEdges             (  const  Vec2d& _posCur, const Vec2d& _posLast, const Vec2d& _posLastOld, f32 _distMax) const;
        void    setCollisionExtremityGameMaterial( const collisionRun& _colRun, PolyPointList& _pointList, bbool _startExtremity) const;
        Vec2d   buildCollision_getCornerExtremityWithCoord( const FriseTextureConfig& _texConfig, collisionRun& _colRun, edgeFrieze* _edgeCur, edgeFrieze* _edgeLast, f32 _offsetCur, f32 _offsetLast) const;

        //  In Extremity
        void    buildFrieze_InExtremity     (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, const f32 _texRatio );
        bbool   buildEdgeRunList_InExtremity(   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    buildVB_InExtremity         (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, f32 _extremityRatio, const Vec2d& _uvXExtremityStart, const Vec2d& _uvXExtremityStop );
        void    buildCollision_InExtremity  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        f32     getExtremityLong            (   f32 _ratio ) const;
        void    setUvXExtremity_InExtremity (   Vec2d& _uvXExtremityStart, Vec2d&  _uvXExtremityStop ) const;

        // In Extremity Simple
        void    buildFrieze_InExtremitySimple(  ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        void    setUvXExtremity_InExtremitySimple( Vec2d& _uvXExtremityStart, Vec2d& _uvXExtremityStop ) const;        

        //  In Archi
        void    buildFrieze_InArchi         (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        bbool   buildEdgeRunList_InArchi    (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        u32     getZoneId_InArchi           (   const Vec2d& _sight ) const;
        u32     getIdStartEdge_InArchi      (   ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        void    buildVB_InArchi_Static      (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, UvLinearArchi* _uvLinear );
        void    buildCollision_InArchi      (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    buildCollision_InArchi_CornerSquare( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        bbool   insertNewEdge_InArchi       (   ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* config, u32 idCurEdge, u32 idLastEdge );
        bbool   isEdgeRun_InArchi           ( ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const;
        Frise::edgeFrieze   buildNewEdge_InArchi        (   ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* config, u32 idCurEdge, u32 idLastEdge) const;
        void    insertNewEdgeInEdgeList     (   ITF_VECTOR<edgeFrieze>& _edgeList, edgeFrieze& _newEdge, u32 _index );
        void    buildVB_Static_CornerSquareExt( const FriseConfig* config, IndexList& _indexList, const edgeFrieze& _edgeCur, const edgeFrieze& _edgeLast, Vec2d* _uv, u32 _colorUp, u32 _colorDown, bbool _flipEdge, f32 _varianceForced = -1.f );
        void    setUvByAngleInArchi         (   ) const;

        // In Archi Simple
         void   buildFrieze_InArchiSimple   (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );

        // In Fluid
        void    buildFrieze_InFluid         (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        bbool   buildEdgeRunList_InFluid    (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    buildEdgeFluidList          (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        i32     getIdStartEdge_InFluid      (   ITF_VECTOR<edgeFrieze>& _edgeList );
        bbool   isEdgeValid_InFluid         (   const edgeFrieze& _edge ) const;
        void    buildCollision_InFluid      (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    buildCollision_EdgeFluidList(   const FriseConfig* _config, u32 _sideIndex );
        void    setEdgeFluidVertexColors    (   const FriseTextureConfig& _texConfig, ITF_VECTOR<EdgeFluid>& _edgeFluidList ) const;
        void    initDataFluid               (   );
        void    clearMeshFluidData          (   );
        void    setEdgeFluidPosWithHoleVisualOnPreviousEdge( EdgeFluid& _edge, const Vec2d& _sightNormalized );
        void    setEdgeFluidPosWithHoleVisualOnNextEdge( EdgeFluid& _edge, const Vec2d& _sightNormalized  );
        void    initDataCooked_InFluid      (   );

        // In String
        void    buildFrieze_InString        (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        void    buildCollision_InString     (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        
        // In Pipe
        void    buildFrieze_InPipe          (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        bbool   insertNewEdge_InPipe        (   ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* config, u32 idCurEdge, u32 idLastEdge );
        u32     getZoneId_InPipe            (   const Vec2d& _sight ) const;
        u32     getIdStartEdge_InPipe       (   ITF_VECTOR<edgeFrieze>& _edgeList, f32 _nornMin ) const;
        bbool   checkEdgeIsValid_InPipe     (   const edgeFrieze& _edge, f32 _nornMin ) const;
        bbool   buildEdgeRunList_InPipe     (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    buildVB_InPipe_Static       (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        void    buildVB_Static_CornerSquareExt_InPipe( const FriseConfig* config, IndexList& _indexList, edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, Vec2d* _uv, u32 _colorUp, u32 _colorDown, bbool _flipEdge );
        bbool   isEdgeRun_InPipe            (   ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const;
        
        void    buildCollision_InPipe       (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    buildCollision_InPipe_CornerSquare( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );

        
        void    buildVB_Static_addVtxOnStartPosEdge( const FriseConfig* _config, const edgeFrieze& edge, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, edgeFrieze* _edgeLast = NULL );
        void    buildVB_Static_addVtxOnStopPosEdge(    const edgeFrieze& edge, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown );
        
        void    setFillParams               (   const FriseConfig* _config, FillingParams& _params ) const;
        void    setFillUv                   (   FillingParams& _params, Vec2d& _uv, const Vec2d& _pos) const;

        // In Atlas
        void    buildFrieze_InAtlas         (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, u32& _atlasIndex );
        void    setEdgeRunCoeffUv_InAtlas   (   const ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun) const;
        void    buildVB_InAtlas             (   ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun, Vec2d* _uvUp, Vec2d* _uvDown );
        f32     setRatioFixed_InAtlas       (   const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList, Vec2d* _uvUp, Vec2d* _uvDown );
        void    MatchEdgesWithRatioFixed( ITF_VECTOR<edgeFrieze>& _edgeList, f32 _longWanted) const;
        void    getUvData                   (   const FriseTextureConfig& _texConfig, const UVdata*& _pUVdata, const UVparameters*& _pUVparameter );
        void    buildFrieze_InAtlasFromRectangle( const FriseConfig* _pConfig, const UVdata* _pUvData, ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        void    buildFrieze_InAtlasFromListOfPoints( const UVdata* _pUvData, const UVparameters* _pUvParam, u32 _idTex );
        void    buildFrieze_InAtlasFromMesh (   const UVdata* _pUvData, const UVparameters* _pUvParam, u32 _idTex );
        f32     getAnimAngle_InAtlas        ( const FriseConfig* _pConfig ) const;

        // In Overlay
        void    buildVB_InOverlay           (   const FriseConfig* _config, const FriseTextureConfig& _texConfig, const ITF_VECTOR<edgeFrieze>& _edgeList );
        void    buildFrieze_InOverlay       (   ITF_VECTOR<edgeFrieze>& _edgeList );
        void    clearMeshOverlayData        (   );
        void    buildMesh_Overlay           (   u32 _idTexConfig );
        void    buildMesh_OverlayFinal      (   );
        void    buildVB_Overlay_Quad        (   Vec2d* _pos, Vec2d* _uv, u32* _color, u32 _idAnim );        
        void    setMeshOverlayDataMatrix    (   const VertexAnim* _anim );

        // In Frame
        void    buildFrieze_InFrame         (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        u32     getIdStartEdge_InFrame      (   ITF_VECTOR<edgeFrieze>& _edgeList, f32 _nornMin ) const;
        bbool   buildEdgeRunList_InFrame    (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        void    buildVB_InFrame_Static      (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        
        // In Pipe Patch Simple
        void    buildFrieze_InPipePatchSimple(  ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );
        bbool   buildEdgeRunList_InPipePatch(   const ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        u32     getZoneId_InPipePatch       (   const Vec2d& _sight ) const;
        u32     getIdStartEdge_InPipePatch  (   const ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        void    buildVB_InPipePatch_Static  (   ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, const UvLinear_InPipePatch* _uvLinear );
        void    transformEdgeList_InPipePatch( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const;
        Vec2d   getAndSetBorderStartOffset_InPipePatch( const FriseConfig* _pConfig, edgeFrieze& _edge, bbool _isSingleEdge, edgeFrieze* _pEdgeNext ) const;        
        Vec2d   getAndSetBorderStopOffset_InPipePatch( const FriseConfig* _pConfig, edgeFrieze& _edge, bbool _isSingleEdge, edgeFrieze* _pEdgeNext ) const;
        void    offsetStartBorderEdgeRun_InPipePatch( const FriseConfig* _pConfig, const edgeRun& _edgeRun, ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        void    offsetStopBorderEdgeRun_InPipePatch( const FriseConfig* _pConfig, const edgeRun& _edgeRun, ITF_VECTOR<edgeFrieze>& _edgeList ) const;
        void    setPositionExtremityOnStartEdgeRun_InPipePatch( const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, Vec2d* _pos, const f32 _coeffOffset, const Vec2d& _scale ) const;
        void    setPositionExtremityOnStopEdgeRun_InPipePatch( const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, Vec2d* _pos, const f32 _coeffOffset, const Vec2d& _scale ) const;
        void    setNormOffsetRatioAndValidEdge( const FriseConfig* pConfig, edgeFrieze& _edge, bbool _start, bbool _isSingleEdge, edgeFrieze* _pEdgeNext ) const;
        void    buildCollision_InPipePatch( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    buildCollision_InPipePatchSimple( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList );
        void    getCenterPatch_InPipePatch( const edgeFrieze& _edge, Vec2d& _pos ) const;
        void    buildCollision_OnPatch( const FriseConfig* _pConfig, SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun* _edgeRunCur, collisionRun& _colRun, f32 _offsetCur, f32 _offsetLast, u32 _startEdgeRun = U32_INVALID) const;
    
		// In 3D
		void buildFrieze_In3D(  ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList );

private:	
        void    updateTransformationFromLocalToWorld        (   );
        void    updateTranslated            (const Vec3d & _prevPos);
        void    setLocalToWorldMatrix       (   Matrix44* _pMatrix );
        void    updateMeshMatrixAndAabb     (   const Transform2d& _xf );
        void    updateMeshMatrixAndAabb     ( const Vec2d & _deltaPos );
        void    updatePolylineCollision     (   const Transform2d& _xf );
        void    updatePolylineCollision     (   const Vec2d& _deltaPos );
        void    transformLocalAabbToWorld   (   const AABB& _local, AABB& _world, const Transform2d& _xf);
        void    growAabbLocalFromVertexAnim (   AABB& _aabb);        
        void    updateFluidData             (   const Transform2d& _xf);
        void    updateFluidData( const Vec2d & _deltaPos);
        void    requestTemplateType();
        virtual const TemplatePickable* requestTemplateType(const Path& _path) const;

        void	executeRecomputeLineData();
        void    executeRecomputeDataCooked();
        void    checkUpdateTransformForDataCooked();

        void    acquireMaterialOverride();
        void    releaseMaterialOverride();

        void    removeAutoBlendCopyMode();
        void    removeAutoBlendCopyMode(ITF_Mesh & _mesh);

#ifdef ITF_SUPPORT_EDITOR      
        void    preProperties               (   );
        void    onPostPropertyChange          (   );   
#endif // ITF_SUPPORT_EDITOR

        // Event
        Color   m_eventShowColorScr;
        Color   m_eventShowColorDst;
        f32     m_eventShowTime;
        f32     m_eventShowTimeSpeed;
        bbool   m_eventShowPauseOnEnd;
        bbool   m_eventShowDestroyOnEnd;

        void    processUpdateColor(f32 _dt);
        ObjectRefList       m_eventListeners;     
    };
    
} // namespace ITF

#endif  // _ITF_FRISE_H_
