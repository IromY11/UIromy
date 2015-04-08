#include "precompiled_engine.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_PARTICULEGENERATORPARAMETERS_H_
#include "engine/display/particle/ParticuleGeneratorParameters.h"
#endif //_ITF_PARTICULEGENERATORPARAMETERS_H_

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

#ifndef _ITF_JOBUPDATEPARTICLE_H_
#include "engine/display/particle/JobUpdateParticle.h"
#endif //_ITF_JOBUPDATEPARTICLE_H_

#ifndef _ITF_SCHEDULER_H_
#include "engine/scheduler/scheduler.h"
#endif //_ITF_SCHEDULER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif // _ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifdef ITF_WII
#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif // _ITF_WII_DRAWPRIM_H_
#endif // ITF_WII

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_BEZIERCURVE4_H_
#include "core/math/BezierCurve4.h"
#endif // _ITF_BEZIERCURVE4_H_

#define USEVB
#define USE_INTERNAL_SPRITE_INDEX_VB
//#define DEBUG_PARTICLE_POOL

#ifndef _ITF_BEZIERBRANCH_H_
#include "engine/BezierTree/BezierBranch.h"
#endif //_ITF_BEZIERBRANCH_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

//#define BUGFIX_PIVOT_INVERTUV

namespace ITF
{

///----------------------------------------------------------------------------//

    static unsigned long msSeed = 1376312589;
    inline unsigned long GenerateSeed()
    {
        // A very, VERY crude LCG but good enough to generate
        // a nice range of seed values
        msSeed = (msSeed * 0x015a4e35L) + 1;
        msSeed = (msSeed>>16)&0x7fff;
        return msSeed;
    }   

    const long RandomLCG::msQuotient  = MAX_LONG / 16807L;
    const long RandomLCG::msRemainder = MAX_LONG % 16807L;

