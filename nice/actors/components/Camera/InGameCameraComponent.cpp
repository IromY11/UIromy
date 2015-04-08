#include "precompiled_engine.h"

#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/InGameCameraComponent.h"
#endif //_ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_
#include "gameplay/Managers/GameManager.h"

namespace ITF
{
    static const f32 ScreenHeightBase = 6.6274f; // 6.6 is height screen for 8 meters of depth offset

    IMPLEMENT_OBJECT_RTTI(InGameCameraComponent)
        BEGIN_SERIALIZATION_CHILD(InGameCameraComponent)
        END_SERIALIZATION()

        IMPLEMENT_OBJECT_RTTI(InGameCameraComponent_Template)
        BEGIN_SERIALIZATION_CHILD(InGameCameraComponent_Template)
        SERIALIZE_OBJECT("CM",m_CMInit);
    END_SERIALIZATION()

    InGameCameraComponent_Template::InGameCameraComponent_Template()
    {
    }

    InGameCameraComponent_Template::~InGameCameraComponent_Template()
    {
    }

    InGameCameraComponent::InGameCameraComponent()
        : Super()
        , m_depthCoeff(1.f)
        , m_screenRealLongBy2(Vec2d::One)
        , m_depth(13.f)
        , m_subjectPosZ(0.f)
        , m_cameraTargetAveragePos(Vec3d::Zero)   
		, m_cameraTargetAveragePosTrue(Vec2d::Zero)
        , m_screenTargetAverageAABB(Vec2d::Zero)
        , m_screenRealAABB(Vec2d::Zero)
        , m_cameraRealPos(Vec3d::Zero)
        , m_timeInterp(0.0f)
        , m_cameraModifier(NULL)        
        , m_constraintModifierOnEnter(bfalse)
        , m_constraintModifierOnExit(bfalse)
        , m_controllerActiveCount(0)
        , m_controllerActiveCountLast(0)
        , m_constraintModifier(NULL)
        , m_constraint(Vec3d::Zero)
        , m_constraintAnticipAABB(Vec2d::Zero)
        , m_constraintAABB(Vec2d::Zero)
        , m_screenConstraintAABB(Vec2d::Zero)
        , m_constraintDepthOffsetMax(F32_INFINITY)
        , m_constraintSecretAreaZ(0.f)
        , m_constraintSecretAreaZOld(0.f)
        , m_leftConstraintContinuity(0.f)
        , m_rightConstraintContinuity(0.f)
        , m_topConstraintContinuity(0.f)
        , m_bottomConstraintContinuity(0.f)
        , m_isConstraintContinuityAxeX(bfalse)
        , m_isConstraintContinuityAxeY(bfalse)
        , m_cameraTargetAverageDezoomDestWanted(0.f)        
        , m_cameraTargetAverageDezoomCur(0.f)
        , m_borderBlending(1.f)
        , m_visibilityDest(Vec2d::Zero)
        , m_visibilityCur(Vec2d::Zero)
        , m_visibilityOffsetCur(Vec2d::Zero)
        , m_visibilityOffsetDest(Vec2d::Zero)
        , m_isScreenLimitCrossed(bfalse)
        , m_resetDezoomTime(bfalse)
        , m_subjectZoneNeutral(Vec2d(0.2f,0.3f))
        , m_subjectUnregisterSpeed(0.f)
        , m_subjectRegisterSpeed(0.f)
        , m_switchModifier(bfalse)
        , m_subjectGlobalDepthMin(0.f)
        , m_subjectGlobalAABB(Vec2d::Zero)
        , m_mainController(NULL)
        , m_isLeaderController(bfalse)
        , m_constraintMatchViewX(bfalse)
        , m_constraintMatchViewY(bfalse)
        , m_visibilitySecretAreaZCur(0.f)
        , m_visibilitySecretAreaZTime(0.f)
        , m_visibilitySecretAreaZLast(0.f)
        , m_rotationSpeed(0.1f)
        , m_rotationDir(Vec3d::YAxis)
        , m_flagMirror(0)
        , m_mainActiveControllerWithoutJoyMoveTime(0.f)
        , m_useVisibilityFromMainActiveController(btrue)  
        , m_resetVisibilityFromMainActiveController(bfalse)
		, m_curInertia(0.0f)
		, m_speedInertia(0.0f)
		, m_useSpatial(false)
		, m_wasSpatial(false)
    {
        m_curveLogistic.m_xMax = 10.f; // to 1
        m_curveLogistic.m_xMin = -10.f; // to 0
        m_controllers.reserve(10);    

		m_cameraSpeedParam.setToZero();
    }

    InGameCameraComponent::~InGameCameraComponent()
    {
    }

    void InGameCameraComponent::onStartDestroy( bbool _hotReload )
{
        Super::onStartDestroy(_hotReload);

        m_CMCur.clear();
        m_CMDestCur.clear();
        m_CMDestFinal.clear();
        m_controllers.clear();
    }

    void InGameCameraComponent::onFinalizeLoad() 
    {
        Super::onFinalizeLoad();
       
        ITF_WARNING_CATEGORY(LD, m_actor, !m_actor->isSerializable(), "SERIALiZED CAMERA! most likely the camera was copy-pasted and saved in the scene, find it and remove it.");

        const CamModifier_Template& cmInit = getTemplate()->getCMInit();
        m_CMCur.init( &cmInit );
        m_CMDestCur.init( &cmInit );
        m_CMDestFinal.init( &cmInit );

        if (getCameraControllerManager())
        {
            getCameraControllerManager()->setDefaultDepth(m_CMCur.m_cameraLookAtOffset.z());

            // register settings
            if ( m_CMCur.m_camModifierTemplate->m_subjectWeightRegisterDelay >0.f)
                m_subjectRegisterSpeed = 1.f /m_CMCur.m_camModifierTemplate->m_subjectWeightRegisterDelay;
            else
                m_subjectRegisterSpeed = F32_INFINITY;

            if ( m_CMCur.m_camModifierTemplate->m_subjectWeightUnregisterDelay >0.f)
                m_subjectUnregisterSpeed = 1.f /m_CMCur.m_camModifierTemplate->m_subjectWeightUnregisterDelay;
            else
                m_subjectUnregisterSpeed = F32_INFINITY;

            // visibility dest
            m_visibilityOffsetDest = m_CMCur.m_camModifierTemplate->m_subjectMainVisibilityHorizontal;

            const u32 playerListCount = getCameraControllerManager()->getSubjectPlayerListCount();
            if ( playerListCount )
            {
                CameraControllerSubject subject;
                getCameraControllerManager()->getSubjectByIndex( 0, subject);

                const Actor* player = (Actor*) GETOBJECT(subject.m_ref);
                if ( player )
                {
                    teleport(player->getPos());
                }

                addControllers();

                for( u32 i=0; i<m_controllers.size(); i++)
                {
                    Controller& controller = m_controllers[i];

                    controller.m_weightRegisterSpeed = F32_INFINITY;
                }
            }

            m_actor->setAABB(AABB(m_actor->get2DPos() - Vec2d::One, m_actor->get2DPos() - Vec2d::One));     
        }
    }

    void InGameCameraComponent::setDepthCoeff()
    {
        const f32 screenHeight = Max( m_screenRealAABB.getHeight(), 1.f);

        m_depthCoeff = ScreenHeightBase /screenHeight;                
    }

