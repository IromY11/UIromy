#include "precompiled_engine.h"

#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#include "engine/actors/components/Camera/CameraModifierComponent.h"
#endif //_ITF_CAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/InGameCameraComponent.h"
#endif //_ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_INGAMECAMERAPARAMETERS_H_
#include "engine/actors/components/Camera/InGameCameraParameters.h"
#endif // _ITF_INGAMECAMERAPARAMETERS_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef ITF_TAGMANAGER_H
#include "engine/TemplateManager/TagManager.h"
#endif // ITF_TAGMANAGER_H

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(CameraModifierComponent)
    BEGIN_SERIALIZATION_CHILD(CameraModifierComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("cameraView",m_cameraView);
            SERIALIZE_MEMBER("ignoreAABB",m_ignoreAABB);
            SERIALIZE_OBJECT("CM",m_CM);
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            SERIALIZE_OBJECT("localAABB",m_localAABB);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()
    
    IMPLEMENT_OBJECT_RTTI(CameraModifierComponent_Template)
    BEGIN_SERIALIZATION_CHILD(CameraModifierComponent_Template)
        SERIALIZE_OBJECT("CM",m_CM);
        SERIALIZE_MEMBER("SCALE", m_scaleInit);
    END_SERIALIZATION()

    CameraModifierComponent_Template::CameraModifierComponent_Template()
        : m_scaleInit( 32.4f, 18.225f)
    {
    }

    CameraModifierComponent_Template::~CameraModifierComponent_Template()
    {
    }

    CameraModifierComponent::CameraModifierComponent()
    : Super()
    , m_cameraView(Camera_Main|Camera_Remote)
    , m_cameraControllerManager(NULL)
    , m_isConstraint(bfalse)
    , m_prevPos(Vec3d::Zero)
    , m_extendedOffsetMinCur(Vec2d::Zero)
    , m_extendedOffsetMaxCur(Vec2d::Zero)
    , m_extendedOffsetMinDest(Vec2d::Zero)
    , m_extendedOffsetMaxDest(Vec2d::Zero)
    , m_isBorderBlending(bfalse)
    , m_ignoreAABB(bfalse)
    , m_registered(bfalse)
    , m_zoneNeutral(Vec2d::Zero)
    , m_isEnabled(btrue)
    {
        m_curveLogistic.m_xMax = 10.f; // to 1
        m_curveLogistic.m_xMin = -10.f; // to 0
    }

    CameraModifierComponent::~CameraModifierComponent()
    {
    }

    void CameraModifierComponent::onActorLoaded(Pickable::HotReloadType _hotReload) 
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventPause_CRC, this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventTrigger_CRC, this);

        m_Update.init(&m_CM, &getTemplate()->getCM());

        if ( GetActor()->getScale().x() == 1.f && GetActor()->getScale().y() == 1.f )
        {
            GetActor()->setWorldInitialScale( getTemplate()->getScaleInit(), btrue );   
            GetActor()->setScale( getTemplate()->getScaleInit() );                     
        }

        if ( m_localAABB.isZero() )
        {
            const Vec2d offset(0.5f, 0.5f);
            m_localAABB.setMin(-offset);
            m_localAABB.setMax(offset);
        }

        
#ifdef ITF_SUPPORT_EDITOR
        if(TAG_MANAGER)
        {
            GetActor()->addTag(TAG_MANAGER->getHardCodedTag(TagManager::Camera_Modifier));
        }
