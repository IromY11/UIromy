#ifndef _ITF_FRIEZE_DRAW_H_
#define _ITF_FRIEZE_DRAW_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_FLUIDSHAPER_H_
#include "engine/display/Frieze/FluidShaper.h"
#endif //_ITF_FLUIDSHAPER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

namespace ITF
{
    class EdgeProcessData
    {
        DECLARE_SERIALIZE()

    public:
        i32         m_id;
        i32         m_indexStart;
        i32         m_indexEnd;
        i32         m_count;
        Vec2d       m_posStart;
        Vec2d       m_posEnd;
        Vec2d       m_posOffset;
        Vec2d       m_normal;

        EdgeProcessData() 
            : m_id(-1)
            , m_indexStart(0)
            , m_indexEnd(0)
            , m_count(0)
            , m_posStart(Vec2d::Zero)
            , m_posEnd(Vec2d::Zero)
            , m_posOffset(Vec2d::Zero)
            , m_normal(Vec2d::Zero)
        {}
    };

#define EDGEFLUID_MAIN_DATA      0x1
#define EDGEFLUID_VISUAL         0x2
#define EDGEFLUID_COLLISION      0x4

    struct EdgeData
    {
        DECLARE_SERIALIZE()

        EdgeData()
            : m_level(1.f)
            , m_pos(0.f)
            , m_way(0)
            , m_cutUV(bfalse)
            , m_heightScale(1.f)
            , m_heightScaleTotal(1.f)
            , m_heightPos(1.f)
            , m_posZ(0.f)
        {
        }

        f32         m_level;
        f32         m_pos;

        i32         m_way; // 0 -> normal, -1 perpendicular back, 1 perpendicular front
        bbool       m_cutUV;

        f32         m_heightScale;
        f32         m_heightScaleTotal;
        f32         m_heightPos; // 1-> base, less move on edge
        f32         m_posZ;


    };

    typedef SafeArray<Vec2d, NB_FLUID_LEVELS> FluidHeightMap;
    typedef SafeArray<u8,NB_FLUID_LEVELS> FluidColorMap;
    typedef SafeArray<u8,NB_FLUID_LEVELS> FluidGameMaterialMap;

    class EdgeFluid
    {
        DECLARE_SERIALIZE()

    public:

        EdgeFluid() 
            : m_material()
            , m_meshLevel(8)
            , m_lastIndex(NB_FLUID_LEVELS-1)
            , m_uvAnimTrans(Vec2d::Zero)
            , m_delta(Vec3d::Zero)
            , m_deltaUV(Vec2d::Zero)
            , m_depth(0.f)
            , m_localNorm(0.f)
            , m_worldNorm(0.f)
            , m_worldHeight(0.f)
            , m_worldAngle(0.f)
            , m_useBlend(bfalse)
        {
            m_pos.resize(4);
            m_uv.resize(4);
            m_colors.resize(GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT);
            m_localAABB.invalidate();
            m_collisionProcesses.resize(maxCollisionProcessCount);
        }

        FixedArray<Vec2d, 4>                                            m_pos;
        FixedArray<Vec2d, 4>                                            m_uv;
        FixedArray<ColorInteger, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> m_colors;
        
        static const u32                    maxCollisionProcessCount=2;
        
        GFX_MATERIAL                        m_material;
        Vec2d                               m_uvAnimTrans;

        u32                                 m_meshLevel;
        f32                                 m_depth;
        FluidHeightMap                      m_heightLevelSet[MAX_FLUID_LAYER_COUNT];
        FluidHeightMap                      m_heightLevelBlended[MAX_FLUID_LAYER_COUNT];
        FluidColorMap                       m_fluidColorMap;
        FluidGameMaterialMap                m_fluidGameMaterialMap;
        u32                                 m_lastIndex;

        f32                                 m_localNorm;
        f32                                 m_worldNorm;
        f32                                 m_worldHeight;
        f32                                 m_worldAngle;
        Vec3d                               m_delta;
        Vec2d                               m_deltaUV;
        bbool                               m_useBlend;

        ITF_VECTOR<EdgeProcessData>         m_collisionProcesses; //collisions, for each side (not necessarily used)
        EdgeProcessData                     m_visualProcess;
        AABB                                m_localAABB;
        AABB                                m_worldAABB;
        
