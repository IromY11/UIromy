#include "precompiled_engine.h"

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_PARTICULEGENERATORPARAMETERS_H_
#include "engine/display/particle/ParticuleGeneratorParameters.h"
#endif //_ITF_PARTICULEGENERATORPARAMETERS_H_

#ifndef _ITF_RESOURCE_CONTAINER_H_
#include "engine/resources/ResourceContainer.h"
#endif //_ITF_RESOURCE_CONTAINER_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

namespace ITF
{
    BEGIN_SERIALIZATION(ParticleGeneratorParameters)

        SERIALIZE_MEMBER("maxParticles",m_maxParticles);
        SERIALIZE_MEMBER("defaultColor",m_defaultColor);
        SERIALIZE_MEMBER("emitParticlesCount",m_emitParticlesCount); if((i32)m_emitParticlesCount < 0) m_emitParticlesCount = U32_INVALID;
        SERIALIZE_MEMBER("forceNoDynamicFog",m_ForceNoDynamicFog);
        SERIALIZE_MEMBER("renderInReflection",m_renderInReflection);
        SERIALIZE_MEMBER("dieFadeTime",m_dieFadeTime);
        SERIALIZE_MEMBER("emitterMaxLifeTime",m_emitterMaxLifeTime);
        SERIALIZE_ENUM_BEGIN("behaviorOnEnd",m_behaviorOnEnd);
            SERIALIZE_ENUM_VAR(OnEnd_Destroy);
            SERIALIZE_ENUM_VAR(OnEnd_Keep);
            SERIALIZE_ENUM_VAR(OnEnd_Loop);
            SERIALIZE_ENUM_VAR(OnEnd_Freeze);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("pos",m_posOffset);
        SERIALIZE_MEMBER("pivot",m_pivot);
        SERIALIZE_MEMBER("velNorm",m_velNorm);
        SERIALIZE_MEMBER("velAngle",m_velParamAngle);
        SERIALIZE_MEMBER("velAngleDelta",m_velParamAngleDelta);
        SERIALIZE_MEMBER("grav",m_grav);
        SERIALIZE_MEMBER("acc",m_acc);
        SERIALIZE_MEMBER("depth",m_depth);
        SERIALIZE_MEMBER("useZAsDepth",m_useZAsDepth);
        SERIALIZE_MEMBER("velocityVar",m_velocityVar);
        SERIALIZE_MEMBER("friction",m_friction);
        SERIALIZE_MEMBER("freq",m_freq);
        SERIALIZE_MEMBER("freqDelta",m_freqDelta);
        SERIALIZE_MEMBER("forceEmitAtStart",m_ForceEmitAtStart);
        SERIALIZE_MEMBER("emitBatchCount",m_emitBatchCount);
        SERIALIZE_MEMBER("emitBatchCount_AAO",m_emitBatchCountAAO);
        SERIALIZE_MEMBER("emitBatchCount_AAO_max",m_emitBatchCountAAOMax);
        SERIALIZE_MEMBER("initAngle",m_initAngle);
        SERIALIZE_MEMBER("angleDelta",m_angleDelta);
        SERIALIZE_MEMBER("angularSpeed",m_angularSpeed);
        SERIALIZE_MEMBER("angularSpeedDelta",m_angularSpeedDelta);
        SERIALIZE_MEMBER("timeTarget",m_timeTarget);
        SERIALIZE_MEMBER("nbPhase",m_nbPhase);
        SERIALIZE_MEMBER("renderPrio",m_renderPrio);
        SERIALIZE_MEMBER("initLifeTime",m_initLifeTime);
        SERIALIZE_MEMBER("circleRadius",m_circleRadius);
        SERIALIZE_MEMBER("innerCircleRadius",m_innerCircleRadius);
        SERIALIZE_MEMBER("scaleShape",m_scaleShape);
        SERIALIZE_MEMBER("rotateShape",m_rotateShape);
        SERIALIZE_MEMBER("randomizeDirection",m_randomizeDirection);
        SERIALIZE_MEMBER("followBezier",m_FollowBezier);
        SERIALIZE_MEMBER("getAtlasSize",m_getAtlasSize);
        SERIALIZE_MEMBER("continuousColorRandom", m_continuousColorRandom);
        SERIALIZE_OBJECT("genBox",m_genBox);
        SERIALIZE_MEMBER("GenSize",m_genSize);
        SERIALIZE_MEMBER("GenSide",m_genSide);
        SERIALIZE_MEMBER("GenPosMin",m_genBezierStart);
        SERIALIZE_MEMBER("GenPosMax",m_genBezierEnd);
        SERIALIZE_MEMBER("GenDensity",m_genBezierDensity);
        SERIALIZE_OBJECT("boundingBox",m_bounding);
        SERIALIZE_MEMBER("orientDir",m_orientDir);
        SERIALIZE_ENUM_BEGIN("UVmode",m_UVmodeDeprecated);
            SERIALIZE_ENUM_VAR(UV_Default);
            SERIALIZE_ENUM_VAR(UV_FlipX);
            SERIALIZE_ENUM_VAR(UV_FlipY);
            SERIALIZE_ENUM_VAR(UV_FlipXY);
            SERIALIZE_ENUM_VAR(UV_FlipXtoDirX);
            SERIALIZE_ENUM_VAR(UV_FlipYtoDirY);
            SERIALIZE_ENUM_VAR(UV_FlipXYtoDirXY);
        SERIALIZE_ENUM_END();
        SERIALIZE_ENUM_FLAGS_BEGIN("UVmodeFlag",m_UVmodeFlag);
            SERIALIZE_ENUM_VAR(UVF_Default);
            SERIALIZE_ENUM_VAR(UVF_FlipX);
            SERIALIZE_ENUM_VAR(UVF_FlipY);
            SERIALIZE_ENUM_VAR(UVF_FlipXtoDirX);
            SERIALIZE_ENUM_VAR(UVF_FlipYtoDirY);
            SERIALIZE_ENUM_VAR(UVF_FlipXRandom);
            SERIALIZE_ENUM_VAR(UVF_FlipYRandom);
        SERIALIZE_ENUM_FLAGS_END();
        // Conversion to UVMode new value. 
        if(m_UVmodeFlag == UVF_Default)
        {
            switch(m_UVmodeDeprecated)
            {
            case UV_Default       : m_UVmodeFlag = UVF_Default; break;
            case UV_FlipX         : m_UVmodeFlag = UVF_FlipX; break;
            case UV_FlipY         : m_UVmodeFlag = UVF_FlipY; break;
            case UV_FlipXY        : m_UVmodeFlag = (UVMODEFLAG) (UVF_FlipX | UVF_FlipY); break;
            case UV_FlipXtoDirX   : m_UVmodeFlag = UVF_FlipXtoDirX; break;
            case UV_FlipYtoDirY   : m_UVmodeFlag = UVF_FlipYtoDirY; break;
            case UV_FlipXYtoDirXY : m_UVmodeFlag = (UVMODEFLAG) (UVF_FlipXtoDirX | UVF_FlipYtoDirY); break;
            }
        }
        SERIALIZE_MEMBER("uniformscale",m_uniformScale);
        SERIALIZE_MEMBER("genangmin",m_genAngMin);
        SERIALIZE_MEMBER("genangmax",m_genAngMax);
        SERIALIZE_MEMBER("useYMin",m_useYMin);
        SERIALIZE_MEMBER("useYMinLocal",m_useYMinLocal);
        SERIALIZE_MEMBER("yMin",m_yMin);
        SERIALIZE_MEMBER("yMinBouncinessMin",m_yMinBouncinessMin);
        SERIALIZE_MEMBER("yMinBouncinessMax",m_yMinBouncinessMax);
        SERIALIZE_MEMBER("yMinMinRebounds",m_yMinMinRebounds);
        SERIALIZE_MEMBER("yMinMaxRebounds",m_yMinMaxRebounds);
        SERIALIZE_MEMBER("yMinKillAfterMaxRebounds",m_yMinKillAfterMaxRebounds);