    void InGameCameraComponent::setZoneNeutral()
    {
        static Vec2d zoneNeutralFromDepth(Vec2d::Zero);

        // from depth
        const f32 deltaZoneDepth = m_CMCur.m_camModifierTemplate->m_zoneScaleDepthMax -m_CMCur.m_camModifierTemplate->m_zoneScaleDepthMin;
        if ( deltaZoneDepth >0.f )
        {
            const f32 coeff = Clamp( (m_depth -m_CMCur.m_camModifierTemplate->m_zoneScaleDepthMin) /deltaZoneDepth, 0.f, 1.f);
            zoneNeutralFromDepth = m_CMCur.m_camModifierTemplate->m_zoneScaleAtDepthMin +( m_CMCur.m_camModifierTemplate->m_zoneScaleAtDepthMax -m_CMCur.m_camModifierTemplate->m_zoneScaleAtDepthMin) *coeff;  
        }

        Vec2d zoneNeutralDest = zoneNeutralFromDepth;
        // from modifiers
        if ( m_cameraModifier )
        {
            const Vec2d& zoneNeutralFromModifier = m_cameraModifier->getZoneNeutral();

            if ( zoneNeutralFromModifier.x() )                    
                zoneNeutralDest.x() = zoneNeutralFromModifier.x();                    
            
            if ( zoneNeutralFromModifier.y() )
                zoneNeutralDest.y() = zoneNeutralFromModifier.y();           
        }
        
        if ( getCameraControllerManager()->isInGameCameraJustReseted() )
        {
            m_subjectZoneNeutral = zoneNeutralDest;
        }
        else
        if ( getCameraControllerManager()->m_setZoneNeutralDest )
        {
            m_subjectZoneNeutral = zoneNeutralDest;
            getCameraControllerManager()->m_setZoneNeutralDest = bfalse;
        }
        else
        {
            m_subjectZoneNeutral += ( zoneNeutralDest -m_subjectZoneNeutral) *m_CMCur.m_camModifierTemplate->m_zoneScaleSpeed;
        }
    }

    void InGameCameraComponent::updateInit()
    {
        m_isScreenLimitCrossed = bfalse;
        m_resetDezoomTime = bfalse;
    }

    void InGameCameraComponent::updateDrift()
    {
        updateModifiers();

        if ( m_controllerActiveCountLast >0 )
        {
            m_controllers.clear();
        }
    }

	// blend from two other blended parameters.
	void InGameCameraComponent::BlendedParam::blend(const BlendedParam &_source, const BlendedParam &_dest, f32 _alpha)
	{
        f32 dst_coef = (1.f - f32_Cos(_alpha * MTH_PI)) * 0.5f;
        f32 src_coef = 1.0f - dst_coef;
		m_offset = _source.m_offset * src_coef + _dest.m_offset * dst_coef;
		m_focale = _source.m_focale * src_coef + _dest.m_focale * dst_coef;
		m_depth = _source.m_depth * src_coef + _dest.m_depth * dst_coef;
		m_horizontalVersusVertical = _source.m_horizontalVersusVertical * src_coef + _dest.m_horizontalVersusVertical * dst_coef;
		m_offsetHVS = _source.m_offsetHVS * src_coef + _dest.m_offsetHVS * dst_coef;
	}

	// Compute smooth movement iteration.
	// _CurPos     : current position, updated by the function.
	// _Speed      : current speed, updated by the function.
	// _Dest       : target position to reach.
	// _SmoothFactor : curve parameter. More this value is big, smoother will be the curve and biggest will be the delay.
	// _DeltaTime  : iteration length time.
	void smoothMove( f32 &_CurPos, f32 &_Speed, const f32 &_Dest, f32 _SmoothFactor, f32 _DeltaTime )
	{
		f32 maxSpeed = _SmoothFactor * 2.0f;
		f32 maxAccel = f32_Sqrt(_SmoothFactor) * 2.0f;
		f32 Change, speedTarget, speedGap;
		static float speedRatio = 0.1f;
  
		Change = _Dest - _CurPos;
		speedTarget = Change * speedRatio;
		f32 n = f32_Abs(speedTarget);
		if ( n > maxSpeed )
		{
			speedTarget *= maxSpeed / n;
		}

		speedGap = (speedTarget - _Speed);
		n = f32_Abs(speedGap);
		if ( n > maxAccel * _DeltaTime )
		{
			speedGap *= (maxAccel * _DeltaTime) / n;
		}

		_Speed += speedGap;

		n = f32_Abs(Change);
		f32 s = f32_Abs(_Speed);
		if ( s > n )
		{
			_Speed  *= n / s;
		}
		_CurPos += _Speed;
	}
	void smoothMove( Vec3d &_CurPos, Vec3d &_Speed, const Vec3d &_Dest, f32 _SmoothFactor, f32 _DeltaTime )
	{
		f32 maxSpeed = _SmoothFactor * 2.0f;
		f32 maxAccel = f32_Sqrt(_SmoothFactor) * 2.0f;
		Vec3d Change, speedTarget, speedGap;
		float speedRatio = f32_Clamp(0.1f * _SmoothFactor, 0.0f, 1.0f);
  
		Change = _Dest - _CurPos;
		speedTarget = Change * speedRatio;
		f32 n = speedTarget.norm();
		if ( n > maxSpeed )
		{
			speedTarget *= maxSpeed / n;
		}

		speedGap = (speedTarget - _Speed);
		n = speedGap.norm();
		if ( n > maxAccel * _DeltaTime )
		{
			speedGap *= (maxAccel * _DeltaTime) / n;
		}

		_Speed += speedGap;

		n = Change.norm();
		f32 s = _Speed.norm();
		if ( s > n )
		{
			_Speed  *= n / s;
		}
		_CurPos += _Speed;
	}
	void smoothMove( Vec2d &_CurPos, Vec2d &_Speed, const Vec2d &_Dest, f32 _SmoothFactor, f32 _DeltaTime )
	{
		f32 maxSpeed = _SmoothFactor * 2.0f;
		f32 maxAccel = f32_Sqrt(_SmoothFactor) * 2.0f;
		Vec2d Change, speedTarget, speedGap;
		static float speedRatio = 0.1f;
  
		Change = _Dest - _CurPos;
		speedTarget = Change * speedRatio;
		f32 n = speedTarget.norm();
		if ( n > maxSpeed )
		{
			speedTarget *= maxSpeed / n;
		}

		speedGap = (speedTarget - _Speed);
		n = speedGap.norm();
		if ( n > maxAccel * _DeltaTime )
		{
			speedGap *= (maxAccel * _DeltaTime) / n;
		}

		_Speed += speedGap;

		n = Change.norm();
		f32 s = _Speed.norm();
		if ( s > n )
		{
			_Speed  *= n / s;
		}
		_CurPos += _Speed;
	}

	// blend parameters to a destination.
	void InGameCameraComponent::BlendedParam::blendSmooth(BlendedParam &_dest, BlendedParam &_speed, f32 _SmoothFactor, f32 _dt)
	{
		smoothMove(m_finalPos, _speed.m_finalPos, _dest.m_finalPos, _SmoothFactor, _dt);
		smoothMove(m_focale, _speed.m_focale, _dest.m_focale, _SmoothFactor * 0.5f, _dt);
		smoothMove(m_depth, _speed.m_depth, _dest.m_depth, _SmoothFactor, _dt);
		smoothMove(m_horizontalVersusVertical, _speed.m_horizontalVersusVertical, _dest.m_horizontalVersusVertical, _SmoothFactor, _dt);
		smoothMove(m_offsetHVS, _speed.m_offsetHVS, _dest.m_offsetHVS, _SmoothFactor, _dt);
	}

	// Reset content to zero.
	void InGameCameraComponent::BlendedParam::setToZero()
	{
		m_finalPos = Vec3d::Zero;
		m_offset = Vec3d::Zero;
		m_focale = 0.0f;
		m_depth = 0.0f;
		m_horizontalVersusVertical = 0.0f;
		m_offsetHVS.set(0.0f, 0.0f);
	}

    void InGameCameraComponent::onManagerUpdate(f32 _deltaTime)
    {
        if ( getCameraControllerManager()->updateCamControllers_SubjectList() )
        {
            removeControllers();
            addControllers(btrue);
        }

        updateInit();
        setControllersSubject();
		m_borderBlending = 1.0f;
		m_wasSpatial = m_useSpatial;
		m_useSpatial = false;

        if ( m_controllerActiveCount > 0 )
        {            
            setModifiers();
            setZoneNeutral();

            if ( setMainController(_deltaTime) )
            {
				setBorderBlending();
                blendingModifiers();
                setControllersPosition();        
                setControllersWeight(_deltaTime);
                setCameraTargetAverage();
            }           
        }
        else
        {
            updateDrift();
        }

		setCameraReal();

		finalBlending(_deltaTime);

        // update actor
        m_actor->setPos(m_cameraFinalParam.m_finalPos);       
    }

