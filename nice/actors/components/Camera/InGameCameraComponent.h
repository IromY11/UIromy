#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#define _ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_BASECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/BaseCameraComponent.h"
#endif //_ITF_BASECAMERACOMPONENT_H_

#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#include "engine/actors/components/Camera/CameraModifierComponent.h"
#endif //_ITF_CAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_CURVE_H_
#include "gameplay/Tools/Curve.h"
#endif // _ITF_CURVE_H_

#include "core/types.h"

namespace ITF
{
    class CameraControllerData;
    class CameraControllerManager;
    class CameraControllerSubject;

    class InGameCameraComponent : public BaseCameraComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(InGameCameraComponent, BaseCameraComponent,303426032)
        DECLARE_SERIALIZE()

    public:
		struct BlendedParam
		{
			BlendedParam()
			{
				m_offset = Vec3d::Zero;
				m_focale = MTH_PIBY4;
				m_horizontalVersusVertical = 0.0f;
				m_offsetHVS.set(0.0f,0.0f);
				m_finalPos = Vec3d::Zero;
				m_depth = 0.0f;
			}
			// blend from two other blended parameters.
			void blend(const BlendedParam &_source, const BlendedParam &_dest, f32 _alpha);
			// blend parameters to a destination.
			void blendSmooth(BlendedParam &_dest, BlendedParam &_speed, f32 _smoothValue, f32 _dt);
			// Reset content to zero.
			void setToZero();

			Vec3d   m_offset;
			f32		m_focale;
			f32		m_depth;
			f32		m_horizontalVersusVertical;
			Vec2d	m_offsetHVS;
			Vec3d   m_finalPos;
		};

        //static InGameCameraComponent* get() { ITF_ASSERT(s_instance); return s_instance; }

        InGameCameraComponent();
        virtual ~InGameCameraComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void onStartDestroy(bbool _hotReload);
        virtual void        onFinalizeLoad();  

        virtual void        getCameraData( CameraData& _data ) const;
        virtual void        teleport(const Vec3d& _newPos);        
        virtual void        onManagerUpdate(f32 _dt);
        CameraModifierComponent *getCameraModifier(bbool _useConstraint =btrue) const;
        void                updateDrift();
        virtual Actor*      getMainSubject() const;
        virtual bbool       isInGameCamera() const { return btrue; }
     
#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug( const CameraControllerData& _data );
#endif //ITF_SUPPORT_DEBUGFEATURE

    protected:
        class SubjectStance 
        {
        public:
            SubjectStance()
                : m_objectRef(ITF_INVALID_OBJREF)
                , m_stick(bfalse)
                , m_lookDir(Vec2d::Zero)                    
                , m_joystickMoveNormalized(Vec2d::Zero)
                , m_justUnstick(bfalse)
                , m_justStick(bfalse)      
                , m_performUTurn(bfalse)
                , m_hang(bfalse)
                , m_wallSlide(bfalse)
                , m_wallSlideLast(bfalse)
                , m_performUTurnLast(bfalse)
                , m_jumpOnAirTimerAllowed(bfalse)
                , m_jumpOnAirTimerAllowedLast(bfalse)
                , m_isPlayer(bfalse)
                , m_isLockedPosition(bfalse)
                , m_wasLockedPosition(bfalse)
                , m_lockedPosition(Vec2d::Zero)
                , m_lockedPositionLast(Vec2d::Zero)
                , m_windForce(Vec2d::Zero)
                , m_leadCamera(bfalse)                
            {
            }

            ObjectRef m_objectRef;
            Vec2d   m_lookDir;       
            Vec2d   m_joystickMoveNormalized;
            Vec2d   m_lockedPosition;
            Vec2d   m_lockedPositionLast;
            Vec2d   m_windForce;   

            u32     m_stick :1,
                    m_justUnstick :1,
                    m_justStick :1,
                    m_hang :1,
                    m_performUTurn :1,
                    m_performUTurnLast :1,
                    m_wallSlide :1,
                    m_wallSlideLast :1,
                    m_jumpOnAirTimerAllowed :1,
                    m_jumpOnAirTimerAllowedLast :1,
                    m_isPlayer :1,
                    m_isLockedPosition :1,
                    m_wasLockedPosition :1,
                    m_leadCamera :1;