        SERIALIZE_MEMBER("bouncinessAffectsXSpeed", m_bouncinessAffectsXSpeed);
        SERIALIZE_MEMBER("bouncinessAffectsAngularSpeed", m_bouncinessAffectsAngularSpeed);

        SERIALIZE_MEMBER("canFlipAngleOffset",m_canFlipAngleOffset);
        SERIALIZE_MEMBER("canFlipInitAngle",m_canFlipInitAngle);
        SERIALIZE_MEMBER("canFlipAngularSpeed",m_canFlipAngularSpeed);
        SERIALIZE_MEMBER("canFlipPivot",m_canFlipPivot);
        SERIALIZE_MEMBER("canFlipPos",m_canFlipPosOffset);
        SERIALIZE_MEMBER("canFlipUV",m_canFlipUV);
        SERIALIZE_MEMBER("canFlipAngleMin",m_canFlipAngleMin);
        SERIALIZE_MEMBER("canFlipAngleMax",m_canFlipAngleMax);
        SERIALIZE_MEMBER("canFlipAccel",m_canFlipAccel);
        SERIALIZE_MEMBER("canFlipOrientDir",m_canFlipOrientDir);
        SERIALIZE_MEMBER("numberSplit",m_nSplit);
        SERIALIZE_MEMBER("splitDelta",m_splitDelta);
        SERIALIZE_ENUM_BEGIN("useMatrix",m_useMatrix);
            SERIALIZE_ENUM_VAR(BOOL_false );
            SERIALIZE_ENUM_VAR(BOOL_true);
            SERIALIZE_ENUM_VAR(BOOL_cond);
        SERIALIZE_ENUM_END();
        SERIALIZE_ENUM_BEGIN("scaleGenBox",m_scaleGenBox);
        SERIALIZE_ENUM_VAR(BOOL_false );
        SERIALIZE_ENUM_VAR(BOOL_true);
        SERIALIZE_ENUM_VAR(BOOL_cond);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("usePhasesColorAndSize", m_usePhasesColorAndSize);
        SERIALIZE_MEMBER("useActorTranslation",m_useActorTranslation);
        SERIALIZE_MEMBER("actorTranslationOffset", m_actorTranslationOffset);
        SERIALIZE_MEMBER("disableLight", m_disableLight);
 
