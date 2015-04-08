#include "precompiled_engine.h"

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_JOBUPDATEPARTICLE_H_
#include "engine/display/particle/JobUpdateParticle.h"
#endif //_ITF_JOBUPDATEPARTICLE_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef _ITF_BEZIERCURVE4_H_
#include "core/math/BezierCurve4.h"
#endif // _ITF_BEZIERCURVE4_H_

#ifndef _ITF_BEZIERBRANCH_H_
#include "engine/BezierTree/BezierBranch.h"
#endif //_ITF_BEZIERBRANCH_H_

namespace ITF
{

    u32 JobUpdateParticle::_jobTag_UpdateParticle    = U32_INVALID;

    void JobUpdateParticle::execute(bbool _immediate)
    {
        PRF_M_SCOPE(JobUpdateParticle);

        mainJobUpdateParticle(&m_context,_immediate);
    }

    //------------------------------------------------------------------------------------------
    Seeder randomParticle(666);

    Vec3d getRandomVectorParticle()
    {
        Vec3d v;
        v.z() = randomParticle.GetFloat( -1.0f, 1.0f );

        f32 r = (float)f32_Sqrt(1 - v.z() * v.z());

        f32 t = randomParticle.GetFloat( -MTH_PI, MTH_PI );

        f32_CosSin(t, &v.x(), &v.y());
        v.x() *= r;
        v.y() *= r;

        return v;
    }

    //------------------------------------------------------------------------------------------

    void updateBlend(ITF_Particle* _particle, f32 _elapsedTime)
    {
        f32 f = 0.0f;
        if (_particle->m_phaselifeTime!=0.0f)
        {
             f = _elapsedTime / _particle->m_phaselifeTime;
        }

        ITFCOLOR_BLEND(&_particle->m_curColor.m_color, &_particle->m_initColor.m_color, &_particle->m_dstColor.m_color, f);
        _particle->m_curSize.Lerp(_particle->m_initSize, _particle->m_dstSize, f);
    }

    //------------------------------------------------------------------------------------------

    void updateBlendings(ITF_Particle* _particle, ITF_ParticleGenerator* _pParticleGenerator)
    {
        const f32 currentTime   = _pParticleGenerator->getCurrentTime();
        const ParticleGeneratorParameters* templateParams = _pParticleGenerator->getTemplateParameters();
        const ParticleGeneratorInstanceParameters* pParams = _pParticleGenerator->getParameters();

        if (_particle->m_flag & PAR_F_BLENDPHASE)
            updateBlend(_particle, currentTime - _particle->m_initTime);

        const f32 parLifeTime = _particle->m_dieTime - _particle->m_birthTime;
        const f32 parTimeFactor = parLifeTime<=0.f?1.f:(currentTime - _particle->m_birthTime)/parLifeTime;
        f32 ranf1 = float(_particle->GetSeed2()) * float(1.0f/255.f);
        f32 ranf2 = float(_particle->GetSeed4()) * float(1.0f/255.f);
        f32 ranf3 = float(_particle->GetSeed1()) * float(1.0f/255.f);
        f32 ranf4 = float(_particle->GetSeed3()) * float(1.0f/255.f);
        if(templateParams->m_parAlpha.isSet() || templateParams->m_parRGB.isSet())
        {
            if(templateParams->m_parAlpha.isSet())
            {
                Vec2d alphav2;
                templateParams->m_parAlpha.getValue(alphav2, parTimeFactor);
                f32 alpha = f32_Lerp(alphav2.x(), alphav2.y(), ranf2);

                if(templateParams->m_parEmitAlphaInit.isSet())
                    alpha *= _particle->m_initColor.getF32Alpha();

                _particle->m_curColor.m_rgba.m_a = ITFCOLOR_F32_TO_U8(alpha);
            }
            if(templateParams->m_parRGB.isSet())
            {
                Vec3d rgb;
                if(templateParams->getContinuousColorRandom() && templateParams->m_parRGB.isSet() && templateParams->m_parRGB1.isSet())
                {
                    Vec3d rgb1, rgb2;
                    templateParams->m_parRGB.getValue(rgb1, parTimeFactor);
                    templateParams->m_parRGB1.getValue(rgb2, parTimeFactor);
                    rgb.x() = f32_Lerp(rgb1.x(), rgb2.x(), ranf1);
                    rgb.y() = f32_Lerp(rgb1.y(), rgb2.y(), ranf1);
                    rgb.z() = f32_Lerp(rgb1.z(), rgb2.z(), ranf1);
                }
                else
                {
                    const ParLifeTimeCurve *curveCol;
                    switch(_particle->m_colCurveID)
                    {
                    case 1: 
                        curveCol = &templateParams->m_parRGB1;break;
                    case 2: 
                        curveCol = &templateParams->m_parRGB2;break;
                    case 3: 
                        curveCol = &templateParams->m_parRGB3;break;
                    default:
                        curveCol = &templateParams->m_parRGB;break;
                    }
                    curveCol->getValue(rgb, parTimeFactor);
                }

                if(templateParams->m_parEmitColorFactorInit.isSet())
                {
                    rgb.x() *= _particle->m_initColor.getF32Red();
                    rgb.y() *= _particle->m_initColor.getF32Green();
                    rgb.z() *= _particle->m_initColor.getF32Red();
                }

                _particle->m_curColor.m_rgba.m_r = ITFCOLOR_F32_TO_U8(rgb.x());
                _particle->m_curColor.m_rgba.m_g = ITFCOLOR_F32_TO_U8(rgb.y());
                _particle->m_curColor.m_rgba.m_b = ITFCOLOR_F32_TO_U8(rgb.z());
            }

            //_pParticle->m_initColor = _pParticle->m_curColor.getAsU32();
            //_pParticle->m_dstColor  = _pParticle->m_curColor.getAsU32();
        }
        if(templateParams->m_parSize.isSet())
        {
            Vec2d v;
            templateParams->m_parSize.getValue(v, parTimeFactor);
            f32 sxy = f32_Lerp(v.x(), v.y(), ranf3);

            _particle->m_curSize.x() = sxy;
            if(pParams->m_uniformScale)
            {
                _particle->m_curSize.y() = sxy * pParams->m_uniformScale;
            }
            else
            {
                if(templateParams->m_parSizeY.isSet())
                {
                    templateParams->m_parSizeY.getValue(v, parTimeFactor);
                    _particle->m_curSize.y() = f32_Lerp(v.x(), v.y(), ranf4);
                }
                else
                    _particle->m_curSize.y() = sxy;
            }

            if(templateParams->m_parEmitSizeXYInit.isSet())
            {
                _particle->m_curSize *= _particle->m_initSize;
            }

            //pParticle->m_initSize = pParticle->m_curSize;
            //pParticle->m_dstSize  = pParticle->m_curSize;
        }
    }

