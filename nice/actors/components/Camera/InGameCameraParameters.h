#ifndef _ITF_INGAMECAMERAPARAMETERS_H_
#define _ITF_INGAMECAMERAPARAMETERS_H_

namespace ITF
{
    struct CamModifier_Template
    {
        DECLARE_SERIALIZE()

        CamModifier_Template()
            : m_ignoreZ(bfalse)                                         
            , m_zoneScaleDepthMin(8.f)
            , m_zoneScaleDepthMax(22.f)
            , m_zoneScaleAtDepthMin(Vec2d(0.1f,0.1f))
            , m_zoneScaleAtDepthMax(Vec2d(2.f,2.f))
            , m_zoneScaleSpeed(0.01f)
            
            // camera decentering
            , m_cameraDecenteringOffsetAtSpeedMin(Vec2d(0.1f, 0.1f))
            , m_cameraDecenteringOffsetAtSpeedMax(Vec2d(0.1f, 0.1f))
            , m_cameraDecenteringSpeedMin(Vec2d::Zero)
            , m_cameraDecenteringSpeedMax(Vec2d::Zero)
            , m_cameraDecenteringBlendAtSpeedMin(Vec2d::Zero)
            , m_cameraDecenteringBlendAtSpeedMax(Vec2d::Zero)
            , m_cameraDecenteringInertieAtSpeedMin(Vec2d::Zero)
            , m_cameraDecenteringInertieAtSpeedMax(Vec2d::Zero)
            , m_cameraDecenteringSmooth(Vec2d::Zero)            
            , m_cameraDecenteringSubjectLookDirWeight(Vec2d::Zero)
            , m_cameraDecenteringDepthMin(0.f)
            , m_cameraDecenteringDepthMax(0.f)
            // camera moving
            , m_cameraMovingBlendAtSpeedMin(Vec2d::Zero)
            , m_cameraMovingBlendAtSpeedMax(Vec2d::Zero)
            , m_cameraMovingSpeedMin(Vec2d::Zero)
            , m_cameraMovingSpeedMax(Vec2d::Zero)
            , m_cameraMovingSmooth(Vec2d::Zero)            
            , m_cameraLookAtOffset(Vec3d::Invalid)
            , m_cameraLookAtOffsetYUp(0.f)
            , m_cameraLookAtOffsetYDown(0.f)
            , m_cameraMovingBlendAccelerationJustSticked(0.f)
            , m_rayCastScaleMax(0.f)
            // screen
            , m_screenLimitUpSpeedMax(0.f)
            , m_screenLimitUpScale(0.f)

            // multi players
            , m_cameraLookAtOffsetMaxInMulti(Vec3d::Invalid)                        
            , m_subjectWeightRegisterDelay(0.f)
            , m_subjectWeightUnregisterDelay(0.f)
            , m_zoomBlendInMultiplayer(F32_INVALID)
            , m_subjectMainVisibilityHorizontal(Vec2d::One)
            , m_subjectMainVisibilityVertical(Vec2d::One)
            , m_cameraMovingSmoothCoeffForLeader(0.5f)
            , m_subjectMainVisibilitySwitchAxeSpeed(0.f)

            // lock position
            , m_lockPositionBlendOnEnter(0.01f)
            , m_lockPositionBlendOnExit(0.1f)

            , m_isMainSubject(btrue)
            , m_isMainDRCPlayer(bfalse)
            , m_isLockedAxe(bfalse)
            , m_isFixed(bfalse)
        {
        }
        
        bbool   m_ignoreZ;
        bbool   m_isMainSubject;
        bbool   m_isMainDRCPlayer;
        bbool   m_isLockedAxe;
        bbool   m_isFixed;
        Angle   m_focale;

        f32     m_zoneScaleDepthMin;
        f32     m_zoneScaleDepthMax;
        Vec2d   m_zoneScaleAtDepthMin;
        Vec2d   m_zoneScaleAtDepthMax;
        f32     m_zoneScaleSpeed;
                        
        Vec2d   m_cameraDecenteringOffsetAtSpeedMin;
        Vec2d   m_cameraDecenteringOffsetAtSpeedMax;
        Vec2d   m_cameraDecenteringSpeedMin;
        Vec2d   m_cameraDecenteringSpeedMax;
        Vec2d   m_cameraDecenteringBlendAtSpeedMin;
        Vec2d   m_cameraDecenteringBlendAtSpeedMax;
        Vec2d   m_cameraDecenteringInertieAtSpeedMin;
        Vec2d   m_cameraDecenteringInertieAtSpeedMax;
        Vec2d   m_cameraDecenteringSmooth;
        Vec2d   m_cameraDecenteringSubjectLookDirWeight;        
        f32     m_cameraDecenteringDepthMin;
        f32     m_cameraDecenteringDepthMax;