        void        FillFluidToDraw(const EdgeData *_data, const ITF_VECTOR<FluidFriseLayer> &_layerInfos, const Transform2d& _xf, f32 _scale, FluidToDraw & _dstFluidToDraw);
        void        updateData( const class FriseConfig *config, u32 _mask);
        void        fillPolyline(PolyPointList& _polypoint, u32 _mask, u32 _subIndex);
        Vec3d       getShift(f32 percent) const { return m_delta*percent; }

        FluidHeightMap &    getHeightLevelSet       (u32 _idx) { return m_heightLevelSet[_idx]; }
        FluidHeightMap &    getHeightLevelBlended   (u32 _idx) { return m_useBlend ? m_heightLevelBlended[_idx] : m_heightLevelSet[_idx]; }

    };

    struct ActorInfo
    {
        ActorInfo() { m_prevPos = Vec2d::Zero; m_prevInfluence = 0.f; m_mass=0.f; m_radius = 1.f; m_waterMultiplier = 1.f;m_queryPosition = bfalse; }
        ObjectRef   m_ref;
        Vec2d       m_prevPos;
        f32         m_prevInfluence;
        f32         m_mass;
        f32         m_radius;
        f32         m_waterMultiplier;
        bbool       m_queryPosition;
    };

    struct EdgeFluidLevel
    {
        DECLARE_SERIALIZE()

        EdgeFluidLevel()
            : m_edgeFluid(NULL)
            , m_scale(1.f)
            , m_idEdgeFluid(0)
        {
            m_xf.setIdentity();
        }

        EdgeFluid       *m_edgeFluid;
        u32             m_idEdgeFluid; // requiered to serialize
        EdgeData        m_data;
        Transform2d     m_xf;
        f32             m_scale;

        ITF_VECTOR<FluidFriseLayer> m_layerInfos;

        f32 getDepth() const
        {
            if (m_data.m_way == 0)
                return m_edgeFluid->m_depth + m_edgeFluid->getShift(m_data.m_pos).z();
            else
                return m_edgeFluid->m_depth + m_data.m_pos;
        }
        f32 getTrueDepth() const
        {
            if (m_data.m_way == 0)
                return m_edgeFluid->m_depth + m_edgeFluid->getShift(m_data.m_pos).z();
            else
                return m_edgeFluid->m_depth +  m_data.m_posZ + m_data.m_pos*m_data.m_heightScale*m_edgeFluid->m_worldHeight;
        }
    };