	// David : My blending done at the end when we know the target to reach.
	void InGameCameraComponent::finalBlending(f32 _deltaTime)
	{
		if ( m_switchModifier )
		{
			m_cameraLastParam = m_cameraCurParam;
			if ( m_wasSpatial )
				m_timeInterp = 1.0f;
			else
				m_timeInterp = 0.0f;
		}

//		Vec3d curOffset(m_cameraRealPos.x() - m_cameraTargetAveragePosTrue.x(), m_cameraRealPos.y() - m_cameraTargetAveragePosTrue.y(), m_cameraRealPos.z());

  //      if ( m_mainController && m_cameraModifier && m_cameraModifier->isBorderBlending())
  //      {
  //          setBorderBlending();
		//}
		//else
		//	m_borderBlending = 1.0f;

/*		if ( m_borderBlending >= 1.0f )
		{
			if ( m_useSpatial )
			{
				m_cameraLastParam = m_cameraCurParam;
				m_timeInterp = 0.0f;
				m_useSpatial = false;
			}
		}
		else if ( m_switchModifier )
		{
			if ( m_borderBlending < 0.1f )
				m_useSpatial = true;
		}*/

		BlendedParam cameraFinalParam;
		getBlendedParamFrom( m_cameraModifier, cameraFinalParam );
		f32 timeLenght = 2.0f;
		f32 moveInertia = 0.0f;
		if ( m_cameraModifier )
		{
			timeLenght = m_cameraModifier->getTransitionTime();
			moveInertia = m_cameraModifier->getMoveInertia();
		}
		smoothMove(m_curInertia, m_speedInertia, moveInertia, 1.0f, _deltaTime);

		static f32 smoothIntensity = 50.0f;
        if ( getCameraControllerManager()->isInGameCameraJustReseted() )
        {
			m_cameraCurParam = cameraFinalParam;
			m_timeInterp = 1.0f;
		}
        else if ( m_useSpatial )
        {
			m_cameraCurParam.blend(m_cameraOtherSpatialParam, cameraFinalParam, Clamp(m_borderBlending, 0.0f, 1.0f));
			if ( f32_IsNull(m_cameraCurParam.m_offset.x() - 0.48138934f, 0.05f ) )
			{
				int a = 0;
				a++;
			}
        }
		else
		{
			if ( timeLenght > 0.0f )
				m_timeInterp += _deltaTime / timeLenght;
			else
				m_timeInterp = 1.0f;
			if ( m_timeInterp < 1.0f )
				m_cameraCurParam.blend(m_cameraLastParam, cameraFinalParam, Clamp(m_timeInterp, 0.0f, 1.0f));
			else
				m_cameraCurParam = cameraFinalParam;
		}

		m_cameraCurParam.m_finalPos = m_cameraTargetAveragePosTrue.to3d(0.0f) + m_cameraCurParam.m_offset;

		if ( getCameraControllerManager()->isInGameCameraJustReseted() )
		{
			m_cameraFinalParam = m_cameraCurParam;
			m_cameraSpeedParam.setToZero();
		}
		else
		{
			if ( m_curInertia > F32_EPSILON )
				m_cameraFinalParam.blendSmooth(m_cameraCurParam, m_cameraSpeedParam, 1.0f / m_curInertia, _deltaTime);
			else
				m_cameraFinalParam = m_cameraCurParam;
		}
	}

    void InGameCameraComponent::teleport(const Vec3d& _pos)
    {
        Super::teleport(_pos);
        
		m_cameraSpeedParam.setToZero();
        removeControllers();
        addControllers();

        resetConstraints();
        const CamModifier_Template& cmInit = getTemplate()->getCMInit();

        const Vec2d pos2D(_pos.x(), _pos.y());

        // modifiers
        m_constraintModifierOnEnter = bfalse;
        m_constraintModifierOnExit = bfalse;
        m_cameraModifier = NULL;        
        m_borderBlending = 1.f;
        m_subjectGlobalAABB.setMinAndMax(pos2D);
        m_subjectGlobalDepthMin = _pos.z();
        setModifiers();
        
        //  remove dead controllers 
        ITF_VECTOR<Controller>::iterator it = m_controllers.begin();
        while( it != m_controllers.end() )
        {
            Controller& controller = *it;
            
            if ( !controller.isActive() )
            {
                it = m_controllers.erase(it);
            }
            else
            {
                it++;
            }
        }

        const u32 subjectCount = m_controllers.size();

        Vec3d posTeleport(_pos);
        
        // if no player registered, try to anticipate cam position in constraints        
        if ( subjectCount == 0 )
        {
            CameraData camData;
            camData.m_pos = posTeleport;

            getCameraControllerManager()->predictCameraDataAfterTeleport( camData );

            // apply predict position
            posTeleport = camData.m_pos;
        }
        else
        {
            posTeleport.z() += cmInit.m_cameraLookAtOffset.z();
        }

#ifdef ITF_SUPPORT_CHEAT
        posTeleport.z() += getCameraControllerManager()->getCheatZoom();
#endif //ITF_SUPPORT_CHEAT
        
        m_depth = f32_Clamp( posTeleport.z() - _pos.z(), 0.001f, 1000.0f);
        
        // controllers
        m_mainActiveControllerWithoutJoyMoveTime = 0.f;
        for ( u32 i=0; i<subjectCount; i++ )
        {
            Controller& controller = m_controllers[i];

            controller.teleport(_pos);           
            controller.m_weight = 1.f;
            controller.m_weightRegisterSpeed = F32_INFINITY;
            controller.m_isMainController = btrue;
        }

        m_subjectPosZ = _pos.z();               
        m_screenTargetAverageAABB.moveCenter(pos2D);

        m_cameraTargetAveragePos = posTeleport;

        m_cameraTargetAverageDezoomDestWanted = 0.f;              
        m_cameraTargetAverageDezoomCur = 0.f;        

        m_cameraRealPos = posTeleport;
		m_cameraCurParam.m_offset.set(0.0f,0.0f,0.0f);
		m_cameraLastParam = m_cameraCurParam;
		m_timeInterp = 1.0f;
        m_screenRealAABB.moveCenter(pos2D);              
        setScreenReal();

        m_visibilityCur = Vec2d::Zero;
        m_visibilityDest = Vec2d::Zero;
        m_visibilityOffsetDest = m_CMCur.m_camModifierTemplate->m_subjectMainVisibilityHorizontal;

        m_visibilitySecretAreaZCur = 0.f;
        m_visibilitySecretAreaZLast = 0.f;
        m_visibilitySecretAreaZTime = 0.f;

        m_rotationDir = Vec3d::YAxis;
        m_rotationAngle.SetRadians(0.f);
    }        