        SERIALIZE_CONTAINER_OBJECT("phases",m_phaseList);

        SERIALIZE_DISPLAYNAME("curvePosition", "Position");  SERIALIZE_USAGEMODE_ATTRIBUTE5("curvePosition",XY,X,Y,Z,XYZ);
        SERIALIZE_OBJECT("curvePosition",m_parPosition);
        SERIALIZE_DISPLAYNAME("curveAngle", "Angle");        SERIALIZE_USAGEMODE_ATTRIBUTE1("curveAngle",WZ);
        SERIALIZE_OBJECT("curveAngle", m_parAngle);
        SERIALIZE_DISPLAYNAME("curveAngularSpeed", "Angular Speed"); SERIALIZE_USAGEMODE_ATTRIBUTE1("curveAngularSpeed", WZ);
        SERIALIZE_OBJECT("curveAngularSpeed", m_parAngularSpeed);
        SERIALIZE_DISPLAYNAME("curveVelocityMult", "Velocity (mult.)"); SERIALIZE_USAGEMODE_ATTRIBUTE1("curveVelocityMult",WZ);
        SERIALIZE_OBJECT("curveVelocityMult",m_parVelocityMult);
        SERIALIZE_DISPLAYNAME("curveAccelX", "Acceleration (X)"); SERIALIZE_USAGEMODE_ATTRIBUTE1("curveAccelX",WZ);
        SERIALIZE_OBJECT("curveAccelX",m_parAccelerationX);
        SERIALIZE_DISPLAYNAME("curveAccelY", "Acceleration (Y)"); SERIALIZE_USAGEMODE_ATTRIBUTE1("curveAccelY",WZ);
        SERIALIZE_OBJECT("curveAccelY",m_parAccelerationY);
        SERIALIZE_DISPLAYNAME("curveAccelZ", "Acceleration (Z)"); SERIALIZE_USAGEMODE_ATTRIBUTE1("curveAccelZ",WZ);
        SERIALIZE_OBJECT("curveAccelZ",m_parAccelerationZ);
        SERIALIZE_DISPLAYNAME("curveSize","Size (square)");  SERIALIZE_USAGEMODE_ATTRIBUTE1("curveSize",WZ);
        SERIALIZE_OBJECT("curveSize",m_parSize);
        SERIALIZE_DISPLAYNAME("curveSizeY","Size (height)"); SERIALIZE_USAGEMODE_ATTRIBUTE1("curveSizeY",WZ);
        SERIALIZE_OBJECT("curveSizeY",m_parSizeY);
        SERIALIZE_DISPLAYNAME("curveAlpha","Alpha");         SERIALIZE_USAGEMODE_ATTRIBUTE1("curveAlpha",WZ);
        SERIALIZE_OBJECT("curveAlpha",m_parAlpha);
        SERIALIZE_DISPLAYNAME("curveRGB","Color");           SERIALIZE_USAGEMODE_ATTRIBUTE1("curveRGB",RGB);
        SERIALIZE_OBJECT("curveRGB",m_parRGB);
        SERIALIZE_DISPLAYNAME("curveRGB1","Color1");         SERIALIZE_USAGEMODE_ATTRIBUTE1("curveRGB1",RGB);
        SERIALIZE_OBJECT("curveRGB1",m_parRGB1);
        SERIALIZE_DISPLAYNAME("curveRGB2","Color2");         SERIALIZE_USAGEMODE_ATTRIBUTE1("curveRGB2",RGB);
        SERIALIZE_OBJECT("curveRGB2",m_parRGB2);
        SERIALIZE_DISPLAYNAME("curveRGB3","Color3");         SERIALIZE_USAGEMODE_ATTRIBUTE1("curveRGB3",RGB);
        SERIALIZE_OBJECT("curveRGB3",m_parRGB3);
        SERIALIZE_DISPLAYNAME("curveAnim","Spreadsheet");    SERIALIZE_USAGEMODE_ATTRIBUTE1("curveAnim",WZ);
        SERIALIZE_OBJECT("curveAnim",m_parAnim);

