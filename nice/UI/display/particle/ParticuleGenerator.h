#ifndef _ITF_PARTICULEGENERATOR_H_
#define _ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_PARTICULEGENERATORPARAMETERS_H_
#include "engine/display/particle/ParticuleGeneratorParameters.h"
#endif //_ITF_PARTICULEGENERATORPARAMETERS_H_

#ifndef _ITF_COLORINTEGER_H_
#include "core/ColorInteger.h"
#endif //_ITF_COLORINTEGER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_ANIMATIONMESHVERTEX_H_
#include    "engine/animation/AnimationMeshVertex.h"
#endif // _ITF_ANIMATIONMESHVERTEX_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H

#ifndef ITF_GFX_PRIMITIVES_H_
#include "engine\display\Primitives\GFXPrimitives.h"
#endif

#ifndef _ITF_PLUGIN_FXEDITOR_H_
#include "tools/plugins/FxEditorPlugin/Plugin_FxEditor.h"
#endif // _ITF_PLUGIN_FXEDITOR_H_

#ifndef ITF_ENGINE_MESH_H_
# include "engine/display/Primitives/Mesh.h"
#endif //ITF_ENGINE_MESH_H_

#define MAX_LONG 2147483647

//#define PARTICLE_GENERATOR_USE_PLACEMENT_NEW

#ifdef ITF_SUPPORT_MULTITHREADED
#ifdef ITF_CAFE
# define DIRECTVB_JOB
#else // ITF_CAFE
# define CACHEDVB_JOB
#endif // ITF_CAFE
#endif // ITF_SUPPORT_MULTITHREADED

//#define PAR_USE_TIMEVELTARGET
//#define PAR_USE_ZSORT

namespace ITF
{
    class BezierBranch;

    // Linear Congruential Method
    class RandomLCG
    {
        protected:
        mutable	long		mSeed;
        public:

                void     SetNewSeed();
        inline	long	 GetSeed()		const		{ return mSeed; }

                void     SetSeed(long s) { mSeed = s; }

        unsigned long    RandI() const
        {
            if(mSeed <= msQuotient)
                mSeed = (mSeed * 16807L) % MAX_LONG;
            else
            {
                const long high_part = mSeed / msQuotient;
                const long low_part  = mSeed % msQuotient;

                const long test = (16807L * low_part) - (msRemainder * high_part);

                if(test>0)	mSeed = test;
                else		mSeed = test + MAX_LONG;
            }
            return mSeed;
        }

        inline float    RandF()			const			// 0.0 .. 1.0 float generator
                        {
                            // default: multiply by 1/(2^31)
                            return float(RandI()) * float(1.0f/2147483647.0f);
                        }

        inline long		getRandI(long i, long n)		// i..n integer generator
                        {
                            //ASSERT(i<=n);
                            return (long)(i + (RandI() % (n - i + 1)) );
                        }   

        inline float    getRandF(float i, float n)			// i..n float generator
                        {
                            //ASSERT(i<=n);
                            return (i + (n - i) * RandF());   
                        }   
        public:
                        RandomLCG()
                        {
                            SetNewSeed();
                        }
                        RandomLCG(long s)
                        {
                            SetSeed(s);
                        }

        protected:
        static	const long msQuotient;
        static	const long msRemainder;
    };


/// flags:

const int PAR_F_ORIENTDIR           = 0x00000001; // Orient angle in particle direction.
const int PAR_F_ORIENTDIRGEN        = 0x00000002; // Orient angle in generator direction.
//const int PAR_F_UNIFORMSCALE        = 0x00000002; // Uniform scale scale y = scale x.
const int PAR_F_USEANIM             = 0x00000004; // Uniform scale scale y = scale x.
const int PAR_F_ANIM_STRETCHTIME    = 0x00000008;
const int PAR_F_ANIM_BLEND          = 0x00000010;
const int PAR_F_BLENDPHASE          = 0x00000020;
const int PAR_F_LOOP                = 0x00000040; // allow particle loop
const int PAR_F_UVINVERT_X          = 0x00000080; // invert uv x
const int PAR_F_UVINVERT_Y          = 0x00000100; // invert uv y
const int PAR_F_FLIPPEDATSPAWN      = 0x00000200; // set if the particle is flipped at spawn. 
const int PAR_F_FROZEN              = 0x00000400; // set if the particle is frozen. 

struct ITF_Particle
{
    ITF_Particle():m_curSize(Vec2d::Zero),m_initSize(Vec2d::Zero),m_dstSize(Vec2d::Zero),
                   m_pos(Vec3d::Zero),m_vel(Vec3d::Zero), m_velAcc(Vec3d::Zero), m_animId(U32_INVALID),
                   m_indexUV(0), m_bAlive(bfalse), m_bezierDist(0.f), m_bounceCount(0), m_seed(0), m_posYInit(0.0f)
#ifdef PAR_USE_TIMEVELTARGET
                   ,m_velTarget(Vec3d::Zero)
#endif
    {
    }