        Vec2d   m_cameraMovingBlendAtSpeedMin;
        Vec2d   m_cameraMovingBlendAtSpeedMax;
        Vec2d   m_cameraMovingSpeedMin;
        Vec2d   m_cameraMovingSpeedMax;
        Vec2d   m_cameraMovingSmooth;
        f32     m_cameraMovingBlendAccelerationJustSticked;
        f32     m_rayCastScaleMax;

        Vec3d   m_cameraLookAtOffset;
        f32     m_cameraLookAtOffsetYUp;
        f32     m_cameraLookAtOffsetYDown;

        // screen
        f32     m_screenLimitUpScale;
        f32     m_screenLimitUpSpeedMax;

        // multi players
        Vec3d   m_cameraLookAtOffsetMaxInMulti;                       
        f32     m_subjectWeightRegisterDelay;
        f32     m_subjectWeightUnregisterDelay;
        Vec2d   m_subjectMainVisibilityHorizontal;
        Vec2d   m_subjectMainVisibilityVertical;
        f32     m_cameraMovingSmoothCoeffForLeader;
        f32     m_subjectMainVisibilitySwitchAxeSpeed;

        // zoom
        f32     m_zoomBlendInMultiplayer;

        // lock position
        f32     m_lockPositionBlendOnEnter;
        f32     m_lockPositionBlendOnExit;
    };

    ///////////////////////////////////////////////////////////////////////////////
    ///Parameters for InGameCameraComponent : Area, constraints, alterations of the camera
    struct ConstraintExtended
    {
        DECLARE_SERIALIZE()

        ConstraintExtended()
            : m_offset(0.f)
            , m_timeToIncrease(1.f)
            , m_timeToDecrease(2.f)
            , m_timeToWaitBeforeDecrease(5.f)
        {
        }

        f32 m_offset;
        f32 m_timeToIncrease;
        f32 m_timeToDecrease;
        f32 m_timeToWaitBeforeDecrease;
    };

    enum CameraFlipView
    {
        CameraFlip_None = 0,
        CameraFlip_X,
        CameraFlip_Y,
        CameraFlip_Both,
        ENUM_FORCE_SIZE_32(CameraFlipView)
    };

    struct CamModifier //rename to InGameCameraParameters
    {
        DECLARE_SERIALIZE()

        CamModifier()
            : m_constraintLeftIsActive(btrue)
            , m_constraintRightIsActive(btrue)
            , m_constraintTopIsActive(btrue)
            , m_constraintBottomIsActive(btrue)
            , m_constraintMatchView(bfalse)    
            , m_cameraModifierPriority(0)
            , m_blendingZoneStart(0.f)
            , m_blendingZoneStop(0.f)
            , m_zoneNeutral(Vec2d::Zero)
            , m_useDecentering(btrue)        
			, m_UpDnAngle(Angle::Zero)
            , m_lookAt(Vec3d(0.f,0.f,13.f))
            , m_lookAtMax(Vec3d(0.f,0.f,22.f))
            , m_rotationSpeed(0.1f)
            , m_flipView(CameraFlip_None)
			, m_transitionTime(2.0f)
			, m_moveInertia(0.0f)
			, m_horizontalVersusVertical(0.0f)
			, m_offsetHVS(0.0f, 0.0f)
        {
        }

        i32     m_cameraModifierPriority;        
        bool    m_constraintLeftIsActive;
        bool    m_constraintRightIsActive;
        bool    m_constraintTopIsActive;
        bool    m_constraintBottomIsActive;
        bool    m_constraintMatchView;
        Vec2d   m_zoneNeutral;
        bool    m_useDecentering;

		Angle   m_UpDnAngle;
        Angle   m_rotationAngle;
        f32     m_rotationSpeed;
        CameraFlipView m_flipView;

        Vec3d   m_lookAt;
        Vec3d   m_lookAtMax;

        ConstraintExtended  m_constraintExtendedLeft;
        ConstraintExtended  m_constraintExtendedRight;
        ConstraintExtended  m_constraintExtendedTop;
        ConstraintExtended  m_constraintExtendedBottom;