    //------------------------------------------------------------------------------------------

    void updateModeManual( ITF_ParticleGenerator* _pParticleGenerator,f32 _dt )
    {
        ITF_Particle  *pParticle;

        /// Update our particle system timer...
        _pParticleGenerator->incCurrentTime(_dt);

        const f32 currentTime = _pParticleGenerator->getCurrentTime();
        const ParticleGeneratorParameters* templateParams = _pParticleGenerator->getTemplateParameters(); 
        const ParticleGeneratorInstanceParameters* pParams = _pParticleGenerator->getParameters(); 

        const u32 nbPhase       = pParams->getNumberPhase();

        Texture* pTexture = _pParticleGenerator->getParameters()->getMaterial().getTexture();
        if ((!pTexture) || (!pTexture->isPhysicallyLoaded()))
            return;

#ifdef ITF_SUPPORT_MULTITHREADED
        VertexPCT* pdata = (VertexPCT*)_pParticleGenerator->getCachedMemory();
        ITF_ASSERT_CRASH(pdata, "[VB] Cached memory error !!");
        Texture * texParams = pParams->getMaterial().getTexture();
        const Vec2d scaleUV((texParams && texParams->getUVAddressModeX() == GFX_TEXADRESSMODE_MIRROR) ? 2.f : 1.f,
                            (texParams && texParams->getUVAddressModeY() == GFX_TEXADRESSMODE_MIRROR) ? 2.f : 1.f);
#endif // ITF_SUPPORT_MULTITHREADED

        Vec2d sizeFactor = Vec2d::One;
        if(templateParams->m_parEmitSizeXY.isSet())
            templateParams->m_parEmitSizeXY.getValue(sizeFactor, _pParticleGenerator->getCurrentTime());

        u32 vertexCountToDisplay = 0;
        Vec3d dir;

        const u32 updatedTot = _pParticleGenerator->m_activeParticlesCount;
        u32 updatedCount = 0;
        for (u32 index = 0;(index<_pParticleGenerator->m_maxParticlesNeed)&&(updatedCount<updatedTot);++index)
        {
            pParticle = &_pParticleGenerator->m_pParticleArrayIndex[index];

            if (pParticle->m_bAlive)
            {
                /// Calculate new position
                f32 elapsedTime = currentTime - pParticle->m_initTime;

                if ( elapsedTime >= pParticle->m_phaselifeTime )
                {
                    updatedCount++;
                    pParticle->m_curPhase++;

                    if (pParticle->m_curPhase == nbPhase && (pParticle->m_flag & PAR_F_LOOP))
                    {
                        pParticle->m_curPhase = 0;
                    }
                    _pParticleGenerator->getParameters()->initToNextPhase(pParticle, currentTime, _pParticleGenerator);
                }

                {
                    /// blend.
                    updateBlendings(pParticle, _pParticleGenerator);
                    pParticle->m_curSize *= sizeFactor;

                    if (pParticle->m_flag & PAR_F_ORIENTDIR)
                    {
                        /// Orient to dir.
                        dir = pParticle->m_vel;
                        dir.normalize();
                        pParticle->m_angle.SetRadians(_pParticleGenerator->getParameters()->getAngle().ToRadians() + atan2f(dir.y(), dir.x()));
                    }

                    #ifdef ITF_SUPPORT_MULTITHREADED
                    _pParticleGenerator->fillNormal(pParticle,pTexture,pdata,scaleUV);
                    pdata += 4;
                    #endif // ITF_SUPPORT_MULTITHREADED
                    vertexCountToDisplay ++;
                }

                if (pParticle->m_flag & PAR_F_USEANIM)
                    _pParticleGenerator->updateParticleAnim(pParticle, elapsedTime);

            }
        }

        _pParticleGenerator->setParticlesCountVtxToDisplay( vertexCountToDisplay);
    }