    ITF_INLINE void live()  {m_bAlive = btrue;}
    ITF_INLINE void dead()  {m_bAlive = bfalse;}
    
    u32 /* 4*/      m_flag:28;
    u32             m_colCurveID:3;
    u32             m_bAlive:1;
    u32             m_seed;
    i32 /* 8*/      m_startAnimIndex;
    i32 /*12*/      m_endAnimIndex;
    u32 /*16*/      m_animId; // MeshAnimVertex

    u32 /*24*/      m_curPhase;

    f32	/*28*/   	m_initTime;
    f32 /*32*/      m_phaselifeTime;

    i32 /*36*/      m_indexUV;

    Angle /*40*/    m_angle;
    Angle /*44*/    m_angleInit;
    Angle /*48*/    m_angularSpeed;

    Vec2d /*52*/    m_curSize;
    Vec2d /*56*/    m_initSize;
    Vec2d /*60*/    m_dstSize;

    Vec3d /*64*/    m_pos;
    Vec3d /*68*/    m_vel;
    Vec3d           m_velAcc;
    Vec3d           m_velReal;
    f32   /*72*/    m_bezierDist;
    
    ColorInteger /*76*/ m_curColor;
    ColorInteger /*80*/ m_initColor;
    ColorInteger /*84*/ m_dstColor;

    f32             m_birthTime;
    f32             m_dieTime;

    u32             m_bounceCount;
    u32             m_bounceMax;

    f32             m_posYInit;

#ifdef PAR_USE_TIMEVELTARGET
    f32             m_timeTarget;
    f32             m_curtimeTarget;
    Vec3d			m_velTarget;
#endif
#ifdef PAR_USE_ZSORT
    f32             m_zsort;
#endif // PAR_USE_ZSORT

    u8 GetSeed1() { return (u8)(m_seed & 0xFF); }
    u8 GetSeed2() { return (u8)((m_seed >> 8) & 0xFF); }
    u8 GetSeed3() { return (u8)((m_seed >> 16) & 0xFF); }
    u8 GetSeed4() { return (u8)((m_seed >> 24) & 0xFF); }
    u8 GetSeed5() { return (u8)(((m_seed >> 7) & 0x0F) + 0x10 * ((m_seed >> 23) & 0x0F)); }
};

struct ITF_ParticleOrder
{
    ITF_Particle* m_part;
    u32 m_mask;
    f32 m_zsort;
};

class PAR_Zlist
{
public:
    PAR_Zlist()
    {
        m_NumberEntry = 0;
        m_reorderingTable = NULL;
        init();
    }

    ~PAR_Zlist()
    {
        if (m_reorderingTable)
            delete[] m_reorderingTable;
    }

    void AddParticle(ITF_ParticleOrder* m_part);

    ITF_INLINE u32 getNumberEntry() {return m_NumberEntry;};

    void sort();
    ITF_INLINE ITF_ParticleOrder* getOrderNodeAt(u32 _val) {return m_reorderingTable[_val];};
    void reset();
    void setSize(u32 _number);

protected:

    void init();

    ITF_ParticleOrder* AddNode();

    SafeArray<ITF_ParticleOrder> m_BufferNode;
    ITF_ParticleOrder** m_reorderingTable;