    void RandomLCG::SetNewSeed()
    {
        SetSeed(GenerateSeed());
    }

//random generators.
//Seeder random(666);
    RandomLCG ITF_ParticleGenerator::randomLCG;

#ifndef ITF_FINAL
bbool ITF_ParticleGenerator::m_DbgSkipUpdate = bfalse;
bbool ITF_ParticleGenerator::m_DbgSkipDraw = bfalse;
bbool ITF_ParticleGenerator::m_DbgNoparticules = bfalse;
#endif // ITF_FINAL

ITF_ParticleGenerator ITF_ParticleGenerator::null_object;

///----------------------------------------------------------------------------//

SafeArray<ITF_ParticleGenerator::ParticleGeneratorPoolEntry> ITF_ParticleGenerator::m_pool;
SafeArray<u32> ITF_ParticleGenerator::m_indexesToFree;
SafeArray<u32> ITF_ParticleGenerator::m_freeIndexes;
#ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
    static u8* m_poolMemory = NULL;
#endif

void ITF_ParticleGenerator::initPool()
{
#ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
    if (m_poolMemory)
        return;
    m_poolMemory = (u8*)Memory::mallocCategory(sizeof(ITF_ParticleGenerator) * m_poolSize, MemoryId::mId_Particle);
#endif //PARTICLE_GENERATOR_USE_PLACEMENT_NEW

    for (u32 i = 0; i < m_poolSize; i++)
    {
        ParticleGeneratorPoolEntry entry;
        #ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
            entry.m_gen = (ITF_ParticleGenerator*)&m_poolMemory[i * sizeof(ITF_ParticleGenerator)];
        #else
            entry.m_gen = NULL;
        #endif
        entry.m_unavailableCounter = ParticleGeneratorPoolEntry::counter_available;
        m_pool.push_back(entry);
        m_freeIndexes.push_back(i);
    }
}

void ITF_ParticleGenerator::freePool()
{
    while(m_indexesToFree.size())
        FlushGenerators();
#ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
    if (m_poolMemory)
    {
        Memory::free(m_poolMemory);
        m_poolMemory = NULL;
    }
#endif
}

ITF_ParticleGenerator* ITF_ParticleGenerator::getGenerator(i32& _entryIndex)
{
    if (m_freeIndexes.size())
    {
        i32 index = (i32)m_freeIndexes.back();
        m_freeIndexes.pop_back();
        ITF_ASSERT(ParticleGeneratorPoolEntry::counter_available == m_pool[index].m_unavailableCounter);
        m_pool[index].m_unavailableCounter = ParticleGeneratorPoolEntry::counter_allocated;
        _entryIndex = index;
        #ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
            new((void*)m_pool[index].m_gen) ITF_ParticleGenerator(); // call placement new
            m_pool[index].m_gen->construct();
        #else
            if (!m_pool[index].m_gen)
                m_pool[index].m_gen = newAlloc(mId_System, ITF_ParticleGenerator());
        #endif
        #ifdef DEBUG_PARTICLE_POOL
            LOG("[PARTICLE POOL] Alloc index %d ptr %08X at frame %d", index, (u32)m_pool[index].m_gen, CURRENTFRAME);
        #endif
        return m_pool[index].m_gen;
    }
    _entryIndex = -1;
    ITF_ASSERT_MSG(bfalse, "please increase m_poolSize");
    return NULL;
}

void ITF_ParticleGenerator::freeGenerator(ITF_ParticleGenerator* _gen, i32 _entryIndex)
{
    i32 foundIndex = -1;
    if (_entryIndex >= 0)
    {
        if (m_pool[_entryIndex].m_gen == _gen)
        {
            foundIndex = _entryIndex;
        }
    }
    else
    {
        for (u32 i = 0; i < m_poolSize; i++)
        {
            if (m_pool[i].m_gen == _gen)
            {
                foundIndex = (i32)i;
                break;
            }
        }
    }
    ITF_ASSERT_MSG(foundIndex >= 0, "pool not found");
    if (foundIndex >= 0)
    {
        m_pool[foundIndex].m_unavailableCounter = 3;
        m_indexesToFree.push_back(foundIndex);
        #ifdef DEBUG_PARTICLE_POOL
            LOG("[PARTICLE POOL] set to be freed index %d ptr %08X at frame %d", foundIndex, (u32)m_pool[foundIndex].m_gen, CURRENTFRAME);
        #endif
    }
}


void ITF_ParticleGenerator::FlushGenerators()
{    
    for (i32 i=(i32)m_indexesToFree.size()-1 ; i>=0 ; --i)
    {
        u32 index = m_indexesToFree[i];
        if (m_pool[index].m_unavailableCounter != ParticleGeneratorPoolEntry::counter_available && m_pool[index].m_unavailableCounter != ParticleGeneratorPoolEntry::counter_allocated)
        {
            m_pool[index].m_unavailableCounter--;
            if (m_pool[index].m_unavailableCounter == ParticleGeneratorPoolEntry::counter_available)
            {
                #ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
                    m_pool[index].m_gen->~ITF_ParticleGenerator();                    
                #else
                    SF_DEL(m_pool[index].m_gen);
                #endif
                m_indexesToFree.eraseNoOrder(i);
                m_freeIndexes.push_back(index);
                #ifdef DEBUG_PARTICLE_POOL
                    LOG("[PARTICLE POOL] really freed (cleared) index %d at frame %d", index, CURRENTFRAME);
                #endif // DEBUG_PARTICLE_POOL
            }
        }
    }
#ifdef DEBUG_PARTICLE_POOL
    u32 count = 0;
    for (u32 i = 0; i < m_poolSize; i++)
    {
        if (ParticleGeneratorPoolEntry::counter_available == m_pool[i].m_unavailableCounter)
        {
            ITF_ASSERT(m_freeIndexes.find(i) >= 0);
        }
        else
        {
            ITF_ASSERT(m_freeIndexes.find(i) < 0);
            count++;
        }
    }
    ITF_ASSERT( m_poolSize - count == m_freeIndexes.size());
#endif
}

///----------------------------------------------------------------------------//

#ifdef PARTICLE_GENERATOR_USE_PLACEMENT_NEW
    void* ITF_ParticleGenerator::operator new(size_t _size, void* _adrs)
    {
        // just fill vf table
        return _adrs;
    }
#endif

ITF_ParticleGenerator::ITF_ParticleGenerator()
{
    construct();
}

void ITF_ParticleGenerator::construct()
{
    m_transformGravity = Vec3d::Zero;
    m_template = NULL;
    m_params.setGeneratorMode(PARGEN_MODE_COMPLEX);
    m_params.setGeneratorGenType(PARGEN_GEN_POINTS);
    m_particlesCountVtx     = 0;
    m_particlesCountVtxToDisplay = 0;
    m_activeParticlesCount  = 0;
    m_currentTime			= 0.0f;
    m_lastUpdateTime	    = 0.0f;
    m_params.setNumberPhase(1);
    m_maxParticlesNeed      = 0;
    m_FrameDieCount         = U32_INVALID;

#ifdef CACHEDVB_JOB
    m_cacheMemory           = NULL;
#endif // CACHEDVB_JOB

    m_params.setBoxMin(Vec2d(-10,-10));
    m_params.setBoxMax(Vec2d(10,10));

    m_ambColor          = Color::white();
    m_alpha             = 1.f;

    m_factorColor       = Color::white();
    m_renderInTarget = 0;
    m_primitiveParam    = GFXPrimitiveParam();

    m_params.setAABBMin(Vec2d(1.f, 1.f));
    m_params.setAABBMax(Vec2d(1.f, 1.f));

    m_particlesToEmitExact = 0.f;
    m_generationStarted = btrue;

    m_maxParticulesClamp = (u32)-1;
    m_totalParticlesGenerated = 0;

    m_particlesZOrdering = NULL;
    
    m_isRender = btrue;
    m_jobParticle = newAlloc(mId_Particle,JobUpdateParticle());
    m_jobParticle->setAsManaged(btrue);
    m_pParticleArrayIndex = NULL;
    m_checkValidity = bfalse;    

    m_aabbNeedCompute = btrue;

    m_spliterAngle = NULL;
    m_SplitCurPos = 0;

    m_LocalRotationZ = 0.f;
    m_LocalScale = Vec2d::One;
    m_LocalScaleMultiplier = Vec2d::One;
    m_genBoxLocalScale = Vec2d::One;
    m_transformMatrix44.setIdentity();

    m_animId = U32_INVALID;
    m_atlasMaxIndex = 0;

    m_BezierBranch = NULL;
#ifdef ITF_SUPPORT_FXEDITOR
    m_bDrawText = bfalse;
#endif // ITF_SUPPORT_FXEDITOR
}
///----------------------------------------------------------------------------//

ITF_ParticleGenerator::~ITF_ParticleGenerator()
{
    destruct();
#ifdef ITF_SUPPORT_FXEDITOR
    ITF_ParticleGenerator::RemoveRef(this);
#endif
}

void ITF_ParticleGenerator::destruct()
{
    clear();

    Job::destroy(m_jobParticle); 
    m_jobParticle = NULL;
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::clear()
{
    //LOG("ITF_ParticleGenerator::clear %d", this);
    SF_DEL_ARRAY(m_pParticleArrayIndex);

#ifdef CACHEDVB_JOB
    if (m_cacheMemory)
        delete[] m_cacheMemory;
#endif // CACHEDVB_JOB

    m_mesh.removeVertexBuffer();

#ifndef USE_INTERNAL_SPRITE_INDEX_VB
    for (u32 i = 0; i < m_mesh.m_ElementList.size(); i++)
    {
        if(m_mesh.m_ElementList[i].m_indexBuffer) 
            GFX_ADAPTER->removeIndexBufferDelay(m_mesh.m_ElementList[i].m_indexBuffer,1);
    }
#endif

    SF_DEL(m_particlesZOrdering);
    SF_DEL_ARRAY(m_spliterAngle);

    m_checkValidity = bfalse;
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::setMaxParticles( u32 _maxParticles )
{
    if (!GFX_ADAPTER)
        return;

//    LOG("ITF_ParticleGenerator::setMaxParticles %d", this);
    u32 replicate = 1;

    SF_DEL_ARRAY(m_pParticleArrayIndex);
    m_pFreeParticleIndex.clear();

    if (_maxParticles == 0)
        _maxParticles = 1;

    ITF_WARNING_CATEGORY(Graph, NULL, _maxParticles<=5000u, "Number of particles exceed 5000");

    m_params.setMaxParticles(Min(_maxParticles, 5000u));

    m_maxParticlesNeed = m_params.getMaxParticles();
    m_maxParticlesNeed = Min(m_maxParticlesNeed, m_params.getMaxParticles());
    ITF_ASSERT(m_maxParticlesNeed > 0);

    m_pParticleArrayIndex = newAlloc(mId_Particle,ITF_Particle[m_maxParticlesNeed]);
#ifndef ITF_FINAL
    if (!m_pParticleArrayIndex)
    {
        ITF_FATAL_ERROR("out memory for particle %d %f",m_maxParticlesNeed,m_params.getFrequency());
    }
#endif //ITF_FINAL

    m_animListRuntime.clear();
    m_animListRuntime.reserve(m_maxParticlesNeed);

    /// Destroy VB.
    m_mesh.removeVertexBuffer();

    /// Create VB.
    m_particlesCountVtx = m_maxParticlesNeed * 4 * replicate;

#ifdef CACHEDVB_JOB
    if (m_cacheMemory)
        delete[] m_cacheMemory;
    m_cacheMemory = newAlloc(mId_Particle, u8[m_particlesCountVtx*sizeof(VertexPCT)]);//TODO use a pool  
#endif // CACHEDVB_JOB

#ifdef DIRECTVB_JOB
    const VBLockType mashVB_type = vbLockType_dynKeep;
#else // DIRECTVB_JOB
    const VBLockType mashVB_type = vbLockType_dynDiscard;
#endif // DIRECTVB_JOB
    m_mesh.createVertexBuffer(m_particlesCountVtx , VertexFormat_PCT, sizeof(VertexPCT), mashVB_type, VB_T_PARTICLE);

    /// Create Element.
    if (m_mesh.getNbMeshElement() == 0)
    {
        m_mesh.addElementAndMaterial(m_params.getMaterial());
    }

#ifdef USE_INTERNAL_SPRITE_INDEX_VB
    m_mesh.getMeshElement().m_indexBuffer = GFX_ADAPTER->needSpriteIndexBuffer(m_maxParticlesNeed * 6 * replicate);
#else
    if(m_mesh.getMeshElement().m_indexBuffer) 
        GFX_ADAPTER->removeIndexBuffer(m_mesh.getMeshElement().m_indexBuffer);
    
    m_mesh.getMeshElement().m_indexBuffer = GFX_ADAPTER->createIndexBuffer(m_maxParticlesNeed * 6 * replicate , bfalse);
    
    /// Fill Element index Buffer.
    u32 i;
    u16* index;

    m_mesh.getMeshElement().m_indexBuffer->Lock((void**)&index);
    u16 vertex = 0;

    for (i = 0; i < m_maxParticlesNeed * replicate ; i++)
    {
        index[i * 6 + 0] = vertex + 0;
        index[i * 6 + 1] = vertex + 1;
        index[i * 6 + 2] = vertex + 2;
        index[i * 6 + 3] = vertex + 2;
        index[i * 6 + 4] = vertex + 3;
        index[i * 6 + 5] = vertex + 0;
        vertex += 4;
    }

    m_mesh.getMeshElement().m_indexBuffer->Unlock();
#endif
 
    reInit();
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::computeAMVInfos()
{
    if(!getAnimMeshVertex())
        return;

    m_params.m_amvID = ANIM_MANAGER->getAnimMeshVertexIdFromMaterial(m_params.m_material);

    m_animId = getAnimMeshVertex()->getAnimIndexByFriendly(m_template->getAnimName());
    ITF_ASSERT_MSG(m_animId != U32_INVALID, "AnimMeshVertex (particle) : anim name %s, doesn't exist.", m_template->getAnimName().getDebugString());

    // Fill missing phase anim data.
    for(u32 i = 0; i <m_params.m_nbPhase; ++ i)
    {
        ParPhase& phase = m_params.m_phaseList[i];
        if(phase.m_animName != StringID::InvalidId)
        {
            phase.m_animId = getAnimMeshVertex()->getAnimIndexByFriendly(phase.m_animName);
            ITF_WARNING(NULL, bfalse, "AnimMeshVertex (particle) : anim name '%s', doesn't exist.", phase.m_animName.getDebugString());
        }

        if(phase.m_animId == U32_INVALID)
        {
            phase.m_animId = m_animId;
            phase.m_animName = m_template->getAnimName();
        }

        if(phase.m_animStart == -1)
            phase.m_animStart = (i32)getStartAnimIndex();
        if(phase.m_animEnd == -1)
            phase.m_animEnd = (i32)getEndAnimIndex();

        // Check anim frames.
        if(phase.m_animId != U32_INVALID)
        {
            u32 maxFrames = getAnimMeshVertex()->getNbFrameForAnim(phase.m_animId)-1;
            if((u32)phase.m_animStart > maxFrames)
            {
                ITF_WARNING(NULL, bfalse, "AnimMeshVertex (particle) : Start frame %u must <= than %u for anim '%s'.",
                            phase.m_animStart, maxFrames, phase.m_animName.getDebugString());
                phase.m_animStart = (i32)0;
            }

            if((u32)phase.m_animEnd > maxFrames)
            {
                ITF_WARNING(NULL, bfalse, "AnimMeshVertex (particle) : End frame %u must be <= %u for anim '%s'.",
                            phase.m_animEnd, maxFrames, phase.m_animName.getDebugString());
                phase.m_animEnd = (i32)maxFrames;
            }
        }
    }
}

void ITF_ParticleGenerator::releaseAMVInfos()
{
    if (m_params.m_amvID != U32_INVALID)
    {
        ANIM_MANAGER->releaseAnimMeshVertexId(m_params.m_amvID);
        m_params.m_amvID = U32_INVALID;
    }
}

void ITF_ParticleGenerator::initTemplate( const class ITF_ParticleGenerator_Template* _template)
{
    m_useAnim = _template->getUseAnim();
    m_computeAABB = _template->getComputeAABB();

    // Copy template parameters into instance parameters
    m_params.initFromTemplate(_template->getParameters());

    u32 nbphase = m_params.m_phaseList.size();
    if (m_params.m_nbPhase > nbphase)
        m_params.m_nbPhase = nbphase;

    m_maxParLiveTimeFromPhases = getTemplateParameters()->computeMaxSumPhaseTime();
    m_primitiveParam.m_useStaticFog = !getTemplateParameters()->getForceNoDynamicFog();
    m_primitiveParam.m_renderInReflection = getTemplateParameters()->getRenderInReflection();
    Texture* pTexture = m_params.getMaterial().getTexture();
    if (pTexture)
    {
        const UVAtlas* atlas = pTexture->getUVAtlas();
        if(atlas)
            m_atlasMaxIndex = atlas->getNumberUV();
    }
}
void ITF_ParticleGenerator::init( const ITF_ParticleGenerator_Template* _template)
{
    m_template = _template;

    initTemplate(_template);

    setMaxParticles(m_template->getParameters().getMaxParticles());

#ifdef ITF_SUPPORT_FXEDITOR
    AddRef(this);
#endif
}

///----------------------------------------------------------------------------//
void ITF_ParticleGenerator::reInit( const ITF_ParticleGenerator_Template* _template)
{
    u32 maxparticles = m_params.m_maxParticles;
    initTemplate(_template);

    if(m_params.m_maxParticles != maxparticles)
    {
        m_params.m_maxParticles = maxparticles;
        reInit();
        setMaxParticles(_template->getParameters().getMaxParticles());
    }
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::reInit()
{
    m_pFreeParticleIndex.clear();
    for (i32 index = m_maxParticlesNeed-1;index>=0 ;index--)
    {
        m_pFreeParticleIndex.push_back(index);
    }

    for (u32 index = 0;index<m_maxParticlesNeed;++index)
    {
        ITF_Particle  *pParticle = &m_pParticleArrayIndex[index];
        pParticle->dead();
    }
    
    m_activeParticlesCount = 0;
    m_FrameDieCount = U32_INVALID;

    m_primitiveParam.m_colorFactor = m_params.getDefaultColor();
}

void ITF_ParticleGenerator::stopGeneration()
{
    m_generationStarted = bfalse;
    if(m_params.m_dieFadeTime>=0.f)
        m_FrameDieCount = (u32) (m_params.m_dieFadeTime * 60.f);
    m_params.setBehaviorOnEnd(OnEnd_Destroy);
}

void ITF_ParticleGenerator::stop()
{
    m_generationStarted = bfalse;
    if(m_params.m_dieFadeTime<=0.f)
        m_FrameDieCount = 0;
    else
        m_FrameDieCount = (u32) (m_params.m_dieFadeTime * 60.f);
    m_params.setBehaviorOnEnd(OnEnd_Destroy);
}
///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::setMaterial( const GFX_MATERIAL & _material)
{
    m_params.setMaterial(_material);
    
    if (m_mesh.getNbMeshElement())
    {
        m_mesh.cleanPassFilterFlag();
        m_mesh.setMaterial(0, _material);
    }

    Texture* pTexture = m_params.getMaterial().getTexture();
    if (pTexture)
    {
        const UVAtlas* atlas = pTexture->getUVAtlas();
        if(atlas)
            m_atlasMaxIndex = atlas->getNumberUV();
    }
}

///----------------------------------------------------------------------------//

void Quad_ToVB(VertexPCT* __restrict  data,Angle _angle,u32 _color,const Vec3d& _pos,const Vec2d& _currentSize, const Vec2d& m_uv0, const Vec2d& m_uv1, const Vec2d& _scaleUV, const Vec2d& _scaleSize)
{
/*  original code
    f32 rayonx = _currentSize.x() * 0.5f;
    f32 rayony = _currentSize.y() * 0.5f;
    f32 c,s;
    f32_CosSin(_angle.Radians(), &c, &s);

    f32 rayonxc = rayonx * c;
    f32 rayonys = rayony * s;
    f32 rayonxs = rayonx * s;
    f32 rayonyc = rayony * c;

    data[0].m_pos.set( -rayonxc - rayonys + _pos.x(), -rayonxs + rayonyc + _pos.y(), _pos.z());
    data[0].m_color = _color;
    data[0].m_uv = m_uv0 * _scaleUV;

    data[1].m_pos.set( -rayonxc + rayonys  + _pos.x(), -rayonxs - rayonyc + _pos.y(), _pos.z());
    data[1].m_color = _color;
    data[1].m_uv.set(m_uv0.x() * _scaleUV.x(), m_uv1.y() * _scaleUV.y());

    data[2].m_pos.set( rayonxc + rayonys + _pos.x(), rayonxs - rayonyc + _pos.y(), _pos.z());
    data[2].m_color = _color;
    data[2].m_uv = m_uv1 * _scaleUV;

    data[3].m_pos.set( rayonxc - rayonys + _pos.x(), rayonxs + rayonyc + _pos.y() , _pos.z());
    data[3].m_color = _color;
    data[3].m_uv.set(m_uv1.x() * _scaleUV.x(), m_uv0.y() * _scaleUV.y());
*/
    Vec2d cs;
    Vec2d::CosSinOpt(&cs, _angle.Radians());

    Vec2d rayon    = _currentSize * _scaleSize * 0.5f;
    Vec2d rayonxcs = Vec2d::ScaleX(&cs, &rayon);
    Vec2d sc       = -cs;
          sc       = Vec2d::mergeYX(&sc, &cs);
    Vec2d rayonysc = Vec2d::ScaleY(&sc, &rayon);

    Vec2d posXY;
    posXY.truncateVec3d(_pos);

    Vec2d pos = rayonysc - rayonxcs + posXY;
    Vec2d uv  = m_uv0 * _scaleUV;

#ifdef USE_WGP
    ITF_VertexBuffer *vb = reinterpret_cast<ITF_VertexBuffer*>(data);
    ITF_ASSERT(vb->bIsDynamic());

    vb->wgp_write(_color, _pos.z(), pos, uv);

    pos = -rayonysc - rayonxcs + posXY;
    uv  = Vec2d::mergeXY(&m_uv0, &m_uv1) * _scaleUV;
    vb->wgp_write(_color, _pos.z(), pos, uv);

    pos = -rayonysc + rayonxcs + posXY;
    uv  = m_uv1 * _scaleUV;
    vb->wgp_write(_color, _pos.z(), pos, uv);

    pos = rayonysc + rayonxcs + posXY;
    uv  = Vec2d::mergeXY(&m_uv1, &m_uv0) * _scaleUV;
    vb->wgp_write(_color, _pos.z(), pos, uv);
#else
    Vec2d::to3d(&data[0].m_pos, &pos, _pos.z());
    data[0].m_color = _color;
    data[0].m_uv = m_uv0 * _scaleUV;

    pos = -rayonysc - rayonxcs + posXY;
    Vec2d::to3d(&data[1].m_pos, &pos, _pos.z());
    data[1].m_color = _color;
    data[1].m_uv = Vec2d::mergeXY(&m_uv0, &m_uv1) * _scaleUV;

    pos = -rayonysc + rayonxcs + posXY;
    Vec2d::to3d(&data[2].m_pos, &pos, _pos.z());
    data[2].m_color = _color;
    data[2].m_uv = m_uv1 * _scaleUV;

    pos = rayonysc + rayonxcs + posXY;
    Vec2d::to3d(&data[3].m_pos, &pos, _pos.z());
    data[3].m_color = _color;
    data[3].m_uv = Vec2d::mergeXY(&m_uv1, &m_uv0) * _scaleUV;
#endif
}

///----------------------------------------------------------------------------//

void Quad_ToVBPivot( VertexPCT* __restrict data, const ITF_Particle* __restrict _pParticle, const Vec2d& _pivot, const Vec2d& _uv0, const Vec2d& _uv1, const Vec2d& _scaleUV, const Vec2d& _scaleSize)
{
/*  original code
    Vec2d rayon, cs, sc, p1, p3, p1cs, p1sc, p3cs, p3sc;
    Vec2d::Scale(&rayon, &_pParticle->m_curSize, 0.5f);
    
    Vec2d::SinCosOpt(&sc, _pParticle->m_angle.Radians());
    cs.x() = sc.y(); cs.y() = sc.x();
    u32 color = _pParticle->m_curColor.getAsU32();

    Vec2d::Sub(&p1, &_pParticle->m_pivot, &rayon);
    Vec2d::Add(&p3, &_pParticle->m_pivot, &rayon);   

    Vec2d::Mul(&p1cs, &p1, &cs);
    Vec2d::Mul(&p3cs, &p3, &cs);
    Vec2d::Mul(&p1sc, &p1, &sc);
    Vec2d::Mul(&p3sc, &p3, &sc);    

    data[0].m_pos = Vec3d(p1cs.x() - p1cs.y() + _pParticle->m_pos.x(), p1sc.x() + p1sc.y() + _pParticle->m_pos.y(), _pParticle->m_pos.z());
    data[0].m_color = color;
    data[0].m_uv = _uv0 * _scaleUV;

    data[1].m_pos = Vec3d(p1cs.x() - p3cs.y() + _pParticle->m_pos.x(), p1sc.x() + p3sc.y() + _pParticle->m_pos.y(), _pParticle->m_pos.z());
    data[1].m_color = color;
    data[1].m_uv = Vec2d(_uv0.x() * _scaleUV.x(), _uv1.y() * _scaleUV.y());

    data[2].m_pos = Vec3d(p3cs.x() - p3cs.y() + _pParticle->m_pos.x(), p3sc.x() + p3sc.y() + _pParticle->m_pos.y(), _pParticle->m_pos.z());
    data[2].m_color = color;
    data[2].m_uv = _uv1 * _scaleUV;

    data[3].m_pos = Vec3d(p3cs.x() - p1cs.y() + _pParticle->m_pos.x(), p3sc.x() + p1sc.y() + _pParticle->m_pos.y(), _pParticle->m_pos.z());
    data[3].m_color = color;
    data[3].m_uv = Vec2d(_uv1.x() * _scaleUV.x(), _uv0.y() * _scaleUV.y());
*/
    // Rewritten to get faster code on Wii
    Vec2d cs;
    Vec2d::CosSinOpt(&cs, _pParticle->m_angle.Radians());
    Vec2d rayon = _pParticle->m_curSize * _scaleSize * 0.5f;

    Vec2d p1 = _pivot - rayon;
    Vec2d p3 = _pivot + rayon;
    Vec2d sc = -cs;
          sc = Vec2d::mergeYX(&sc, &cs);

    u32 color = _pParticle->m_curColor.getAsU32();
    Vec2d posXY;
    posXY.truncateVec3d(_pParticle->m_pos);
    f32 posZ = _pParticle->m_pos.z();
#ifdef BUGFIX_PIVOT_INVERTUV
    Vec2d pos = Vec2d::ScaleX(&cs, &p1) + Vec2d::ScaleY(&sc, &p3) + posXY;
#else
    Vec2d pos = Vec2d::ScaleX(&cs, &p1) + Vec2d::ScaleY(&sc, &p1) + posXY;
#endif
    Vec2d uv  = _uv0 * _scaleUV;

#ifdef USE_WGP
    ITF_VertexBuffer *vb = reinterpret_cast<ITF_VertexBuffer*>(data);
    ITF_ASSERT(vb->bIsDynamic());

    vb->wgp_write(color, posZ, pos, uv);

#ifdef BUGFIX_PIVOT_INVERTUV
    pos = Vec2d::ScaleX(&cs, &p1) + Vec2d::ScaleY(&sc, &p1) + posXY;
#else
    pos = Vec2d::ScaleX(&cs, &p1) + Vec2d::ScaleY(&sc, &p3) + posXY;
#endif
    uv  = Vec2d::mergeXY(&_uv0, &_uv1) * _scaleUV;
    vb->wgp_write(color, posZ, pos, uv);

#ifdef BUGFIX_PIVOT_INVERTUV
    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p1) + posXY;
#else
    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p3) + posXY;
#endif
    uv  = _uv1 * _scaleUV;
    vb->wgp_write(color, posZ, pos, uv);
#ifdef BUGFIX_PIVOT_INVERTUV
    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p3) + posXY;
#else
    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p1) + posXY;
#endif
    uv  = Vec2d::mergeXY(&_uv1, &_uv0) * _scaleUV;
    vb->wgp_write(color, posZ, pos, uv);
#else
    Vec2d::to3d(&data[0].m_pos, &pos, posZ);
    data[0].m_color = color;
    data[0].m_uv = _uv0 * _scaleUV;
#ifdef BUGFIX_PIVOT_INVERTUV
    pos = Vec2d::ScaleX(&cs, &p1) + Vec2d::ScaleY(&sc, &p1) + posXY;
#else
    pos = Vec2d::ScaleX(&cs, &p1) + Vec2d::ScaleY(&sc, &p3) + posXY;
#endif
    uv  = Vec2d::mergeXY(&_uv0, &_uv1) * _scaleUV;
    Vec2d::to3d(&data[1].m_pos, &pos, posZ);
    data[1].m_color = color;
    data[1].m_uv = uv;
#ifdef BUGFIX_PIVOT_INVERTUV
    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p1) + posXY;
#else
    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p3) + posXY;