        f32     m_blendingZoneStart;
        f32     m_blendingZoneStop;
		f32		m_transitionTime;
		f32     m_moveInertia;
		f32		m_horizontalVersusVertical;
		Vec2d	m_offsetHVS;
    };

    struct CamModifierUpdate
    {
        CamModifierUpdate()
            : m_camModifier(NULL)
            , m_camModifierTemplate(NULL)
            , m_focale(MTH_PIBY4)
            , m_focale_SpeedBlend(0.f)
            , m_cameraLookAtOffsetYUp(0.f)
            , m_cameraLookAtOffsetYDown(0.f)
            , m_cameraLookAtOffset(Vec3d::Zero)
            , m_cameraLookAtOffsetMaxInMulti(Vec3d::Zero)
            , m_cameraModifierDirectionNormalized(Vec2d::Right)
            , m_constraintLeftIsActive(bfalse)
            , m_constraintRightIsActive(bfalse)
            , m_constraintTopIsActive(bfalse)
            , m_constraintBottomIsActive(bfalse)
            , m_constraintMatchView(bfalse)
            , m_ignoreZ(bfalse)
            , m_directionLookAtOffset(0.f)
            , m_directionLookAtOffset_SpeedBlend(0.f)
            , m_isMainSubject(btrue)
            , m_isLockedAxeX(bfalse)
            , m_isLockedAxeY(bfalse)
        {
        }

        void                                                getModifierList( CamModifierUpdate& _to ) const;
        void                                                init( const CamModifier* _cm, const CamModifier_Template* _cmTemplate );
        void                                                init( const CamModifier_Template* _cmTemplate );
        void                                                clear();
        const Vec2d&                                        getDirection() const {return m_cameraModifierDirectionNormalized;}
        void                                                setIgnoreZ(bbool _ignoreZ) { m_ignoreZ = _ignoreZ; }

        const CamModifier*                                  m_camModifier;
        const CamModifier_Template*                         m_camModifierTemplate;
        f32                                                 m_focale;
        f32                                                 m_focale_SpeedBlend;
        f32                                                 m_cameraLookAtOffsetYUp;
        f32                                                 m_cameraLookAtOffsetYDown;
        Vec3d                                               m_cameraLookAtOffset;
        Vec3d                                               m_cameraLookAtOffsetMaxInMulti;
        Vec2d                                               m_cameraModifierDirectionNormalized;
        bbool                                               m_ignoreZ;
        bbool                                               m_constraintLeftIsActive;
        bbool                                               m_constraintRightIsActive;
        bbool                                               m_constraintTopIsActive;
        bbool                                               m_constraintBottomIsActive;
        bbool                                               m_constraintMatchView;
        f32                                                 m_directionLookAtOffset;
        f32                                                 m_directionLookAtOffset_SpeedBlend;        
        bbool                                               m_isMainSubject;
        bbool                                               m_isLockedAxeX;
        bbool                                               m_isLockedAxeY;

        static void update_focale(const CamModifier_Template& _from, CamModifierUpdate &_to) { _to.m_focale = _from.m_focale.ToRadians(); }
        static void update_constraintLeftIsActive(const CamModifier& _from, CamModifierUpdate &_to) { _to.m_constraintLeftIsActive = _from.m_constraintLeftIsActive; }
        static void update_constraintRightIsActive(const CamModifier& _from, CamModifierUpdate &_to) { _to.m_constraintRightIsActive = _from.m_constraintRightIsActive; }
        static void update_constraintTopIsActive(const CamModifier& _from, CamModifierUpdate &_to) { _to.m_constraintTopIsActive = _from.m_constraintTopIsActive; }
        static void update_constraintBottomIsActive(const CamModifier& _from, CamModifierUpdate &_to) { _to.m_constraintBottomIsActive = _from.m_constraintBottomIsActive; }
        static void update_constraintMatchView(const CamModifier& _from, CamModifierUpdate &_to) { _to.m_constraintMatchView = _from.m_constraintMatchView; }                    

        typedef void (*ingameCameraParameterTemplateFncDecl)(const CamModifier_Template&, CamModifierUpdate&); // from, to
        typedef void (*ingameCameraParameterInstanceFncDecl)(const CamModifier&, CamModifierUpdate&); // from, to

        ITF_VECTOR<ingameCameraParameterTemplateFncDecl>    m_updateModifiersListTemplate;
        ITF_VECTOR<ingameCameraParameterInstanceFncDecl>    m_updateModifiersListInstance;


    };
}

#endif