    u32 m_NumberEntry;
};

#if 0
    ITF_Particle Generate()
    {
        ITF_Particle a;

        a.m_pos = m_pos;
        a.m_vel = m_vel;
        Vector orthogonal(a.m_vel);
        
        Vec_Rotate(&orthogonal, 23.f);
        Vec_Cross(&orthogonal, &a.m_vel, &orthogonal);		
        f32 rn = rndf(0.f,1.f);

        Vec_ArbitraryRotate(&a.m_vel, m_angle_of_cut_out+(m_angle_of_scatter - m_angle_of_cut_out)*rn, orthogonal);
        Vec_ArbitraryRotate(&a.m_vel, rndf(0.f, 360.f), m_vel);

        a.m_color = E3D_Color_RND(m_ColorMin, m_ColorMax);
        
        a.m_grav = m_grav;
        a.m_size = m_size;
        a.m_lifetime = m_lifetime - 0.2f * m_lifetime * rndf(0.f,1.f);
        a.m_t = 0;
        m_time_to_add_particle += m_lifetime/(f32)m_particlesMax;
        return a;
    }
};
#endif

enum PARGEN_ZSORT_MODE
{
    PARGEN_ZSORT_NONE = 0, 
    PARGEN_ZSORT_OLDER_FIRST, 
    PARGEN_ZSORT_NEWER_FIRST, 
};

class ITF_ParticleGenerator_Template : public TemplateObj
{
    DECLARE_OBJECT_CHILD_RTTI(ITF_ParticleGenerator_Template, TemplateObj,1063724644);
    DECLARE_SERIALIZE()

public:

    ITF_ParticleGenerator_Template();
    ~ITF_ParticleGenerator_Template();

    void onLoaded( class ResourceContainer * _parentContainer = NULL );

    u32                                         getComputeAABB() const { return m_computeAABB; }
    bool                                        getUseAnim() const { return m_useAnim; }
    bbool                                       getIsParticlePhaseLoop() const { return m_loop != 0; }
    bbool                                       getIsGeneratorLoop() const { return m_params.getNumToEmit() == U32_INVALID; }
    u32                                         getUseUVRandom() const { return m_useUVRandom; }
    i32                                         getStartAnimIndex() const { return m_startAnimIndex; }
    i32                                         getEndAnimIndex() const { return m_endAnimIndex; }
    f32                                         getAnimUVFreq() const { return m_AnimUVfreq; }
    const ParticleGeneratorParameters&          getParameters() const { return m_params; }
    ParticleGeneratorParameters *               getParametersPtr() { return &m_params; }
    const StringID&                             getAnimName() const {return m_animName;}
    PARGEN_ZSORT_MODE                           getZSortMode() const { return m_zSortMode; }

    const ResourceID&                           getAmvResourceID() const {return m_amvResourceID;}

#ifdef ITF_SUPPORT_EDITOR
    bbool                                       getDrawDebugShape() const { return m_drawDebugShape; }
    void                                        setDrawDebugShape(bbool _drawDebugShape) { m_drawDebugShape = _drawDebugShape; }

    void                                        drawDebugShape( const Vec3d &_shapeOrigin, const Vec2d &_actorScale ) const;
#endif // ITF_SUPPORT_EDITOR

private:

    u32                                         m_computeAABB;
    bool                                        m_useAnim;
    u32                                         m_loop;
    u32                                         m_useUVRandom;
    i32                                         m_startAnimIndex;
    i32                                         m_endAnimIndex;
    f32                                         m_AnimUVfreq;
    ParticleGeneratorParameters                 m_params;
    PARGEN_ZSORT_MODE                           m_zSortMode;

    // AnimMeshVertex
    Path                                        m_amvPath;
    ResourceID                                  m_amvResourceID;
    StringID                                    m_animName;

#ifdef ITF_SUPPORT_EDITOR
    bbool                                       m_drawDebugShape;
#endif
};

class JobUpdateParticle;
class ITF_ParticleGenerator
{

public:
#ifndef ITF_FINAL
    static bbool m_DbgNoparticules;
    static bbool m_DbgSkipUpdate;
    static bbool m_DbgSkipDraw;
#endif // ITF_FINAL
    #ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
        void* operator new(size_t, void*);
    #endif
    ITF_ParticleGenerator();