        SERIALIZE_DISPLAYNAME("parEmitVelocity","Velocity");            SERIALIZE_USAGEMODE_ATTRIBUTE1("parEmitVelocity",WZ);
        SERIALIZE_OBJECT("parEmitVelocity",m_parEmitVelocity);
        SERIALIZE_DISPLAYNAME("parEmitVelocityAngle","Velocity angle"); SERIALIZE_USAGEMODE_ATTRIBUTE1("parEmitVelocityAngle",WZ);
        SERIALIZE_OBJECT("parEmitVelocityAngle",m_parEmitVelocityAngle);
        SERIALIZE_DISPLAYNAME("parEmitAngle","Angle");                  SERIALIZE_USAGEMODE_ATTRIBUTE1("parEmitAngle",WZ);
        SERIALIZE_OBJECT("parEmitAngle",m_parEmitAngle);
        SERIALIZE_DISPLAYNAME("parEmitAngularSpeed","Angular speed");   SERIALIZE_USAGEMODE_ATTRIBUTE1("parEmitAngularSpeed",WZ);
        SERIALIZE_OBJECT("parEmitAngularSpeed",m_parEmitAngularSpeed);
        SERIALIZE_DISPLAYNAME("curveFreq","Frequency");                 SERIALIZE_USAGEMODE_ATTRIBUTE1("curveFreq",WZ);
        SERIALIZE_OBJECT("curveFreq",m_genFreq);
        SERIALIZE_DISPLAYNAME("curveParLifeTime", "Particles lifetime");SERIALIZE_USAGEMODE_ATTRIBUTE1("curveParLifeTime",WZ);
        SERIALIZE_OBJECT("curveParLifeTime",m_parLifeTime);
        SERIALIZE_DISPLAYNAME("curveEmitAlpha", "Alpha factor (Global)");        SERIALIZE_USAGEMODE_ATTRIBUTE1("curveEmitAlpha",X);
        SERIALIZE_OBJECT("curveEmitAlpha",m_parEmitAlpha);
        SERIALIZE_DISPLAYNAME("curveEmitAlphaInit", "Alpha factor (Init)");      SERIALIZE_USAGEMODE_ATTRIBUTE1("curveEmitAlphaInit",WZ);
        SERIALIZE_OBJECT("curveEmitAlphaInit",m_parEmitAlphaInit);
        SERIALIZE_DISPLAYNAME("curveEmitColorFactor", "Color factor (Global)");  SERIALIZE_USAGEMODE_ATTRIBUTE1("curveEmitColorFactor",RGB);
        SERIALIZE_OBJECT("curveEmitColorFactor",m_parEmitColorFactor);
        SERIALIZE_DISPLAYNAME("curveEmitColorFactorInit", "Color factor (Init)");SERIALIZE_USAGEMODE_ATTRIBUTE1("curveEmitColorFactorInit",RGB);
        SERIALIZE_OBJECT("curveEmitColorFactorInit",m_parEmitColorFactorInit);
        SERIALIZE_DISPLAYNAME("curveEmitSizeXY", "Size factor (Global)");        SERIALIZE_USAGEMODE_ATTRIBUTE3("curveEmitSizeXY",XY,X,Y);
        SERIALIZE_OBJECT("curveEmitSizeXY",m_parEmitSizeXY);
        SERIALIZE_DISPLAYNAME("curveEmitSizeXYInit", "Size factor (Init)");      SERIALIZE_USAGEMODE_ATTRIBUTE3("curveEmitSizeXYInit",XY,X,Y);
        SERIALIZE_OBJECT("curveEmitSizeXYInit",m_parEmitSizeXYInit);
        SERIALIZE_DISPLAYNAME("curveEmitAcceleration", "Acceleration"); SERIALIZE_USAGEMODE_ATTRIBUTE5("curveEmitAcceleration",XY,X,Y,Z,XYZ);
        SERIALIZE_OBJECT("curveEmitAcceleration",m_parEmitAcceleration);
        SERIALIZE_DISPLAYNAME("curveEmitGravity", "Gravity");           SERIALIZE_USAGEMODE_ATTRIBUTE5("curveEmitGravity",XY,X,Y,Z,XYZ);
        SERIALIZE_OBJECT("curveEmitGravity",m_parEmitGravity);
        SERIALIZE_DISPLAYNAME("curveEmitAnim", "Spreadsheet init");     SERIALIZE_USAGEMODE_ATTRIBUTE1("curveEmitAnim",WZ);
        SERIALIZE_OBJECT("curveEmitAnim",m_parEmitAnim);

        SERIALIZE_ENUM_BEGIN("genGenType",m_genGenType);
            SERIALIZE_ENUM_VAR(PARGEN_GEN_POINTS );
            SERIALIZE_ENUM_VAR(PARGEN_GEN_RECTANGLE);
            SERIALIZE_ENUM_VAR(PARGEN_GEN_CIRCLE);
            SERIALIZE_ENUM_VAR(PARGEN_GEN_BEZIER);
        SERIALIZE_ENUM_END();

        SERIALIZE_ENUM_BEGIN("genMode",m_genMode);
            SERIALIZE_ENUM_VAR(PARGEN_MODE_FOLLOW );
            SERIALIZE_ENUM_VAR(PARGEN_MODE_COMPLEX);
        SERIALIZE_ENUM_END();

        SERIALIZE_ENUM_BEGIN("genEmitMode",m_genEmitMode);
            SERIALIZE_ENUM_VAR(PARGEN_EMITMODE_OVERTIME );
            SERIALIZE_ENUM_VAR(PARGEN_EMITMODE_ALLATONCE);
            SERIALIZE_ENUM_VAR(PARGEN_EMITMODE_OVERDISTANCE);
        SERIALIZE_ENUM_END();

    END_SERIALIZATION()