            void    initSubjectStance(); 
            void    resetSubjectStance();

        };

        virtual const Actor* getMainActivePlayer() const { return NULL; }
        virtual void setSubjectStance( SubjectStance* /*_player*/, const AABB& /*_screen*/ ) {}
        virtual bbool isPlayingPageSequence( ObjectRef _objRef ) { return bfalse; }
        virtual bbool isPlayingShooter( ObjectRef _objRef ) { return bfalse; }

    private:

        const class InGameCameraComponent_Template* getTemplate() const;

        void    projectAABB     ( AABB& _aabb, f32 _originalZ, f32 _newZ);
        void    updateInit      ();  
        

        // modifiers
        CamModifierUpdate   m_CMCur;
        CamModifierUpdate   m_CMDestCur;
        CamModifierUpdate   m_CMDestFinal;

		Angle				  m_UpDnAngle;
        Angle                 m_rotationAngle;
        f32                   m_rotationSpeed;
        Vec3d                 m_rotationDir;
        u32                   m_flagMirror;

        class Controller
        {
            public:

            Controller(InGameCameraComponent* _component )             
                : m_component(_component)                
                , m_weight(0.f)
                , m_weightDest(1.f)
                , m_subjectAABB(Vec2d::Zero)
                , m_subjectAABBCenterLast(Vec2d::Zero)                
                , m_subjectSpeed(Vec2d::Zero)
                , m_subjectPosReal(Vec3d::Zero)
                , m_zoneAABB(Vec2d::Zero)
                , m_zoneOffsetFromSubject(Vec2d::Zero)
                , m_zoneSpeedByDepth(Vec2d::Zero)
                , m_zoneSpeedReal(Vec2d::Zero)
                , m_zoneAABBCenterLast(Vec2d::Zero)
                , m_screenBaseAABB(Vec2d::Zero)
                , m_screenRealLimitUp(0.f)
                , m_screenRealLimitUpTargetDist(0.f)
                , m_screenRealLimitUpRatio(0.f)    
                , m_screenRealLimitUpDist(0.f)
                , m_screenRealLimitUpSpeedMax(0.f)
                , m_screenTargetAABB(Vec2d::Zero)        
                , m_cameraTargetDecentering(Vec2d::Zero)
                , m_cameraTargetDecenteringDest(Vec2d::Zero)
                , m_cameraTargetDecenteringDir(Vec2d::Zero)
                , m_cameraTargetDecenteringDirFromSubjectStance(Vec2d::Zero)
                , m_cameraTargetDecenteringSpeed(Vec2d::Zero)
                , m_cameraTargetDecenteringDestSpeed(Vec2d::Zero)
                , m_cameraBaseMovingSpeed(Vec3d::Zero)
                , m_cameraBaseMovingLockedDestY(bfalse)
                , m_cameraBasePos(Vec3d::Zero)
                , m_cameraBasePosDest(Vec3d::Zero)
                , m_cameraTargetPos(Vec3d::Zero)                
                , m_cameraBaseMovingBlend(Vec2d::Zero)
                , m_cameraTargetDecenteringSpeedAverageAbsOfTarget(Vec2d::Zero)
                , m_cameraTargetDecenteringSpeedAverageRealOfTarget(Vec2d::Zero)
                , m_cameraTargetDecenteringBlend(Vec2d::Zero)
                , m_cameraTargetDecenteringInertie(Vec2d::Zero)
                , m_cameraTargetDecenteringCoeffLast(Vec2d::Zero)
                , m_cameraBaseMovingSpeedAverageAbsOfTarget(Vec2d::Zero)
                , m_cameraBaseMovingSpeedAverageOfTarget(Vec2d::Zero)
                , m_cameraBaseMovingBlendYMinInSticked(0.f)
                , m_rayCastDownLengthMax(0.f)
                , m_rayCastDown(Vec2d::Zero)
                , m_rayCastCollisionDownFound(bfalse)       
                , m_subjectLockedPosition_blend(0.f)
                , m_weightRegisterSpeed(0.f)
                , m_registerStartPos(Vec3d::Zero)
                , m_weightTime(0.f)
                , m_isMainController(bfalse)
                , m_wasMainController(bfalse)
                , m_isPaused(bfalse)
                , m_subjectJustTeleported(bfalse)
                , m_depthRef(0.f)
                , m_ignoreZ(bfalse)
                , m_backupPreviousZ(F32_INFINITY)
            {
            }