    ~ITF_ParticleGenerator();

    void computeAMVInfos(); // Only possible if ressources are ready !!!
    void releaseAMVInfos();

    void init( const class ITF_ParticleGenerator_Template* _template);
    void clear();
    void reInit();
    void reInit( const ITF_ParticleGenerator_Template* _template);
    void construct();
    void destruct();
    void update(f32 _dt, bbool _bDisableCreation = bfalse);

    bbool render(const class GraphicComponent * _graphicComponent, const ITF_VECTOR <class View*>& _views);
    bbool drawParticles(const class GraphicComponent * _graphicComponent, const ITF_VECTOR <class View*>& _views, bbool _direct = bfalse);

#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    void lightProcess();
#ifdef PAR_USE_ZSORT
    u32 FillZsort(Texture* pTexture, VertexPCT* pdata);
#endif // PAR_USE_ZSORT
    void fillNormal(const ITF_Particle *__restrict pParticle,const Texture* __restrict pTexture, VertexPCT* pdata, const Vec2d& _scaleUV);

    void applyDefaultColor(const Color& _col);
    void applyAmbiantColor(const Color& _col);
    ITF_INLINE Color getAmbiantColor() {return m_ambColor;}

    void    setColorFactor(const Color& _color);
    void    setGFXPrimitiveParamToMesh( const GFXPrimitiveParam& _param);
    const GFXPrimitiveParam& getGfxPrimitiveParam() const { return m_primitiveParam;}
    GFXPrimitiveParam& getGfxPrimitiveParam() { return m_primitiveParam;}
    void    setGFXPrimitiveParam( const GFXPrimitiveParam& _param){ m_primitiveParam = _param;}

    f32     getAlpha() const { return m_alpha; }
    void    setAlpha(f32 _alpha) { m_alpha = _alpha; }
    void    setRenderInTarget(bbool _val);
    ITF_INLINE const BezierBranch* getBezierBranch() const { return m_BezierBranch; }
    ITF_INLINE void setBezierBranch( const BezierBranch* _value ) { m_BezierBranch = _value; }

    void updateParticleAnim(ITF_Particle* _particle, f32 _time);
    void SetUVFromUVmode(ITF_Particle* _par);

    void updateAABB(ITF_Particle* _particle);
    void transformBoundingBoxWithMatrix();
    void computeTransformMatrix();
    const Vec3d & getTransformGravity() const { return m_transformGravity; }
    Vec3d getParticlesAcceleration() const { return m_transformGravity + m_params.getAcc(); }

    void setFlip( bbool _flip );

    void synchronize();

    static ITF_ParticleGenerator* getGenerator(i32& _entryIndex);
    static void freeGenerator(ITF_ParticleGenerator* _gen, i32 _entryIndex = -1);
    static void FlushGenerators();

    //This operator is forbidden
    const ITF_ParticleGenerator& operator = (const ITF_ParticleGenerator& src)
    {
        ITF_ASSERT(0);
        return ITF_ParticleGenerator::null_object;
    }

    static ITF_ParticleGenerator null_object;

    void setMaxParticles( u32 _maxParticles );
    void setMaxParticlesClamp( u32 _maxParticles ) { m_maxParticulesClamp = _maxParticles; }
    u32 getMaxParticlesClamp() const { return m_maxParticulesClamp; }
    ITF_INLINE ParticleGeneratorInstanceParameters * getParameters() { return & m_params;}
    ITF_INLINE const ParticleGeneratorParameters * getTemplateParameters() const { return &m_template->getParameters(); }
    ITF_INLINE bbool getActive( )
    {
        if(!m_generationStarted)
            return (m_activeParticlesCount > 0);
        else
            return (m_activeParticlesCount > 0) || (m_totalParticlesGenerated < m_params.m_emitParticlesCount);
    }

    ITF_INLINE bbool isUseAnim() { return m_useAnim; }
    ITF_INLINE void setTrueAABB(u32 _val) { m_computeAABB = _val;};
    ITF_INLINE u32  getTrueAABB() const { return m_computeAABB; }
 