#endif

        init();           
    }

    void CameraModifierComponent::onStartDestroy( bbool _hotReload )
    {
        Super::onStartDestroy(_hotReload);
        unregisterCameraModifier();
    }

    void CameraModifierComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        m_prevPos = m_actor->getPos();
    }

    void CameraModifierComponent::onSceneActive()
    {
        Super::onSceneActive();

        if (m_actor->isEnabled())
            registerCameraModifier();
    }
    void CameraModifierComponent::onSceneInactive()
    {
        Super::onSceneInactive();

        unregisterCameraModifier();
    }

    void CameraModifierComponent::enable() 
    { 
        m_isEnabled = btrue; 
        registerCameraModifier();
    }

    void CameraModifierComponent::disable() 
    { 
        m_isEnabled = bfalse; 
        unregisterCameraModifier();
    }

    void CameraModifierComponent::onEvent(Event* _event)
    {
        if ( EventPause* onPause = DYNAMIC_CAST(_event,EventPause) )
        {
            if ( onPause->getPause() )
                disable();
            else
                enable();
        }
        else if ( EventTrigger* onTrigger= DYNAMIC_CAST(_event,EventTrigger) )
        {
            if (onTrigger->getActivated())
            {
                enable();
            }
            else
            {
                disable();
            }
        }
    }

    void CameraModifierComponent::tryToSetAABBToRatio16By9( AABB &_aabb)
    {
        const f32 aabbHeight = _aabb.getHeight();

        if ( aabbHeight > 0.f )
        {
            const f32 aabbWidth = _aabb.getWidth();
            const f32 aabbRatio = aabbWidth /aabbHeight;
            const f32 deltaRatio = MTH_16BY9 -aabbRatio;

            if ( deltaRatio < 0.f )
            {
                const f32 deltaX = aabbWidth -aabbHeight *MTH_16BY9;
                if ( deltaX > 0.f && deltaX <= 1.f )
                {
                    const Vec2d offset( deltaX *0.5f, 0.f);
                    _aabb.setMin( _aabb.getMin() +offset );
                    _aabb.setMax( _aabb.getMax() -offset );
                }
            }
            else if ( deltaRatio > 0.f )
            {
                const f32 deltaY = aabbHeight -aabbWidth *MTH_9BY16;
                if ( deltaY > 0.f && deltaY <= 1.f )
                {
                    const Vec2d offset( 0.f, deltaY *0.5f);
                    _aabb.setMin( _aabb.getMin() +offset );
                    _aabb.setMax( _aabb.getMax() -offset );
                }
            }            
        }
    }

    void CameraModifierComponent::updateAABB()
    {
        AABB worldAABB(m_localAABB);
        worldAABB.Scale(GetActor()->getScale());
        worldAABB.Translate(GetActor()->get2DPos());

 //       tryToSetAABBToRatio16By9(worldAABB);

        GetActor()->growAABB(worldAABB);
	}
	
	void CameraModifierComponent::getLookAtOffset(Vec3d &_offset)
	{
		_offset = m_CM.m_lookAt;
	}

	void CameraModifierComponent::addLookAtOffset(Vec3d _offset)
	{
		m_CM.m_lookAt += _offset;
	}

    void CameraModifierComponent::init()
    {
        updateAABB();

        //  sens map
        f32_CosSin(GetActor()->getAngle(), &m_Update.m_cameraModifierDirectionNormalized.x(), &m_Update.m_cameraModifierDirectionNormalized.y());
        if ( GetActor()->getIsFlipped() )
            m_Update.m_cameraModifierDirectionNormalized *= -1.f;

        ITF_ASSERT(m_Update.m_cameraModifierDirectionNormalized.isNormalized());

        initBorderBlendings();
        initConstraintsExtended();
        initLockedAxes();

        m_CM.m_zoneNeutral.x() = Max( 0.f, m_CM.m_zoneNeutral.x());
        m_CM.m_zoneNeutral.y() = Max( 0.f, m_CM.m_zoneNeutral.y());

        m_zoneNeutral = m_CM.m_zoneNeutral;
    }

    void CameraModifierComponent::limitBlendingZone( bbool _isVertical)
    {
        f32 longMax;
        
        if ( _isVertical )
            longMax = GetActor()->getAABB().getHeight();
        else
            longMax = GetActor()->getAABB().getWidth();

        if( m_CM.m_blendingZoneStart && m_CM.m_blendingZoneStop )
        {
            f32 longDelta = m_CM.m_blendingZoneStart +m_CM.m_blendingZoneStop -longMax;

            if ( longDelta >0.f)
            {
                m_CM.m_blendingZoneStart -= longDelta *0.5f;
                m_CM.m_blendingZoneStop -= longDelta *0.5f;
            }
        }
        else
        {
            m_CM.m_blendingZoneStart = Min( m_CM.m_blendingZoneStart, longMax);
            m_CM.m_blendingZoneStop = Min( m_CM.m_blendingZoneStop, longMax);
        }
    }

    void CameraModifierComponent::initLockedAxes()
    {
        m_Update.m_isLockedAxeX = bfalse;
        m_Update.m_isLockedAxeY = bfalse;

        if ( getTemplate()->getCM().m_isFixed )
        {
            m_Update.m_isLockedAxeX = btrue;
            m_Update.m_isLockedAxeY = btrue;
        }
        else if ( getTemplate()->getCM().m_isLockedAxe )
        {
            if ( f32_Abs( m_Update.m_cameraModifierDirectionNormalized.y() ) >= MTH_SQRT2BY2 )
                m_Update.m_isLockedAxeX = btrue;
            else
                m_Update.m_isLockedAxeY = btrue;
        }
    }

    void CameraModifierComponent::initBorderBlendings()
    {
        m_borderBlendingLeft.init();
        m_borderBlendingRight.init();
        m_borderBlendingTop.init();
        m_borderBlendingBottom.init();

        m_CM.m_blendingZoneStart = f32_Abs(m_CM.m_blendingZoneStart);
        m_CM.m_blendingZoneStop = f32_Abs(m_CM.m_blendingZoneStop);
     
        if ( m_Update.m_cameraModifierDirectionNormalized.y() <= -MTH_SQRT2BY2 ) // TOP
        {
            limitBlendingZone(btrue);

            if ( m_CM.m_blendingZoneStart )
            {
                m_borderBlendingTop.m_stop = GetActor()->getAABB().getMax().y() -m_CM.m_blendingZoneStart;
                m_borderBlendingTop.m_ratio = 1.f /m_CM.m_blendingZoneStart;
            }

            if ( m_CM.m_blendingZoneStop )
            {
                m_borderBlendingBottom.m_stop = GetActor()->getAABB().getMin().y() +m_CM.m_blendingZoneStop;
                m_borderBlendingBottom.m_ratio = 1.f /m_CM.m_blendingZoneStop;
            }
        }
        else if ( m_Update.m_cameraModifierDirectionNormalized.y() >= MTH_SQRT2BY2 ) // BOTTOM
        {
            limitBlendingZone(btrue);

            if ( m_CM.m_blendingZoneStart )
            {
                m_borderBlendingBottom.m_stop = GetActor()->getAABB().getMin().y() +m_CM.m_blendingZoneStart;
                m_borderBlendingBottom.m_ratio = 1.f /m_CM.m_blendingZoneStart;
            }
            if ( m_CM.m_blendingZoneStop )
            {
                m_borderBlendingTop.m_stop = GetActor()->getAABB().getMax().y() -m_CM.m_blendingZoneStop;
                m_borderBlendingTop.m_ratio = 1.f /m_CM.m_blendingZoneStop;
            }
        }
        else if ( m_Update.m_cameraModifierDirectionNormalized.x() >= MTH_SQRT2BY2 ) // LEFT
        {
            limitBlendingZone(bfalse);

            if ( m_CM.m_blendingZoneStart )
            {
                m_borderBlendingLeft.m_stop = GetActor()->getAABB().getMin().x() +m_CM.m_blendingZoneStart;
                m_borderBlendingLeft.m_ratio = 1.f /m_CM.m_blendingZoneStart;
            }
            if ( m_CM.m_blendingZoneStop )
            {
                m_borderBlendingRight.m_stop = GetActor()->getAABB().getMax().x() -m_CM.m_blendingZoneStop;
                m_borderBlendingRight.m_ratio = 1.f /m_CM.m_blendingZoneStop;
            }
        }
        else // RIGHT   
        {
            limitBlendingZone(bfalse);

            if ( m_CM.m_blendingZoneStart )
            {
                m_borderBlendingRight.m_stop = GetActor()->getAABB().getMax().x() -m_CM.m_blendingZoneStart;
                m_borderBlendingRight.m_ratio = 1.f /m_CM.m_blendingZoneStart;
            }
            if ( m_CM.m_blendingZoneStop )
            {
                m_borderBlendingLeft.m_stop = GetActor()->getAABB().getMin().x() +m_CM.m_blendingZoneStop;
                m_borderBlendingLeft.m_ratio = 1.f /m_CM.m_blendingZoneStop;
            }
        } 

        m_isBorderBlending = m_borderBlendingTop.m_ratio>0.f || m_borderBlendingBottom.m_ratio>0.f || m_borderBlendingRight.m_ratio>0.f || m_borderBlendingLeft.m_ratio>0.f;
    }

    void CameraModifierComponent::initConstraintsExtended()
    {
        if ( m_CM.m_constraintLeftIsActive )
            initConstraintExtended( m_CM.m_constraintExtendedLeft );
        else
            m_CM.m_constraintExtendedLeft.m_offset = 0.f;

        if ( m_CM.m_constraintRightIsActive )
            initConstraintExtended( m_CM.m_constraintExtendedRight );
        else
            m_CM.m_constraintExtendedRight.m_offset = 0.f;

        if ( m_CM.m_constraintTopIsActive )
            initConstraintExtended( m_CM.m_constraintExtendedTop );
        else
            m_CM.m_constraintExtendedTop.m_offset = 0.f;

        if ( m_CM.m_constraintBottomIsActive )
            initConstraintExtended( m_CM.m_constraintExtendedBottom );
        else
            m_CM.m_constraintExtendedBottom.m_offset = 0.f;
    }

    void CameraModifierComponent::setConstraintExtendedTimeCoeff( f32& _timeCoeff, f32 _time ) const
    {
        if ( _time )
            _timeCoeff = 1.f /_time;
        else
            _timeCoeff = F32_INFINITY *getSign(_time);
    }

    void CameraModifierComponent::initConstraintExtended( ConstraintExtended& _constraint )
    {
        _constraint.m_offset = f32_Max( 0.f, _constraint.m_offset);
        _constraint.m_timeToIncrease = f32_Max( 0.f, _constraint.m_timeToIncrease);
        _constraint.m_timeToDecrease= f32_Max( 0.f, _constraint.m_timeToDecrease);        
        _constraint.m_timeToWaitBeforeDecrease = f32_Max( 0.f, _constraint.m_timeToWaitBeforeDecrease);
    }

    f32 CameraModifierComponent::getExtendedDecreaseOffset( ConstraintExtended& _constraintExtended, ExtendedParamsDecrease& _decrease, f32 _deltaTime)
    {
        _decrease.m_time += _deltaTime *_decrease.m_timeCoeff;
        _decrease.m_time = Max( 0.f, _decrease.m_time);

        if ( _decrease.m_time <= 0.f )
            return 0.f;       

        return m_curveLogistic.getValue(_decrease.m_time) *_constraintExtended.m_offset;
    }

    void CameraModifierComponent::updateExtendedParams( ConstraintExtended& _constraintExtended, ExtendedParams& _extendedParams, f32& _cur, f32& _dest, f32 _deltaTime)
    {
        if ( _extendedParams.m_state != Extended_None )
        {
            // old decrease
            f32 offsetDecreaseOld = getExtendedDecreaseOffset( _constraintExtended, _extendedParams.m_decrease, _deltaTime);

            _extendedParams.m_time += _deltaTime *_extendedParams.m_timeCoeff;
            _extendedParams.m_time = Clamp( _extendedParams.m_time, 0.f, 1.f);

            if ( _extendedParams.m_state == Extended_WaitToDecrease && offsetDecreaseOld == 0.f )
            {
                if ( _extendedParams.m_time >= 1.f )
                {
                    _extendedParams.m_state = Extended_Decrease;
                    setConstraintExtendedTimeCoeff( _extendedParams.m_timeCoeff, -_constraintExtended.m_timeToDecrease);                                   
                } 
            }
            else
            {                
                _cur = m_curveLogistic.getValue(_extendedParams.m_time) *(_constraintExtended.m_offset -offsetDecreaseOld) +offsetDecreaseOld;

                // stop update
                if ( offsetDecreaseOld == 0.f )
                {
                    if ( _extendedParams.m_state == Extended_Decrease && _extendedParams.m_time == 0.f )
                    {
                        _dest = 0.f; 
                        _extendedParams.m_state = Extended_None;
                    }
                    else if ( _extendedParams.m_state == Extended_Increase && _extendedParams.m_time >= 1.f )
                    {
                        _extendedParams.m_time = 0.f;
                        setConstraintExtendedTimeCoeff( _extendedParams.m_timeCoeff, _constraintExtended.m_timeToWaitBeforeDecrease);                 

                        _extendedParams.m_state = Extended_WaitToDecrease;
                    }
                }
            }
        }
    }

    void CameraModifierComponent::setConstraintExtentedTimeMin( ConstraintExtended& _constraint, ExtendedParams& _params, f32 _delta) const
    {
        if ( _params.m_time <1.f )
        {
            f32 timeMin = _delta/_constraint.m_offset;
            _params.m_time = Max( _params.m_time, timeMin);
        }
    }

    void CameraModifierComponent::setConstraintExtended( ConstraintExtended& _constraint, ExtendedParams& _params, f32& _dest, f32 _delta ) const
    {
        if ( _params.m_state == Extended_Increase )
        {
            setConstraintExtentedTimeMin( _constraint, _params, _delta);
            return;
        }

        if ( _params.m_state == Extended_WaitToDecrease )
        {
            _params.m_time = 0.f;
            return;
        }
        
        if ( _params.m_state == Extended_Decrease )
        {
            _params.m_decrease.m_time = _params.m_time;
            _params.m_decrease.m_timeCoeff = _params.m_timeCoeff;
        }

        _params.m_time = 0.f;  
        setConstraintExtentedTimeMin( _constraint, _params, _delta);

        _params.m_state = Extended_Increase;         
        setConstraintExtendedTimeCoeff( _params.m_timeCoeff, _constraint.m_timeToIncrease);

        _dest = _constraint.m_offset;
    }

    void CameraModifierComponent::updateConstraintExtended( f32 _deltaTime )
    {
        updateExtendedParams( m_CM.m_constraintExtendedLeft, m_extendedLeft, m_extendedOffsetMinCur.x(), m_extendedOffsetMinDest.x(), _deltaTime);
        updateExtendedParams( m_CM.m_constraintExtendedRight, m_extendedRight, m_extendedOffsetMaxCur.x(), m_extendedOffsetMaxDest.x(), _deltaTime);
        updateExtendedParams( m_CM.m_constraintExtendedBottom, m_extendedBottom, m_extendedOffsetMinCur.y(), m_extendedOffsetMinDest.y(), _deltaTime);
        updateExtendedParams( m_CM.m_constraintExtendedTop, m_extendedTop, m_extendedOffsetMaxCur.y(), m_extendedOffsetMaxDest.y(), _deltaTime);
    }

    void CameraModifierComponent::Update( f32 _deltaTime )
    {
        if ( m_prevPos != m_actor->getPos() )
        {
            init();
            m_prevPos = m_actor->getPos();
        }

        updateAABB();
        updateConstraintExtended( _deltaTime );
    }

    void CameraModifierComponent::setConstraintExtendedLeft(f32 _delta)
    { 
        setConstraintExtended( m_CM.m_constraintExtendedLeft, m_extendedLeft, m_extendedOffsetMinDest.x(), _delta);
    }

    void CameraModifierComponent::setConstraintExtendedRight( f32 _delta) 
    { 
        setConstraintExtended( m_CM.m_constraintExtendedRight, m_extendedRight, m_extendedOffsetMaxDest.x(), _delta);
    }

    void CameraModifierComponent::setConstraintExtendedBottom(f32 _delta)
    { 
        setConstraintExtended( m_CM.m_constraintExtendedBottom, m_extendedBottom, m_extendedOffsetMinDest.y(), _delta);
    }

    void CameraModifierComponent::setConstraintExtendedTop(f32 _delta)   
    { 
        setConstraintExtended( m_CM.m_constraintExtendedTop, m_extendedTop, m_extendedOffsetMaxDest.y(), _delta);
    }

    void CameraModifierComponent::setConstraintExtendedLeftToDest()
    { 
        m_extendedLeft.m_time = 1.f;
        m_extendedOffsetMinCur.x() = m_extendedOffsetMinDest.x();
    }

    void CameraModifierComponent::setConstraintExtendedRightToDest()
    { 
        m_extendedRight.m_time = 1.f;
        m_extendedOffsetMaxCur.x() = m_extendedOffsetMaxDest.x();
    }

    void CameraModifierComponent::setConstraintExtendedBottomToDest()
    { 
        m_extendedBottom.m_time = 1.f;
        m_extendedOffsetMinCur.y() = m_extendedOffsetMinDest.y();
    }

    void CameraModifierComponent::setConstraintExtendedTopToDest()
    { 
        m_extendedTop.m_time = 1.f;
        m_extendedOffsetMaxCur.y() = m_extendedOffsetMaxDest.y();
    }

    AABB CameraModifierComponent::getModifierAABBCur()  const 
    { 
        return AABB( m_actor->getAABB().getMin() -m_extendedOffsetMinCur, m_actor->getAABB().getMax() +m_extendedOffsetMaxCur); 
    }

    AABB CameraModifierComponent::getModifierAABBDest()  const 
    { 
        return AABB( m_actor->getAABB().getMin() -m_extendedOffsetMinDest, m_actor->getAABB().getMax() +m_extendedOffsetMaxDest); 
    }

    AABB CameraModifierComponent::getModifierAABBMax()  const 
    { 
        return AABB( m_actor->getAABB().getMin() -Vec2d(m_CM.m_constraintExtendedLeft.m_offset,m_CM.m_constraintExtendedBottom.m_offset), 
                    m_actor->getAABB().getMax() +Vec2d(m_CM.m_constraintExtendedRight.m_offset,m_CM.m_constraintExtendedTop.m_offset)); 
    }

    f32 CameraModifierComponent::getBorderBlending( const Vec2d& _pos, AABB& _borderAABB, bbool _setAABB) const 
    {
        f32 blend = 1.f;

        if ( m_borderBlendingLeft.m_ratio && _pos.x() < m_borderBlendingLeft.m_stop )
        {
            blend = ( _pos.x() -m_actor->getAABB().getMin().x() ) *m_borderBlendingLeft.m_ratio;

            if ( _setAABB )
            {
                _borderAABB.setMin( m_actor->getAABB().getMin() -Vec2d(1.01f,0.f));
                _borderAABB.setMax( m_actor->getAABB().getMinXMaxY() -Vec2d(0.01f,0.f));
            }
        }
        else if ( m_borderBlendingRight.m_ratio && _pos.x() > m_borderBlendingRight.m_stop )
        {
            blend = ( m_actor->getAABB().getMax().x() -_pos.x() ) *m_borderBlendingRight.m_ratio;

            if ( _setAABB )
            {
                _borderAABB.setMin( m_actor->getAABB().getMaxXMinY() +Vec2d(0.01f,0.f));
                _borderAABB.setMax( m_actor->getAABB().getMax() +Vec2d(1.01f,0.f));
            }
        }
        else if ( m_borderBlendingBottom.m_ratio && _pos.y() < m_borderBlendingBottom.m_stop )
        {
            blend = ( _pos.y() -m_actor->getAABB().getMin().y() ) *m_borderBlendingBottom.m_ratio;

            if ( _setAABB )
            {
                _borderAABB.setMin( m_actor->getAABB().getMin() -Vec2d(0.f,1.01f));
                _borderAABB.setMax( m_actor->getAABB().getMaxXMinY() -Vec2d(0.f,0.01f));
            }
        }
        else if ( m_borderBlendingTop.m_ratio && _pos.y() > m_borderBlendingTop.m_stop )
        {
            blend = ( m_actor->getAABB().getMax().y() -_pos.y() ) *m_borderBlendingTop.m_ratio;

            if ( _setAABB )
            {
                _borderAABB.setMin( m_actor->getAABB().getMinXMaxY() +Vec2d(0.f,0.01f));
                _borderAABB.setMax( m_actor->getAABB().getMax() +Vec2d(0.f,1.01f));
            }
        }

        return Clamp( blend, 0.f, 1.f);
    }

    void CameraModifierComponent::resetExtension()
    {
        if ( isConstraintExtended() )
        {
            m_extendedRight.reset();
            m_extendedLeft.reset();
            m_extendedTop.reset();
            m_extendedBottom.reset();

            m_extendedOffsetMinCur = Vec2d::Zero;
            m_extendedOffsetMaxCur = Vec2d::Zero;
        }
    }

    void CameraModifierComponent::registerCameraModifier()
    {
        if (!m_registered && isEnabled() )
        {
            m_registered = btrue;
            if (m_cameraView & Camera_Main)
                CameraControllerManager::getManagerFromId(CAMID_MAIN)->registerCamModifierComponent( this );       
// #ifdef ITF_USE_REMOTEVIEW
//             if (m_cameraView & Camera_Remote)
//                 CameraControllerManager::getManagerFromId(CAMID_REMOTE)->registerCamModifierComponent( this );       
// #endif // ITF_USE_REMOTEVIEW
        }
    }