    void InGameCameraComponent::projectAABB(AABB& _aabb, f32 _originalZ, f32 _newZ)
    {
        if ( m_cameraRealPos.z() <= _newZ || _newZ <= _originalZ )
            return ;

        Vec3d aabbMin( _aabb.getMin().x(), _aabb.getMin().y(), _originalZ);
        Vec3d aabbMax( _aabb.getMax().x(), _aabb.getMax().y(), _originalZ);

        f32 dummy;       
        Plane objectPlane(Vec3d(0,0,1), _newZ);

        if(objectPlane.testLineIntersection(m_cameraRealPos, aabbMin-m_cameraRealPos, aabbMin, dummy)
            && objectPlane.testLineIntersection(m_cameraRealPos, aabbMax-m_cameraRealPos, aabbMax, dummy)
            )
        {
            _aabb.setMin(aabbMin.truncateTo2D());
            _aabb.setMax(aabbMax.truncateTo2D());
           
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /// Screen Target Average
    void InGameCameraComponent::setScreenTargetAverage()
    {
        const f32 depth = m_cameraTargetAveragePos.z() -m_subjectPosZ/* -m_constraintSecretAreaZ*/;
        Vec2d screenTargetLongBy2;
        screenTargetLongBy2.y() = f32_Abs( tan(m_CMCur.m_focale *0.5f) *depth);
        screenTargetLongBy2.x() = screenTargetLongBy2.y() *getCameraControllerManager()->m_screenRatio;

        m_screenTargetAverageAABB.setMin( m_cameraTargetAveragePos.truncateTo2D() -screenTargetLongBy2 );
        m_screenTargetAverageAABB.setMax( m_cameraTargetAveragePos.truncateTo2D() +screenTargetLongBy2 );
    }

    //////////////////////////////////////////////////////////////////////////
    /// Screen Real
    void InGameCameraComponent::setScreenReal()
    {
		if (m_constraintModifier)
			m_depth = f32_Clamp( m_cameraTargetAveragePos.z() - m_constraintModifier->GetActor()->getPos().z(), 0.001f, 1000.0f);
		else
			m_depth = f32_Clamp( m_cameraTargetAveragePos.z(), 0.001f, 1000.0f);

		f32 wantedScreenRatio = getCameraControllerManager()->getWantedScreenRatio();
		f32 screenRatio = getCameraControllerManager()->getScreenRatio();
		f32 hvs = getCurHVS();
		Vec2d offsetHVS = getCurOffsetHVS();

		Vec2d screenRealLongBy2H, screenRealLongBy2V;
		screenRealLongBy2H.y() = f32_Abs( tan(m_CMCur.m_focale *0.5f) *m_depth);
		screenRealLongBy2H.x() = screenRealLongBy2H.y() * screenRatio;

		screenRealLongBy2V.x() = screenRealLongBy2H.y() * wantedScreenRatio;
		screenRealLongBy2V.y() = screenRealLongBy2V.x() / screenRatio;

		m_screenRealLongBy2.Lerp(screenRealLongBy2H, screenRealLongBy2V, hvs);

		//f32 ox = (m_screenRealLongBy2.x() - screenRealLongBy2H.x()) * offsetHVS.x() * 5.0f;
		//f32 oy = (m_screenRealLongBy2.y() - screenRealLongBy2H.y()) * offsetHVS.y() * 5.0f;
		//Vec2d targetPos(m_cameraTargetAveragePos.x() + ox, m_cameraTargetAveragePos.y() + oy);  offset seems ognored later
		Vec2d targetPos(m_cameraTargetAveragePos.x(), m_cameraTargetAveragePos.y());
		m_screenRealAABB.setMin( targetPos -m_screenRealLongBy2 );
		m_screenRealAABB.setMax( targetPos +m_screenRealLongBy2 );
    }

    //////////////////////////////////////////////////////////////////////////
    /// Camera Real
    void InGameCameraComponent::setCameraReal()
    {
        setScreenReal();
        updateConstraints();
       
        m_cameraRealPos = m_cameraTargetAveragePos +m_constraint;
        
        setDepthCoeff();        


        // test WIP
        m_flagMirror = 0;

        Angle rotationDestAngle;
        if ( m_cameraModifier )
        {
			m_UpDnAngle = m_cameraModifier->getCM().m_UpDnAngle;
			rotationDestAngle = m_cameraModifier->getCM().m_rotationAngle;
            m_rotationSpeed = f32_Clamp( m_cameraModifier->getCM().m_rotationSpeed, 0.f, 1.f );

            // flip cam
            switch ( m_cameraModifier->getCM().m_flipView )
            {
            case CameraFlip_Both:
                m_flagMirror |= CAMERAFLAG_MIRROR_X;
                m_flagMirror |= CAMERAFLAG_MIRROR_Y;
                break;

            case CameraFlip_X:
                m_flagMirror |= CAMERAFLAG_MIRROR_X;
                break;

            case CameraFlip_Y:
                m_flagMirror |= CAMERAFLAG_MIRROR_Y;
                break;

            default:                
                break;     
            }
        }

        if ( f32_Abs( rotationDestAngle.ToRadians() ) > MTH_EPSILON || f32_Abs( m_rotationAngle.ToRadians() ) > MTH_EPSILON )
        {
            f32 shortestDeltaAngle = getShortestAngleDelta( m_rotationAngle.ToRadians(), rotationDestAngle.ToRadians() ) * m_rotationSpeed;

            m_rotationAngle.SetRadians( canonizeAngle( m_rotationAngle.ToRadians() + shortestDeltaAngle ) );


            Vec2d rot2D( m_rotationDir.truncateTo2D() );
            rot2D = rot2D.Rotate( shortestDeltaAngle );

            m_rotationDir.x() = rot2D.x();
            m_rotationDir.y() = rot2D.y();
        }
        else
        {
            m_rotationDir = Vec3d::YAxis;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /// Camera Target Average
    void InGameCameraComponent::setCameraTargetAverage()
    {        
        Vec2d pos(Vec2d::Zero);
        f32 weightTotal(0.f);

        const u32 controllersListCount = m_controllers.size();
        for ( u32 i=0; i< controllersListCount; i++ )
        {
            Controller& controller = m_controllers[i];

            pos += controller.m_cameraTargetPos.truncateTo2D() *controller.m_weight;            
            weightTotal += controller.m_weight;
        }

        if ( weightTotal > 0.f )
        {
            const f32 coeffPos = 1.f /weightTotal;
            m_cameraTargetAveragePos.x() = pos.x() *coeffPos;
            m_cameraTargetAveragePos.y() = pos.y() *coeffPos;
        }

        updateVisibility();
        setDezoomDest();
        setDezoomCur();
        setVisibilityOffset();

        m_cameraTargetAveragePos.x() += m_visibilityCur.x();
        m_cameraTargetAveragePos.y() += m_visibilityCur.y();
        m_cameraTargetAveragePos.z() = m_mainController->m_cameraTargetPos.z() +m_cameraTargetAverageDezoomCur;

        setScreenTargetAverage();
    }

    void InGameCameraComponent::setScreenAABBLongBy2( Vec2d& _screenLongBy2, f32 _depth )
    {
        _screenLongBy2.y() = f32_Abs( tan(m_CMCur.m_focale *0.5f) *_depth);
        _screenLongBy2.x() = _screenLongBy2.y() *getCameraControllerManager()->m_screenRatio;
    }

    void InGameCameraComponent::setScreenAABB( AABB& _aabb, const Vec2d& _pos, f32 _depth)
    {
        Vec2d screenLongBy2;
        setScreenAABBLongBy2( screenLongBy2, _depth );

        _aabb.setMin( _pos -screenLongBy2 );
        _aabb.setMax( _pos +screenLongBy2 );
    }

    void InGameCameraComponent::updateVisibility()
    {
        // set visibility aabb and secret area Z
        if ( m_switchModifier )
        {
            if ( f32_Abs(m_CMDestFinal.m_cameraModifierDirectionNormalized.y()) > MTH_SQRT2BY2)
                m_visibilityOffsetDest = m_CMCur.m_camModifierTemplate->m_subjectMainVisibilityVertical;
            else
                m_visibilityOffsetDest = m_CMCur.m_camModifierTemplate->m_subjectMainVisibilityHorizontal;

            m_visibilitySecretAreaZLast = m_visibilitySecretAreaZCur;

            if ( getCameraControllerManager()->isInGameCameraJustReseted())
            {
                m_visibilitySecretAreaZTime = 1.f;
            }
            else
            {
                m_visibilitySecretAreaZTime = 0.f;
            }
        }

        // update visibility from secret area
/*        if ( m_visibilitySecretAreaZTime < 1.f )
        {
            m_visibilitySecretAreaZTime += _dt * 0.5f;
            m_visibilitySecretAreaZCur = m_visibilitySecretAreaZLast +( m_constraintSecretAreaZ -m_visibilitySecretAreaZLast)*m_curveLogistic.getValue(m_visibilitySecretAreaZTime);
        }
        else
        {*/
            m_visibilitySecretAreaZCur = m_constraintSecretAreaZ;
//        }
        

        // set screen min aabb
        setScreenAABB( m_visibilityScreenMinAABB, m_cameraTargetAveragePos.truncateTo2D(), m_CMCur.m_cameraLookAtOffset.z());

        // set screen max aabb
        Vec2d screenMaxLongBy2;
        setScreenAABBLongBy2( screenMaxLongBy2, m_CMCur.m_cameraLookAtOffsetMaxInMulti.z() +m_visibilitySecretAreaZCur);
        m_visibilityScreenMaxAABB.setMin( m_cameraTargetAveragePos.truncateTo2D() -screenMaxLongBy2 );
        m_visibilityScreenMaxAABB.setMax( m_cameraTargetAveragePos.truncateTo2D() +screenMaxLongBy2 );

        Vec2d offsetWanted = m_visibilityOffsetDest +m_subjectZoneNeutral;
        offsetWanted.x() = Min( screenMaxLongBy2.x() -offsetWanted.x()*0.5f, offsetWanted.x());
        offsetWanted.y() = Min( screenMaxLongBy2.y() -offsetWanted.y()*0.5f, offsetWanted.y());

        m_visibilityOffsetCur = offsetWanted;

        m_visibilityAABB.setMin( m_mainController->m_cameraTargetPos.truncateTo2D() -m_visibilityOffsetCur);
        m_visibilityAABB.setMax( m_mainController->m_cameraTargetPos.truncateTo2D() +m_visibilityOffsetCur);
    }

    Vec2d InGameCameraComponent::getVisibilityOffset()
    {
        if ( !m_CMCur.m_isMainSubject || !m_useVisibilityFromMainActiveController )
            return Vec2d::Zero;

        Vec2d offsetWanted(Vec2d::Zero);
        const Vec2d anticipDelta( m_visibilityAABB.getWidth()*0.25f, m_visibilityAABB.getHeight()*0.25f);
        const AABB anticipAABB( m_visibilityScreenMaxAABB.getMin() +anticipDelta, m_visibilityScreenMaxAABB.getMax() -anticipDelta);

        // axe X
        if ( m_visibilityAABB.getMin().x() < anticipAABB.getMin().x() )
        {
            f32 delta = anticipAABB.getMin().x() -m_visibilityAABB.getMin().x();            

            if ( delta <= anticipDelta.x() *2.f )   
            {
                f32 coeff = delta /(anticipDelta.x() *4.f); 
                offsetWanted.x() = -delta *coeff;
            }
            else
            {
                offsetWanted.x() = m_visibilityAABB.getMin().x() -m_visibilityScreenMaxAABB.getMin().x();
            }               
        }
        else if ( m_visibilityAABB.getMax().x() > anticipAABB.getMax().x() )
        {
            f32 delta = m_visibilityAABB.getMax().x() -anticipAABB.getMax().x();

            if ( delta <= anticipDelta.x() *2.f )
            {
                f32 coeff = delta /(anticipDelta.x() *4.f);
                offsetWanted.x() = delta *coeff;
            }
            else
            {
                offsetWanted.x() = m_visibilityAABB.getMax().x() -m_visibilityScreenMaxAABB.getMax().x();
            }            
        }

        // axe Y
        if ( m_visibilityAABB.getMin().y() < anticipAABB.getMin().y() )
        {
            f32 delta = anticipAABB.getMin().y() -m_visibilityAABB.getMin().y();            

            if ( delta <= anticipDelta.y() *2.f )   
            {
                f32 coeff = Min( delta /(anticipDelta.y() *4.f), 0.5f); 
                offsetWanted.y() = -delta *coeff;
            }
            else
            {
                offsetWanted.y() = m_visibilityAABB.getMin().y() -m_visibilityScreenMaxAABB.getMin().y();
            }               
        }
        else if ( m_visibilityAABB.getMax().y() > anticipAABB.getMax().y() )
        {
            f32 delta = m_visibilityAABB.getMax().y() -anticipAABB.getMax().y();

            if ( delta <= anticipDelta.y() *2.f )
            {
                f32 coeff = delta /(anticipDelta.y() *4.f);
                offsetWanted.y() = delta *coeff;
            }
            else
            {
                offsetWanted.y() = m_visibilityAABB.getMax().y() -m_visibilityScreenMaxAABB.getMax().y();
            }            
        }

        return offsetWanted;
    }

    void InGameCameraComponent::setVisibilityOffset()
    {       
        m_visibilityCur = getVisibilityOffset();
    }

    void InGameCameraComponent::setDezoomDest()
    {
        m_cameraTargetAverageDezoomDestWanted = 0.f;

		//m_constraintSecretAreaZ = ( m_CMCur.m_cameraLookAtOffsetMaxInMulti.z() * ( GAMEMANAGER->getCameraZoom() - 1.0f));
        const f32 deltaDezoom = m_CMCur.m_cameraLookAtOffsetMaxInMulti.z() +m_constraintSecretAreaZ - m_CMCur.m_cameraLookAtOffset.z();
/*
        if ( deltaDezoom <= 0.f)  && !GAMEMANAGER->getCameraZoom())     
        {
            if ( GAMEMANAGER->getCameraZoom())
				m_cameraTargetAverageDezoomDestWanted = (m_CMCur.m_cameraLookAtOffsetMaxInMulti.z() +m_constraintSecretAreaZ) * ( GAMEMANAGER->getCameraZoom());
            return;        
        }*/

#ifdef ITF_SUPPORT_CHEAT
        if( getCameraControllerManager()->isCheatDezoomMax() )
        {
            m_cameraTargetAverageDezoomDestWanted = deltaDezoom; // *= ( GAMEMANAGER->getCameraZoom()); // [0.9f -f 1.1f]
            return;
        }
#endif //ITF_SUPPORT_CHEAT

        const u32 controllersListCount = m_controllers.size();
        if ( controllersListCount == 1 ) // if only one player, we won't dezoom from screen visibility
            return;

        f32 ratioX = 0.f;    
        f32 ratioY = 0.f;
        f32 deltaScreenLongX = ( m_visibilityScreenMaxAABB.getWidth() -m_visibilityScreenMinAABB.getWidth() )*0.5f;        
        f32 deltaScreenLongY = ( m_visibilityScreenMaxAABB.getHeight() -m_visibilityScreenMinAABB.getHeight() )*0.5f;

        for ( u32 i=0; i< controllersListCount; i++ )
        {
            const Controller& controller = m_controllers[i];

            if ( !controller.isActive() && !controller.m_isPaused )
                continue;

            const Vec2d offsetZoomCurAABB = m_visibilityOffsetCur *controller.m_weight;
            const AABB subjectZoomAABB(controller.m_cameraTargetPos.truncateTo2D() -offsetZoomCurAABB, controller.m_cameraTargetPos.truncateTo2D() +offsetZoomCurAABB);            

// #ifdef DEVELOPER_JAY_CAMERA
//             GFX_ADAPTER->drawDBGAABB(subjectZoomAABB,Color::white());
// #endif

            // axe X
            if( deltaScreenLongX >0.f )
            {
                f32 deltaVisibilityX = Max( 0.f, subjectZoomAABB.getMax().x() -m_visibilityScreenMinAABB.getMax().x());
                deltaVisibilityX = Max( deltaVisibilityX, m_visibilityScreenMinAABB.getMin().x() -subjectZoomAABB.getMin().x());

                if ( deltaVisibilityX >0.f )
                    ratioX = Max( deltaVisibilityX /deltaScreenLongX, ratioX);
            }

            // axe y
            if( deltaScreenLongY >0.f )
            {
                f32 deltaVisibilityY = Max( 0.f, subjectZoomAABB.getMax().y() -m_visibilityScreenMinAABB.getMax().y());
                deltaVisibilityY = Max( deltaVisibilityY, m_visibilityScreenMinAABB.getMin().y() -subjectZoomAABB.getMin().y());

                if ( deltaVisibilityY >0.f )
                    ratioY = Max( deltaVisibilityY /deltaScreenLongY, ratioY);
            }
        }

        ratioX = Min( ratioX, 1.f);
        ratioY = Min( ratioY, 1.f);

        m_cameraTargetAverageDezoomDestWanted = Max( ratioX, ratioY) *deltaDezoom;
    }

    void InGameCameraComponent::setDezoomCur()
    {
        m_cameraTargetAverageDezoomCur = m_cameraTargetAverageDezoomDestWanted; 
    }

    //////////////////////////////////////////////////////////////////////////
    /// Modifiers
    CameraModifierComponent* InGameCameraComponent::getCameraModifier(bbool _useConstraint /*=btrue*/) const
    {
        CameraModifierComponent* cameraModifier = m_cameraModifier;
        CameraModifierComponent* constraintModifier = m_constraintModifier;
        validateCamModifier(cameraModifier);
        validateCamModifier(constraintModifier);
        //updateModifiers(); // it breaks the virtual call because the function is not const anymore with this...

        if (_useConstraint && constraintModifier)
            return constraintModifier;

        return cameraModifier;
    }

    bbool InGameCameraComponent::keepModifier()
    {
        if ( !m_constraintModifier )
            return bfalse;

        if ( !getCameraControllerManager()->isCamModifierComponentActive(m_constraintModifier))
            return bfalse;

        // RO2-20625
//         if ( !getCameraControllerManager()->isMainCameraController(this) && !getCameraControllerManager()->cameraControllerHasInfluence(this))
//             return bfalse;

        return btrue;

//         m_constraintAABB = m_constraintModifier->getModifierAABBCur();
// 
//         if ( m_CMCur.m_constraintLeftIsActive && m_constraint.x() >0.f && m_controllers[m_controllerMainId].m_subjectAABB.getMax().x() < m_constraintAABB.getMin().x())
//             return btrue;
// 
//         if ( m_CMCur.m_constraintRightIsActive && m_constraint.x() <0.f && m_controllers[m_controllerMainId].m_subjectAABB.getMin().x() > m_constraintAABB.getMax().x())
//             return btrue;
// 
//         if ( m_CMCur.m_constraintBottomIsActive && m_constraint.y() >0.f && m_controllers[m_controllerMainId].m_subjectAABB.getMax().y() < m_constraintAABB.getMin().y())
//             return btrue;
// 
//         if ( m_CMCur.m_constraintTopIsActive && m_constraint.y() <0.f && m_controllers[m_controllerMainId].m_subjectAABB.getMin().y() > m_constraintAABB.getMax().y())
//             return btrue;
// 
//         return bfalse;
    }

    void InGameCameraComponent::getBlendedParamFrom(CameraModifierComponent* cameraModifier, BlendedParam &_blendParam)
    {
		_blendParam.m_offset.set(m_cameraRealPos.x() - m_cameraTargetAveragePosTrue.x(), m_cameraRealPos.y() - m_cameraTargetAveragePosTrue.y(), m_cameraRealPos.z());
		_blendParam.m_focale = m_CMCur.m_focale;
		_blendParam.m_depth = m_depth;
		if ( cameraModifier )
		{
			_blendParam.m_horizontalVersusVertical = m_cameraModifier->getHorizontalVersusVertical();
			_blendParam.m_offsetHVS = m_cameraModifier->getOffsetHVS();
		}
		else
		{
			_blendParam.m_horizontalVersusVertical = 0.0f;
			_blendParam.m_offsetHVS.set(0.0f,0.0f);
		}
	}

    void InGameCameraComponent::setBorderBlending()
    {
        AABB borderBlendingAABB;
		if ( !m_cameraModifier )
			return;
        const f32 blend = m_cameraModifier->getBorderBlending(m_mainController->m_subjectAABB.getCenter(), borderBlendingAABB, m_borderBlending == 1.f);

        if ( blend <1.f && m_borderBlending == 1.f )
        {
            CameraModifierComponent* cameraModifierPrevious = (CameraModifierComponent*) getCameraControllerManager()->getCameraModifierComponent( borderBlendingAABB, m_mainController->m_subjectPosReal.z());
			CameraModifierComponent* cameraModifierConstraint = (CameraModifierComponent*)getCameraControllerManager()->getCameraModifierComponent( borderBlendingAABB, m_mainController->m_subjectPosReal.z(), btrue);
            ITF_ASSERT( cameraModifierPrevious != m_cameraModifier);

			CameraModifierComponent* cameraModifierSave = m_cameraModifier;
			CameraModifierComponent* constraintModifierSave = m_constraintModifier;
			Vec3d cameraRealPosSave = m_cameraRealPos;
			AABB  screenRealAABBSave = m_screenRealAABB;

			m_constraintModifier = cameraModifierConstraint;
			m_cameraModifier = cameraModifierPrevious;
            blendingModifiers();
			setControllersPosition();        
            setControllersWeight(0.0f);
            setCameraTargetAverage();
			setScreenReal();

			updateConstraints();
       
			m_cameraRealPos = m_cameraTargetAveragePos +m_constraint;
        
/*			setScreenReal();
			setDepthCoeff();        


			// test WIP
			m_flagMirror = 0;

			Angle rotationDestAngle;
			if ( m_cameraModifier )
			{
				m_UpDnAngle = m_cameraModifier->getCM().m_UpDnAngle;
				rotationDestAngle = m_cameraModifier->getCM().m_rotationAngle;
				m_rotationSpeed = f32_Clamp( m_cameraModifier->getCM().m_rotationSpeed, 0.f, 1.f );

				// flip cam
				switch ( m_cameraModifier->getCM().m_flipView )
				{
				case CameraFlip_Both:
					m_flagMirror |= CAMERAFLAG_MIRROR_X;
					m_flagMirror |= CAMERAFLAG_MIRROR_Y;
					break;

				case CameraFlip_X:
					m_flagMirror |= CAMERAFLAG_MIRROR_X;
					break;

				case CameraFlip_Y:
					m_flagMirror |= CAMERAFLAG_MIRROR_Y;
					break;

				default:                
					break;     
				}
			}

			if ( f32_Abs( rotationDestAngle.ToRadians() ) > MTH_EPSILON || f32_Abs( m_rotationAngle.ToRadians() ) > MTH_EPSILON )
			{
				f32 shortestDeltaAngle = getShortestAngleDelta( m_rotationAngle.ToRadians(), rotationDestAngle.ToRadians() ) * m_rotationSpeed;

				m_rotationAngle.SetRadians( canonizeAngle( m_rotationAngle.ToRadians() + shortestDeltaAngle ) );


				Vec2d rot2D( m_rotationDir.truncateTo2D() );
				rot2D = rot2D.Rotate( shortestDeltaAngle );

				m_rotationDir.x() = rot2D.x();
				m_rotationDir.y() = rot2D.y();
			}
			else
			{
				m_rotationDir = Vec3d::YAxis;
			}*/

			getBlendedParamFrom(m_cameraModifier, m_cameraOtherSpatialParam);

			m_screenRealAABB = screenRealAABBSave;
			m_cameraRealPos = cameraRealPosSave;
			m_cameraModifier = cameraModifierSave;
			m_constraintModifier = constraintModifierSave;
			m_timeInterp = 1.0f;
			m_useSpatial = true;
		}

        m_borderBlending = blend;
    }

    void InGameCameraComponent::setModifierLookAtOffset(CamModifierUpdate& _cm)
    {        
        // set y
        if ( m_isLeaderController )
            _cm.m_directionLookAtOffset = 0.f;
        else if ( _cm.m_cameraModifierDirectionNormalized.y() > MTH_SQRT2BY2)
            _cm.m_directionLookAtOffset = _cm.m_cameraLookAtOffsetYUp;        
        else if ( _cm.m_cameraModifierDirectionNormalized.y() < -MTH_SQRT2BY2 )
            _cm.m_directionLookAtOffset = _cm.m_cameraLookAtOffsetYDown; 
        else
            _cm.m_directionLookAtOffset = 0.f;

        // set z
        _cm.m_cameraLookAtOffsetMaxInMulti.z() = Min( _cm.m_cameraLookAtOffsetMaxInMulti.z(), m_constraintDepthOffsetMax);
        _cm.m_cameraLookAtOffset.z() = Min( _cm.m_cameraLookAtOffset.z(), _cm.m_cameraLookAtOffsetMaxInMulti.z());
    }

    void InGameCameraComponent::blendingModifiers()
    {            
        if ( m_cameraModifier )
            m_cameraModifier->getUpdateData().getModifierList(m_CMDestFinal); 
		else
			m_CMDestFinal.init( &getTemplate()->getCMInit());

		setModifierLookAtOffset(m_CMDestFinal);

		m_CMDestCur = m_CMDestFinal;
	    m_CMCur = m_CMDestFinal;
	}

    void InGameCameraComponent::validateCamModifier(CameraModifierComponent *& _camMod) const
    {
        if (_camMod != NULL)
        {        
            if (!getCameraControllerManager()->isCamModifierRegistered(_camMod))
            {
                _camMod = NULL;
            }
        }
    }

    void InGameCameraComponent::updateModifiers()
    {
        validateCamModifier(m_cameraModifier);
        validateCamModifier(m_constraintModifier);
    }

    void InGameCameraComponent::setModifiers()
    {
        m_constraintModifierOnEnter = bfalse;
        m_constraintModifierOnExit = bfalse;
        m_switchModifier = bfalse;

        const CameraModifierComponent* cameraModifierLast = m_cameraModifier;

		updateModifiers();

		m_cameraTargetAveragePosTrue = m_subjectGlobalAABB.getCenter();

        m_cameraModifier = (CameraModifierComponent*) getCameraControllerManager()->getCameraModifierComponent( m_subjectGlobalAABB, m_subjectGlobalDepthMin);

        //  modifiers dest final
        if ( m_cameraModifier )
        {
            // get modifier list
            m_cameraModifier->getUpdateData().getModifierList(m_CMDestFinal); 

            // switch constraint
            if ( m_cameraModifier != cameraModifierLast )
            {     
                m_switchModifier = btrue;
                setModifierConstraint();
            }     
        }
        else 
        {
            if ( keepModifier() )
            {                
                m_cameraModifier = m_constraintModifier;                

                // get modifier list
                //m_cameraModifier->getUpdateData().getModifierList(m_CMDestFinal);
            }
            else
            {
                // switch constraint
                if ( m_constraintModifier )                        
                    m_constraintModifierOnExit = btrue;    

                // switch modifier
                if ( cameraModifierLast )    
                    m_switchModifier = btrue;
 
                m_constraintModifier = NULL;                

                //m_CMDestFinal.init( &getTemplate()->getCMInit());
            }
        }

        //setModifierLookAtOffset(m_CMDestFinal);

        //if ( getCameraControllerManager()->isInGameCameraJustReseted() )
        //{
        //    m_CMDestCur = m_CMDestFinal;
        //    m_CMCur = m_CMDestFinal;
        //    m_borderBlending = 1.f;
        //}
    }

    void InGameCameraComponent::getCameraData( CameraData& _data ) const
    {
        _data.m_pos = m_cameraFinalParam.m_finalPos;
        _data.m_focale = m_cameraFinalParam.m_focale;
        _data.m_horizontalVersusVertical = m_cameraFinalParam.m_horizontalVersusVertical;
        _data.m_offsetHVS = m_cameraFinalParam.m_offsetHVS;
        _data.m_depth = m_cameraFinalParam.m_depth;
        _data.m_deltaFogZ = m_cameraFinalParam.m_depth;
        _data.m_upDir = m_rotationDir;
		_data.m_UpDnAngle = m_UpDnAngle;
        _data.m_flagMirror = m_flagMirror;
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    //////////////////////////////////////////////////////////////////////////
    /// Debug
    void InGameCameraComponent::drawDebug( const CameraControllerData& _data )
    {
        if (_data.m_finalWeight < 1.f )
        {
            GFX_ADAPTER->drawDBGText("");
            GFX_ADAPTER->drawDBGText("InGameCamera does not lead camera");
        }

        if( m_controllerActiveCount == 0 )
        {
            String8 txt;
            txt.setTextFormat("Subject Manager Count = %i", getCameraControllerManager()->getSubjectListCount());
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("InGameCamera Position: ( %f, %f, %f )", m_cameraRealPos.x(), m_cameraRealPos.y(), m_cameraRealPos.z() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("*** ANY SUBJECT TO FILM ***");
            GFX_ADAPTER->drawDBGText(txt);
            return;
        }

        if ( !m_mainController )
        {
            String8 txt;
            txt.setTextFormat("Subject Manager Count = %i", getCameraControllerManager()->getSubjectListCount());
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("InGameCamera Position: ( %f, %f, %f )", m_cameraRealPos.x(), m_cameraRealPos.y(), m_cameraRealPos.z() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("*** ANY MAIN CONTROLLER ***");
            GFX_ADAPTER->drawDBGText(txt);
            return;
        }

        String8 txt;
        f32 z = m_subjectPosZ;
        f32 debugDuration = 0.f;
        f32 debugSize = 5.f;        

        /*
        if ( m_controllers.size() == 1)
        {
#ifdef DEVELOPER_JAY_CAMERA
            Controller& controller = m_controllers[0];

            // ray cast
            txt.setTextFormat("Ray Cast Length Max: %f", controller.m_rayCastDownLengthMax );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);

            //  screen limit
            txt.setTextFormat("Screen Limit Up Speed Max: %f", controller.m_screenRealLimitUpSpeedMax );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Screen Limit Up ratio: %f", controller.m_screenRealLimitUpRatio );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Screen Limit Up Target Dist: %f", controller.m_screenRealLimitUpTargetDist );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Screen Limit Up dist: %f", controller.m_screenRealLimitUpDist );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Depth Coeff: %f", m_depthCoeff );
            GFX_ADAPTER->drawDBGText(txt);

            // subject
            txt.setTextFormat("Subject Stance Wall Slide: %i", controller.m_subjectStance.m_wallSlide);
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Subject Stance Look Dir: ( %f, %f)", controller.m_subjectStance.m_lookDir.x(), controller.m_subjectStance.m_lookDir.y() );
            GFX_ADAPTER->drawDBGText(txt);    
            txt.setTextFormat("Subject Stance Sticked: %i", controller.m_subjectStance.m_stick);
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Subject Stance Joystick Move: ( %f, %f)", controller.m_subjectStance.m_joystickMoveNormalized.x(), controller.m_subjectStance.m_joystickMoveNormalized.y() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Subject Stance Jump On Air Timer Allowed: %i", controller.m_subjectStance.m_jumpOnAirTimerAllowed );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Subject Z: %f", m_subjectPosZ );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Subject Speed: ( %f, %f)", controller.m_subjectSpeed.x(), controller.m_subjectSpeed.y() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);

            // zone
            txt.setTextFormat("Zone Speed By Depth: ( %f, %f)", controller.z()oneSpeedByDepth.x(), controller.z()oneSpeedByDepth.y() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Zone Speed Real: ( %f, %f)", controller.z()oneSpeedReal.x(), controller.z()oneSpeedReal.y() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);

            // decentering
            txt.setTextFormat("Decentering Target Speed Average Real: ( %f, %f)", controller.m_cameraTargetDecenteringSpeedAverageRealOfTarget.x(), controller.m_cameraTargetDecenteringSpeedAverageRealOfTarget.y() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Decentering Target Speed Average Abs: ( %f, %f)", controller.m_cameraTargetDecenteringSpeedAverageAbsOfTarget.x(), controller.m_cameraTargetDecenteringSpeedAverageAbsOfTarget.y() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Decentering Inertie: ( %f, %f)", controller.m_cameraTargetDecenteringInertie.x(), controller.m_cameraTargetDecenteringInertie.y() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Decentering Blend: ( %f, %f)", controller.m_cameraTargetDecenteringBlend.x(), controller.m_cameraTargetDecenteringBlend.y() );
            GFX_ADAPTER->drawDBGText(txt);

            txt.setTextFormat("Decentering Offset: ( %f, %f)", controller.m_cameraTargetDecentering.x(), controller.m_cameraTargetDecentering.y() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);

            // camera
            txt.setTextFormat("Camera Target Speed Average Abs: ( %f, %f)", controller.m_cameraBaseMovingSpeedAverageAbsOfTarget.x(), controller.m_cameraBaseMovingSpeedAverageAbsOfTarget.y() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Camera Blend Y Min In Sticked: %f", controller.m_cameraBaseMovingBlendYMinInSticked );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Camera Blend: ( %f, %f)", controller.m_cameraBaseMovingBlend.x(), controller.m_cameraBaseMovingBlend.y() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Camera Speed: ( %f, %f, %f)", controller.m_cameraBaseMovingSpeed.x(), controller.m_cameraBaseMovingSpeed.y(), controller.m_cameraBaseMovingSpeed.z() );
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("");
            GFX_ADAPTER->drawDBGText(txt);
#endif
        }*/

        if ( getCameraControllerManager()->isDebugDrawModifiers() )
        {               
            updateModifiers();

            // modifier
            if ( m_cameraModifier )      
            {
                GFX_ADAPTER->drawDBGAABB( m_cameraModifier->getModifierAABBCur(), Color::white(), debugDuration, debugSize, m_cameraModifier->GetActor()->getDepth());
            }

            // constraint        
            if ( m_constraintModifier )
            {             
                GFX_ADAPTER->drawDBGAABB( m_constraintAABB, Color::white(), debugDuration, debugSize, m_constraintModifier->GetActor()->getDepth() );

#ifdef DEVELOPER_JAY_CAMERA
                GFX_ADAPTER->drawDBGAABB( m_screenConstraintAABB, Color::grey(), debugDuration, debugSize*0.5f, m_constraintModifier->GetActor()->getDepth() );

                txt.setTextFormat("");
                GFX_ADAPTER->drawDBGText(txt);

                if ( m_isConstraintContinuityAxeX )
                {
                    txt.setTextFormat("Constraint Continuity X");
                    GFX_ADAPTER->drawDBGText(txt);
                }

                if ( m_isConstraintContinuityAxeY )
                {
                    txt.setTextFormat("Constraint Continuity Y");
                    GFX_ADAPTER->drawDBGText(txt);
                }

                txt.setTextFormat("Constraint Match View X = %i, Y = %i", m_constraintMatchViewX, m_constraintMatchViewY);
                GFX_ADAPTER->drawDBGText(txt);

                f32 debugConstraintSize = 2.5f;

                GFX_ADAPTER->drawDBGAABB( m_constraintAnticipAABB, Color::white(), debugDuration, debugConstraintSize, m_constraintModifier->GetActor()->getDepth() );
#endif
            }
        }

#ifdef DEVELOPER_JAY_CAMERA
        txt.setTextFormat("Constraint Secret Area Z: %f", m_constraintSecretAreaZ );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Constraint Time X: %f", m_constraintTimeAxeX );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Constraint Time Y: %f", m_constraintTimeAxeY );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Constraint Time Coeff: ( %f, %f, %f)", m_constraintActivationCoeff.x(), m_constraintActivationCoeff.y(), m_constraintActivationCoeff.z() );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Constraint Depth max: %f", m_constraintDepthOffsetMax);
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("");
        GFX_ADAPTER->drawDBGText(txt);
#endif

        for ( u32 i=0; i<m_controllers.size(); i++)
        {
            Controller& controller = m_controllers[i];
            /*
#ifdef DEVELOPER_JAY_CAMERA
            // screen target AABB
            controller.setScreenTarget();
            GFX_ADAPTER->drawDBGAABB( controller.m_screenTargetAABB, Color::grey(), debugDuration, debugSize, z);

            // screen base AABB
         //   controller.setScreenBase();
            //             GFX_ADAPTER->drawDBGAABB( m_screenBaseAABB, Color::white(), debugDuration, debugSize, z );

            // ray cast
            Color rayCastColor = Color::yellow();
            if ( controller.m_rayCastCollisionDownFound )
                rayCastColor = Color::red();

            GFX_ADAPTER->drawDBGLine( controller.m_subjectAABB.getCenter(), controller.m_subjectAABB.getCenter() +controller.m_rayCastDown,             
                rayCastColor, debugDuration, debugSize, z);

            //  screen limit
            GFX_ADAPTER->drawDBGLine( Vec2d( controller.m_screenTargetAABB.getMax().x(), controller.m_screenRealLimitUp), Vec2d( controller.m_screenTargetAABB.getMin().x(), controller.m_screenRealLimitUp),             
                Color::red(), debugDuration, debugSize, z);            

            // decentering
            GFX_ADAPTER->drawDBGCircle( controller.m_cameraBasePos.truncateTo2D() +controller.m_cameraTargetDecenteringDest, 0.12f, Color::red());
            GFX_ADAPTER->drawDBGCircle( controller.m_cameraBasePos.truncateTo2D() +controller.m_cameraTargetDecentering, 0.1f, Color::blue());
#endif
*/
            // zone
            GFX_ADAPTER->drawDBGAABB( controller.m_zoneAABB, Color::black(), debugDuration, debugSize, z );

            Color colorSubject(Color::green());
            if( controller.m_isPaused )
                colorSubject = Color::grey();
            else if ( !controller.isActive())
                colorSubject = Color::black();
            else if ( controller.m_subjectStance.m_leadCamera )
                colorSubject = Color::red();
            else if( controller.m_isMainController )
                colorSubject = Color::orange();
             
            GFX_ADAPTER->drawDBGAABB( controller.m_subjectAABB, colorSubject, debugDuration, debugSize, z );

        }

        // controllers register count
        txt.setTextFormat("Subject Manager Count = %i", getCameraControllerManager()->getSubjectListCount());
        GFX_ADAPTER->drawDBGText(txt);

        // controllers active count
        txt.setTextFormat("Subject Camera Count = %i", m_controllerActiveCount);
        GFX_ADAPTER->drawDBGText(txt);         

// #ifdef DEVELOPER_JAY_CAMERA
//         // Camera target        
//         GFX_ADAPTER->drawDBGAABB( m_screenTargetAverageAABB, Color::grey(), debugDuration, debugSize*0.5f, z);
// #endif

        // Screen
        txt.setTextFormat("Screen Size: ( %f, %f)", m_screenRealAABB.getWidth(), m_screenRealAABB.getHeight());
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("Screen Resolution: ( %i, %i)", GFX_ADAPTER->getScreenWidth(), GFX_ADAPTER->getScreenHeight());
        GFX_ADAPTER->drawDBGText(txt);

        // Camera real           
        GFX_ADAPTER->drawDBGAABB( m_screenRealAABB, Color::black(), debugDuration, debugSize, z);

        // lookAt offset
        Vec2d lookAtOffset=Vec2d::Zero;
        if ( m_controllers.size())
        {
            for ( u32 i=0; i<m_controllers.size(); i++)
            {
                Controller& controller = m_controllers[i];

                lookAtOffset += controller.m_cameraTargetDecentering;
            }

            lookAtOffset = lookAtOffset/(f32)m_controllers.size() +m_CMCur.m_cameraLookAtOffset.truncateTo2D();
            lookAtOffset.y() += m_CMCur.m_directionLookAtOffset;
        }

#ifdef DEVELOPER_JAY_CAMERA
        txt.setTextFormat("Visibility Offset: ( %f, %f)", m_visibilityCur.x(), m_visibilityCur.y());
        GFX_ADAPTER->drawDBGText(txt);        
#endif

        txt.setTextFormat("Constraint: ( %f, %f, %f)", m_constraint.x(), m_constraint.y(), m_constraint.z() );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("InGameCamera Delta Z From MultiPlayers: %f )", m_cameraTargetAverageDezoomCur );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("InGameCamera Focale: %f ", m_CMCur.m_focale * MTH_RADTODEG );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("InGameCamera Offset: ( %f, %f, %f )", lookAtOffset.x(), lookAtOffset.y(), m_cameraRealPos.z() -m_subjectPosZ );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("InGameCamera Position: ( %f, %f, %f )", m_cameraRealPos.x(), m_cameraRealPos.y(), m_cameraRealPos.z() );
        GFX_ADAPTER->drawDBGText(txt);
        txt.setTextFormat("");
        GFX_ADAPTER->drawDBGText(txt);

        // Visibility
//         f32 mainDepth = m_mainController->m_subjectPosReal.z();
//         GFX_ADAPTER->drawDBGAABB( m_visibilityAABB, Color::pink(), 0.f, 1.f, mainDepth);
//         GFX_ADAPTER->drawDBGAABB( m_visibilityScreenMinAABB, Color::yellow(), 0.f, 1.f, mainDepth);            
//         GFX_ADAPTER->drawDBGAABB( m_visibilityScreenMaxAABB, Color::orange(), 0.f, 1.f, mainDepth);            
    }

#endif


    void InGameCameraComponent::Controller::setIgnoreZ( bbool _ignoreZ )
    {
        m_ignoreZ = _ignoreZ;

        if (_ignoreZ)
            m_backupPreviousZ = m_component->m_subjectPosZ;
        else
            m_backupPreviousZ = F32_INFINITY;
    }

}// namespace ITF