    //------------------------------------------------------------------------------------------

    void updateModeFollow(ITF_ParticleGenerator* _pParticleGenerator,f32 _dt, bbool _bDisableCreation)
    {
        ITF_Particle* __restrict pParticle = NULL;
#ifdef PAR_USE_TIMEVELTARGET
        Vec3d Vel;
#endif // PAR_USE_TIMEVELTARGET

        /// Update our particle system timer...
        _pParticleGenerator->incCurrentTime(_dt);

        // Start at the head of the active list
        const f32 currentTime = _pParticleGenerator->getCurrentTime();
        const ParticleGeneratorParameters* templateParams = _pParticleGenerator->getTemplateParameters();     
        const ParticleGeneratorInstanceParameters* pParams = _pParticleGenerator->getParameters();     

        const u32 nbPhase = pParams->getNumberPhase();
        Texture* pTexture = _pParticleGenerator->getParameters()->getMaterial().getTexture();
        if ((!pTexture) || (!pTexture->isPhysicallyLoaded()))
            return;

        #ifdef ITF_SUPPORT_MULTITHREADED
        VertexPCT* pdata = (VertexPCT*)_pParticleGenerator->getCachedMemory();
        ITF_ASSERT_CRASH(pdata, "[VB] Cached memory error !!");
        Texture * texParams = pParams->getMaterial().getTexture();
        const Vec2d scaleUV((texParams && texParams->getUVAddressModeX() == GFX_TEXADRESSMODE_MIRROR) ? 2.f : 1.f,
                            (texParams && texParams->getUVAddressModeY() == GFX_TEXADRESSMODE_MIRROR) ? 2.f : 1.f);
        #endif // ITF_SUPPORT_MULTITHREADED

#ifndef PAR_USE_TIMEVELTARGET
        const Vec3d acc = _pParticleGenerator->getParticlesAcceleration();
#endif // PAR_USE_TIMEVELTARGET
        u32 vertexCountToDisplay = 0;

        u32 NumParticlesToEmit = _pParticleGenerator->computeParticleToEmitCount(_dt, _bDisableCreation);
        if( NumParticlesToEmit > 0 )
        {
            for( u32 i = 0; i < NumParticlesToEmit; ++i )
            {
                if( _pParticleGenerator->m_activeParticlesCount < _pParticleGenerator->m_maxParticlesNeed)
                {
                    pParticle = _pParticleGenerator->allocateParticle();

                    _pParticleGenerator->initNewParticleFollow(pParticle);

                    ++_pParticleGenerator->m_activeParticlesCount;
                    ++_pParticleGenerator->m_totalParticlesGenerated;
                }
            }
        }

#ifdef ITF_X360
        for( int i = 0; i < 1024; i += 128 )
            __dcbt( i, _pParticleGenerator->m_pParticleArrayIndex );
#endif //ITF_X360

        Vec2d sizeFactor = Vec2d::One;
        if(templateParams->m_parEmitSizeXY.isSet())
            templateParams->m_parEmitSizeXY.getValue(sizeFactor, _pParticleGenerator->getCurrentTime());

        const u32 updatedTot = _pParticleGenerator->m_activeParticlesCount;
        u32 updatedCount = 0;
        for (u32 index = 0;(index<_pParticleGenerator->m_maxParticlesNeed)&&(updatedCount<updatedTot);++index)
        {
            pParticle = &_pParticleGenerator->m_pParticleArrayIndex[index];

#ifdef ITF_X360
            for( int i = 0; i < 4096; i += 128 )
                __dcbt( i+index*sizeof(ITF_Particle), _pParticleGenerator->m_pParticleArrayIndex  );
#endif //ITF_X360

            /// Set a pointer to the head
            if (pParticle->m_bAlive)
            {
                updatedCount++;
                /// Calculate new position
                f32 elapsedTime =  currentTime - pParticle->m_initTime;

                if( elapsedTime >= pParticle->m_phaselifeTime )
                {
                    pParticle->m_curPhase++;
                    if (pParticle->m_curPhase == nbPhase && (pParticle->m_flag & PAR_F_LOOP))
                        pParticle->m_curPhase = 0;
                    if (pParticle->m_curPhase == nbPhase)
                    {
                        _pParticleGenerator->deallocateParticle(pParticle); 
                        --_pParticleGenerator->m_activeParticlesCount;
                        continue;
                    }
                    else
                    {
                        _pParticleGenerator->getParameters()->initToNextPhase(pParticle, currentTime, _pParticleGenerator);
                        elapsedTime =  currentTime - pParticle->m_initTime;
                    }
                }
#ifdef PAR_USE_TIMEVELTARGET
                pParticle->m_curtimeTarget -= _dt;

                /// new target ??.
                if (pParticle->m_curtimeTarget < 0.f)
                {
                    Vec3d randomVec = getRandomVectorParticle();
                    pParticle->m_vel = pParticle->m_velTarget;
                    pParticle->m_velTarget = randomVec * randomParticle.GetFloat(0.1f, 1.f);
                    pParticle->m_timeTarget = randomParticle.GetFloat(3.5, 14.f);
                    pParticle->m_curtimeTarget = pParticle->m_timeTarget;
                }

                f32 r = (pParticle->m_timeTarget - pParticle->m_curtimeTarget) / pParticle->m_timeTarget;
                Vel.x() = f32_Lerp(pParticle->m_velTarget.x(), pParticle->m_vel.x() , r);
                Vel.y() = f32_Lerp(pParticle->m_velTarget.y(), pParticle->m_vel.y() , r);
                Vel.z() = 0.f;
                Vec3d::ScaleAdd(&pParticle->m_pos, &Vel, _dt, &pParticle->m_pos);
#else
                Vec3d::ScaleAdd(&pParticle->m_vel, &acc, _dt, &pParticle->m_vel);
                pParticle->m_vel *= pParams->getFriction();
                Vec3d::ScaleAdd(&pParticle->m_pos, &pParticle->m_vel, _dt, &pParticle->m_pos);
#endif // PAR_USE_TIMEVELTARGET

                pParticle->m_angle += pParticle->m_angularSpeed * _dt;

                /// blend.
                updateBlendings(pParticle, _pParticleGenerator);
                pParticle->m_curSize *= sizeFactor;

                if (pParticle->m_flag & PAR_F_USEANIM)
                    _pParticleGenerator->updateParticleAnim(pParticle, elapsedTime);

                _pParticleGenerator->SetUVFromUVmode(pParticle);

                if (_pParticleGenerator->getTrueAABB())
                    _pParticleGenerator->updateAABB(pParticle);

                #ifdef ITF_SUPPORT_MULTITHREADED
                _pParticleGenerator->fillNormal(pParticle,pTexture,pdata, scaleUV);
                pdata += 4;
                #endif // ITF_SUPPORT_MULTITHREADED
                vertexCountToDisplay ++;
            }
        }

        _pParticleGenerator->setParticlesCountVtxToDisplay( vertexCountToDisplay);
    }