#endif
    uv  = _uv1 * _scaleUV;
    Vec2d::to3d(&data[2].m_pos, &pos, posZ);
    data[2].m_color = color;
    data[2].m_uv = uv;
#ifdef BUGFIX_PIVOT_INVERTUV
    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p3) + posXY;
#else
    pos = Vec2d::ScaleX(&cs, &p3) + Vec2d::ScaleY(&sc, &p1) + posXY;
#endif
    uv  = Vec2d::mergeXY(&_uv1, &_uv0) * _scaleUV;
    Vec2d::to3d(&data[3].m_pos, &pos, posZ);
    data[3].m_color = color;
    data[3].m_uv = uv;
#endif
}

///----------------------------------------------------------------------------//

void Quad_ToVBOrient( VertexPCT* __restrict _data,const ITF_Particle* __restrict _pParticle,const Vec2d& _pivot, const Vec2d& _uv0, const Vec2d& _uv1, const Vec2d& _scaleUV, const Vec2d& _scaleSize)
{
    Vec3d   vertex[4];

    /// orient dir to cam.
    Vec3d SZ;
    GFX_Vector4 vTmp;
    const Matrix44* mview = GFX_ADAPTER->getViewMatrix();
    storeXYZW((f32*)&vTmp, mview->I());
    SZ.setX(vTmp.z());
    storeXYZW((f32*)&vTmp, mview->J());
    SZ.setY(vTmp.z());
    storeXYZW((f32*)&vTmp, mview->K());
    SZ.setZ(vTmp.z());

    Vec3d SY = _pParticle->m_velReal;
    Vec3d SX;
    SY.normalize();
    SY.crossProduct(SZ, SX);
    SX.normalize();

    //SX = Vec3d(1.f,0.f,0.f);//Vec3d( mview->_11, mview->_21, mview->_31);
    //SY = Vec3d(0.f,1.f,0.f);//Vec3d( mview->_12, mview->_22, mview->_32);
    //SZ = Vec3d(0.f,1.f,0.f);//Vec3d( mview->_13, mview->_23, mview->_33);

    SX *= (_pParticle->m_curSize.x() * _scaleSize.x() * 0.5f);
    SY *= (_pParticle->m_curSize.y() * _scaleSize.y() * 0.5f);

    Vec3d normal0 = SX + SY;
    Vec3d normal1 = SX - SY;
    u32 color = _pParticle->m_curColor.getAsU32();

    if (!_pivot.IsEqual(Vec2d::Zero,MTH_EPSILON))
    {
        Vec2d v_Size = _pParticle->m_curSize * _scaleSize;
        Vec2d v_off  = _pivot * v_Size;
        v_Size *= 0.5f;

        SX.normalize();
        SY.normalize();

        Vec3d nSX = -SX;
        Vec3d nSY = -SY;

        SX *= (v_Size.x() - v_off.x());
        SY *= (v_Size.y() - v_off.y());
        nSX *= (v_Size.x() + v_off.x());
        nSY *= (v_Size.y() + v_off.y());
     
        //SZ *= _quad->m_pivot.z();
        //v += SZ;
        SZ = Vec3d(Vec3d::Zero);
        
        vertex[0] = _pParticle->m_pos + nSX + nSY;
        vertex[1] = _pParticle->m_pos + nSX + SY;
        vertex[2] = _pParticle->m_pos + SX + SY;
        vertex[3] = _pParticle->m_pos + SX + nSY;
    }
    else
    {
        vertex[0] = _pParticle->m_pos - normal0;
        vertex[1] = _pParticle->m_pos - normal1;
        vertex[2] = _pParticle->m_pos + normal0;
        vertex[3] = _pParticle->m_pos + normal1;
    }

    Vec2d uv0 = _uv0 * _scaleUV;
    Vec2d uv1 = Vec2d::mergeXY(&_uv1, &_uv0) * _scaleUV;
    Vec2d uv2 = Vec2d::mergeXY(&_uv0, &_uv1) * _scaleUV;
    Vec2d uv3 = _uv1 * _scaleUV;

#ifdef USE_WGP
    ITF_VertexBuffer *vb = reinterpret_cast<ITF_VertexBuffer*>(_data);
    ITF_ASSERT(vb->bIsDynamic());
    vb->wgp_write(color, vertex[1].z(), vertex[1].truncateTo2D(), uv0);
    vb->wgp_write(color, vertex[0].z(), vertex[0].truncateTo2D(), uv1);
    vb->wgp_write(color, vertex[2].z(), vertex[2].truncateTo2D(), uv2);
    vb->wgp_write(color, vertex[3].z(), vertex[3].truncateTo2D(), uv3);
#else
    _data[0].m_pos = vertex[1];
    _data[0].m_color = color;
    _data[0].m_uv = uv0;

    _data[3].m_pos = vertex[0];
    _data[3].m_color = color;
    _data[3].m_uv = uv1;

    _data[1].m_pos = vertex[2];
    _data[1].m_color = color;
    _data[1].m_uv = uv2;

    _data[2].m_pos = vertex[3];
    _data[2].m_color = color;
    _data[2].m_uv = uv3;
#endif
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::fillNormal(const ITF_Particle * __restrict pParticle, const Texture* __restrict pTexture, VertexPCT* pdata, const Vec2d& _scaleUV)
{
    Vec2d uv0(Vec2d::Zero);
    Vec2d uv1(Vec2d::One);
    Vec2d sizeScale(Vec2d::One);

    if (pTexture->getUVAtlas() && pParticle->m_indexUV >= 0 )
    {
        i32 idx = pParticle->m_indexUV;
//#ifndef ITF_FINAL  //Work around for yves milestone
        if((u32)pParticle->m_indexUV >= pTexture->getUVAtlas()->getNumberUV())
            idx = pTexture->getUVAtlas()->getNumberUV()-1;
//#endif
        const UVdata &uvData = pTexture->getUVAtlas()->getUVDatabyIndex(idx);
        uv0 = uvData.getUV0();
        uv1 = uvData.getUV1();

        if(getTemplateParameters()->getAtlasSize())
            sizeScale = uv1 - uv0;
    }    

    if (pParticle->m_flag & PAR_F_UVINVERT_X)
    {
        Vec2d tmp(uv0);

        if (pParticle->m_flag & PAR_F_UVINVERT_Y)
        {
            uv0 = uv1;
            uv1 = tmp;
        }
        else
        {
            uv0 = Vec2d::mergeXY(&uv1, &uv0);
            uv1 = Vec2d::mergeXY(&tmp, &uv1);
        }
    }
    else if (pParticle->m_flag & PAR_F_UVINVERT_Y)
    {
        Vec2d tmp(uv0);
        uv0 = Vec2d::mergeXY(&uv0, &uv1);
        uv1 = Vec2d::mergeXY(&uv1, &tmp);
    }
        
#ifdef USEVB		
    /// compute pivot.
    Vec2d pivot;
    Vec2d::Mul(&pivot, &pParticle->m_curSize, &getTemplateParameters()->getPivot());
    if((m_params.isFlip() && (pParticle->m_flag & PAR_F_FLIPPEDATSPAWN) == 0) || (!m_params.isFlip() && (pParticle->m_flag & PAR_F_FLIPPEDATSPAWN) != 0))
        pivot.x() *= -1.f;

    if (pivot.IsEqual(Vec2d::Zero,MTH_EPSILON)) // ???
    {
        if (getTemplateParameters()->getOrientDir() == 2)
            Quad_ToVBOrient(pdata,pParticle, pivot, uv0, uv1, _scaleUV, sizeScale);
        else
            Quad_ToVB(pdata, pParticle->m_angle,pParticle->m_curColor.getAsU32(),pParticle->m_pos,pParticle->m_curSize, uv0, uv1, _scaleUV, sizeScale);
    }
    else
    {
        if (getTemplateParameters()->getOrientDir() == 2)
        {
            pivot = getTemplateParameters()->getPivot();
            if((m_params.isFlip() && (pParticle->m_flag & PAR_F_FLIPPEDATSPAWN) == 0) || (!m_params.isFlip() && (pParticle->m_flag & PAR_F_FLIPPEDATSPAWN) != 0))
                pivot.x() *= -1.f;

            Quad_ToVBOrient(pdata,pParticle, pivot, uv0, uv1, _scaleUV, sizeScale);
        }
        else
            Quad_ToVBPivot(pdata, pParticle, pivot, uv0, uv1, _scaleUV, sizeScale);
    }
#else
    ITF_ASSERT(0);
#endif
}

///----------------------------------------------------------------------------//

#ifdef PAR_USE_ZSORT
u32 ITF_ParticleGenerator::FillZsort(Texture* pTexture, VertexPCT* pdata)
{
    //TOBE IMPLEMENTED
    ITF_ASSERT(0);
    u32 count = 0;
    return count;
}
#endif // PAR_USE_ZSORT

bbool ITF_ParticleGenerator::render(const GraphicComponent * _graphicComponent, const ITF_VECTOR <class View*>& _views)
{
#ifndef ITF_FINAL
    if(m_DbgSkipDraw)
        return btrue;
#endif

    drawParticles(_graphicComponent, _views);
        
    return btrue;
}

#ifdef ITF_SUPPORT_EDITOR
void ITF_ParticleGenerator::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const
{
    Vec2d pos2D = m_params.getPosition().truncateTo2D();
    DebugDraw::AABB(getTrueAABB() ? Vec2d::Zero : pos2D, m_params.getPosition().z(), getLocalRotationZ(), m_params.m_bounding, Color::yellow());
}
#endif // ITF_SUPPORT_EDITOR

///----------------------------------------------------------------------------//
bbool ITF_ParticleGenerator::checkGeneratorValidity()
{
    if (m_checkValidity) return btrue;

    /// check atlas and phase.
    Texture* pTexture = m_params.getMaterial().getTexture();
    if ((!pTexture) || (!pTexture->isPhysicallyLoaded()))
        return bfalse;

//    ITF_ASSERT_MSG(!pTexture->getPath().getString().strstr(Resource::getTracedResourceName()), "ITF_ParticleGenerator client of traced resource");


    const UVAtlas* atlas = pTexture->getUVAtlas();
    if (atlas && !getAnimMeshVertex())
    {
        u32 atlasmaxindex = m_atlasMaxIndex;
        
        if (getStartAnimIndex() >= 0 && getEndAnimIndex() >= 0)
        {
            if ((u32)getStartAnimIndex() >= atlasmaxindex || (u32)getEndAnimIndex() >= atlasmaxindex)
            {
                m_validityMessage.setTextFormat("global anim index > max atlas index from texture: %s", m_params.getMaterial().getTextureResID().getDebugString().cStr());
                m_checkValidity = btrue;
                return bfalse;
            }

            u32 nbphase = m_params.getNumberPhase();
            for (u32 i =0 ; i < nbphase; i++)
            {
                if (m_params.m_phaseList[i].m_animStart >= 0 && m_params.m_phaseList[i].m_animEnd >= 0)
                {
                    if ((u32)m_params.m_phaseList[i].m_animStart >= atlasmaxindex || (u32)m_params.m_phaseList[i].m_animEnd >= atlasmaxindex)
                    {
                        m_validityMessage.setTextFormat("phase anim index > max atlas index from texture: %s", m_params.getMaterial().getTextureResID().getDebugString().cStr());
                        m_checkValidity = btrue;
                        return bfalse;
                    }
                }
           
            }
        }
    }

    m_checkValidity = btrue;
    m_validityMessage.clear();
    return btrue;
}

void ITF_ParticleGenerator::lightProcess()
{
    if (!m_params.getDisableLight())
    {
        Color m_LightColorComputed(Color::white());
        applyAmbiantColor(m_LightColorComputed);
    }
}

#ifndef ITF_SUPPORT_MULTITHREADED
void ITF_ParticleGenerator::fillActiveList(ITF_VertexBuffer *_VB)
{
    if (!m_particlesCountVtxToDisplay)
        return;

    Texture* pTexture = getParameters()->getMaterial().getTexture();
    ITF_ASSERT(pTexture && pTexture->isPhysicallyLoaded());

    const ParticleGeneratorParameters* templateParams = getTemplateParameters(); 
    const Vec2d scaleUV(pParams->isUseTextureMirrorU() ? 2.f : 1.f, templateParams->isUseTextureMirrorV() ? 2.f : 1.f);

    for(ITF_Particle* pParticle = m_pActiveList; pParticle ; pParticle = pParticle->m_pNext)
    {
        if(pParticle->m_bAlive)
        {
            fillNormal(pParticle,pTexture,reinterpret_cast<VertexPCT*>(_VB), scaleUV);
        }
    }
}

//------------------------------------------------------------------------------------------

void ITF_ParticleGenerator::fillParticuleArray(ITF_VertexBuffer *_VB)
{
    if (!m_particlesCountVtxToDisplay)
        return;

    Texture* pTexture = getParameters()->getMaterial().getTexture();
    ITF_ASSERT(pTexture && pTexture->isPhysicallyLoaded());

    const ParticleGeneratorParameters* templateParams = getTemplateParameters(); 
    const Vec2d scaleUV(pParams->isUseTextureMirrorU() ? 2.f : 1.f, templateParams->isUseTextureMirrorV() ? 2.f : 1.f);

    for (u32 index = 0;index<m_maxParticlesNeed;++index)
    {
        if (m_pParticleArrayIndex[index].m_bAlive)
        {
            fillNormal(&m_pParticleArrayIndex[index],pTexture,reinterpret_cast<VertexPCT*>(_VB),scaleUV);
        }
    }
}

//------------------------------------------------------------------------------------------

void ITF_ParticleGenerator::fillParticles(ITF_VertexBuffer *_VB)
{
    switch(getParameters()->getGeneratorMode())
    {
    case PARGEN_MODE_MANUAL:
        fillActiveList(_VB);
        break;
    case PARGEN_MODE_FOLLOW: 
    case PARGEN_MODE_COMPLEX:
    default:
        fillParticuleArray(_VB);
        break;
    }
}
#endif // ITF_SUPPORT_MULTITHREADED


bbool ITF_ParticleGenerator::drawParticles(const GraphicComponent * _graphicComponent, const ITF_VECTOR <class View*>& _views, bbool _direct)
{
//    LOG("ITF_ParticleGenerator::drawParticles %d", this);

    STATS_PARTICULE_DRAWINC
    if (!m_isRender)
        return bfalse;
#if 0
    if(getTrueAABB())
    {
        bbool bInView = bfalse;
        for(u32 i = 0; (i < _views.size()) && !bInView; ++i)
            bInView |= _views[i]->getCamera()->isRectVisible( getParameters()->getBoundingBoxWorking(), _graphicComponent->GetActor()->getDepth() );
        if(!bInView)
            return bfalse;
    }
#endif

    Texture* pTexture = getParameters()->getMaterial().getTexture();
    if ((!pTexture) || (!pTexture->isPhysicallyLoaded()))
        return bfalse;

   if (m_activeParticlesCount == 0)
        return bfalse;

    AnimMeshVertex *animMeshVertex = getAnimMeshVertex();
    if(animMeshVertex)
    {
        Transform3d tansform;
        f32 z;
        if(getParameters()->m_useMatrix || getParameters()->m_useActorTranslation)
        {
            GFX_Vector4 vI, vJ, vT;
            storeXY ((f32*)&vI, m_transformMatrix44.I());
            storeXY ((f32*)&vJ, m_transformMatrix44.J());
            storeXYZ((f32*)&vT, m_transformMatrix44.T());

            tansform.m_rot.set(vI.x(), vI.y(), vJ.x(), vJ.y());
            tansform.m_pos = *(const Vec2d*)&vT;
            tansform.m_z = vT.z();
            z = vT.z();
        }
        else
        {
            tansform.setIdentity();
            z = _graphicComponent->GetActor()->getDepth() + _graphicComponent->getDepthOffset();
        }

        AMVRenderData amvData(getParameters()->m_amvID, z,
                        AMVRenderData::computeMaskFromViews(_views), 
                        _graphicComponent->getGfxPrimitiveParam(),
                        _graphicComponent->GetActor()->getRef());
        amvData.growAABB(_graphicComponent->GetActor()->getAABB());
        ANIM_MANAGER->computeAnimMeshVertex(*animMeshVertex, m_animListRuntime, amvData, tansform);
        return btrue;
    }

    lightProcess();

#ifdef USEVB
    #ifndef ITF_SUPPORT_MULTITHREADED
        ITF_VertexBuffer * pVB = NULL;
        #ifdef VBMANAGER_USE_DYNAMICRING_VB
            // Set real vtx number to avoid extra lock
            if(m_mesh.m_vtxBuffer->bIsDynamic())
                m_mesh.m_vtxBuffer->m_nVertex = min(m_particlesCountVtxToDisplay, m_maxParticlesNeed) * 4;
            if(!m_mesh.m_vtxBuffer->m_nVertex)
                return bfalse;
        #endif // VBMANAGER_USE_DYNAMICRING_VB
        fillParticles(LockVB(&pVB));
    #endif // ITF_SUPPORT_MULTITHREADED
#else // USEVB
    /// setup Matrix to identity.
    GFX_ADAPTER->setMatrixTransform(Matrix44::identity());
    GFX_ADAPTER->setAlphaBlend(m_params.m_blendMode);
#endif // USEVB

    if (!m_particlesCountVtxToDisplay)
    {
        #ifndef ITF_SUPPORT_MULTITHREADED
        UnlockVB(pVB);
        #endif // ITF_SUPPORT_MULTITHREADED
        return bfalse;
    }
    
#ifdef USEVB
# ifdef CACHEDVB_JOB
    ITF_VertexBuffer * pVB = NULL;
    VertexPCT* data = LockVB(&pVB);
    if(data != NULL)
# endif // CACHEDVB_JOB
    {
# ifdef CACHEDVB_JOB
        #ifdef USE_WGP
            ITF_ASSERT(pVB->bIsDynamic());
            const VertexPCT* pVtx = (VertexPCT*)m_cacheMemory;
            for(int i = 0; i < m_particlesCountVtxToDisplay*4; ++i, ++pVtx)
            {
                pVB->wgp_write(pVtx->m_pos);
                pVB->wgp_write(pVtx->m_color);
                pVB->wgp_write(pVtx->m_uv);
            }
        #else // USE_WGP
            ITF_MemcpyWriteCombined(data,m_cacheMemory,sizeof(VertexPCT)*m_particlesCountVtxToDisplay*4);
        #endif // USE_WGP
# endif // CACHEDVB_JOB

#ifndef DIRECTVB_JOB
        UnlockVB(pVB);
#endif // DIRECTVB_JOB

        ITF_MeshElement & _elem = m_mesh.getMeshElement();
        _elem.m_count = m_particlesCountVtxToDisplay * 6;

        Color globalColor;
        if (!m_params.getDisableLight())
        {
            globalColor.m_r = m_ambColor.m_r;
            globalColor.m_g = m_ambColor.m_g;
            globalColor.m_b = m_ambColor.m_b;
            globalColor.m_a = 1.0f;
        } 
        else
        {
            globalColor     = Color::white();
        }
        globalColor = globalColor * m_factorColor;
        globalColor.m_a *= getAlpha();

        Color curveGlobalColorFactor = Color::white();
        if(getTemplateParameters()->m_parEmitColorFactor.isSet())
        {
            Vec3d colfactor;
            getTemplateParameters()->m_parEmitColorFactor.getValue(colfactor, getCurrentTime());
            curveGlobalColorFactor.m_r = colfactor.x();
            curveGlobalColorFactor.m_g = colfactor.y();
            curveGlobalColorFactor.m_b = colfactor.z();
        }
        if(getTemplateParameters()->m_parEmitAlpha.isSet())
            curveGlobalColorFactor.m_a = getTemplateParameters()->m_parEmitAlpha.getValue(getCurrentTime());

        globalColor = globalColor * curveGlobalColorFactor;

        if( m_FrameDieCount != U32_INVALID) 
        {
            globalColor.m_a *= (f32)(m_FrameDieCount) / (m_params.m_dieFadeTime * 60.f);
        }

        GFXPrimitiveParam primParam = getGfxPrimitiveParam();
        primParam.m_colorFactor = globalColor;
        primParam.m_BV = m_params.getBoundingBox();
        setGFXPrimitiveParamToMesh(primParam);

        if (_direct)
        {
            RenderPassContext passCtx;
            PrimitiveContext primitiveCxt;
            primitiveCxt.setRenderPassContext(&passCtx);
            primitiveCxt.setPrimitiveParam(&primParam);

            GFX_ADAPTER->drawMesh(primitiveCxt, m_mesh); 

            #ifdef ITF_WII
            // Pass for separate alpha
            if (m_params.isImpostor())
            {
                GFX_ADAPTER_WII->startImpostorAlphaRendering(m_renderTargetImpostor);
                GFX_ADAPTER->drawMesh(m_mesh, bfalse); 
            }
            #endif // ITF_WII
        }
        else
        {
            if(getParameters()->m_useMatrix || getParameters()->m_useActorTranslation)
                m_mesh.setMatrix(m_transformMatrix44);
            else
                m_mesh.setMatrix(Matrix44::identity());

            f32 zr = 0.f;
            if (m_params.m_useZAsDepth)
                zr = m_params.m_pos.z();
            else
                zr = m_params.m_depth;

            zr+= m_params.m_renderPrio;
            
            if (m_params.getIs2D())
            {
                m_mesh.set2DPosAndScale(m_params.m_pos.truncateTo2D(), Vec2d::One);
                GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_2D>(_views, &m_mesh, zr, _graphicComponent->GetActor()->getRef());
            }
            else
            {
                if (m_renderInTarget)
                {
                    // GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_RTARGET>(_views, &m_mesh, zr, ObjectRef::InvalidRef);
                    GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_mesh, zr, _graphicComponent->GetActor()->getRef());
                }
                else
                    GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_mesh, zr, _graphicComponent->GetActor()->getRef());
            }
        }
    }