    BEGIN_SERIALIZATION(ParPhase)

        SERIALIZE_MEMBER("phaseTime",m_phaseTime);
        SERIALIZE_MEMBER("colorMin",m_colorMin);
        SERIALIZE_MEMBER("colorMax",m_colorMax);
        SERIALIZE_MEMBER("sizeMin",m_sizeMin);
        SERIALIZE_MEMBER("sizeMax",m_sizeMax);
        SERIALIZE_MEMBER("animstart",m_animStart);
        SERIALIZE_MEMBER("animend",m_animEnd);
        SERIALIZE_MEMBER("animname",m_animName);
        SERIALIZE_MEMBER("deltaphasetime",m_deltaphasetime);
        SERIALIZE_MEMBER("animstretchtime",m_animstretchtime);
        SERIALIZE_MEMBER("blendtonextphase",m_blendToNextPhase);
   
    END_SERIALIZATION()

    ParCurve::ParCurve()
    {
        m_outputMin = Vec3d::Zero; m_outputMax = Vec3d::One;
        m_time = m_invtime = 1.f;
    }

    IMPLEMENT_OBJECT_RTTI(ParCurve)
    BEGIN_SERIALIZATION(ParCurve)
        SERIALIZE_MEMBER("baseTime",m_time);
        SERIALIZE_MEMBER("outputMin",m_outputMin);
        SERIALIZE_MEMBER("outputMax",m_outputMax);
        SERIALIZE_OBJECT("curve",m_curve);
        m_invtime = m_time>0.f?1.f/m_time:0.f;
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(ParLifeTimeCurve)
    BEGIN_SERIALIZATION_CHILD(ParLifeTimeCurve)
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(EmitLifeTimeCurve)
    BEGIN_SERIALIZATION_CHILD(EmitLifeTimeCurve)
    END_SERIALIZATION()

    f32 ParCurve::getValue(f32 _t) const
    {
        _t = _t*m_invtime;
        if(!m_curve.GetNumPoints())
            return Interpolate(m_outputMin.x(), m_outputMax.x(), Clamp(_t, 0.f, 1.f));

        m_curve.GetInterpolatedAtTimeX(_t, _t);
        return Interpolate(m_outputMin.x(), m_outputMax.x(), _t);
    }

    void ParCurve::getValue(Vec2d& _dst, f32 _t) const
    {
        _t = _t*m_invtime;
        if(!m_curve.GetNumPoints())
        {
            _dst = Interpolate(m_outputMin.truncateTo2D(), m_outputMax.truncateTo2D(), Clamp(_t, 0.f, 1.f));
            return;
        }

        m_curve.GetInterpolatedAtTimeXY(_t, _dst);
        _dst = Interpolate(m_outputMin.truncateTo2D(), m_outputMax.truncateTo2D(), _dst);
    }

    void ParCurve::getValue(Vec3d& _dst, f32 _t) const
    {
        _t = _t*m_invtime;
        if(!m_curve.GetNumPoints())
        {
            _dst = Interpolate(m_outputMin, m_outputMax, Clamp(_t, 0.f, 1.f));
            return;
        }

        m_curve.GetInterpolatedAtTime(_t, _dst);
        _dst = Interpolate(m_outputMin, m_outputMax, _dst);
    }