            Controller( )
            {
                Controller(NULL);
            }

            f32 m_depthRef;

            InGameCameraComponent* m_component;
            f32   m_weight;    
            f32   m_weightTime;
            f32   m_weightDest;
            f32   m_weightRegisterSpeed;
            Vec3d m_registerStartPos;
            bbool m_isPaused;

            void    update(); 
            void    teleport(Vec3d _pos);
            bbool   isActive() const { return m_weightDest == 1.f; }

            // subject
            SubjectStance m_subjectStance;
            AABB    m_subjectAABB;
            Vec2d   m_subjectAABBCenterLast;            
            Vec2d   m_subjectSpeed;           
            Vec3d   m_subjectPosReal;
            f32     m_subjectLockedPosition_blend;
            bbool   m_subjectJustTeleported;
            void    setSubjectLockedPosition();

            bbool   m_isMainController;
            bbool   m_wasMainController;
            
            void    setSubject(const Actor* _pActor);           
            void    setSubjectSpeed( Vec2d _pos );
            void    setSubjectAABB(const Actor* _pActor);

            // zone
            AABB    m_zoneAABB;
            Vec2d   m_zoneOffsetFromSubject;
            Vec2d   m_zoneSpeedByDepth;        
            Vec2d   m_zoneSpeedReal;
            Vec2d   m_zoneAABBCenterLast;   
            void    setZone();
            void    setZoneAABB();
            void    setZoneSpeed();
            f32     getZoneOffsetXMax();
            f32     getZoneOffsetYMax();
            void    setZoneOffsetX();
            void    setZoneOffsetY();

            bbool   m_ignoreZ;
            f32     m_backupPreviousZ;
            void    setIgnoreZ(bbool _ignoreZ);

            // screen base
            AABB    m_screenBaseAABB;
            f32     m_screenRealLimitUp;
            f32     m_screenRealLimitUpDist;
            f32     m_screenRealLimitUpSpeedMax;
            f32     m_screenRealLimitUpTargetDist;
            f32     m_screenRealLimitUpRatio;
            void    setScreenBase();
            void    setControllerScreenRealLimit();
            void    setScreenBaseAABB();

            // screen target
            AABB    m_screenTargetAABB;
            void    setScreenTarget();

            // camera base
            Vec3d   m_cameraBasePos;
            Vec3d   m_cameraBasePosDest;
            Vec3d   m_cameraBaseMovingSpeed;
            Vec2d   m_cameraBaseMovingSpeedAverageAbsOfTarget;
            Vec2d   m_cameraBaseMovingSpeedAverageOfTarget;
            Vec2d   m_cameraBaseMovingBlend;
            bbool   m_cameraBaseMovingLockedDestY;
            f32     m_cameraBaseMovingBlendYMinInSticked;
            void    setCameraBaseBlendX();
            void    setCameraBaseBlendY();
            void    setCameraBasePositionX();
            void    setCameraBasePositionY();
            void    setCameraBasePositionYDest();
            void    setCameraBasePositionZ();
            void    setCameraBasePosition();
            void    setCameraBase();
            void    increazeCameraMovingBaseSpeedAverageAbsOfTarget();