    ITF_INLINE void setAnimUVfreq(f32 _freq);

    void        spawnOneParticle();
    void        spawnNParticle( u32 _numParticles, f32 maxTimeRange );
    void        spawnAtTime(f32 _Time);
    u32         computeParticleToEmitCount(f32 _dt, bbool _bDisableCreation);
    ITF_INLINE void        startGeneration() {m_generationStarted = btrue;}
               void        stopGeneration();
               void        stop();
    ITF_INLINE bbool        isGenerationStarted() {return m_generationStarted;}
    ITF_INLINE u32  getNumGeneratedParticles() const { return m_totalParticlesGenerated; }
    ITF_INLINE u32  getActiveParticlesCount() const { return m_activeParticlesCount; }
    ITF_INLINE u32  getMaxParticlesCount() const { return m_maxParticlesNeed; }
    void setMaterial( const GFX_MATERIAL & _material);
    void updateZSort();
    Vec3d getParticlesSpawnPoint();

    Color applyGlobalColor( const Color& _color) const;

    void startManualUpdate( const Vec3d& _pos );
    void updateManualParticle( u32 _index, const Vec3d& _pos, const Vec3d& _speed, Angle _rot );
    void changeManualParticleState( u32 _index, bbool _isAlive);
    void changeManualParticleFlip( u32 _index, bbool _isFlip);
    void changeManualParticleSize(const u32 _index, const f32 _size);
    void changeManualParticleColor(const u32 _index, const Color &_color);
    void changeManualParticleAlpha(const u32 _index, const u8 _alpha);
    void changeManualAllParticlesColor(const Color & _color);
    void changeManualAllParticlesSize(const f32 _size);

    ITF_INLINE i32 getStartAnimIndex() const;
    ITF_INLINE i32 getEndAnimIndex() const;
    ITF_INLINE u32 getAnimId() const {return m_animId; }
    ITF_INLINE AnimMeshVertex * getAnimMeshVertex();

    ITF_INLINE f32 getCurrentTime() const {return m_currentTime;}
    ITF_INLINE void incCurrentTime(f32 _dt)  {m_currentTime += _dt;}
    ITF_INLINE void setParticlesCountVtxToDisplay(u32 _particlesCountVtxToDisplay) {m_particlesCountVtxToDisplay = _particlesCountVtxToDisplay;}
#ifdef DIRECTVB_JOB
    ITF_INLINE u8* getCachedMemory() {return (u8*)m_mesh.getCurrentVB()->getLockedData();}
#endif // DIRECTVB_JOB
#ifdef CACHEDVB_JOB
    ITF_INLINE u8* getCachedMemory() {return m_cacheMemory;}
#endif // CACHEDVB_JOB

    u32				    m_activeParticlesCount;
    f32				    m_lastUpdateTime;
    f32				    m_particlesToEmitExact;
    bbool               m_generationStarted;
    u32                 m_FrameDieCount;

    u32                 m_totalParticlesGenerated;
    u32                 m_maxParticulesClamp;

    ITF_Particle*   allocateParticle();
    void deallocateParticle(ITF_Particle* pParticle);

    void initNewParticleFollow(ITF_Particle* _par);
    void addOneNewComplexParticle();
    bbool checkGeneratorValidity();
    const String8& getValidityMessage() const {return m_validityMessage;}

    f32                 getLocalRotationZ() const {return m_LocalRotationZ;}
    void                setLocalRotationZ(f32 _az) {m_LocalRotationZ = _az;}
    void                setLocalScale(const Vec2d& _scale) {m_LocalScale = _scale; applyLocalScaleMultiplier();}
    ITF_INLINE void     setLocalScaleMultiplier(const Vec2d& _mul) { m_LocalScaleMultiplier = _mul; }
    const Vec2d &       getGenBoxLocalScale() const { return m_genBoxLocalScale; }
    ITF_INLINE void     setGenBoxLocalScale(const Vec2d& _scale) { m_genBoxLocalScale = _scale; }
    ITF_INLINE void     applyLocalScaleMultiplier()   { m_LocalScale *= m_LocalScaleMultiplier; }