    ParticleGeneratorParameters::ParticleGeneratorParameters()
    {
        m_maxParticles			= 1;
        m_emitParticlesCount	= U32_INVALID;
        m_ForceNoDynamicFog     = bfalse;
        m_renderInReflection    = btrue;
        m_ForceEmitAtStart      = bfalse;
        m_emitBatchCount		= 1;
        m_emitBatchCountAAO     = 1;
        m_emitBatchCountAAOMax  = U32_INVALID;
        m_defaultColor          = Color::white();

        m_pos					= Vec3d::Zero;
        m_posOffset             = Vec3d::Zero;
        m_pivot                 = Vec2d::Zero;
        m_velNorm               = 1.f;
        m_vel			        = Vec3d::Zero;
        m_velParamAngle	        = 0.f;
        m_velParamAngleDelta    = 0.f;
        m_grav					= Vec3d::Zero;
        m_acc					= Vec3d::Zero;
        m_velocityVar			= 1.0f;

        m_useZAsDepth           = btrue;
        m_depth                 = 0.f;

        m_freq					= 0.05f;
        m_freqDelta             = 0.f;

        m_renderPrio            = 0.f;

        m_nbPhase               = 1;

        m_initLifeTime          = 0.f;

        m_circleRadius          = 1.f;
        m_innerCircleRadius     = 0.f;
        m_scaleShape            = Vec3d::One;
        m_rotateShape           = Vec3d::Zero;

        m_orientDir             = 0;
        m_UVmodeDeprecated      = UV_Default;
        m_UVmodeFlag            = UVF_Default;
        m_uniformScale          = 0.f;

        m_randomizeDirection    = btrue;
        m_FollowBezier          = 1;
        m_getAtlasSize          = bfalse;
        m_continuousColorRandom = bfalse;

        m_friction = 1.f;
        m_timeTarget = 0.f;
        m_genMode = PARGEN_MODE_COMPLEX;
        m_genGenType = PARGEN_GEN_POINTS;
        m_genEmitMode = PARGEN_EMITMODE_OVERTIME;

        m_genAngMin = Angle(btrue, -180);
        m_genAngMax = Angle(btrue, 180);

        m_canFlipAngleOffset = bfalse;
        m_canFlipInitAngle = bfalse;
        m_canFlipAngularSpeed = bfalse;
        m_canFlipPivot = bfalse;
        m_canFlipPosOffset = bfalse;
        m_canFlipUV = bfalse;
        m_canFlipAngleMin = bfalse;
        m_canFlipAngleMax = bfalse;
        m_canFlipAccel = bfalse;
        m_canFlipBoundingBox = bfalse;
        m_canFlipOrientDir = bfalse;
        m_useMatrix = BOOL_false;
        m_scaleGenBox = BOOL_cond;
        m_useActorTranslation = bfalse;
        m_actorTranslationOffset = Vec2d::Zero;

        m_disableLight = bfalse;

        m_usePhasesColorAndSize = btrue;

        m_nSplit = 0;
        m_splitDelta = Angle(btrue, 0);

        m_genSize = 0.f;
        m_genSide = 0;
        m_genBezierStart = 0.f;
        m_genBezierEnd = 1.f;
        m_genBezierDensity = -1.f;

        m_dieFadeTime = -1.f;
        m_emitterMaxLifeTime = -1.f;
        m_behaviorOnEnd = OnEnd_Destroy;
        m_useYMin = bfalse;
        m_useYMinLocal = btrue;
        m_yMin = 0.f;
        m_yMinBouncinessMin = 0.f;
        m_yMinBouncinessMax = 0.f;
        m_yMinMinRebounds = 0;
        m_yMinMaxRebounds = 0;
        m_yMinKillAfterMaxRebounds = btrue;
        m_bouncinessAffectsAngularSpeed = btrue;
        m_bouncinessAffectsXSpeed = btrue;
    }

    ///----------------------------------------------------------------------------//
    
    f32 ParticleGeneratorParameters::computeSumPhaseTime() const
    {
       f32 phaseSumTime = 0.0f;
       for (ITF_VECTOR<ParPhase>::const_iterator iter = m_phaseList.begin();iter!=m_phaseList.end();++iter)
           phaseSumTime+=(*iter).m_phaseTime;

       return phaseSumTime;
    }
    
    ///----------------------------------------------------------------------------//
    
    f32 ParticleGeneratorParameters::computeMaxSumPhaseTime() const
    {
       f32 phaseSumTime = 0.0f;
       for (ITF_VECTOR<ParPhase>::const_iterator iter = m_phaseList.begin();iter!=m_phaseList.end();++iter)
           phaseSumTime+=(*iter).m_phaseTime + f32_Abs((*iter).m_deltaphasetime);

       return phaseSumTime;
    }
    
    ///----------------------------------------------------------------------------//
    
    void setParticleFromPhaseInfo(ITF_Particle* _par, const ParPhase* _phase, f32 _lifeTimeMult)
    {
        if (_phase->m_blendToNextPhase)
            _par->m_flag |= PAR_F_BLENDPHASE;
        else
            _par->m_flag &= ~PAR_F_BLENDPHASE;

        // if phase have anim information
        if (_phase->m_animstretchtime)
            _par->m_flag |= PAR_F_ANIM_STRETCHTIME;
        else
            _par->m_flag &= ~PAR_F_ANIM_STRETCHTIME;

        if (_phase->m_animStart >= 0)
        {
            _par->m_startAnimIndex = _phase->m_animStart;
            _par->m_endAnimIndex = _phase->m_animEnd;
        }
        _par->m_animId = _phase->m_animId;

        _par->m_phaselifeTime = _phase->m_phaseTime + ITF_ParticleGenerator::randomLCG.getRandF(-_phase->m_deltaphasetime, _phase->m_deltaphasetime);
        _par->m_phaselifeTime *= _lifeTimeMult;
    }

    ///----------------------------------------------------------------------------//