# ifdef CACHEDVB_JOB
    else
        return bfalse;
# endif // CACHEDVB_JOB
#endif // USEVB

    return btrue;
}

///----------------------------------------------------------------------------//

Vec3d getRandomVector()
{
    Vec3d v;
    v.x() = ITF_ParticleGenerator::randomLCG.getRandF( -1.0f, 1.0f );
    v.y() = ITF_ParticleGenerator::randomLCG.getRandF( -1.0f, 1.0f );
    v.z() = ITF_ParticleGenerator::randomLCG.getRandF( -1.0f, 1.0f );
    v.normalize();
    return v;
}
///----------------------------------------------------------------------------//

void   ITF_ParticleGenerator::deallocateParticle(ITF_Particle* pParticle)
{
    //compute the index
    u32 index = (u32)((uPtr)pParticle-(uPtr)m_pParticleArrayIndex)/sizeof(ITF_Particle);
    pParticle->dead();

    m_pFreeParticleIndex.push_back(index);
}
///----------------------------------------------------------------------------//

ITF_Particle*   ITF_ParticleGenerator::allocateParticle()
{
    const  u32 size = m_pFreeParticleIndex.size();
    ITF_ASSERT(size != 0);

    const u32 index = size-1;

    ITF_Particle* pParticle = &m_pParticleArrayIndex[m_pFreeParticleIndex[index]];
    m_pFreeParticleIndex.eraseNoOrder(index);

    pParticle->live();
    return pParticle;
}
///----------------------------------------------------------------------------//

Vec3d getRandomVectorInter(f32 _ang1 , f32 _ang2, f32 _zinter1, f32 _zinter2)
{
    Vec3d v;
    v.z() = ITF_ParticleGenerator::randomLCG.getRandF( _zinter1, _zinter2 );
    
    f32 r = (float)f32_Sqrt(1 - v.z() * v.z());
    
    f32 t = ITF_ParticleGenerator::randomLCG.getRandF( _ang1, _ang2 );

    f32_CosSin(t, &v.x(), &v.y());
    v.x() *= r;
    v.y() *= r;

    return v;
}
///----------------------------------------------------------------------------//