    u32                 m_maxParticlesNeed;

    ITF_Particle*       m_pParticleArrayIndex;
    const class ITF_ParticleGenerator_Template* getTemplate() const { return m_template; }

    ITF_INLINE VertexPCT* LockVB(ITF_VertexBuffer **_pVB)
    {
        VertexPCT* data = NULL;

        m_mesh.swapVBForDynamicMesh();
        *_pVB = m_mesh.getCurrentVB();

        GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(*_pVB,(void**)&data);
        ITF_ASSERT_MSG(data, "Unable to lock a vertex buffer");

        return data;
    }

    ITF_INLINE void UnlockVB(ITF_VertexBuffer *_pVB)
    {
        ITF_ASSERT_MSG(_pVB,"try to unlock a null VB");
        if (_pVB)
        {
            GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(_pVB);
        }        
    }

#ifdef DIRECTVB_JOB
    ITF_INLINE void unlockMeshVB()
    {
        GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(m_mesh.getCurrentVB());
    }
#endif // DIRECTVB_JOB


    ITF_INLINE bbool bCheckBoolCond(BOOL_COND _val) const
    {
        if(_val == BOOL_cond)
            return m_activeParticlesCount ? bfalse:btrue;
        else
            return _val == BOOL_false ? bfalse:btrue;
    }

    ITF_INLINE void   setUseMatrix(bbool _opt) {m_params.m_useMatrix = _opt ? BOOL_true : BOOL_false;}
    ITF_INLINE bbool  isUseMatrix() const  {return bCheckBoolCond(m_params.m_useMatrix);}
    ITF_INLINE bbool  isScaleGenBox() const { return m_params.m_scaleGenBox == BOOL_cond ? isUseMatrix() : m_params.m_scaleGenBox != BOOL_false; }

    static void    initPool();
    static void    freePool();

private:
    struct ParticleGeneratorPoolEntry
    {
        static const u32 counter_available = 0;
        static const u32 counter_allocated = 0xffffffff;
        ParticleGeneratorPoolEntry()
        {
            m_gen = NULL;
            m_unavailableCounter = counter_allocated;
        }
        ITF_ParticleGenerator*  m_gen;
        u32                     m_unavailableCounter;
    };

#ifdef ITF_SUPPORT_EDITOR
    static const u32 m_poolSize = 1024;
#else
    static const u32 m_poolSize = 512;
#endif

    static SafeArray<ParticleGeneratorPoolEntry> m_pool;
    static SafeArray<u32>       m_indexesToFree;
    static SafeArray<u32>       m_freeIndexes;

#ifndef ITF_SUPPORT_MULTITHREADED
    void        fillActiveList(ITF_VertexBuffer *_VB);
    void        fillParticuleArray(ITF_VertexBuffer *_VB);
    void        fillParticles(ITF_VertexBuffer *_VB);
#endif // ITF_SUPPORT_MULTITHREADED

private:
    void initTemplate( const class ITF_ParticleGenerator_Template* _template);
    const class ITF_ParticleGenerator_Template* m_template;
    SafeArray<u32>      m_pFreeParticleIndex;

    Quad3DPrim          m_3dQuad;

 
    void initNewParticleComplex(ITF_Particle* _par);
    void updateFreeIndexListAfterSort();

    ParticleGeneratorInstanceParameters m_params;

#ifdef CACHEDVB_JOB
    u8*                 m_cacheMemory;//TODO move it using a pool
#endif // CACHEDVB_JOB
    u32                 m_particlesCountVtx;
    u32                 m_particlesCountVtxToDisplay;
    u32                 m_computeAABB;

    JobUpdateParticle*  m_jobParticle;

    /// Engine.
    f32				    m_currentTime;
    f32                 m_maxParLiveTimeFromPhases;
    
    bbool               m_isRender;

    /// Mesh.
    ITF_Mesh            m_mesh;

    Color               m_ambColor;
    Color               m_factorColor;
    GFXPrimitiveParam   m_primitiveParam;
    bbool               m_renderInTarget;
    f32                 m_alpha;

    /// z sorting.
    PAR_Zlist*          m_particlesZOrdering;