    ParticleGeneratorInstanceParameters::ParticleGeneratorInstanceParameters()
    {
        m_maxParticles			= 1;
        m_emitParticlesCount	= U32_INVALID;
        m_ForceEmitAtStart      = bfalse;
        m_defaultColor          = Color::white();

        m_pos					= Vec3d::Zero;
        m_posOffset             = Vec3d::Zero;
        m_pivot                 = Vec2d::Zero;
        m_velNorm               = 1.f;
        m_vel			        = Vec3d::Zero;
        m_velParamAngle	        = 0.f;
        m_velParamAngleDelta    = 0.f;
        m_grav					= Vec3d::Zero;
        m_acc					= Vec3d::Zero;
        m_velocityVar			= 1.0f;

        m_lifeTimeMultiplier    = 1.0f;

        m_useZAsDepth           = btrue;
        m_depth                 = 0.f;

        m_freq					= 0.05f;
        m_freqDelta             = 0.f;

        m_renderPrio            = 0.f;

        m_nbPhase               = 1;

        m_UVmodeFlag            = UVF_Default;
        m_uniformScale          = 0.f;

        m_randomizeDirection    = btrue;

        m_friction = 1.f;
        m_genMode = PARGEN_MODE_COMPLEX;
        m_genGenType = PARGEN_GEN_POINTS;
        m_behaviorOnEnd = OnEnd_Destroy;

        m_genAngMin = Angle(btrue, -180);
        m_genAngMax = Angle(btrue, 180);

        m_flipped   = bfalse;

        m_useMatrix = BOOL_false;
        m_scaleGenBox = BOOL_cond;
        m_useActorTranslation = bfalse;
        m_actorTranslationOffset = Vec2d::Zero;

        m_disableLight = bfalse;

        m_usePhasesColorAndSize = btrue;

        m_nSplit = 0;
        m_splitDelta = Angle(btrue, 0);
        m_amvID = U32_INVALID;

        m_genSize = 0.f;
        m_genSide = 0;
        m_genBezierStart = 0.f;
        m_genBezierEnd = 1.f;
        m_genBezierDensity = -1.f;

        m_is2d = bfalse;

        m_dieFadeTime = -1.f;
        m_emitterMaxLifeTime = -1.f;
    }

    void ParticleGeneratorInstanceParameters::initFromTemplate(const ParticleGeneratorParameters &_parameters)
    {
        m_defaultColor                  = _parameters.m_defaultColor;
        m_maxParticles			        = _parameters.m_maxParticles;
        m_emitParticlesCount	        = _parameters.m_emitParticlesCount;
        m_pos					        = _parameters.m_pos;
        m_posOffset                     = _parameters.m_posOffset;
        m_pivot                         = _parameters.m_pivot;
        m_velNorm                       = _parameters.m_velNorm;
        m_vel			                = _parameters.m_vel;
        m_velParamAngle	                = _parameters.m_velParamAngle;
        m_velParamAngleDelta            = _parameters.m_velParamAngleDelta;
        m_grav					        = _parameters.m_grav;				
        m_acc					        = _parameters.m_acc;
        m_velocityVar			        = _parameters.m_velocityVar;
        m_friction                      = _parameters.m_friction;
        m_depth                         = _parameters.m_depth;
        m_useZAsDepth                   = _parameters.m_useZAsDepth;
        m_freq					        = _parameters.m_freq;
        m_freqDelta                     = _parameters.m_freqDelta;
        m_ForceEmitAtStart              = _parameters.m_ForceEmitAtStart;
        m_initAngle                     = _parameters.m_initAngle;
        m_angleDelta                    = _parameters.m_angleDelta;
        m_angularSpeed                  = _parameters.m_angularSpeed;
        m_angularSpeedDelta             = _parameters.m_angularSpeedDelta;
        m_nbPhase                       = _parameters.m_nbPhase;
        m_phaseList                     = _parameters.m_phaseList;
        m_renderPrio                    = _parameters.m_renderPrio;
        m_genGenType                    = _parameters.m_genGenType;
        m_genMode                       = _parameters.m_genMode;
        m_behaviorOnEnd                 = _parameters.m_behaviorOnEnd;
        m_genBox                        = _parameters.m_genBox;
        m_genSize                       = _parameters.m_genSize;
        m_genSide                       = _parameters.m_genSide;
        m_genBezierStart                = _parameters.m_genBezierStart;
        m_genBezierEnd                  = _parameters.m_genBezierEnd;
        m_genBezierDensity              = _parameters.m_genBezierDensity;
        m_UVmodeFlag                    = _parameters.m_UVmodeFlag;
        m_uniformScale                  = _parameters.m_uniformScale;
        m_randomizeDirection            = _parameters.m_randomizeDirection;
        m_genAngMin                     = _parameters.m_genAngMin;
        m_genAngMax                     = _parameters.m_genAngMax;
        m_disableLight                  = _parameters.m_disableLight;
        m_useMatrix                     = _parameters.m_useMatrix;
        m_scaleGenBox                   = _parameters.m_scaleGenBox;
        m_usePhasesColorAndSize         = _parameters.m_usePhasesColorAndSize;
        m_useActorTranslation           = _parameters.m_useActorTranslation;
        m_actorTranslationOffset        = _parameters.m_actorTranslationOffset;
        m_dieFadeTime                   = _parameters.m_dieFadeTime;
        m_emitterMaxLifeTime            = _parameters.m_emitterMaxLifeTime;  
        m_nSplit                        = _parameters.m_nSplit;
        m_splitDelta                    = _parameters.m_splitDelta;
    }