            // camera target
            Vec3d   m_cameraTargetPos;
            Vec2d   m_cameraTargetDecentering;       
            Vec2d   m_cameraTargetDecenteringDest;
            Vec2d   m_cameraTargetDecenteringDestSpeed;
            Vec2d   m_cameraTargetDecenteringSpeed;
            Vec2d   m_cameraTargetDecenteringDir; 
            Vec2d   m_cameraTargetDecenteringDirFromSubjectStance;
            Vec2d   m_cameraTargetDecenteringSpeedAverageAbsOfTarget;
            Vec2d   m_cameraTargetDecenteringSpeedAverageRealOfTarget;
            Vec2d   m_cameraTargetDecenteringBlend;
            Vec2d   m_cameraTargetDecenteringInertie; 
            Vec2d   m_cameraTargetDecenteringCoeffLast;
            void    setCameraTarget();
            void    setCameraTargetPos();
            void    setCameraTargetDecenteringX();
            f32     getCameraTargetDecenteringDestinationX();
            void    setCameraTargetDecenteringDirectionX();        
            void    setCameraTargetDecentering();
            void    setCameraTargetDecenteringXBlendAndInertie();
            void    resetCameraTargetDecentering();

            // ray cast
            Vec2d   m_rayCastDown;
            f32     m_rayCastDownLengthMax;
            bbool   m_rayCastCollisionDownFound;
            bbool   isRayCastCollisionDown();
            bbool   isRayCastCollisionUp();
            void    initRayCastCollisionDown();     
                       
        };
        
        // controllers
        u32     m_controllerActiveCount;
        u32     m_controllerActiveCountLast;   
        bbool   m_isScreenLimitCrossed;
        bbool   m_resetDezoomTime;
        ITF_VECTOR<Controller> m_controllers;
        Controller* m_mainController;

        f32         m_mainActiveControllerWithoutJoyMoveTime;
        bbool       m_useVisibilityFromMainActiveController;       
        bbool       m_resetVisibilityFromMainActiveController;
        void        setMainActiveController(f32 _deltaTime);

        bbool   m_isLeaderController;
        void    setControllersPosition();
        void    addControllers(bbool _teleportIfAlone = bfalse);
        void    removeControllers();
        void    setControllersSubject();
        void    setControllersWeight(f32 _dt);
        bbool   setMainController(f32 _deltaTime);
        void    updateSubjectGlobalDatas( const Controller& _controller);

        //  subject
        f32     m_subjectPosZ;
        Vec2d   m_subjectZoneNeutral;
        f32     m_subjectRegisterSpeed;
        f32     m_subjectUnregisterSpeed;
        AABB    m_subjectGlobalAABB;
        f32     m_subjectGlobalDepthMin;        
        void    setZoneNeutral();
      
                    
        // screen target average
        AABB    m_screenTargetAverageAABB;
        void    setScreenTargetAverage();

        // camera target average
        Vec3d   m_cameraTargetAveragePos;
        Vec2d   m_cameraTargetAveragePosTrue;
        f32     m_cameraTargetAverageDezoomDestWanted;              
        f32     m_cameraTargetAverageDezoomCur;        
        void    setCameraTargetAverage();
        void    setDezoomCur();        

        // camera real
        AABB    m_screenRealAABB;
        Vec2d   m_screenRealLongBy2;
        f32     m_depth;
        Vec3d   m_cameraRealPos;
        void    setScreenReal();                
        void    setCameraReal();

		BlendedParam  m_cameraCurParam;
		BlendedParam  m_cameraLastParam;
		BlendedParam  m_cameraSpeedParam;
		BlendedParam  m_cameraFinalParam;
		BlendedParam  m_cameraOtherSpatialParam;
		f32		m_timeInterp;
		f32     m_curInertia;
		f32		m_speedInertia;
		bbool   m_useSpatial;
		bbool   m_wasSpatial;

        // modifiers
        bbool   m_constraintModifierOnEnter;
        bbool   m_constraintModifierOnExit;
        bbool   m_switchModifier;