#ifdef ITF_SUPPORT_EDITOR

    void CameraModifierComponent::onPrePropertyChange()
    {
        unregisterCameraModifier();
    }

    void CameraModifierComponent::onPostPropertyChange()
    {
        registerCameraModifier();
    }

#endif //ITF_SUPPORT_EDITOR

    void CameraModifierComponent::unregisterCameraModifier()
    {
        if (m_registered)
        {
            m_registered = bfalse;
            if (m_cameraView & Camera_Main)
                CameraControllerManager::getManagerFromId(CAMID_MAIN)->unregisterCamModifierComponent( this );       
#ifdef ITF_USE_REMOTEVIEW
            if (m_cameraView & Camera_Remote)
                CameraControllerManager::getManagerFromId(CAMID_REMOTE)->unregisterCamModifierComponent( this );       
#endif // ITF_USE_REMOTEVIEW
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void CameraModifierComponent::drawDebug() const
    {
        const Actor* actor = GetActor();
        if (!actor)
            return;

        f32 lineSizeConstraint = 10.f;

        f32 debugDuration = 0.f;
        f32 z = actor->getPos().z();
        AABB aabb = actor->getAABB();
        GFX_ADAPTER->drawDBGAABB( aabb, Color::red(), 0.f, 1.f, z);

        // extended
        GFX_ADAPTER->drawDBGAABB( getModifierAABBMax(), Color::red(), 0.f, 1.f, z);

        //  constraint
        const CamModifier& cm = getCM();

        const AABB modifierAABBCur = getModifierAABBCur();

        if ( cm.m_constraintLeftIsActive )
            GFX_ADAPTER->drawDBGLine( modifierAABBCur.getMinXMaxY(), modifierAABBCur.getMin(), Color::red(), debugDuration, lineSizeConstraint, z);  
        if ( cm.m_constraintRightIsActive )                        
            GFX_ADAPTER->drawDBGLine( modifierAABBCur.getMaxXMinY(), modifierAABBCur.getMax(), Color::red(), debugDuration, lineSizeConstraint, z);  
        if ( cm.m_constraintTopIsActive )
            GFX_ADAPTER->drawDBGLine( modifierAABBCur.getMax(), modifierAABBCur.getMinXMaxY(), Color::red(), debugDuration, lineSizeConstraint, z);  
        if ( cm.m_constraintBottomIsActive )
            GFX_ADAPTER->drawDBGLine( modifierAABBCur.getMin(), modifierAABBCur.getMaxXMinY(), Color::red(), debugDuration, lineSizeConstraint, z); 

        //  sens map du modifier
        if ( getCMTemplate().m_isMainSubject && !getCMTemplate().m_isMainDRCPlayer )
        {
            const f32 longArrow = f32_Min( aabb.getHeight(), aabb.getWidth()) *0.25f;
            GFX_ADAPTER->drawDBGArrow(actor->get2DPos(), actor->get2DPos()+m_Update.m_cameraModifierDirectionNormalized* longArrow, 1,0,0, 0.5f, actor->getPos().z(), 0.f, bfalse );
        }

        // blending border
        Color borderColor(Color::red());
        borderColor.m_a = 0.25f;

        if ( m_borderBlendingLeft.m_ratio )
        {   
            GFX_ADAPTER->drawDBGTriangle( aabb.getMin(), aabb.getMinXMaxY(), Vec2d(m_borderBlendingLeft.m_stop, aabb.getMax().y()), borderColor, 0.f, z);          
            GFX_ADAPTER->drawDBGTriangle( aabb.getMin(), Vec2d(m_borderBlendingLeft.m_stop, aabb.getMax().y()), Vec2d(m_borderBlendingLeft.m_stop, aabb.getMin().y()), borderColor, 0.f, z);                      
        }
        if ( m_borderBlendingRight.m_ratio )
        {
            GFX_ADAPTER->drawDBGTriangle( Vec2d(m_borderBlendingRight.m_stop, aabb.getMin().y()), Vec2d(m_borderBlendingRight.m_stop, aabb.getMax().y()), aabb.getMax(), borderColor, 0.f, z);             
            GFX_ADAPTER->drawDBGTriangle( Vec2d(m_borderBlendingRight.m_stop, aabb.getMin().y()), aabb.getMax(), aabb.getMaxXMinY(), borderColor, 0.f, z);                         
        }
        if ( m_borderBlendingTop.m_ratio )
        {   
            GFX_ADAPTER->drawDBGTriangle( Vec2d(aabb.getMin().x(), m_borderBlendingTop.m_stop), aabb.getMinXMaxY(), aabb.getMax(), borderColor, 0.f, z); 
            GFX_ADAPTER->drawDBGTriangle( Vec2d(aabb.getMin().x(), m_borderBlendingTop.m_stop), aabb.getMax(), Vec2d(aabb.getMax().x(), m_borderBlendingTop.m_stop), borderColor, 0.f, z);             
        }
        if ( m_borderBlendingBottom.m_ratio )
        {            
            GFX_ADAPTER->drawDBGTriangle( aabb.getMin(), Vec2d(aabb.getMin().x(), m_borderBlendingBottom.m_stop), Vec2d(aabb.getMax().x(), m_borderBlendingBottom.m_stop), borderColor, 0.f, z); 
            GFX_ADAPTER->drawDBGTriangle( aabb.getMin(), Vec2d(aabb.getMax().x(), m_borderBlendingBottom.m_stop), aabb.getMaxXMinY(), borderColor, 0.f, z); 
        }

        // priority
        Vec3d pos3dIn(actor->getPos());
        Vec3d pos3dOut;
        View::getCurrentView()->compute3DTo2D(pos3dIn, pos3dOut); 
       
        String8 txt;
        const f32 textSize = f32(GFX_ADAPTER->getDebugFontSize());
        txt.setTextFormat("P%i", cm.m_cameraModifierPriority);      
        GFX_ADAPTER->drawDBG2dBox( pos3dOut.truncateTo2D(), textSize *1.5f, textSize );
        GFX_ADAPTER->drawDBGText(txt, Color::red(), pos3dOut.x(), pos3dOut.y() );
    }


#endif //ITF_SUPPORT_DEBUGFEATURE

#ifdef ITF_SUPPORT_EDITOR
    void CameraModifierComponent::onEditorCreated( class Actor* _original )
    {
        init();
    }
#endif // ITF_SUPPORT_EDITOR

}   // namespace ITF

 