    bbool               m_checkValidity;
    String8             m_validityMessage;

    bbool               m_aabbNeedCompute;
    bbool               m_useAnim;

    u32                 m_SplitCurPos;
    Angle*              m_spliterAngle;

    // for matrix orientation and scale:
    f32                 m_LocalRotationZ;
    Vec2d               m_LocalScale;
    Vec2d               m_LocalScaleMultiplier;
    Vec2d               m_genBoxLocalScale;
    Matrix44            m_transformMatrix44;
    Matrix44            m_transformMatrixRotation;
    Vec3d               m_transformGravity;

    // animmeshvertex
    u32                 m_animId; // default animID
    u32                 m_atlasMaxIndex;

    const BezierBranch* m_BezierBranch;
public :
    SingleAnimDataRuntimeList       m_animListRuntime;

    static RandomLCG randomLCG;
#ifdef ITF_SUPPORT_FXEDITOR
    bbool m_bDrawText;

    // data managment    
    static void AddRef(class ITF_ParticleGenerator_Template* _template, const StringID& _name );
    static void RemoveRef(class ITF_ParticleGenerator_Template* _template);
    static void AddRef(class ITF_ParticleGenerator* _gen);
    static void RemoveRef(class ITF_ParticleGenerator* _gen);
    class ObjRef
    {
    public:
        ObjRef() : m_ParticleGenerator_Template(NULL)
        {}
        ObjRef(class ITF_ParticleGenerator_Template* _template) : m_ParticleGenerator_Template(_template)
        {}
        StringID m_Name;
        class ITF_ParticleGenerator_Template* m_ParticleGenerator_Template;
        ITF_VECTOR<class ITF_ParticleGenerator*> m_vParticleGenerator;
    };
    static ITF_VECTOR<ObjRef> m_vReferences;
    static ITF_THREAD_CRITICAL_SECTION m_csReferences;
    static bbool m_csReferencesInitDone;
    static void EnterCSref();
    static void LeaveCSref();
    static ObjRef *findRef(class ITF_ParticleGenerator_Template* _template);
    static ObjRef *findRef(const Path& _path, const StringID& _name);
    static ObjRef *addRef(class ITF_ParticleGenerator_Template* _template);
#endif
};

ITF_INLINE i32 ITF_ParticleGenerator::getStartAnimIndex() const { return m_template->getStartAnimIndex(); }
ITF_INLINE i32 ITF_ParticleGenerator::getEndAnimIndex() const { return m_template->getEndAnimIndex(); }
ITF_INLINE AnimMeshVertex * ITF_ParticleGenerator::getAnimMeshVertex() { return (m_template->getAmvResourceID()==ResourceID::Invalid) ? 0 : static_cast<AnimMeshVertexResource *>(m_template->getAmvResourceID().getResource())->getOwnerData(); }

///----------------------------------------------------------------------------//

ITF_INLINE void ITF_ParticleGenerator::synchronize()
{
    if (getTrueAABB())
        getParameters()->synchronizeBoundingBox();
}

ITF_INLINE Color ITF_ParticleGenerator::applyGlobalColor( const Color& _color ) const
{
    return _color * m_primitiveParam.m_colorFactor;
}

ITF_INLINE void ITF_ParticleGenerator::applyDefaultColor( const Color& _color )
{
    m_params.setDefaultColor(_color);
    m_primitiveParam.m_colorFactor = m_params.getDefaultColor();
}

ITF_INLINE void ITF_ParticleGenerator::applyAmbiantColor( const Color& _color )
{
    m_ambColor = _color;
}

ITF_INLINE void ITF_ParticleGenerator::setGFXPrimitiveParamToMesh( const GFXPrimitiveParam& _param)
{
    m_mesh.setCommonParam(_param);
}

ITF_INLINE void ITF_ParticleGenerator::setRenderInTarget(bbool _val)
{
     m_renderInTarget = _val;
}

ITF_INLINE void ITF_ParticleGenerator::setColorFactor(const Color& _color)
{
    m_factorColor = _color;
}

} // namespace ITF

#endif // _ITF_PARTICULEGENERATOR_H_