    ///----------------------------------------------------------------------------//

    void updateModeComplex(ITF_ParticleGenerator* _pParticleGenerator,f32 _dt, bbool _bDisableCreation)
    {
        ITF_Particle* __restrict pParticle;

        /// Update our particle system timer...
        _pParticleGenerator->incCurrentTime(_dt);

        const f32 currentTime   = _pParticleGenerator->getCurrentTime();
        const ParticleGeneratorParameters* templateParams = _pParticleGenerator->getTemplateParameters();
        const ParticleGeneratorInstanceParameters* pParams = _pParticleGenerator->getParameters();     
        const u32 nbPhase       = pParams->getNumberPhase();
        Texture* pTexture = _pParticleGenerator->getParameters()->getMaterial().getTexture();
        if ((!pTexture) || (!pTexture->isPhysicallyLoaded()))
            return;

        #ifdef ITF_SUPPORT_MULTITHREADED
        VertexPCT* pdata = (VertexPCT*)_pParticleGenerator->getCachedMemory();
        ITF_ASSERT_CRASH(pdata, "[VB] Cached memory error !!");
        Texture * texParams = pParams->getMaterial().getTexture();
        const Vec2d scaleUV((texParams && texParams->getUVAddressModeX() == GFX_TEXADRESSMODE_MIRROR) ? 2.f : 1.f,
                            (texParams && texParams->getUVAddressModeY() == GFX_TEXADRESSMODE_MIRROR) ? 2.f : 1.f);
        #endif // ITF_SUPPORT_MULTITHREADED

        u32 vertexCountToDisplay = 0;
        u32 NumParticlesToEmit = _pParticleGenerator->computeParticleToEmitCount(_dt, _bDisableCreation);
        for( u32 i = 0; i < NumParticlesToEmit; ++i )
        {
            _pParticleGenerator->addOneNewComplexParticle();
        }

        _pParticleGenerator->updateZSort();

#ifdef ITF_X360
        for( int i = 0; i < 1024; i += 128 )
            __dcbt( i, _pParticleGenerator->m_pParticleArrayIndex );
#endif //ITF_X360

        const bbool bMeshAnimVertex = _pParticleGenerator->getAnimMeshVertex()?btrue:bfalse;
        const Vec3d& acc = _pParticleGenerator->getParticlesAcceleration();
        const bbool bUseBezier = templateParams->getFollowBezier() && _pParticleGenerator->getBezierBranch();
        
        if( bUseBezier )
        {
            f32 maxDist = _pParticleGenerator->getBezierBranch()->getCurrentLength();
            const BranchCurve& curve = _pParticleGenerator->getBezierBranch()->getCurve();
            const u32 updatedTot = _pParticleGenerator->m_activeParticlesCount;
            u32 updatedCount = 0;
            for (u32 index = 0;(index<_pParticleGenerator->m_maxParticlesNeed)&&(updatedCount<updatedTot);++index)
            {
                pParticle = &_pParticleGenerator->m_pParticleArrayIndex[index];
                if (!pParticle->m_bAlive)
                    continue;
                updatedCount++;
                if(maxDist < pParticle->m_bezierDist)
                    continue;
                f32 velNorm = pParticle->m_vel.norm();
                if(_pParticleGenerator->getTemplateParameters()->getFollowBezier()>0)
                {// particles follow bezier from start to end
                    pParticle->m_bezierDist += velNorm * _dt;
                }
                else
                {// particles follow bezier from end to start
                    velNorm *= -1.f;
                    pParticle->m_bezierDist += velNorm * _dt;
                    if(pParticle->m_bezierDist<0.f)
                        continue;
                }
                if(maxDist < pParticle->m_bezierDist)
                    continue;
                const u32 edgeIndex = curve.getEdgeIndexAtDistance(pParticle->m_bezierDist);
                const f32 t = curve.getTAtDistance(edgeIndex, pParticle->m_bezierDist);
                pParticle->m_vel = curve.getTanAtT(edgeIndex, t).normalize() * velNorm;
            }
        }

        Vec2d sizeFactor = Vec2d::One;
        if(templateParams->m_parEmitSizeXY.isSet())
            templateParams->m_parEmitSizeXY.getValue(sizeFactor, _pParticleGenerator->getCurrentTime());

        const u32 updatedTot = _pParticleGenerator->m_activeParticlesCount;
        u32 updatedCount = 0;
        for (u32 index = 0;(index<_pParticleGenerator->m_maxParticlesNeed)&&(updatedCount<updatedTot);++index)
        {
            pParticle = &_pParticleGenerator->m_pParticleArrayIndex[index];

#ifdef ITF_X360
            for( int i = 0; i < 4096; i += 128 )
                __dcbt( i+index*sizeof(ITF_Particle), _pParticleGenerator->m_pParticleArrayIndex  );
#endif //ITF_X360

            if (pParticle->m_bAlive)
            {
                STATS_PARTICULE_COMPLEXACTIVEINC
                updatedCount++;

                if(!(_pParticleGenerator->getTemplate()->getIsParticlePhaseLoop()) && 
                    (pParticle->m_dieTime <= currentTime))
                {
                    switch(_pParticleGenerator->getParameters()->getBehaviorOnEnd())
                    {
                    case OnEnd_Destroy : 
                        // Destroy the particle (normal behavior). 
                        _pParticleGenerator->deallocateParticle(pParticle); 
                        --_pParticleGenerator->m_activeParticlesCount;
                        continue;

                    case OnEnd_Keep: 
                        // The particle ignores its lifetime. 
                        // It will be updated according the last value of the particles curve. 
                        break;

                    case OnEnd_Loop: 
                        // The particle curves are reset to their start points. 
                        // It keeps its current velocity and acceleration. 
                        pParticle->m_dieTime = currentTime + (pParticle->m_dieTime - pParticle->m_birthTime);
                        pParticle->m_birthTime = currentTime;
                        break;

                    case OnEnd_Freeze: 
                        // The particle is frozen : it is not updated anymore and will stay at its position forever. 
                        pParticle->m_flag |= PAR_F_FROZEN;
                        break;
                    }
                }

                // No updates for frozen particles. 
                if((pParticle->m_flag & PAR_F_FROZEN) == 0)
                {
                    if(templateParams->getUseYMin())
                    {
                        // Y offset (represents the position of the spawn point). 
                        f32 offset = 0;

                        // If useActorTranslation is set, particle position is relative to the spawn point. 
                        if(templateParams->getUseYMinLocal())
                        {
                            // Position is already local if useActorPosition is set. 
                            if(!_pParticleGenerator->getParameters()->m_useActorTranslation)
                                offset = pParticle->m_posYInit;
                        }
                        else
                        {
                            // The particle position must be converted to a world position. 
                            if (_pParticleGenerator->getParameters()->m_useActorTranslation)
                                offset = -_pParticleGenerator->getParticlesSpawnPoint().y();
                        }

                        if (pParticle->m_pos.getY() < templateParams->getYMin() + offset)
                        {
                            if(pParticle->m_bounceCount < pParticle->m_bounceMax)
                            {
                                // Bounce! 
                                pParticle->m_bounceCount++;
                                f32 bounciness = _pParticleGenerator->randomLCG.getRandF(templateParams->getYMinBouncinessMin(), templateParams->getYMinBouncinessMax());
                                pParticle->m_vel.y() = - pParticle->m_vel.y() * bounciness;
                                pParticle->m_velAcc.y() = - pParticle->m_velAcc.y() * bounciness;

                                if(templateParams->getBouncinessAffectsXSpeed())
                                {
                                    pParticle->m_vel.x() = pParticle->m_vel.x() - pParticle->m_vel.x() * (1 - bounciness);
                                    pParticle->m_velAcc.x() = pParticle->m_velAcc.x() - pParticle->m_velAcc.x() * (1 - bounciness);
                                }

                                if(templateParams->getBouncinessAffectsAngularSpeed())
                                {
                                    pParticle->m_angularSpeed = pParticle->m_angularSpeed - pParticle->m_angularSpeed * (1 - bounciness);
                                }

                                pParticle->m_pos.y() = templateParams->getYMin() + offset;
                            }
                            else if(templateParams->getYMinKillAfterMaxRebounds())
                            {
                                // Kill the particle. 
                                _pParticleGenerator->deallocateParticle(pParticle); 
                                --_pParticleGenerator->m_activeParticlesCount;
                                continue;
                            }
                            else
                            {
                                pParticle->m_pos.y() = templateParams->getYMin() + offset;
                                pParticle->m_vel.y() = 0.f;
                                pParticle->m_velAcc.y() = 0.f;
                                pParticle->m_vel.x() = 0.f;
                                pParticle->m_velAcc.x() = 0.f;
                            }
                        }
                    }

                    f32 elapsedTime = currentTime - pParticle->m_initTime;
                    while ( elapsedTime >= pParticle->m_phaselifeTime )
                    {
                        pParticle->m_curPhase++;
                        if (!_bDisableCreation && pParticle->m_curPhase == nbPhase && (pParticle->m_flag & PAR_F_LOOP))
                            pParticle->m_curPhase = 0;
                        if (pParticle->m_curPhase == nbPhase)
                        {
                            _pParticleGenerator->deallocateParticle(pParticle); 
                            --_pParticleGenerator->m_activeParticlesCount;
                            break;;
                        }
                        else
                        {
                            elapsedTime = (elapsedTime - pParticle->m_phaselifeTime);
                            _pParticleGenerator->getParameters()->initToNextPhase(pParticle, currentTime - elapsedTime, _pParticleGenerator);
                        }
                    }
                    if(!pParticle->m_bAlive)
                        continue;

                    /// Calculate new position
                    const f32 currentTime = _pParticleGenerator->getCurrentTime();
                    const f32 parLifeTime = pParticle->m_dieTime - pParticle->m_birthTime;
                    const f32 parTimeFactor = parLifeTime<=0.f?1.f:(currentTime - pParticle->m_birthTime)/parLifeTime;
                    const f32 ranf1 = float(pParticle->GetSeed4()) * float(1.0f/255.f);
                    const f32 ranf2 = float(pParticle->GetSeed1()) * float(1.0f/255.f);
                    const f32 ranf3 = float(pParticle->GetSeed3()) * float(1.0f/255.f);
                    const f32 ranf4 = float(pParticle->GetSeed2()) * float(1.0f/255.f);

                    if(!bUseBezier && templateParams->m_parPosition.isSet())
                    {
                        // Fixed position mode - disables all speed / acceleration settings. 
                        Vec3d oldPos = pParticle->m_pos;
                        Vec3d newPos;
                        templateParams->m_parPosition.getValue(newPos, parTimeFactor);
                        pParticle->m_pos = newPos + _pParticleGenerator->getParameters()->getPosition();
                        pParticle->m_velReal = pParticle->m_vel = (pParticle->m_pos - oldPos) / _dt;
                    }
                    else
                    {
                        // Standard mode. 
                        Vec3d::ScaleAdd(&pParticle->m_velAcc, &acc, _dt, &pParticle->m_velAcc);

                        if(templateParams->m_parAccelerationX.isSet()
                            || templateParams->m_parAccelerationY.isSet()
                            || templateParams->m_parAccelerationZ.isSet())
                        {
                            Vec2d accCurve;
                            Vec3d accParticle;
                            accParticle.clear();
                            if(templateParams->m_parAccelerationX.isSet())
                            {
                                templateParams->m_parAccelerationX.getValue(accCurve, parTimeFactor);
                                accParticle.x() = f32_Lerp(accCurve.x(), accCurve.y(), ranf1);
                                if(_pParticleGenerator->getParameters()->isFlip() && templateParams->canFlipAccel())
                                    accParticle.x() = -accParticle.x();
                            }
                            if(templateParams->m_parAccelerationY.isSet())
                            {

                                templateParams->m_parAccelerationY.getValue(accCurve, parTimeFactor);
                                accParticle.y() = f32_Lerp(accCurve.x(), accCurve.y(), ranf2);
                            }
                            if(templateParams->m_parAccelerationZ.isSet())
                            {
                                templateParams->m_parAccelerationZ.getValue(accCurve, parTimeFactor);
                                accParticle.z() = f32_Lerp(accCurve.x(), accCurve.y(), ranf3);
                            }

                            Vec3d::ScaleAdd(&pParticle->m_velAcc, &accParticle, _dt, &pParticle->m_velAcc);
                        }

                        pParticle->m_vel *= pParams->getFriction();

                        pParticle->m_velReal = pParticle->m_vel;
                        if (templateParams->m_parVelocityMult.isSet())
                        {
                            Vec2d velMultCurve;
                            templateParams->m_parVelocityMult.getValue(velMultCurve, parTimeFactor);
                            f32 velMultFactor = f32_Lerp(velMultCurve.x(), velMultCurve.y(), ranf4);
                            pParticle->m_velReal *= velMultFactor;
                        }

                        pParticle->m_velReal += pParticle->m_velAcc;
                        Vec3d::ScaleAdd(&pParticle->m_pos, &pParticle->m_velReal, _dt, &pParticle->m_pos);
                    }

                    // Update angular speed from curve. 
                    if (templateParams->m_parAngularSpeed.isSet())
                    {
                        f32 ranf5 = float(pParticle->GetSeed5()) * float(1.0f/255.f);
                        Vec2d v;
                        templateParams->m_parAngularSpeed.getValue(v, parTimeFactor);
                        f32 angle = f32_Lerp(v.x(), v.y(), ranf5);

                        pParticle->m_angularSpeed.SetDegrees(angle);
                    }

                    // Update particle angle. 
                    if (pParticle->m_flag & PAR_F_ORIENTDIR)
                    {
                        /// Orient to dir.
                        Vec3d dir = pParticle->m_velReal;
                        dir.normalize();
                        pParticle->m_angle.SetRadians(pParticle->m_angleInit.ToRadians() + atan2f(dir.y(), dir.x()));

                        if (templateParams->CanFlipOrientDir() && pParams->isFlip())
                        {
                            pParticle->m_angle -= MTH_PI;
                        }
                    }
                    else if (pParticle->m_flag & PAR_F_ORIENTDIRGEN)
                    {
                        /// Orient to generator position.
                        Vec3d dir = pParticle->m_pos -_pParticleGenerator->getParameters()->getPosition();
                        dir.normalize();
                        pParticle->m_angle.SetRadians(pParticle->m_angleInit.ToRadians() + atan2f(dir.y(), dir.x()));
                    }
                    else if(templateParams->m_parAngle.isSet())
                    {
                        Vec2d angleCurve;
                        templateParams->m_parAngle.getValue(angleCurve, parTimeFactor);
                        pParticle->m_angle.SetDegrees(f32_Lerp(angleCurve.x(), angleCurve.y(), ranf3));
                    }
                    else
                    {
                        pParticle->m_angle += pParticle->m_angularSpeed * _dt;
                    }

                    /// blend.
                    updateBlendings(pParticle, _pParticleGenerator);
                    pParticle->m_curSize *= sizeFactor;

                    /// todo: collision ??.

                    if (pParticle->m_flag & PAR_F_USEANIM)
                        _pParticleGenerator->updateParticleAnim(pParticle, elapsedTime);

                    _pParticleGenerator->SetUVFromUVmode(pParticle);
                }

                if (_pParticleGenerator->getTrueAABB())
                    _pParticleGenerator->updateAABB(pParticle);

                if(!bMeshAnimVertex)
                {
#ifdef ITF_SUPPORT_MULTITHREADED
                    _pParticleGenerator->fillNormal(pParticle,pTexture,pdata, scaleUV);
                    pdata += 4;
#endif // ITF_SUPPORT_MULTITHREADED
                }
                vertexCountToDisplay ++;
            }
        }

        _pParticleGenerator->setParticlesCountVtxToDisplay( vertexCountToDisplay);
    }