    class FullFluidToDraw : public GFXPrimitive
    {
    public:
        ITF_VECTOR<FluidToDraw>     m_fluidList;
        i32                         m_mode;
        f32                         m_depth;
        AABB                        m_worldAABB;

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);
    };

    class DataFluid
    {
        DECLARE_SERIALIZE()

        enum FluidFxType
        {
            FluidFxType_Enter,
            FluidFxType_Leave,
            FluidFxType_Swim,
            FluidFxType_Plouf
        };


    public:

        DataFluid() 
            : m_isCushion(bfalse)
            , m_weightMultiplier(1.f)
            , m_fxActor(NULL)
            , m_fxBindDone(btrue)
        {
            m_frieze.invalidate();
        }

        ~DataFluid();

        void                        setFrieze(class Frise * _frieze);
        class Frise *               getFrieze();

        void                        deleteFluidShapers();

        void                        addActor(ObjectRef _actorRef, f32 _speed, f32 _radius, f32 _waterMultiplier, f32 _weight, bbool _queryPosition, bbool _noFX );
        void                        removeActor(ObjectRef _actorRef, f32 _speed, bbool _noFX );
        
        void                        addStringWaveGenerator(ObjectRef _actorRef);
        void                        removeStringWaveGenerator(ObjectRef _actorRef);
        const SafeArray<ObjectRef>& getStringWaveGenerators() const {return m_stringWaveGenerators;}

        void                        sendPerturbation(const Vec2d& _pos, f32 _alteration, f32 _radius, bbool radiusRandom, f32 _mass);

        void                        computeEdgeFluidLevels(u32 _levelsFront, u32 _levelBack, bbool _cutUV);
        void                        addEdgeFluidLevelPerpendicular(i32 _way, f32 _heightScale, f32 _heightScaleTotal, f32 _heightPos, f32 _posZ, f32 _z); // _cutUV = true
        void                        computeAABB( AABB& _aabb );

        ITF_VECTOR<EdgeFluid>&      getEdgeFluid()       { return m_edgeFluidList; }
        ITF_VECTOR<EdgeFluidLevel>& getEdgeFluidLevels() { return m_edgeFluidListLevels; }

        void                        ProcessEdgeFluidList_InFluid(f32 _elapsed);
        void                        ProcessEdgeFluidList_InString(f32 _elapsed);
        void                        checkActorInfluence();
        void                        handlePropagation(Frise *frieze, const FriseConfig *config);
        void                        handleAbsorptionAtEdgeStartEnd_String( const FriseConfig *config);
        void                        copyFromSetToBlend(const FriseConfig *config);
        void                        checkElevationMapInitialization( const FriseConfig *config);
        void                        processVirtualPerturbers(Frise *frieze, const FriseConfig *_config);
        void                        updateCollisionPolyLine_InFluid(Frise *frieze, const FriseConfig *config, const Transform2d& _xf);
        void                        updateCollisionPolyLine_InString(Frise *frieze, const FriseConfig *config, const Transform2d& _xf);
        void                        updateVisualPolyLine(Frise *frieze, const FriseConfig *config, const Transform2d& _xf);
        void                        clear();

        void                        setIsCushion(bbool _isCushion) {m_isCushion = _isCushion;}
        bbool                       isCushion() const {return m_isCushion;}

        void                        setWeightMultiplier(f32 _multiplier) {m_weightMultiplier = _multiplier;}
        void                        setLayers(const ITF_VECTOR<FluidFriseLayer> &_layerInfos) {m_layerInfos = _layerInfos;}
        void                        spawnFxActor(const Path & _path);
        void                        bindFxActor();

        void                        addFluidShaper(u32 _layerIndex, FluidShaper *_shaper);
        void                        removeFluidShaper(u32 _layerIndex, FluidShaper *_shaper);

        void                        setLayerCollisionHeightMultiplier(u32 _layerIndex, f32 _multiplier);
        u32                         getLayerCount() const {return m_layerInfos.size();}
        void                        resetAllLayerSettings();
        
        ITF_VECTOR<FullFluidToDraw>&    getDrawDataList() { return  m_drawDataList; }
        Actor *                         getFxActor() { return m_fxActor; }

    private:
        void                        computeInfluence(Vec2d _pos, f32 speedNorm, f32 _raidus, f32 * prevInfluence, f32 way , bbool _radiusRandom, f32 _mass, u32 & _edgeIdx ,u32 & _pointIdx);
        ITF_INLINE f32              computeFluidHeight_InString( const FriseConfig *_config, EdgeFluid &_edge, u32 _edgeIndex, i32 _indexForCollision, u32 _sideIndex, StringID::StringIdValueType &gameMaterialID);
        void                        applyLocalInfluence(Frise *_frieze, const FriseConfig *_config, f32 _deltaHeight, i32 _edgeIndex, i32 _referencePointIndex, i32 _startIndex, i32 _endIndex, i32 _indexStep, bbool _radiusRandom, i32 _influenceRadiusIndices);
        void                        updateUV(f32 _elapsed);
        void                        processFx(u32 _edgeIdx ,u32 _pointIdx, FluidFxType _fxType);

        ObjectRef                   m_frieze;
        ITF_VECTOR<EdgeFluid>       m_edgeFluidList;
        ITF_VECTOR<EdgeFluidLevel>  m_edgeFluidListLevels;
        SafeArray<ActorInfo>        m_contactActorsInfo;
        SafeArray<ObjectRef>        m_stringWaveGenerators;
        bbool                       m_isCushion; //cushion is malleable solid
        f32                         m_weightMultiplier;
        ITF_VECTOR<FluidFriseLayer> m_layerInfos;
        SafeArray<FluidShaper*>     m_virtualPerturbers[MAX_FLUID_LAYER_COUNT];
        ITF_FORCE_INLINE u8         getColorIndexAt_InString(const EdgeFluid *_edge, u32 _edgeIndex, i32 _posIndex) const;
        ITF_FORCE_INLINE u8         getGameMaterialIndexAt_InString(const EdgeFluid *_edge, u32 _edgeIndex, i32 _posIndex) const;
        
        Actor *                     m_fxActor;
        bbool                       m_fxBindDone;

        ITF_VECTOR<FullFluidToDraw> m_drawDataList;
    };


} // namespace ITF

#endif  // _ITF_FRIEZE_DRAW_H_