    void ParticleGeneratorInstanceParameters::getDstBlend(ITF_Particle* _par, const ITF_ParticleGenerator* _gen) const
    {
        if(m_phaseList.size() == 0)
            return;

        /// Set blend with next phase in case there are an another one after.
        u32 nextPhase = _par->m_curPhase + 1;
        if (nextPhase >= m_nbPhase && _par->m_flag & PAR_F_LOOP)
            nextPhase = 0;

        if (nextPhase < m_nbPhase)
        {
            const ParPhase* phasenext = &m_phaseList[nextPhase];

            Color randomColor = Color::colorBlend(&phasenext->m_colorMin, &phasenext->m_colorMax,ITF_ParticleGenerator::randomLCG.getRandF(0.f, 1.f));
            _par->m_dstColor = _gen->applyGlobalColor(randomColor).getAsU32();

            _par->m_dstSize.x() = ITF_ParticleGenerator::randomLCG.getRandF(phasenext->m_sizeMin.x(), phasenext->m_sizeMax.x());
           
            if ( m_uniformScale )
                _par->m_dstSize.y() = _par->m_dstSize.x() * m_uniformScale;
            else
                _par->m_dstSize.y() = ITF_ParticleGenerator::randomLCG.getRandF(phasenext->m_sizeMin.y(), phasenext->m_sizeMax.y());
        }
        /// in case that its the last phase and blending mode is on, then blend with color 0 and size 0
        else if ( _par->m_flag & PAR_F_BLENDPHASE)
        {
            _par->m_dstColor = 0;
            _par->m_dstSize = Vec2d::Zero;
        }
        /// else, keep the size and color.
        else
        {
            _par->m_dstColor = _par->m_initColor;
            _par->m_dstSize = _par->m_initSize;
        }
    }

    ///----------------------------------------------------------------------------//

    void ParticleGeneratorInstanceParameters::initFromPhase(ITF_Particle* _par, f32 _time, const ITF_ParticleGenerator* _owner) const
    {
        _par->m_initTime = _time;
        if(m_phaseList.size() == 0)
        {
            _par->m_phaselifeTime = F32_INFINITY;
            return;
        }

        const ParPhase* phase = &m_phaseList[_par->m_curPhase];
        setParticleFromPhaseInfo(_par, phase, m_lifeTimeMultiplier);

        //if particle uses phases color and size data (bfalse for a color and size manual update)
        if( m_usePhasesColorAndSize )
        {
            _par->m_initSize.x() = ITF_ParticleGenerator::randomLCG.getRandF(phase->m_sizeMin.x(), phase->m_sizeMax.x());
        
            if ( m_uniformScale )
                _par->m_initSize.y() = _par->m_initSize.x() * m_uniformScale;
            else
                _par->m_initSize.y() = ITF_ParticleGenerator::randomLCG.getRandF(phase->m_sizeMin.y(), phase->m_sizeMax.y());

            Color randomColor = Color::Interpolate(phase->m_colorMin, phase->m_colorMax, ITF_ParticleGenerator::randomLCG.getRandF(0.f, 1.f));
            _par->m_initColor = _owner->applyGlobalColor(randomColor).getAsU32();

            getDstBlend(_par, _owner);

            _par->m_curColor = _par->m_initColor;
            _par->m_curSize = _par->m_initSize;
        }
    }

    ///----------------------------------------------------------------------------//

    void ParticleGeneratorInstanceParameters::initToNextPhase(ITF_Particle* _par, f32 _time, const ITF_ParticleGenerator* _owner) const
    {
        _par->m_initTime = _time;        
        if(m_phaseList.size() == 0)
        {
            _par->m_phaselifeTime = F32_INFINITY;
            return;
        }
        const ParPhase* phase = &m_phaseList[_par->m_curPhase];
        setParticleFromPhaseInfo(_par, phase, m_lifeTimeMultiplier);

        //if particle uses phases color and size data (bfalse for a color and size manual update)
        if( m_usePhasesColorAndSize )
        {
            //get color and scale from previous phase.
            _par->m_initSize = _par->m_dstSize;
            _par->m_initColor = _par->m_dstColor;
        
            getDstBlend(_par, _owner);

            _par->m_curColor = _par->m_initColor;
            _par->m_curSize = _par->m_initSize;
        }
    }


    ///----------------------------------------------------------------------------//

    void ParticleGeneratorInstanceParameters::setNumberPhase( u32 _nphase )
    {
        m_nbPhase = _nphase;

        m_phaseList.clear();
        m_phaseList.reserve(m_nbPhase);

        ParPhase newphase;

        for (u32 i=0;i< m_nbPhase;i++)
            m_phaseList.push_back(newphase);
    }

    ///----------------------------------------------------------------------------//

    void ParticleGeneratorInstanceParameters::setVelocity( const Vec3d& _vel )
    {
        if(m_velParamAngle)
        {
            f32 angle = f32_ACosOpt(_vel.x());
            angle = f32_Sel(_vel.y(), angle, -angle);
            angle += (m_velParamAngle + ITF_ParticleGenerator::randomLCG.getRandF(-m_velParamAngleDelta, m_velParamAngleDelta)) * MTH_DEGTORAD;
            Vec2d cs;
            Vec2d::CosSin(&cs, angle);
            m_vel.setFromVec2d(cs, &_vel.z());
        }
        else
            m_vel = _vel;
    }
} // namespace ITF