    //------------------------------------------------------------------------------------------
    
    void  JobUpdateParticle::synchronize()
    {
        m_context.m_pParticleGenerator->synchronize();
    }

    void mainJobUpdateParticle(JobUpdateParticleContext* _context,bbool _immediate)
    {
        STATS_PARTICULE_UPDATEINC

        ITF_ParticleGenerator*    pParticleGenerator = _context->m_pParticleGenerator;
        ParticleGeneratorInstanceParameters * pParameters = pParticleGenerator->getParameters();
        const ParticleGeneratorParameters * const templateParameters = pParticleGenerator->getTemplateParameters();

        if (pParticleGenerator->getTrueAABB() || pParticleGenerator->isUseMatrix())
        {
            //init working bounding box with generator pos
            if(pParticleGenerator->getTrueAABB() || pParameters->m_useActorTranslation)
                pParameters->setWorkingBoundingBox(pParameters->getPosition());
            else
                pParameters->setWorkingBoundingBox(Vec3d::Zero);
        }

        f32 dt = _context->m_deltaTime;
        bbool disableCreation = _context->m_disableCreation;

        if(!disableCreation && !pParticleGenerator->getNumGeneratedParticles() && templateParameters->getInitLifeTime())
        {
            f32 t = templateParameters->getInitLifeTime() - dt;
            if(t<=0.f)
                t = 0.f;
            pParticleGenerator->spawnAtTime(t);
        }

        switch(pParameters->getGeneratorMode())
        {
        case PARGEN_MODE_FOLLOW: 
            updateModeFollow(pParticleGenerator,dt, disableCreation);
            break;
        case PARGEN_MODE_MANUAL:
            updateModeManual(pParticleGenerator,dt);
            break;
        case PARGEN_MODE_COMPLEX:
        default:
            updateModeComplex(pParticleGenerator,dt, disableCreation);
            break;
        }

        // In bounding box auto compute mode, add pivot offset to bounding box. 
        if (pParticleGenerator->getTrueAABB() && pParticleGenerator->getParameters()->getPivot() != Vec2d::Zero)
        {
            // Particle rotation force us to compute the pivot offset in a wide way. 
            f32 pivotNorm = pParticleGenerator->getParameters()->getPivot().norm();
            pParameters->getBoundingBoxWorking().grow(pivotNorm, pivotNorm);
        }

        if (pParticleGenerator->isUseMatrix() /*&& !pParticleGenerator->getTrueAABB()*/)
        {
            pParticleGenerator->transformBoundingBoxWithMatrix();
        }

        // MeshAnimVertex
        if(pParticleGenerator->getAnimMeshVertex())
        {
            pParticleGenerator->m_animListRuntime.clear();
            SingleAnimDataRuntime sadr;
            ITF_Particle* pParticle;

            for(u32 index = 0; index < pParticleGenerator->m_maxParticlesNeed; ++index)
            {
                pParticle = &pParticleGenerator->m_pParticleArrayIndex[index];
                if(!pParticle->m_bAlive)
                    continue;

#ifndef ITF_FINAL
                if(pParticle->m_animId == U32_INVALID)
                {
                    ITF_WARNING(NULL, bfalse, "AnimMeshVertex (particle) : Invalid anim ID.");
                    continue;
                }

                if((u32)pParticle->m_indexUV > pParticleGenerator->getAnimMeshVertex()->getNbFrameForAnim(pParticle->m_animId)-1)
                {
                    ITF_WARNING(NULL, bfalse, "AnimMeshVertex (particle) : Invalid frame ID %d.", pParticle->m_indexUV);
                    continue;
                }
#endif
                sadr.setColor(pParticle->m_curColor.getAsU32());
                sadr.setAnim(pParticle->m_animId);
                sadr.setFrame((u32) pParticle->m_indexUV);
                sadr.setTransform3dFrom(pParticle->m_pos, pParticle->m_angle.ToRadians(), pParticle->m_curSize, (pParticle->m_flag & PAR_F_UVINVERT_X) != 0);
                pParticleGenerator->m_animListRuntime.push_back(sadr);
            }
        }
        if (_immediate)
            pParticleGenerator->synchronize();

#ifdef DIRECTVB_JOB
        pParticleGenerator->unlockMeshVB();
#endif // DIRECTVB_JOB
    }
}