ITF_INLINE Vec3d getRandomVector2d(f32 _min, f32 _max)
{
    f32 t = ITF_ParticleGenerator::randomLCG.getRandF( _min, _max );
    Vec3d v;
    f32_CosSin(t, &v.x(), &v.y());
    v.z() = 0.f;
    return v;
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::update(f32 _dt, bbool _bDisableCreation)
{
//    LOG("ITF_ParticleGenerator::update %d", this);
#ifndef ITF_FINAL
    if(m_DbgNoparticules)
    {
        if(m_activeParticlesCount)
            reInit();
        return;
    }

    if(m_DbgSkipUpdate)
        return;
#endif

    if(m_FrameDieCount != U32_INVALID)
    {
        _bDisableCreation = btrue;
        if(m_FrameDieCount)
        {
            m_FrameDieCount--;
        }
        else
        {
            reInit();
        }
    }

    m_isRender = btrue;

    ParticleGeneratorInstanceParameters* parameter = getParameters(); 

    if (m_aabbNeedCompute && getTrueAABB())
    {        
        parameter->setWorkingBoundingBox(parameter->getPosition());
        parameter->synchronizeBoundingBox();
        m_aabbNeedCompute = bfalse;
    }

    // early skip, because nothing to do.
    if(!m_activeParticlesCount && _bDisableCreation)
        return;

    if(getTemplateParameters()->m_parEmitGravity.isSet())
    {
        Vec3d grav;
        getTemplateParameters()->m_parEmitGravity.getValue(grav, getCurrentTime());
        m_params.setGravity(grav);
    }

    if(getTemplateParameters()->m_parEmitAcceleration.isSet())
    {
        Vec3d acc;
        getTemplateParameters()->m_parEmitAcceleration.getValue(acc, getCurrentTime());
        m_params.setAcc(acc);
    }

    if(!m_params.m_randomizeDirection && getTemplateParameters()->m_parEmitVelocityAngle.isSet())
    {
        Vec3d tmp;
        getTemplateParameters()->m_parEmitVelocityAngle.getValue(tmp, getCurrentTime());
        f32 angle = randomLCG.getRandF(tmp.x(), tmp.y());
        if(m_params.isFlip() && (getTemplateParameters()->canFlipAngleMin() || getTemplateParameters()->canFlipAngleMax()))
            angle = 180.0f - angle;
        angle *= MTH_DEGTORAD;
        Vec2d cs;
        Vec2d::CosSinOpt(&cs, angle);
        m_params.m_vel.setFromVec2d(cs);
    }

    if(getTemplateParameters()->m_parEmitAngularSpeed.isSet())
    {
        Vec3d aspeed;
        getTemplateParameters()->m_parEmitAngularSpeed.getValue(aspeed, getCurrentTime());
        f32 angle = randomLCG.getRandF(aspeed.x(), aspeed.y());
        m_params.m_angularSpeed.SetDegrees(angle);
    }

    if(getTemplateParameters()->m_parEmitAngle.isSet())
    {
        Vec3d vAngle;
        getTemplateParameters()->m_parEmitAngle.getValue(vAngle, getCurrentTime());
        f32 angle = randomLCG.getRandF(vAngle.x(), vAngle.y());
        if(m_params.isFlip() && getTemplateParameters()->canFlipInitAngle())
            angle = 180.f - angle;
        m_params.m_initAngle.SetDegrees(-angle);
        m_params.m_angleDelta.SetRadians(0.f);
    }

    if (isUseMatrix() || parameter->m_useActorTranslation)
    {
        computeTransformMatrix();
    }
    else if(!isUseMatrix() && m_params.m_useMatrix == BOOL_cond)
    {
        m_transformMatrixRotation.transformVector(m_transformGravity, m_params.getGravity());
    }
    else
    {
        m_transformGravity = m_params.getGravity();
    }

    if(getAnimMeshVertex())
    {
        if (getParameters()->m_amvID == U32_INVALID)
        {
            computeAMVInfos();
            ITF_ASSERT_CRASH(getParameters()->m_amvID != U32_INVALID, "amvId must not be null!!");
        }
    }
    
    // PUSH THE JOB AT THE END OF THE UPDATE PROCESS
    if (m_maxParticlesNeed>0)
    {
#ifdef DIRECTVB_JOB
        VertexPCT* data = NULL;
        m_mesh.swapVBForDynamicMesh();
        ITF_ASSERT_CRASH(getCachedMemory() == NULL, "[VB] Previous isn't finished ?");
        GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(m_mesh.getCurrentVB(),(void**)&data);
        ITF_ASSERT_MSG(data, "[VB] Unable to lock a vertex buffer");
#endif // DIRECTVB_JOB
        // We only disable creation for infinite particles
        bbool forceDisableCreation = _bDisableCreation && getParameters()->m_emitParticlesCount == U32_INVALID;
        
        JobUpdateParticleContext context(this,_dt, forceDisableCreation);
        ITF_ASSERT(m_jobParticle);
        m_jobParticle->setContext(context);
        Scheduler::get()->pushJob(m_jobParticle);
    }
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::addOneNewComplexParticle()
{
    ITF_ASSERT(m_params.m_genMode!=PARGEN_MODE_MANUAL);

    if( m_activeParticlesCount < m_maxParticlesNeed )
    {
        STATS_PARTICULE_COMPLEXNEWINC

        ITF_Particle  *pParticle = allocateParticle(); //pParticle = newAlloc(mId_Particle,ITF_Particle);

        initNewParticleComplex(pParticle);

        ++m_activeParticlesCount;

        ITF_ASSERT(m_activeParticlesCount<=m_maxParticlesNeed);
        ++m_totalParticlesGenerated;
    }
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::spawnNParticle( u32 _numParticles, f32 maxTimeRange )
{
    setMaxParticles(_numParticles);

    for ( u32 i = 0; i < _numParticles; i++ )
    {
        ITF_Particle *pParticle = allocateParticle();
        initNewParticleComplex(pParticle);
        // add random time
        pParticle->m_initTime += randomLCG.getRandF( 0.f, maxTimeRange );
    }

    // force data to _numParticles
    m_activeParticlesCount      = _numParticles;
    m_totalParticlesGenerated   = _numParticles;
}

    ///----------------------------------------------------------------------------//
u32 ITF_ParticleGenerator::computeParticleToEmitCount(f32 _dt, bbool _bDisableCreation)
{
    f32 currentTime = getCurrentTime();
    const ParticleGeneratorInstanceParameters* pParams = getParameters();
    const ParticleGeneratorParameters* templateParams = getTemplateParameters();

    f32 newPar = 0.f;
    bbool stopGen = bfalse;

    if( pParams->m_emitterMaxLifeTime > 0.f)
    {
        if(getCurrentTime()>= pParams->m_emitterMaxLifeTime)
        {
            currentTime = pParams->m_emitterMaxLifeTime;
            stopGen = btrue;
        }
    }

    f32 fq = pParams->getFrequency() + randomLCG.getRandF( 0.f, pParams->getFrequencyDelta() );
    if(templateParams->getGeneratorEmitMode() != PARGEN_EMITMODE_ALLATONCE) // ignore freq curve in allatonce mode
    {
        if(templateParams->m_genFreq.isSet())
        {
            Vec2d vFq;
            templateParams->m_genFreq.getValue(vFq, currentTime);
            fq = randomLCG.getRandF( vFq.x(), vFq.y() );
            if(fq > 0.f)
                fq = f32_Inv(fq);
        }
    }
    if(fq <= 0.f)
        return 0;
    newPar = (currentTime - m_lastUpdateTime) / fq;

    if( (pParams->m_genBezierDensity > 0.f) && getBezierBranch())
    {
        f32 maxDist = getBezierBranch()->getCurrentLength();
        newPar *= Min(maxDist / pParams->m_genBezierDensity, 1.f);
    }

    if( (m_lastUpdateTime == 0.f) && templateParams->getForceEmitAtStart() && (newPar < 1.f) )
    {
        m_particlesToEmitExact = 0.f;
        newPar = 1.f;
    }

    m_particlesToEmitExact += newPar;
    u32 NumParticlesToEmit = templateParams->getEmitBatchCount() * (u32)m_particlesToEmitExact;
    if(templateParams->getGeneratorEmitMode() == PARGEN_EMITMODE_ALLATONCE && templateParams->getEmitBatchCountMax() != U32_INVALID)
    {
        u32 NumParticlesToEmitMax = templateParams->getEmitBatchCountMax() * (u32)m_particlesToEmitExact;
        if(NumParticlesToEmitMax >= NumParticlesToEmit)
            NumParticlesToEmit = randomLCG.getRandI( NumParticlesToEmit, NumParticlesToEmitMax );
    }

    if (_bDisableCreation || (!m_generationStarted))
    {
        NumParticlesToEmit = 0;
        m_particlesToEmitExact = 0.f;
    }

    m_lastUpdateTime = currentTime;
    m_particlesToEmitExact -= (u32)m_particlesToEmitExact;
    if(m_particlesToEmitExact < 0.f)
        m_particlesToEmitExact = 0.f;

    if( NumParticlesToEmit > 0 )
    {
        if (m_totalParticlesGenerated <= pParams->m_emitParticlesCount)
        {
            NumParticlesToEmit = Min(pParams->m_emitParticlesCount-m_totalParticlesGenerated,NumParticlesToEmit);
            NumParticlesToEmit = Min(NumParticlesToEmit, m_maxParticlesNeed - m_activeParticlesCount);
            if(m_activeParticlesCount >= getMaxParticlesClamp())
                NumParticlesToEmit = 0;
            else
                NumParticlesToEmit = Min(NumParticlesToEmit, getMaxParticlesClamp() - m_activeParticlesCount);
        }
    }
    ITF_ASSERT_CRASH(NumParticlesToEmit < 10000000.f, "Insane number of particles to emit");
    
    if (stopGen)
        stopGeneration();
    
    return NumParticlesToEmit;
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::spawnAtTime(f32 _Time)
{    
    // Compute required particules count
    if(m_activeParticlesCount >= m_params.getMaxParticles())
        return;

#if 0
    u32 numParticules = m_params.getMaxParticles() - getActiveParticlesCount();
    if((int)m_params.m_emitParticlesCount >= 0)
        numParticules = Min(numParticules, getNumGeneratedParticles() - m_params.m_emitParticlesCount);
    numParticules = Min(numParticules, m_params.getEmitBatchCount()*(u32)(ceil(_Time/batchInterval)));
    incCurrentTime(_Time);
    m_lastUpdateTime = getCurrentTime();

#else
    incCurrentTime(APPLOOPDT);
    u32 numParticules = computeParticleToEmitCount(APPLOOPDT, bfalse);
    incCurrentTime(_Time-APPLOOPDT);
    numParticules += computeParticleToEmitCount(_Time-APPLOOPDT, bfalse);
#endif
    numParticules = Min(m_maxParticlesNeed, numParticules);

    f32 batchInterval = m_params.getFrequency();

    if( (m_params.m_genBezierDensity > 0.f) && getBezierBranch())
    {
        f32 maxDist = getBezierBranch()->getCurrentLength();
        numParticules *= static_cast<u32>(Min(maxDist/m_params.m_genBezierDensity, 1.f));
    }

    if(!numParticules)
        return;

    ITF_VECTOR<ITF_Particle*> parVec(numParticules);

    f32 *phaseLifeTime;
    f32 *phaseLifeTimeRand;
    f32 *phaseLifeTimeMax;
    f32 *phaseLife;
    f32 baseLifeTime = 0.f;
    f32 maxLifeTime = 0.f;
    f32 maxDeltaLifeTime = 0.f;

    if(m_params.getNumberPhase())
    {
        phaseLifeTime = (f32*)ITF_alloca(sizeof(f32)*m_params.getNumberPhase());
        phaseLifeTimeRand = (f32*)ITF_alloca(sizeof(f32)*m_params.getNumberPhase());
        phaseLifeTimeMax = (f32*)ITF_alloca(sizeof(f32)*m_params.getNumberPhase());
        phaseLife = (f32*)ITF_alloca(sizeof(f32)*m_params.getNumberPhase());

        for(u32 i = 0; i < m_params.getNumberPhase(); ++i)
        {
            phaseLifeTime[i] = m_params.m_phaseList[i].m_phaseTime * m_params.getLifeTimeMultiplier();
            phaseLifeTimeRand[i] = m_params.m_phaseList[i].m_deltaphasetime * m_params.getLifeTimeMultiplier();
            phaseLifeTimeMax[i] = phaseLifeTime[i] + phaseLifeTimeRand[i];
            baseLifeTime += phaseLifeTime[i];
            maxDeltaLifeTime += phaseLifeTimeRand[i];
        }
    }
    else
    {
        phaseLifeTime = (f32*)ITF_alloca(sizeof(f32));
        phaseLifeTimeRand = (f32*)ITF_alloca(sizeof(f32));
        phaseLifeTimeMax = (f32*)ITF_alloca(sizeof(f32));
        phaseLife = (f32*)ITF_alloca(sizeof(f32));

        phaseLifeTime[0] = 0;
        phaseLifeTimeRand[0] = 0;
        phaseLifeTimeMax[0] = F32_INFINITY;
        baseLifeTime = F32_INFINITY;
    }

    phaseLife[0] = phaseLifeTimeMax[0];
    for(u32 i = 1; i < m_params.getNumberPhase(); ++i)
        phaseLife[i] = phaseLifeTimeMax[i] + phaseLifeTimeMax[i-1];

    maxLifeTime = baseLifeTime + maxDeltaLifeTime;

    if(_Time > maxLifeTime)
        _Time = maxLifeTime;

    u32 batchCount = 0;
    u32 batchCountMax = 0;
    switch(getTemplateParameters()->getGeneratorEmitMode())
    {
        case PARGEN_EMITMODE_ALLATONCE:
        {
            batchCount = getTemplateParameters()->getEmitBatchCount();
            batchCountMax = getTemplateParameters()->getEmitBatchCountMax();
            if(batchCountMax != U32_INVALID)
                batchCount = (u32) randomLCG.getRandI(batchCount, batchCountMax);
            break;
        }
        case PARGEN_EMITMODE_OVERTIME:
        default:
        {            
            batchCount = getTemplateParameters()->getEmitBatchCount() * (u32)(0.5f + (APPLOOPDT / batchInterval));
            if(!batchCount)
                batchCount = 1;
            batchInterval = _Time / ((f32)numParticules / (f32)batchCount);
        }
        break;
    }

    u32 curBatchCount = 0;
    f32 spawnTime = _Time;
    Vec2d spawnLiveTime;
    u32 phaseMin = 0;
    u32 phaseMax = 0;

    f32 phTime, fm, fM;

    phTime = spawnTime>maxLifeTime ? spawnTime - maxLifeTime * ((u32)(spawnTime/maxLifeTime)) : spawnTime;
    fM = 0.f; phaseMin = 0; fm = phaseLifeTimeMax[phaseMin];
    for(phaseMax = 0; phaseMax < m_params.getNumberPhase()-1; ++phaseMax)
    {
        fM += phaseLifeTimeMax[phaseMax];
        if(fM>phTime)
            break;
        if(fm<phTime)
            fm += phaseLifeTimeMax[++phaseMin];
    }

    if(getTemplateParameters()->m_parLifeTime.isSet())
        getTemplateParameters()->m_parLifeTime.getValue(spawnLiveTime, spawnTime);

    for(u32 i = 0; i < numParticules; ++i)
    {
        STATS_PARTICULE_COMPLEXNEWINC

        ITF_Particle *pParticle = allocateParticle();
        parVec[i] = pParticle;
        initNewParticleComplex(pParticle);
        ++m_activeParticlesCount;
        ++m_totalParticlesGenerated;

        pParticle->m_curPhase = randomLCG.getRandI(phaseMin, phaseMax);
        pParticle->m_phaselifeTime = -randomLCG.getRandF(0.f,phaseLifeTimeRand[pParticle->m_curPhase]);
        pParticle->m_initTime = spawnTime - pParticle->m_phaselifeTime; // store life time in m_initTime
        pParticle->m_birthTime = getCurrentTime() - pParticle->m_initTime;
        if(getTemplateParameters()->m_parLifeTime.isSet())
            pParticle->m_dieTime = pParticle->m_birthTime + randomLCG.getRandF( spawnLiveTime.x(), spawnLiveTime.y() );
        else
            pParticle->m_dieTime = pParticle->m_birthTime + m_maxParLiveTimeFromPhases;
        pParticle->m_phaselifeTime += phaseLife[pParticle->m_curPhase];

        curBatchCount++;
        if(curBatchCount >= batchCount)
        {
            curBatchCount = 0;
            spawnTime -= batchInterval;

            if(!(m_template->getIsParticlePhaseLoop() || getTemplateParameters()->m_parLifeTime.isSet()))
            {
                phTime = spawnTime>maxLifeTime ? spawnTime - maxLifeTime * ((u32)(spawnTime/maxLifeTime)) : spawnTime;
                fM = 0.f; phaseMin = 0; fm = phaseLifeTimeMax[phaseMin];
                for(phaseMax = 0; phaseMax < m_params.getNumberPhase()-1; ++phaseMax)
                {
                    fM += phaseLifeTimeMax[phaseMax];
                    if(fM>phTime)
                        break;
                    if(fm<phTime)
                        fm += phaseLifeTimeMax[++phaseMin];
                }
            }

            if(getTemplateParameters()->m_parLifeTime.isSet())
                getTemplateParameters()->m_parLifeTime.getValue(spawnLiveTime, spawnTime);
        }
    }

    const Vec3d &acc = getParticlesAcceleration();

    Vec3d bezierPosBeg(0,0,0);
    Vec3d bezierPosEnd(0,0,0);
    Vec3d bezierTanBeg_m(0,0,0);
    Vec3d bezierTanEnd(0,0,0);
    f32 maxBezierDist = 0.f;
    if(getTemplateParameters()->getFollowBezier() && m_BezierBranch)
    {
        const BranchCurve& curve = m_BezierBranch->getCurve();
        maxBezierDist = m_BezierBranch->getCurrentLength();

        curve.getPosTanAtDistance(0.f, bezierPosBeg, bezierTanBeg_m);
        curve.getPosTanAtDistance(maxBezierDist, bezierPosEnd, bezierTanEnd);
        bezierTanBeg_m = -bezierTanBeg_m.normalize();
        bezierTanEnd.normalize();
    }

    // forward particule time.
    for(u32 i = 0; i < numParticules; ++i)
    {
        ITF_Particle *pParticle = parVec[i];

        f32 parAliveTime = pParticle->m_initTime;

        // pos, speed
        pParticle->m_velAcc = acc * parAliveTime;
        pParticle->m_vel *= getParameters()->getFriction();
        pParticle->m_velReal = pParticle->m_vel + pParticle->m_velAcc;

        if(getTemplateParameters()->getFollowBezier() && m_BezierBranch)
        {
            const BranchCurve& curve = m_BezierBranch->getCurve();
            f32 velNorm = pParticle->m_vel.norm();
            if(pParticle->m_bezierDist)
                if(maxBezierDist >= pParticle->m_bezierDist )
                    pParticle->m_pos = bezierPosBeg + pParticle->m_pos - curve.getPosAtDistance(pParticle->m_bezierDist);

            if(getTemplateParameters()->getFollowBezier() > 0)
            {// particles follow bezier from start to end
                pParticle->m_bezierDist += velNorm*parAliveTime;
                if(maxBezierDist > pParticle->m_bezierDist )
                    pParticle->m_pos = curve.getPosAtDistance(pParticle->m_bezierDist) + pParticle->m_pos - bezierPosBeg;
                else
                {
                    Vec3d::ScaleAdd(&pParticle->m_pos, &bezierTanEnd, pParticle->m_bezierDist-maxBezierDist, &bezierPosEnd);
                    pParticle->m_vel = bezierTanEnd * velNorm;
                }
            }
            else
            {// particles follow bezier from end to start
                pParticle->m_bezierDist -= velNorm*parAliveTime;
                if(pParticle->m_bezierDist >= 0.f)
                    pParticle->m_pos = curve.getPosAtDistance(pParticle->m_bezierDist) + pParticle->m_pos - bezierPosBeg;
                else
                {
                    Vec3d::ScaleAdd(&pParticle->m_pos, &bezierTanBeg_m, pParticle->m_bezierDist-maxBezierDist, &bezierPosBeg);
                    pParticle->m_vel = bezierTanBeg_m * velNorm;
                }
            }
        }
        else
        {
            Vec3d::ScaleAdd(&pParticle->m_pos, &pParticle->m_velReal, parAliveTime, &pParticle->m_pos);
        }

        //getParameters()->initFromPhase(pParticle, getCurrentTime() - parAliveTime, this);

        // Begin initFromPhase
        //
        pParticle->m_initTime = getCurrentTime() - parAliveTime;
        if(m_params.m_phaseList.size() != 0)
        {
            ParPhase* phase = &m_params.m_phaseList[pParticle->m_curPhase];
        
            // begin setParticleFromPhaseInfo(pParticle, phase, m_lifeTimeMultiplier);
            //
            if (phase->m_blendToNextPhase)
                pParticle->m_flag |= PAR_F_BLENDPHASE;
            else
                pParticle->m_flag &= ~PAR_F_BLENDPHASE;

            // if phase have anim information
            if (phase->m_animstretchtime)
                pParticle->m_flag |= PAR_F_ANIM_STRETCHTIME;
            else
                pParticle->m_flag &= ~PAR_F_ANIM_STRETCHTIME;

            if (phase->m_animStart >= 0)
            {
                pParticle->m_startAnimIndex = phase->m_animStart;
                pParticle->m_endAnimIndex = phase->m_animEnd;
                pParticle->m_animId = phase->m_animId;
            }

            //
            // end

            //if particle uses phases color and size data (bfalse for a color and size manual update)
            if( m_params.m_usePhasesColorAndSize )
            {
                pParticle->m_initSize.x() = randomLCG.getRandF(phase->m_sizeMin.x(), phase->m_sizeMax.x());
        
                if ( m_params.m_uniformScale )
                    pParticle->m_initSize.y() = pParticle->m_initSize.x() * m_params.m_uniformScale;
                else
                    pParticle->m_initSize.y() = randomLCG.getRandF(phase->m_sizeMin.y(), phase->m_sizeMax.y());

                Color randomColor = Color::Interpolate(phase->m_colorMin, phase->m_colorMax, randomLCG.getRandF(0.f, 1.f));
                pParticle->m_initColor = applyGlobalColor(randomColor).getAsU32();

                m_params.getDstBlend(pParticle, this);

                pParticle->m_curColor = pParticle->m_initColor;
                pParticle->m_curSize = pParticle->m_initSize;
            }
            //
            // end initFromPhase
        }
    }
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::startManualUpdate( const Vec3d& _pos )
{
    m_params.setBoundingBox(AABB(_pos,_pos));
    getParameters()->setPosition(_pos);
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::updateManualParticle( u32 _index, const Vec3d& _pos, const Vec3d& _speed, Angle _rot )
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_pParticleArrayIndex);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_pParticleArrayIndex )
    {
        return;
    }

    ITF_Particle* particle = &m_pParticleArrayIndex[_index];
    particle->m_pos = _pos;
    particle->m_vel = _speed;
    particle->m_angle = _rot;
    
    m_params.getBoundingBox().grow(_pos);
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualParticleState( u32 _index, bbool _isAlive )
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_pParticleArrayIndex);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_pParticleArrayIndex )
    {
        return;
    }

    ITF_Particle* particle = &m_pParticleArrayIndex[_index];
    particle->m_bAlive = _isAlive;
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualParticleFlip( u32 _index, bbool _isFlip )
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_pParticleArrayIndex);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_pParticleArrayIndex )
    {
        return;
    }

    ITF_Particle* particle = &m_pParticleArrayIndex[_index];
    if (_isFlip)
        particle->m_flag |= PAR_F_UVINVERT_X;
    else
        particle->m_flag &= ~PAR_F_UVINVERT_X;
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualParticleSize(const u32 _index, const f32 _size)
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_pParticleArrayIndex);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_pParticleArrayIndex )
    {
        return;
    }

    ITF_Particle* particle = &m_pParticleArrayIndex[_index];
    particle->m_dstSize.x() = _size;
    particle->m_dstSize.y() = _size;
    particle->m_initSize.x() = _size;
    particle->m_initSize.y() = _size;
    particle->m_curSize.x() = _size;
    particle->m_curSize.y() = _size;
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualParticleColor(const u32 _index, const Color &_color)
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_pParticleArrayIndex);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_pParticleArrayIndex )
    {
        return;
    }

    ITF_Particle* particle = &m_pParticleArrayIndex[_index];
    particle->m_initColor = _color.getAsU32();
    particle->m_dstColor = _color.getAsU32();
    particle->m_curColor = _color.getAsU32();
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualParticleAlpha(const u32 _index, const u8 _alpha)
{
    ITF_ASSERT(m_params.m_genMode == PARGEN_MODE_MANUAL && m_pParticleArrayIndex);
    ITF_ASSERT(_index < m_totalParticlesGenerated);

    if ( !m_pParticleArrayIndex )
    {
        return;
    }

    ITF_Particle* particle = &m_pParticleArrayIndex[_index];
    particle->m_initColor.m_rgba.m_a = _alpha;
    particle->m_dstColor.m_rgba.m_a = _alpha;
    particle->m_curColor.m_rgba.m_a = _alpha;
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualAllParticlesColor(const Color &_color)
{
    for(u32 index = 0; index < m_totalParticlesGenerated; ++index)
    {
        changeManualParticleColor(index, _color);
    }
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::changeManualAllParticlesSize(const f32 _size)
{
    for(u32 index = 0; index < m_totalParticlesGenerated; ++index)
    {
        changeManualParticleSize(index, _size);
    }
}


///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::updateParticleAnim(ITF_Particle* _particle, f32 _time)
{
    const ParticleGeneratorParameters* pParams = getTemplateParameters();
    if(pParams->m_parAnim.isSet())
    {
        f32 ranf1 = float(_particle->GetSeed1()) * float(1.0f/255.f);
        Vec2d indexUVRange;

        const f32 currentTime = getCurrentTime();
        const f32 parLifeTime = _particle->m_dieTime - _particle->m_birthTime;
        const f32 parTimeFactor = parLifeTime<=0.f?1.f:(currentTime - _particle->m_birthTime)/parLifeTime;
        
        getTemplateParameters()->m_parAnim.getValue(indexUVRange, parTimeFactor);
        _particle->m_indexUV = (i32)f32_Lerp(indexUVRange.x(), indexUVRange.y(), ranf1);
        //_particle->m_indexUV = (i32)f32_Lerp(m_template->getStartAnimIndex(), m_template->getEndAnimIndex(),parTimeFactor);
        if((u32)_particle->m_indexUV >= m_atlasMaxIndex)
            _particle->m_indexUV = m_atlasMaxIndex-1;
    }
    else
    {
        if (_particle->m_flag & PAR_F_ANIM_STRETCHTIME)
        {
            f32 f = _time/_particle->m_phaselifeTime;
            if (_particle->m_endAnimIndex < _particle->m_startAnimIndex)
            {
                _particle->m_indexUV = (i32)f32_Lerp((f32)_particle->m_endAnimIndex, (f32)(_particle->m_startAnimIndex + 1), f);
                _particle->m_indexUV = Clamp(_particle->m_indexUV, _particle->m_endAnimIndex, _particle->m_startAnimIndex);
            } else
            {
                _particle->m_indexUV = (i32)f32_Lerp((f32)_particle->m_startAnimIndex, (f32)(_particle->m_endAnimIndex + 1), f);
                _particle->m_indexUV = Clamp(_particle->m_indexUV, _particle->m_startAnimIndex, _particle->m_endAnimIndex);
            }
        } else
        {
            if (_particle->m_endAnimIndex < _particle->m_startAnimIndex)
            {
                u32 mod = (_particle->m_startAnimIndex - _particle->m_endAnimIndex) + 1;
                _particle->m_indexUV = _particle->m_startAnimIndex - (((i32)(_time * m_template->getAnimUVFreq() )) % mod);
            } else
            {
                u32 mod = (_particle->m_endAnimIndex - _particle->m_startAnimIndex) + 1;
                _particle->m_indexUV = _particle->m_startAnimIndex + (((i32)(_time * m_template->getAnimUVFreq() )) % mod);
            }
       }
    }
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::updateAABB(ITF_Particle* _particle)
{
    Vec2d aabbMax = _particle->m_curSize * MTH_SQRT2BY2;
    Vec2d aabbMin = -aabbMax;
    AABB partAABB(aabbMin, aabbMax);
    partAABB.Translate(_particle->m_pos.truncateTo2D());

    //check if we use actor relative space
    if( m_params.m_useActorTranslation )
    {
        //update AABB with gen position + relative position
        partAABB.Translate(m_params.m_pos.truncateTo2D());
    }

    m_params.getBoundingBoxWorking().grow(partAABB);
}

void ITF_ParticleGenerator::computeTransformMatrix()
{
    Matrix44 out;
    out.setIdentity();

    if( m_params.m_useMatrix )
    {
        Matrix44 rotScale;
        Vec3d  scale;

        rotScale.setRotationZ(getLocalRotationZ());
        Vec2d::to3d(&scale, &m_LocalScale, 1.f);
        rotScale.mulScale(scale);
        if( !m_params.m_useActorTranslation )
        {
            Matrix44 invtrans;
            Matrix44 mtransform;
            Matrix44 translation;
            invtrans.setIdentity34();
            invtrans.setT((-m_params.m_pos).toFloat4_1());
            mtransform.mul44(invtrans, rotScale);
            translation.setIdentity34();
            translation.setTranslation(m_params.m_pos);
            out.mul44(mtransform, translation);
        }
        else
        {            
            out = rotScale;
        }

        rotScale.setRotationZ(-getLocalRotationZ());
        Vec2d invScale2D;
        Vec2d::Inv(&invScale2D, &m_LocalScale);
        Vec2d::to3d(&scale, &invScale2D, 1.f);
        rotScale.mulScale(scale);
        rotScale.transformVector(m_transformGravity, m_params.getGravity());
        m_transformMatrixRotation = rotScale;
    }
    else
        m_transformGravity = m_params.getGravity();

    if( m_params.m_useActorTranslation )
    {
        Matrix44 currentTransformation = out;
        Matrix44 translation;
        //get offset and scale it
        const Vec2d offset = m_params.getActorTranslationOffset() * m_LocalScale;
        //compute translation matrix
        translation.setIdentity34();
        translation.setT(loadXYZW(m_params.m_pos.x() + offset.x(), m_params.m_pos.y() + offset.y(), m_params.m_pos.z(),1.f));
        //apply translation matrix to transform matrix
        out.mul44(currentTransformation, translation);
    }

    m_transformMatrix44 = out;
}

void ITF_ParticleGenerator::transformBoundingBoxWithMatrix()
{
    AABB& aabb = m_params.getBoundingBoxWorking();
    Vec2d minAABB = aabb.getMin();
    Vec2d maxAABB = aabb.getMax();
    if (m_params.m_useActorTranslation)
    {
        minAABB -= m_params.m_pos.truncateTo2D();
        maxAABB -= m_params.m_pos.truncateTo2D();
    }

    f32 depth = m_params.m_pos.z();
    Float4 vt1 = m_transformMatrix44.transformPoint(loadXYZW(minAABB.x(), minAABB.y(), depth, 1.f));
    Float4 vt2 = m_transformMatrix44.transformPoint(loadXYZW(maxAABB.x(), maxAABB.y(), depth, 1.f));
    Float4 vt3 = m_transformMatrix44.transformPoint(loadXYZW(minAABB.x(), maxAABB.y(), depth, 1.f));
    Float4 vt4 = m_transformMatrix44.transformPoint(loadXYZW(maxAABB.x(), minAABB.y(), depth, 1.f));
    Float4 vmin = min(min(vt1, vt2), min(vt3,vt4));
    Float4 vmax = max(max(vt1, vt2), max(vt3,vt4));

    m_params.getBoundingBoxWorking().setMin((Vec2d&)vmin);
    m_params.getBoundingBoxWorking().setMax((Vec2d&)vmax);
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::initNewParticleFollow(ITF_Particle* _par)
{
    _par->m_flag = 0;

    if (getTemplateParameters()->getOrientDir() == 1)
        _par->m_flag |= PAR_F_ORIENTDIR;

    Vec3d randomVec = getRandomVector();
    _par->m_vel = randomVec * m_params.m_velocityVar;
#ifdef PAR_USE_TIMEVELTARGET
    randomVec = getRandomVector();
    _par->m_velTarget = randomVec * m_params.m_velocityVar;
#endif // PAR_USE_TIMEVELTARGET
    f32 angularSpeedDelta = f32_Abs(m_params.m_angularSpeedDelta.Radians());
    _par->m_angularSpeed.SetRadians(randomLCG.getRandF(m_params.m_angularSpeed.Radians() - angularSpeedDelta, m_params.m_angularSpeed.Radians() + angularSpeedDelta));
    _par->m_angle.SetRadians(0.0f);

    SetUVFromUVmode(_par);

    //_par->m_initTime = m_currentTime;
    
    _par->m_pos.z() = m_params.m_pos.z();
    /// TopLeft, TopRight, BottomLeft, BottomRight;
    Vec2d V[4];
    CAMERA->getScreenCornersAtZ(_par->m_pos.z(), V);

    Vec2d ptMin = m_params.m_pos.truncateTo2D() + m_params.m_genBox.getMin() * getGenBoxLocalScale();
    Vec2d ptMax = m_params.m_pos.truncateTo2D() + m_params.m_genBox.getMax() * getGenBoxLocalScale();

    _par->m_pos.x() = randomLCG.getRandF(ptMin.x(), ptMax.x());
    _par->m_pos.y() = randomLCG.getRandF(ptMin.y(), ptMax.y());
#ifdef PAR_USE_TIMEVELTARGET
    _par->m_timeTarget = m_params.m_timeTarget;
    _par->m_curtimeTarget = _par->m_timeTarget;
#endif // PAR_USE_TIMEVELTARGET
    _par->m_curPhase = 0;

    _par->m_indexUV = m_template->getStartAnimIndex();
    if (m_template->getStartAnimIndex() >= 0 && !m_template->getUseUVRandom())
    {
        _par->m_startAnimIndex = m_template->getStartAnimIndex();
        _par->m_endAnimIndex = m_template->getEndAnimIndex();
        _par->m_flag |= PAR_F_USEANIM;
        m_useAnim = btrue;
    }
    else
    {
        _par->m_flag &= ~PAR_F_USEANIM;
        m_useAnim = bfalse;
    }
    _par->m_animId = getAnimId();
    if (m_template->getIsParticlePhaseLoop())
        _par->m_flag |= PAR_F_LOOP;
    else
        _par->m_flag &= ~PAR_F_LOOP;

    if ( m_template->getUseUVRandom() )
    {
        _par->m_indexUV = randomLCG.getRandI(m_template->getStartAnimIndex(), m_template->getEndAnimIndex());
    }

    m_params.initFromPhase(_par, m_currentTime, this);
    _par->m_birthTime = m_currentTime;
    if(getTemplateParameters()->m_parLifeTime.isSet())
    {
        Vec2d vlifeTime;
        getTemplateParameters()->m_parLifeTime.getValue(vlifeTime, m_currentTime);
        _par->m_dieTime = _par->m_birthTime + randomLCG.getRandF( vlifeTime.x(), vlifeTime.y() );
    }
    else
        _par->m_dieTime = _par->m_birthTime + m_maxParLiveTimeFromPhases;
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::initNewParticleComplex(ITF_Particle* _par)
{
    Vec3d dir(0.f, 1.f, 0.f);
    _par->m_flag = 0;
    _par->m_seed = (u32)randomLCG.RandI() + (randomLCG.getRandI(0,1) == 1 ? 0x80000000 : 0); // RandI bound to i32 positive values. 
    _par->m_bezierDist = 0.f;
    _par->m_velAcc = Vec3d::Zero;
    _par->m_bounceCount = 0;
    u32 minRebounds = getTemplateParameters()->getYMinMinRebounds();
    u32 maxRebounds = getTemplateParameters()->getYMinMaxRebounds();
    if(minRebounds > maxRebounds)
        _par->m_bounceMax = minRebounds;
    else
        _par->m_bounceMax = randomLCG.getRandI(getTemplateParameters()->getYMinMinRebounds(), getTemplateParameters()->getYMinMaxRebounds());

    Vec3d GenPos = m_params.m_pos;
    if (m_params.getIs2D() || m_params.m_useActorTranslation )
    {
        GenPos = Vec3d::Zero;
    }

    _par->m_posYInit = m_params.m_pos.y();

    if (getTemplateParameters()->getOrientDir() == 1)
        _par->m_flag |= PAR_F_ORIENTDIR;
    else
        if (getTemplateParameters()->getOrientDir() == 3)
            _par->m_flag |= PAR_F_ORIENTDIRGEN;

    if(m_params.isFlip())
        _par->m_flag |= PAR_F_FLIPPEDATSPAWN;

    float parVelNorm = 1.f;
    float parVelNormDelta = 0.f;
    if(getTemplateParameters()->m_parEmitVelocity.isSet())
    {
        Vec3d v;
        getTemplateParameters()->m_parEmitVelocity.getValue(v, getCurrentTime());
        parVelNorm = v.z();
        parVelNormDelta = randomLCG.getRandF( v.x(), v.y() ) - parVelNorm;
    }
    else
    {
        parVelNorm = m_params.m_velNorm;
        parVelNormDelta = randomLCG.getRandF(0.f, m_params.m_velocityVar);
    }

    float parVelocityNorm = parVelNorm + parVelNormDelta;

    switch (m_params.m_genGenType)
    {
    case PARGEN_GEN_CIRCLE:
    {
        if (m_params.m_nSplit > 1 && m_params.m_nSplit < 90)
        {
            if (!m_spliterAngle)
            {
                m_spliterAngle = newAlloc(mId_Particle, Angle[m_params.m_nSplit]);
                f32 max = m_params.getGenAngMin().ToRadians();
                f32 min = m_params.getGenAngMax().ToRadians();
                if (min < 0)
                    min = MTH_2PI - f32_Abs(min);

                u32 div = m_params.m_nSplit - 1;

                f32 dist = (max - min);

                if (f32_Abs(dist) > MTH_2PI)
                    dist = MTH_2PI;
                if (f32_Abs(dist) == MTH_2PI || dist == 0.f)
                    div++;

                f32 inc = dist / div;

                for (u32 i = 0; i < m_params.m_nSplit; i++)
                    m_spliterAngle[i].SetRadians(min + i*inc);
            }

            //generate
            dir = getRandomVector2d(m_spliterAngle[m_SplitCurPos].ToRadians() - m_params.m_splitDelta.ToRadians(), m_spliterAngle[m_SplitCurPos].ToRadians() + m_params.m_splitDelta.ToRadians());

            m_SplitCurPos++;
            if (m_SplitCurPos >= m_params.m_nSplit)
                m_SplitCurPos = 0;
        }
        else 
        {
            f32 a1 = m_params.getGenAngMin().ToRadians();
            f32 a2 = m_params.getGenAngMax().ToRadians();
            /*if(a2<a1) // follow a1..a2 arc // not for RO2
            {
                f32 tmp = a2;
                a2 = a1;
                a1 = tmp;
            }*/
            dir = getRandomVector2d(a1,a2);
        }
        
        const f32 radius = randomLCG.getRandF(getTemplateParameters()->getInnerCircleRadius(), getTemplateParameters()->getCircleRadius());
        const Vec3d& scale = getTemplateParameters()->getScaleShape();
        dir.x() *= scale.x();
        dir.y() *= scale.y();
        // NO SCALE Z for circle --- > dir.z() *= scale.z();

        const Vec3d& vrot = getTemplateParameters()->getRotateShape();

        if (!vrot.IsEqual(Vec3d::Zero,MTH_EPSILON))
        {
            //reorient dir
            Matrix44 rotX;
            Matrix44 rotY;
            Matrix44 mat;
            rotX.setRotationX(vrot.x());
            rotY.setRotationY(vrot.y());
            mat.mul44(rotX,rotY);
            mat.transformVector(dir, dir);
        }
        _par->m_pos = GenPos + dir * radius;

        if (m_params.m_randomizeDirection)
        {
            _par->m_vel = dir * parVelocityNorm;
        }
        else
        {
            _par->m_vel = m_params.m_vel * parVelocityNorm;
        }
        break;
    }

    case PARGEN_GEN_RECTANGLE:
    {
        dir = getRandomVector2d(m_params.getGenAngMin().ToRadians(), m_params.getGenAngMax().ToRadians());
        Vec2d localPos;
        localPos.x() = randomLCG.getRandF(m_params.m_genBox.getMin().x() * getGenBoxLocalScale().x(), m_params.m_genBox.getMax().x() * getGenBoxLocalScale().x());
        localPos.y() = randomLCG.getRandF(m_params.m_genBox.getMin().y() * getGenBoxLocalScale().y(), m_params.m_genBox.getMax().y() * getGenBoxLocalScale().y());
        _par->m_pos = GenPos + localPos.to3d();

        if (m_params.m_randomizeDirection)
        {
            _par->m_vel = _par->m_pos - GenPos;
            _par->m_vel = _par->m_vel.normalize() * parVelocityNorm; 
        }
        else
        {
            _par->m_vel = m_params.m_vel * parVelocityNorm;
        }
        break;
    }

    case PARGEN_GEN_SPHERE:
    {
        if (m_params.m_randomizeDirection)
        {
            dir = getRandomVector();
            _par->m_vel = dir * parVelocityNorm; 
        }
        else
        {
            _par->m_vel = m_params.m_vel * parVelocityNorm;
        }
        _par->m_pos = GenPos;
        break;
     }

    case PARGEN_GEN_HEMISPHERE:
    {
        dir = getRandomVectorInter(-MTH_PI, MTH_PI, 0.f, 1.f);
        dir.normalize();
        f32 radius = randomLCG.getRandF(getTemplateParameters()->getInnerCircleRadius(), getTemplateParameters()->getCircleRadius());

        _par->m_pos = GenPos + dir * radius;

        if (m_params.m_randomizeDirection)
        {
            _par->m_vel = dir * parVelocityNorm; 
        }
        else
        {
            _par->m_vel = m_params.m_vel * parVelocityNorm;
        }
        break;
    }

    case PARGEN_GEN_BEZIER:
    {
        if (m_BezierBranch)
        {
            const BranchCurve& curve = m_BezierBranch->getCurve();
            Vec3d beziertTan(0,0,0);
            Vec3d dir3d(1,0,0);
            Vec3d vel = m_params.m_vel;
            _par->m_bezierDist = randomLCG.getRandF(m_params.m_genBezierStart, m_params.m_genBezierEnd);
            _par->m_bezierDist = Min(_par->m_bezierDist, 0.99f);
            _par->m_bezierDist = Max(0.f, _par->m_bezierDist);
            _par->m_bezierDist *= m_BezierBranch->getCurrentLength();

            curve.getPosTanAtDistance(_par->m_bezierDist, _par->m_pos, beziertTan);

            vel.normalize();
            
            Matrix44 mat;
            Matrix44 matRotZ;
            Vec3d mI, mJ, mK;
            mI = beziertTan.normalize();
            Vec3d::ZAxis.crossProduct(mI, mJ);
            mI.crossProduct(mJ, mK);
            mat.setFromFloat(mI.x(),mI.y(),mI.z(),0,
                             mJ.x(),mJ.y(),mJ.z(),0,
                             mK.x(),mK.y(),mK.z(),0,
                             0,0,0,1);
            matRotZ.setRotationZ(MTH_PIBY2);
            mat.mul33(mat, matRotZ);
            mat.transformVector(vel, vel);

            if (m_params.m_randomizeDirection)
                dir = getRandomVector();              
            else
                dir = vel;
            _par->m_vel = vel*parVelNorm;
            _par->m_vel += dir * parVelNormDelta;

            if(m_params.m_genSize)
            {
                f32 tgenw = randomLCG.getRandF(-m_params.m_genSize, m_params.m_genSize);
                vel = _par->m_vel;
                vel.normalize();

                switch(m_params.m_genSide)
                {
                    case 0:
                        if(tgenw < 0.f)
                            _par->m_vel = -_par->m_vel;
                        break;

                    case 2:
                        _par->m_vel = -_par->m_vel;
                        break;
                };

                Vec3d::ScaleAdd(&_par->m_pos, &vel, tgenw, &_par->m_pos);
            }
        }

        break;
    }

    case PARGEN_GEN_POINTS:
    default:
        if (m_params.m_randomizeDirection)
            dir = getRandomVector();
        _par->m_vel = m_params.m_vel*parVelNorm;
        _par->m_vel += dir * parVelNormDelta;
        _par->m_pos = GenPos;
        break;
    }
    
    /// Orient to dir.
    if (_par->m_flag & PAR_F_ORIENTDIR)
    {
        _par->m_angle.SetRadians(getParameters()->getAngle().ToRadians() + atan2f(dir.y(), dir.x()));
        _par->m_angleInit = getParameters()->getAngle();
    }
    else
    {
        f32 angleDelta = f32_Abs(m_params.m_angleDelta.Radians());
        _par->m_angle.SetRadians(randomLCG.getRandF(m_params.m_initAngle.Radians() - angleDelta, m_params.m_initAngle.Radians() + angleDelta));
        _par->m_angleInit = _par->m_angle;

        f32 angularSpeedDelta = f32_Abs(m_params.m_angularSpeedDelta.Radians());
        _par->m_angularSpeed.SetRadians(randomLCG.getRandF(m_params.m_angularSpeed.Radians() - angularSpeedDelta, m_params.m_angularSpeed.Radians() + angularSpeedDelta));
    }

    SetUVFromUVmode(_par);

    _par->m_initTime  = m_currentTime;
    _par->m_birthTime = m_currentTime;

    if(getTemplateParameters()->m_parLifeTime.isSet())
    {
        Vec2d vlifeTime;
        getTemplateParameters()->m_parLifeTime.getValue(vlifeTime, m_currentTime);
        _par->m_dieTime = _par->m_birthTime + randomLCG.getRandF( vlifeTime.x(), vlifeTime.y() );
    }
    else
        _par->m_dieTime = _par->m_birthTime + m_maxParLiveTimeFromPhases;

    _par->m_curPhase = 0;
    _par->m_indexUV = m_template->getStartAnimIndex();

    /// Anim.
    if (m_template->getStartAnimIndex() >= 0 && !m_template->getUseUVRandom())
    {
        _par->m_startAnimIndex = m_template->getStartAnimIndex();
        _par->m_endAnimIndex = m_template->getEndAnimIndex();
        _par->m_flag |= PAR_F_USEANIM;
        m_useAnim = btrue;
    }
    else
    {
        if(getTemplateParameters()->m_parAnim.isSet() || getTemplateParameters()->m_parEmitAnim.isSet())
        {
            _par->m_flag |= PAR_F_USEANIM;
            m_useAnim = btrue;
        }

        if(getTemplateParameters()->m_parEmitAnim.isSet())
        {
            Vec3d v;
            getTemplateParameters()->m_parEmitAnim.getValue(v, getCurrentTime());
            _par->m_indexUV = (i32)randomLCG.getRandF( v.x(), v.y() );
            if((u32)_par->m_indexUV >= m_atlasMaxIndex)
                _par->m_indexUV = m_atlasMaxIndex-1;
            _par->m_startAnimIndex = _par->m_indexUV;
            _par->m_endAnimIndex = _par->m_indexUV;
        }
    }

    _par->m_animId = getAnimId();

    if (m_template->getIsParticlePhaseLoop() || getTemplateParameters()->m_parLifeTime.isSet())
        _par->m_flag |= PAR_F_LOOP;
    else
        _par->m_flag &= ~PAR_F_LOOP;

    int ranColor = 0;
    if(getTemplateParameters()->m_parRGB1.isSet())
        ranColor++;
    if(getTemplateParameters()->m_parRGB2.isSet())
        ranColor++;
    if(getTemplateParameters()->m_parRGB3.isSet())
        ranColor++;
    _par->m_colCurveID = randomLCG.getRandI(0,ranColor);

    _par->m_initColor = 0xFFFFFFFF;
    if(getTemplateParameters()->m_parEmitColorFactorInit.isSet())
    {
        Vec3d colorFactorInit;

        getTemplateParameters()->m_parEmitColorFactorInit.getValue(colorFactorInit, getCurrentTime());
        _par->m_initColor.m_rgba.m_r = ITFCOLOR_F32_TO_U8(colorFactorInit.x());
        _par->m_initColor.m_rgba.m_g = ITFCOLOR_F32_TO_U8(colorFactorInit.y());
        _par->m_initColor.m_rgba.m_b = ITFCOLOR_F32_TO_U8(colorFactorInit.z());
    }

    if(getTemplateParameters()->m_parEmitAlphaInit.isSet())
    {
        Vec3d alphaFactorInitVect;
        getTemplateParameters()->m_parEmitAlphaInit.getValue(alphaFactorInitVect, getCurrentTime());
        f32 alphaFactorInit = randomLCG.getRandF(alphaFactorInitVect.x(), alphaFactorInitVect.y());
        _par->m_initColor.m_rgba.m_a = ITFCOLOR_F32_TO_U8(alphaFactorInit);
    }

    _par->m_initSize = Vec2d::One;
    if(getTemplateParameters()->m_parEmitSizeXYInit.isSet())
    {
        getTemplateParameters()->m_parEmitSizeXYInit.getValue(_par->m_initSize, getCurrentTime());
    }

    if ( m_template->getUseUVRandom() )
    {
        //_par->m_indexUV = (i32)random.GetU32(m_startAnimIndex*100, (m_endAnimIndex+1)*100) / 100;
        _par->m_indexUV = (long)randomLCG.getRandI((long)m_template->getStartAnimIndex(), (long)m_template->getEndAnimIndex());
    }

    if(m_params.m_UVmodeFlag & UVF_FlipX)
        _par->m_flag |= PAR_F_UVINVERT_X;
    if(m_params.m_UVmodeFlag & UVF_FlipY)
        _par->m_flag |= PAR_F_UVINVERT_Y;

    if(m_params.m_UVmodeFlag & UVF_FlipXRandom)
    {
        if ((long)randomLCG.getRandI(0, 1) == 0)
            _par->m_flag = (_par->m_flag | PAR_F_UVINVERT_X) & ~(_par->m_flag & PAR_F_UVINVERT_X);
    }
    if(m_params.m_UVmodeFlag & UVF_FlipYRandom)
    {
        if ((long)randomLCG.getRandI(0, 1) == 0)
            _par->m_flag |= (_par->m_flag | PAR_F_UVINVERT_Y) & ~(_par->m_flag & PAR_F_UVINVERT_Y);
    }

    m_params.initFromPhase(_par, m_currentTime, this);
}

///----------------------------------------------------------------------------//

void ITF_ParticleGenerator::spawnOneParticle()
{
    addOneNewComplexParticle();
}

void ITF_ParticleGenerator::SetUVFromUVmode(ITF_Particle* _par)
{
    UVMODEFLAG uvModeFlag = m_params.getUVmodeFlag();
    if(uvModeFlag & UVF_FlipXtoDirX)
    {
        if(_par->m_velReal.x() < 0.f)
            _par->m_flag |= PAR_F_UVINVERT_X;
    }
    if(uvModeFlag & UVF_FlipYtoDirY)
    {
        if(_par->m_velReal.y() < 0.f)
            _par->m_flag |= PAR_F_UVINVERT_Y;
    }
}

void ITF_ParticleGenerator::setFlip( bbool _flip )
{
    if (_flip == m_params.m_flipped)
        return;

    const ParticleGeneratorParameters* const templateParams = getTemplateParameters();

    m_params.m_flipped = _flip;

    AABB    tmpAABB;
        
    if (templateParams->canFlipAccel())
    {
        m_params.m_acc.x() *= -1.f;
    }

    if (templateParams->canFlipAngularSpeed())
        m_params.setAngularSpeed(-m_params.getAngularSpeed());

    if (templateParams->canFlipInitAngle())
        m_params.m_initAngle = Angle(btrue, 180.f - m_params.m_initAngle.ToDegrees());

    if (templateParams->canFlipPivot())
    {
        m_params.m_pivot.x() *= -1.f;
    }

    if (templateParams->getCanClipPosOffset())
    {
        m_params.m_posOffset.x() *= -1.f;
    }

    m_params.getGenBox().FlipHorizontaly(bfalse);
    
    if (templateParams->canFlipBoundingBox())
    {
        m_params.getBoundingBox().FlipHorizontaly(bfalse);
    }

    Angle maxAng = m_params.getGenAngMax();
    Angle minAng = m_params.getGenAngMin();
 
    if (templateParams->canFlipAngleMin())
        m_params.setGenAngMin(Angle(btrue, 180.f - minAng.ToDegrees()));

    if (templateParams->canFlipAngleMax())
        m_params.setGenAngMax(Angle(btrue, 180.f - maxAng.ToDegrees()));

    UVMODEFLAG uvModeFlag = m_params.getUVmodeFlag();
    uvModeFlag = (UVMODEFLAG) ((uvModeFlag|UVF_FlipX) & ~(uvModeFlag & UVF_FlipX));
    m_params.setUVmodeFlag(uvModeFlag);
}

///----------------------------------------------------------------------------//

int particleSortNewerFirst(const void* p1, const void* p2)
{
    ITF_Particle *particle1 = (ITF_Particle *)p1;
    ITF_Particle *particle2 = (ITF_Particle *)p2;

    if(!particle1->m_bAlive)
        return 1;
    if(!particle2->m_bAlive)
        return -1;
    if(particle1->m_birthTime < particle2->m_birthTime)
        return 1;
    else if(particle1->m_birthTime > particle2->m_birthTime)
        return -1;
    else 
        return 0;
}

int particleSortOlderFirst(const void* p1, const void* p2)
{
    ITF_Particle *particle1 = (ITF_Particle *)p1;
    ITF_Particle *particle2 = (ITF_Particle *)p2;

    if(!particle1->m_bAlive)
        return 1;
    if(!particle2->m_bAlive)
        return -1;
    if(particle1->m_birthTime < particle2->m_birthTime)
        return -1;
    else if(particle1->m_birthTime > particle2->m_birthTime)
        return 1;
    else 
        return 0;
}

void ITF_ParticleGenerator::updateZSort()
{
    switch(getTemplate()->getZSortMode())
    {
    case PARGEN_ZSORT_NEWER_FIRST : 
        qsort(m_pParticleArrayIndex, m_maxParticlesNeed, sizeof(ITF_Particle), particleSortNewerFirst);
        updateFreeIndexListAfterSort();
        break;
    case PARGEN_ZSORT_OLDER_FIRST : 
        qsort(m_pParticleArrayIndex, m_maxParticlesNeed, sizeof(ITF_Particle), particleSortOlderFirst);
        updateFreeIndexListAfterSort();
        break;
    }
}

Vec3d ITF_ParticleGenerator::getParticlesSpawnPoint()
{
    Vec3d transOut;
    if(m_params.m_useActorTranslation)
        transOut = Vec3d(m_transformMatrix44.T());
    else
        transOut = m_params.m_pos;
    return transOut;
}

void ITF_ParticleGenerator::updateFreeIndexListAfterSort()
{
    // All free indexes are in the bottom of the particle array list. 
    m_pFreeParticleIndex.clear();
    ITF_Particle *curParticleArrayIndex = m_pParticleArrayIndex + m_maxParticlesNeed;
    while(curParticleArrayIndex != m_pParticleArrayIndex)
    {
        curParticleArrayIndex--;
        if(curParticleArrayIndex->m_bAlive)
            break;

        u32 index = (u32)((uPtr)curParticleArrayIndex-(uPtr)m_pParticleArrayIndex)/sizeof(ITF_Particle);
        m_pFreeParticleIndex.push_back(index);
    }
}

///----------------------------------------------------------------------------//

BEGIN_SERIALIZATION(ITF_ParticleGenerator_Template)
    SERIALIZE_MEMBER("computeAABB",m_computeAABB);
    SERIALIZE_MEMBER("useAnim",m_useAnim);
    SERIALIZE_MEMBER("loop", m_loop);
    SERIALIZE_MEMBER("amvPath", m_amvPath)
    SERIALIZE_MEMBER("useUVRandom",m_useUVRandom);
    SERIALIZE_MEMBER("animstart",m_startAnimIndex);
    SERIALIZE_MEMBER("animend",m_endAnimIndex);
    SERIALIZE_MEMBER("animname",m_animName);
    SERIALIZE_MEMBER("AnimUVfreq",m_AnimUVfreq);
    SERIALIZE_OBJECT("params",m_params);
    SERIALIZE_ENUM_BEGIN("zSortMode",m_zSortMode);
        SERIALIZE_ENUM_VAR(PARGEN_ZSORT_NONE);
        SERIALIZE_ENUM_VAR(PARGEN_ZSORT_NEWER_FIRST);
        SERIALIZE_ENUM_VAR(PARGEN_ZSORT_OLDER_FIRST);
    SERIALIZE_ENUM_END();

END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(ITF_ParticleGenerator_Template)

ITF_ParticleGenerator_Template::ITF_ParticleGenerator_Template()
{
    m_computeAABB       = 0;
    m_useAnim           = 0;
    m_loop              = 0;
    m_startAnimIndex    = -1;
    m_endAnimIndex      = -1;
    m_AnimUVfreq        = 1.f;
    m_useUVRandom       = 0;
    m_zSortMode         = PARGEN_ZSORT_NONE;

    m_amvPath = Path::EmptyPath;
    m_amvResourceID = ResourceID::Invalid;

#ifdef ITF_SUPPORT_EDITOR
    m_drawDebugShape = bfalse;
#endif
}

ITF_ParticleGenerator_Template::~ITF_ParticleGenerator_Template()
{
#ifdef ITF_SUPPORT_FXEDITOR
    ITF_ParticleGenerator::RemoveRef(this);
#endif // ITF_SUPPORT_FXEDITOR
}

void ITF_ParticleGenerator_Template::onLoaded( ResourceContainer * _parentContainer )
{
    if(m_amvPath != Path::EmptyPath)
    { // init AnimMeshVertex
        ITF_ASSERT(_parentContainer);
        m_amvResourceID = _parentContainer->addResourceFromFile(Resource::ResourceType_AnimMeshVertex, m_amvPath);
    }
}

#ifdef ITF_SUPPORT_EDITOR
void ITF_ParticleGenerator_Template::drawDebugShape( const Vec3d& _shapeOrigin, const Vec2d& _actorScale ) const
{
    Vec3d pos = m_params.getPosition() + m_params.getPositionOffset();
    float scaleNorm = 1.0f;
    if(m_params.getUseMatrix())
    {
        pos *= Vec3d(_actorScale.x(), _actorScale.y(), 1.0f);
    }

    Vec2d pos2D = (pos + _shapeOrigin).truncateTo2D();

    switch(m_params.getGeneratorGenType())
    {
        case PARGEN_GEN_RECTANGLE:
            {
                AABB genBox = m_params.getGenBox();
                if(m_params.getUseMatrix())
                    genBox.Scale(_actorScale);
                DebugDraw::AABB(pos2D, m_params.getPosition().z(), /*getLocalRotationZ()*/ 0.f, genBox, Color::blue());
                break;
            }
        case PARGEN_GEN_CIRCLE:
            {
                if(m_params.getUseMatrix())
                    scaleNorm = _actorScale.norm();
                DebugDraw::angleRange(pos2D, m_params.getPosition().z(), m_params.getGenAngMin().ToRadians(), m_params.getGenAngMax().ToRadians(), m_params.getCircleRadius() * scaleNorm);
                DebugDraw::circle(pos2D, m_params.getPosition().z(), m_params.getCircleRadius() * scaleNorm, Color::green());
                DebugDraw::circle(pos2D, m_params.getPosition().z(), m_params.getInnerCircleRadius() * scaleNorm, Color::blue());
                break;
            }
    }
}
#endif // ITF_SUPPORT_EDITOR


///----------------------------------------------------------------------------//
/// Particles Z list.
///----------------------------------------------------------------------------//

void PAR_Zlist::AddParticle(ITF_ParticleOrder* m_part)
{
    ITF_ParticleOrder* pNode;
    pNode = AddNode();

    if (!pNode) return;

    *pNode = *m_part;
}

///----------------------------------------------------------------------------//

ITF_ParticleOrder* PAR_Zlist::AddNode()
{
    ITF_ParticleOrder* pNewNode = &m_BufferNode[m_NumberEntry];
    m_reorderingTable[m_NumberEntry] = pNewNode;

    m_NumberEntry++;

    return pNewNode;
}

///----------------------------------------------------------------------------//

void PAR_Zlist::reset()
{
    m_NumberEntry = 0;
}

///----------------------------------------------------------------------------//

void PAR_Zlist::init()
{
}

///----------------------------------------------------------------------------//

#ifdef ITF_PS3
#define ITF_USE_STD_SORT
#endif

#ifdef ITF_USE_STD_SORT
struct ParticleOrderPtrZLess
{
    bool operator()(ITF_ParticleOrder * _r, ITF_ParticleOrder * _l)
    {
        return _r->m_zsort < _l->m_zsort;
    }
};
#endif

int parsorting(const void* n1, const void* n2)
{
    ITF_ParticleOrder* a = *(ITF_ParticleOrder**) n1;
    ITF_ParticleOrder* b = *(ITF_ParticleOrder**) n2;
       
    if( a->m_zsort < b->m_zsort ) return -1;
    else if( a->m_zsort == b->m_zsort ) return 0;
    else return 1;
}

///----------------------------------------------------------------------------//

void PAR_Zlist::sort()
{
#ifdef ITF_USE_STD_SORT
    ITF::sort(m_reorderingTable, m_reorderingTable+m_NumberEntry, ParticleOrderPtrZLess());
#else
    qsort((void*)m_reorderingTable, m_NumberEntry, sizeof(ITF_ParticleOrder*), parsorting);
#endif
}

///----------------------------------------------------------------------------//

void PAR_Zlist::setSize(u32 _number)
{
    reset();
    m_BufferNode.resize(_number);

    if (m_reorderingTable)
        delete[] m_reorderingTable;

    m_reorderingTable = newAlloc(mId_Particle, ITF_ParticleOrder*[_number]);
    for (u32 i = 0; i < _number;i++)
    {
        m_reorderingTable[i] = &m_BufferNode[i];
    }
}

#ifdef ITF_SUPPORT_FXEDITOR
ITF_VECTOR<ITF_ParticleGenerator::ObjRef> ITF_ParticleGenerator::m_vReferences;
ITF_THREAD_CRITICAL_SECTION ITF_ParticleGenerator::m_csReferences;
bbool ITF_ParticleGenerator::m_csReferencesInitDone = bfalse;
void ITF_ParticleGenerator::EnterCSref()
{
    if(!m_csReferencesInitDone)
    {
        Synchronize::createCriticalSection(&m_csReferences);
        m_csReferencesInitDone = btrue;
    }

    Synchronize::enterCriticalSection(&m_csReferences);
}

void ITF_ParticleGenerator::LeaveCSref()
{
    Synchronize::leaveCriticalSection(&m_csReferences);
}

ITF_ParticleGenerator::ObjRef *ITF_ParticleGenerator::findRef(class ITF_ParticleGenerator_Template* _template)
{    
    for(size_t i = 0; i < m_vReferences.size(); ++i)
    {
        if(m_vReferences[i].m_ParticleGenerator_Template == _template)
            return &m_vReferences[i];
    }
    return NULL;
}

ITF_ParticleGenerator::ObjRef *ITF_ParticleGenerator::findRef(const Path& _path, const StringID& _name)
{
    for(size_t i = 0; i < m_vReferences.size(); ++i)
    {
        if(m_vReferences[i].m_ParticleGenerator_Template->getFile() == _path)
            if(m_vReferences[i].m_Name == _name)
                return &m_vReferences[i];
    }
    return NULL;
}

void ITF_ParticleGenerator::AddRef(class ITF_ParticleGenerator_Template* _template, const StringID& _name )
{
    EnterCSref();
    ObjRef *r = addRef(_template);
    r->m_Name = _name;
    LeaveCSref();
}

ITF_ParticleGenerator::ObjRef * ITF_ParticleGenerator::addRef(class ITF_ParticleGenerator_Template* _template)
{
    ObjRef *r = findRef(_template);
    if(r)
        return r;
    EnterCSref();
    m_vReferences.push_back(ObjRef(_template));
    r = &m_vReferences.back();
    LeaveCSref();
    return r;
}

void ITF_ParticleGenerator::RemoveRef(class ITF_ParticleGenerator_Template* _template)
{
    EnterCSref();
    ITF_VECTOR<ObjRef>::iterator iter = m_vReferences.begin();
	while (iter!=m_vReferences.end())
	{
        if((*iter).m_ParticleGenerator_Template == _template)
            iter = m_vReferences.erase(iter);
        else
            ++iter;
	}
    LeaveCSref();
}

void ITF_ParticleGenerator::AddRef(class ITF_ParticleGenerator* _gen)
{
    ObjRef *r = addRef((ITF_ParticleGenerator_Template*)_gen->getTemplate());
    for(size_t i = 0; i < r->m_vParticleGenerator.size(); ++i)
    {
        if(r->m_vParticleGenerator[i] == _gen)
            return;
        
    }
    EnterCSref();
    r->m_vParticleGenerator.push_back(_gen);
    LeaveCSref();
}

void ITF_ParticleGenerator::RemoveRef(class ITF_ParticleGenerator* _gen)
{
    EnterCSref();
    for(size_t i = 0; i < m_vReferences.size(); ++i)
    {
        ITF_VECTOR<ITF_ParticleGenerator*>::iterator iter = m_vReferences[i].m_vParticleGenerator.begin();
	    while (iter!=m_vReferences[i].m_vParticleGenerator.end())
	    {
            if((*iter) == _gen)
                iter = m_vReferences[i].m_vParticleGenerator.erase(iter);
            else
                ++iter;
	    }
    }
    LeaveCSref();
}
#endif
} // namespace ITF