        CameraModifierComponent*    m_cameraModifier;        
        f32     m_borderBlending;
        void    setModifiers();
        void    setModifierConstraint();
        void    setConstraintOnEnter(CameraModifierComponent* _modifier);
        void    setConstraintOnExit();
        bbool   keepModifier();
		void	getBlendedParamFrom(CameraModifierComponent* cameraModifier, BlendedParam &_blendParam);
        void    setBorderBlending();
        void    blendingModifiers();
		void    finalBlending(f32 _deltaTime);
        void    setModifierLookAtOffset( CamModifierUpdate& _cm);
        void    updateModifiers();
        void    setConstraintContinuity();
        void    addContinuityOffset(AABB& _aabb) const;
        void    resetConstraintContinuity();
        void    validateCamModifier(CameraModifierComponent *& _camMod) const;
		f32	    getCurHVS() const
		{
			return m_cameraModifier ? m_cameraModifier->getHorizontalVersusVertical() : 0.0f;
		}
		Vec2d	getCurOffsetHVS() const
		{
			return m_cameraModifier ? m_cameraModifier->getOffsetHVS() : Vec2d::Zero;
		}
                
        // constraints
        bbool   m_isConstraintContinuityAxeX;
        bbool   m_isConstraintContinuityAxeY;
        f32     m_leftConstraintContinuity;
        f32     m_rightConstraintContinuity;
        f32     m_topConstraintContinuity;
        f32     m_bottomConstraintContinuity;        
        
        CameraModifierComponent*    m_constraintModifier;        
        AABB    m_constraintAABB;       
        AABB    m_constraintAnticipAABB;
        AABB    m_screenConstraintAABB;
        Vec3d   m_constraint;
        f32     m_constraintDepthOffsetMax;
        LogisticParams m_curveLogistic;

        bbool   m_constraintMatchViewX;
        bbool   m_constraintMatchViewY;
        f32     m_constraintSecretAreaZ;
        f32     m_constraintSecretAreaZOld;        

        void    updateConstraints();
        void    setConstraintX();
        void    setConstraintY();
        void    setConstraintZ();
        void    setConstraintAABB();
        void    setConstraintTimeOnEnter();
        f32     getConstraintAnticipationX();
        f32     getConstraintAnticipationY();               
        void    resetConstraints();
        void    resetConstraintX();
        void    resetConstraintY();
        void    resetConstraintZ();
        void    setScreenConstraintAABB();
        void    testConstraintExtendedAxeX();
        void    testConstraintExtendedAxeY();    
        void    setConstraintSecretAreaZ(f32 _screenRatio);
        void    initConstraintParams();
        void    setConstraintMatchView();

        // 
        f32     m_depthCoeff;
        void    setDepthCoeff();        

        // visibility
        AABB    m_visibilityAABB;
        AABB    m_visibilityScreenMinAABB;
        AABB    m_visibilityScreenMaxAABB;        
        Vec2d   m_visibilityDest;
        Vec2d   m_visibilityCur;

        f32     m_visibilitySecretAreaZCur;
        f32     m_visibilitySecretAreaZLast;
        f32     m_visibilitySecretAreaZTime;

        Vec2d   m_visibilityOffsetCur;
        Vec2d   m_visibilityOffsetDest;

        void    setDezoomDest();
        void    setScreenAABB( AABB& _aabb, const Vec2d& _pos, f32 _depth);
        void    setScreenAABBLongBy2( Vec2d& _screenLongBy2, f32 _depth );
        void    updateVisibility();
        void    setVisibilityOffset();
        Vec2d   getVisibilityOffset();

        public:
            Controller* getMainController() { return m_mainController; }
    };   


    class InGameCameraComponent_Template : public BaseCameraComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(InGameCameraComponent_Template,BaseCameraComponent_Template,3647884283)
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(InGameCameraComponent);

    public:

        InGameCameraComponent_Template();
        ~InGameCameraComponent_Template();

        const CamModifier_Template& getCMInit() const { return m_CMInit; }

    private:

        CamModifier_Template                m_CMInit;
    };

    ITF_INLINE const InGameCameraComponent_Template* InGameCameraComponent::getTemplate() const
    {
        return static_cast<const InGameCameraComponent_Template*>(m_template);
    }
}
#endif // _ITF_INGAMECAMERACOMPONENT_H_
