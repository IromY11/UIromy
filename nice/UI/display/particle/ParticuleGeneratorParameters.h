#ifndef _ITF_PARTICULEGENERATORPARAMETERS_H_
#define _ITF_PARTICULEGENERATORPARAMETERS_H_

namespace ITF
{
    struct ITF_Particle;

    enum PARGEN_GEN_TYPE
    {
        PARGEN_GEN_POINTS = 0,
        PARGEN_GEN_RECTANGLE,
        PARGEN_GEN_CIRCLE,
        PARGEN_GEN_BEZIER,
        PARGEN_GEN_SPHERE,
        PARGEN_GEN_HEMISPHERE,
        ENUM_FORCE_SIZE_32(PARGEN_GEN_TYPE)
    };

    enum PARGEN_MODE
    {
        PARGEN_MODE_FOLLOW = 0,
        PARGEN_MODE_COMPLEX,
        PARGEN_MODE_MANUAL,
        ENUM_FORCE_SIZE_32(PARGEN_MODE)
    };

    enum PARGEN_EMITMODE
    {
        PARGEN_EMITMODE_OVERTIME = 0,
        PARGEN_EMITMODE_ALLATONCE,
        PARGEN_EMITMODE_OVERDISTANCE,
        ENUM_FORCE_SIZE_32(PARGEN_EMITMODE)
    };

    enum UVMODE
    {
        UV_Default = 0,
        UV_FlipX,
        UV_FlipY,
        UV_FlipXY,
        UV_FlipXtoDirX,
        UV_FlipYtoDirY,
        UV_FlipXYtoDirXY,
        ENUM_FORCE_SIZE_32(UVMODE)
    };

    enum UVMODEFLAG
    {
        UVF_Default       = 0x0000, 
        UVF_FlipX         = 0x0001, 
        UVF_FlipY         = 0x0002, 
        UVF_FlipXtoDirX   = 0x0004, 
        UVF_FlipYtoDirY   = 0x0008,
        UVF_FlipXRandom   = 0x0010, 
        UVF_FlipYRandom   = 0x0020,
        ENUM_FORCE_SIZE_32(UVMODEFLAG)
    };

    enum BOOL_COND
    {
        BOOL_false  = 0,
        BOOL_true   = 1,
        BOOL_cond   = 2,
        ENUM_FORCE_SIZE_32(BOOL_COND)
    };

    enum PARGEN_ONEND
    {
        OnEnd_Destroy = 0, 
        OnEnd_Keep = 1, 
        OnEnd_Loop = 2, 
        OnEnd_Freeze = 3, 
        ENUM_FORCE_SIZE_32(PARGEN_ONEND)
    };

    struct ParPhase
    {
        DECLARE_SERIALIZE()

        ParPhase() 
        {
            m_phaseTime = 1.f;
            m_colorMin = Color::white();
            m_colorMax = Color::white();
            m_sizeMin = Vec2d(1.f, 1.f);
            m_sizeMax = Vec2d(1.f, 1.f);
            m_animStart = -1;
            m_animEnd = -1;
            m_animName = StringID::InvalidId;
            m_animId = U32_INVALID;
            m_deltaphasetime = 0.f;
            m_animstretchtime = bfalse;
            m_blendToNextPhase = btrue;
        };

        f32     m_phaseTime;
        f32     m_deltaphasetime;
        Color   m_colorMin;
        Color   m_colorMax;
        Vec2d   m_sizeMin;
        Vec2d   m_sizeMax;
        i32     m_animStart;
        i32     m_animEnd;
        bbool   m_animstretchtime;
        bbool   m_blendToNextPhase;

        // MeshAnimVertex
        StringID m_animName;
        u32      m_animId;
    };

    class ParCurve : public IRTTIObject
    {
    public :
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(ParCurve,IRTTIObject,4064401154)
        ParCurve();
        bbool isSet() const {return (m_time > 0.f) && m_curve.GetNumPoints();}
        f32 getValue(f32 _t) const;
        void getValue(Vec2d& _dst, f32 _t) const;
        void getValue(Vec3d& _dst, f32 _t) const;
        Spline m_curve;
        f32 m_time, m_invtime;
        Vec3d  m_outputMin, m_outputMax;
    };

    // used for reflexion
    class ParLifeTimeCurve : public ParCurve
    {
    public :
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(ParLifeTimeCurve,ParCurve,1996869)
    };

    // used for reflexion
    class EmitLifeTimeCurve : public ParCurve
    {
    public :
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(EmitLifeTimeCurve,ParCurve,1953316454)
    };

    class ParticleGeneratorParameters
    {
        friend class ParticleGeneratorInstanceParameters;
    private:

        AABB                m_bounding;

    public:        
        DECLARE_SERIALIZE()
        ParticleGeneratorParameters();

        ITF_INLINE u32                  getMaxParticles(	) const { return m_maxParticles; }

        ITF_INLINE u32                  getNumToEmit(	) const { return m_emitParticlesCount; }

        ITF_INLINE bbool                getForceNoDynamicFog(	) const { return m_ForceNoDynamicFog; }

        ITF_INLINE bbool                getRenderInReflection(	) const { return m_renderInReflection; }

        ITF_INLINE bbool                getForceEmitAtStart(	) const { return m_ForceEmitAtStart || (m_genEmitMode==PARGEN_EMITMODE_ALLATONCE); }

        ITF_INLINE u32                  getEmitBatchCount(	) const { return (m_genEmitMode == PARGEN_EMITMODE_ALLATONCE) ? m_emitBatchCountAAO : m_emitBatchCount; }
        ITF_INLINE u32                  getEmitBatchCountMax(	) const { return (m_genEmitMode == PARGEN_EMITMODE_ALLATONCE) ? m_emitBatchCountAAOMax : U32_INVALID; }

        ITF_INLINE f32                  getFrequency(	) const { return m_freq; }

        ITF_INLINE const Vec3d&         getPosition(	) const { return m_pos; }
        ITF_INLINE const Vec3d&         getPositionOffset(	) const { return m_posOffset; }

        ITF_INLINE f32                  getVelocityNorm(	) const { return m_velNorm; }

        ITF_INLINE PARGEN_GEN_TYPE      getGeneratorGenType() const { return m_genGenType; }
        ITF_INLINE PARGEN_EMITMODE      getGeneratorEmitMode() const { return m_genEmitMode; }
        ITF_INLINE PARGEN_ONEND         getBehaviorOnEnd() const { return m_behaviorOnEnd; }
        ITF_INLINE void                 setRenderPrio(f32 _renderPrio) { m_renderPrio = _renderPrio; }
        ITF_INLINE f32                  getRenderPrio() const { return m_renderPrio; }
        ITF_INLINE f32                  getInitLifeTime() const { return m_initLifeTime; }
        ITF_INLINE f32                  getCircleRadius() const {return m_circleRadius;}
        ITF_INLINE f32                  getInnerCircleRadius()const  {return m_innerCircleRadius;}
        ITF_INLINE const Vec3d&         getScaleShape()const  {return m_scaleShape;}
        ITF_INLINE const Vec3d&         getRotateShape()const  {return m_rotateShape;}
        ITF_INLINE const Vec2d&         getPivot() const { return m_pivot; }
        ITF_INLINE Angle                getGenAngMin(	) const { return m_genAngMin; }
        ITF_INLINE Angle                getGenAngMax(	) const { return m_genAngMax; }

        ITF_INLINE const AABB&          getGenBox() const {return m_genBox;}

        f32                             computeSumPhaseTime() const;
        f32                             computeMaxSumPhaseTime() const;

        ITF_INLINE bbool                canFlipAccel()const  {return m_canFlipAccel;}
        ITF_INLINE bbool                canFlipAngleOffset()const  {return m_canFlipAngleOffset;}
        ITF_INLINE bbool                canFlipInitAngle()const  {return m_canFlipInitAngle;}
        ITF_INLINE bbool                canFlipAngularSpeed()const  {return m_canFlipAngularSpeed;}
        ITF_INLINE bbool                canFlipPivot()const  {return m_canFlipPivot;}
        ITF_INLINE bbool                canFlipUV()const  {return m_canFlipUV;}
        ITF_INLINE bbool                canFlipAngleMin()const  {return m_canFlipAngleMin;}
        ITF_INLINE bbool                canFlipAngleMax()const  {return m_canFlipAngleMax;}
        ITF_INLINE bbool                canFlipBoundingBox()const  {return m_canFlipBoundingBox;}
        ITF_INLINE bbool                CanFlipOrientDir()const  {return m_canFlipOrientDir;}

        ITF_INLINE BOOL_COND            getUseMatrix() const { return m_useMatrix; }

        ITF_INLINE const i32            getFollowBezier() const { return m_FollowBezier; }
        ITF_INLINE const bbool          getContinuousColorRandom() const { return m_continuousColorRandom; }
        ITF_INLINE const u32            getOrientDir() const { return m_orientDir; }
        ITF_INLINE const bbool          getCanClipPosOffset() const { return m_canFlipPosOffset; }
        ITF_INLINE const bbool          getAtlasSize() const { return m_getAtlasSize; }
        ITF_INLINE const bbool          getUseYMin() const { return m_useYMin; }
        ITF_INLINE const bbool          getUseYMinLocal() const { return m_useYMinLocal; }     
        ITF_INLINE const f32            getYMin() const { return m_yMin; }
        ITF_INLINE const f32            getYMinBouncinessMin() const { return m_yMinBouncinessMin; }
        ITF_INLINE const f32            getYMinBouncinessMax() const { return m_yMinBouncinessMax; }
        ITF_INLINE const u32            getYMinMinRebounds() const { return m_yMinMinRebounds; }
        ITF_INLINE const u32            getYMinMaxRebounds() const { return m_yMinMaxRebounds; }

        ITF_INLINE const bbool          getYMinKillAfterMaxRebounds() const { return m_yMinKillAfterMaxRebounds; }

        ITF_INLINE const bbool          getBouncinessAffectsXSpeed() const { return m_bouncinessAffectsXSpeed; }
        ITF_INLINE const bbool          getBouncinessAffectsAngularSpeed() const { return m_bouncinessAffectsAngularSpeed; }
       
    protected:
        Color           m_defaultColor;
        u32				m_maxParticles;
        u32				m_emitParticlesCount;

        Vec3d			m_pos;				// position.
        Vec3d			m_posOffset;    	// positionOffset.
        Vec2d           m_pivot;			// pivot.
        f32 			m_velNorm;  		// velocity.
        f32		        m_velParamAngle;    // velocity angle (Z) set by parameter.
        f32             m_velParamAngleDelta;
        Vec3d			m_vel;				// velocity (set by fxbank)
        Vec3d			m_grav;				// gravitation.
        Vec3d			m_acc;				// wind/acceleration.
        f32				m_velocityVar;      // velocity variations
        f32				m_friction;

        f32             m_depth;            // depth
        bbool           m_useZAsDepth;
        bbool           m_ForceEmitAtStart; // Force at least one particle emition at startup.

        f32				m_freq;
        f32				m_freqDelta;
        u32				m_emitBatchCount;        
        u32             m_emitBatchCountAAO;
        u32             m_emitBatchCountAAOMax;

        Angle   		m_initAngle;
        Angle		    m_angleDelta;

        Angle	     	m_angularSpeed;
        Angle   		m_angularSpeedDelta;
        f32             m_timeTarget;

        u32             m_nbPhase;
        ITF_VECTOR<ParPhase> m_phaseList;

        f32             m_renderPrio;

        f32             m_initLifeTime; // Life time at first generation

        PARGEN_GEN_TYPE     m_genGenType;
        PARGEN_MODE         m_genMode;
        PARGEN_EMITMODE     m_genEmitMode;

public:
        // Accessors should be added to protect those variables !
        ParLifeTimeCurve    m_parPosition;
        ParLifeTimeCurve    m_parAngle;
        ParLifeTimeCurve    m_parAngularSpeed;
        ParLifeTimeCurve    m_parVelocityMult;
        ParLifeTimeCurve    m_parAccelerationX;
        ParLifeTimeCurve    m_parAccelerationY;
        ParLifeTimeCurve    m_parAccelerationZ;
        ParLifeTimeCurve    m_parSize;
        ParLifeTimeCurve    m_parSizeY;
        ParLifeTimeCurve    m_parAlpha;
        ParLifeTimeCurve    m_parRGB;
        ParLifeTimeCurve    m_parRGB1;
        ParLifeTimeCurve    m_parRGB2;
        ParLifeTimeCurve    m_parRGB3;
        ParLifeTimeCurve    m_parAnim;

        EmitLifeTimeCurve   m_parEmitVelocity;
        EmitLifeTimeCurve   m_parEmitVelocityAngle;
        EmitLifeTimeCurve   m_parEmitAngle;
        EmitLifeTimeCurve   m_parEmitAngularSpeed;
        EmitLifeTimeCurve   m_genFreq;
        EmitLifeTimeCurve   m_parLifeTime;
        EmitLifeTimeCurve   m_parEmitAlpha;
        EmitLifeTimeCurve   m_parEmitAlphaInit;
        EmitLifeTimeCurve   m_parEmitColorFactor;
        EmitLifeTimeCurve   m_parEmitColorFactorInit;
        EmitLifeTimeCurve   m_parEmitSizeXY;
        EmitLifeTimeCurve   m_parEmitSizeXYInit;
        EmitLifeTimeCurve   m_parEmitGravity;
        EmitLifeTimeCurve   m_parEmitAcceleration;
        EmitLifeTimeCurve   m_parEmitAnim;

protected:
        /// params for gen Type:
        /// circle:
        f32                 m_circleRadius;
        f32                 m_innerCircleRadius;
        Vec3d               m_scaleShape;
        Vec3d               m_rotateShape;
        /// rectangle:
        AABB                m_genBox;
        /// bezier:
        f32                 m_genSize; // bezier generation band width
        u32                 m_genSide; // 0 both, 1 up, 2 down.
        f32                 m_genBezierStart;
        f32                 m_genBezierEnd;
        f32                 m_genBezierDensity;

        /// options:
        u32                 m_orientDir;
        UVMODE              m_UVmodeDeprecated;
        UVMODEFLAG          m_UVmodeFlag;
        f32                 m_uniformScale;
        i32                 m_FollowBezier;

        ///
        Angle               m_genAngMin;
        Angle               m_genAngMax;

        bbool               m_randomizeDirection;
        bbool               m_getAtlasSize;
        bbool               m_continuousColorRandom;
        bbool               m_canFlipAngleOffset;
        bbool               m_canFlipInitAngle;
        bbool               m_canFlipAngularSpeed;
        bbool               m_canFlipPivot;
        bbool               m_canFlipPosOffset;
        bbool               m_canFlipUV;
        bbool               m_canFlipAngleMin;
        bbool               m_canFlipAngleMax;
        bbool               m_canFlipAccel;
        bbool               m_canFlipBoundingBox;
        bbool               m_canFlipOrientDir;
        bbool               m_disableLight;
        bbool               m_usePhasesColorAndSize;
        bbool               m_useActorTranslation;

        bbool               m_useYMin;
        bbool               m_useYMinLocal;

        bbool               m_ForceNoDynamicFog;
        bbool               m_renderInReflection;

        bbool               m_yMinKillAfterMaxRebounds;

        // Collisions
        bbool               m_bouncinessAffectsXSpeed;
        bbool               m_bouncinessAffectsAngularSpeed;

        BOOL_COND           m_useMatrix;
        BOOL_COND           m_scaleGenBox;

        Vec2d               m_actorTranslationOffset;

        f32                 m_dieFadeTime;
        f32                 m_emitterMaxLifeTime;
        PARGEN_ONEND        m_behaviorOnEnd;

        // Y Min
        f32                 m_yMin;
        f32                 m_yMinBouncinessMin;
        f32                 m_yMinBouncinessMax;
        u32                 m_yMinMinRebounds;
        u32                 m_yMinMaxRebounds;
        
        /// splitmode.
        u32                 m_nSplit;
        Angle               m_splitDelta;
    };

    class ParticleGeneratorInstanceParameters
    {
        friend class ITF_ParticleGenerator;
    private:

        AABB                m_bounding;
        AABB                m_boundingWorking;//the bounding box used by thread

    public:        
        ParticleGeneratorInstanceParameters();
        void initFromTemplate(const ParticleGeneratorParameters &_parameters);
        void setMaxParticles( u32 _maxParticles ) { m_maxParticles = _maxParticles; }

        ITF_INLINE u32 getMaxParticles(	) const { return m_maxParticles; }

        ITF_INLINE void setNumToEmit( u32 _emitParticlesCount ) { m_emitParticlesCount = _emitParticlesCount; }
        ITF_INLINE u32 getNumToEmit(	) const { return m_emitParticlesCount; }

        ITF_INLINE void setFrequency( f32 _freq ) { m_freq = _freq; }
        ITF_INLINE f32 getFrequency(	) const { return m_freq; }
        ITF_INLINE f32 getFrequencyDelta(	) const { return m_freqDelta; }

        ITF_INLINE void setPosition( const Vec3d& _pos ) { m_pos = _pos; }
        ITF_INLINE const Vec3d& getPosition(	) const { return m_pos; }
        ITF_INLINE const Vec3d& getPositionOffset(	) const { return m_posOffset; }

        ITF_INLINE void setDepth( f32 _depth ) { m_depth = _depth; }
        ITF_INLINE f32  getDepth(	) const { return m_depth; }
        ITF_INLINE void useDepth( bbool _useDepth ) { m_useZAsDepth = !_useDepth; }

        void                    setVelocity( const Vec3d& _vel );
        ITF_INLINE Vec3d        getVelocity(	) const { return m_vel; }

        ITF_INLINE void         setVelocityNorm( f32 _velNorm ) { m_velNorm = _velNorm; }
        ITF_INLINE f32          getVelocityNorm(	) const { return m_velNorm; }

        ITF_INLINE void         setFriction( f32 _friction) { m_friction = _friction; }
        ITF_INLINE f32          getFriction(	) const { return m_friction; }

        ITF_INLINE void         setGravity( const Vec3d& _grav ) { m_grav = _grav; }
        ITF_INLINE const Vec3d& getGravity(	) const { return m_grav; }

        ITF_INLINE void         setAcc( Vec3d _acc ) { m_acc = _acc; }
        ITF_INLINE const Vec3d& getAcc(	) const { return m_acc; }

        void setIs2D(bbool _bval) {m_is2d = _bval;}
        bbool getIs2D() const     {return m_is2d;}

    protected:
        void setMaterial( const GFX_MATERIAL & _material) { m_material = _material; }

    public:
        ITF_INLINE const GFX_MATERIAL&  getMaterial(	) const { return m_material; }

        ITF_INLINE void     setAngularSpeed( Angle _angSpeed ) { m_angularSpeed = _angSpeed; }
        ITF_INLINE Angle    getAngularSpeed(	) const { return m_angularSpeed; }

        ITF_INLINE void     setAngularSpeedDelta( Angle _angSpeedDt ) { m_angularSpeedDelta = _angSpeedDt; }
        ITF_INLINE Angle    getAngularSpeedDelta(	) const { return m_angularSpeedDelta; }

        ITF_INLINE void     setAngle( Angle _min ) { m_initAngle = _min; }
        ITF_INLINE Angle    getAngle(	) const { return m_initAngle; }

        ITF_INLINE void     setAngleDelta( Angle _delta ) { m_angleDelta = _delta; }
        ITF_INLINE Angle    getAngleDelta(	) const { return m_angleDelta; }

        ITF_INLINE void     setVelocityVar( f32 _velocityVar ) { m_velocityVar = _velocityVar; }
        ITF_INLINE f32      getVelocityVar() const { return m_velocityVar; }

        ITF_INLINE void             setGeneratorGenType( PARGEN_GEN_TYPE _genType ) { m_genGenType = _genType; }
        ITF_INLINE PARGEN_GEN_TYPE  getGeneratorGenType() const { return m_genGenType; }

        ITF_INLINE void             setGeneratorMode( PARGEN_MODE _genMode ) { m_genMode = _genMode; }
        ITF_INLINE PARGEN_MODE      getGeneratorMode() const { return m_genMode; }

        ITF_INLINE void             setBehaviorOnEnd( PARGEN_ONEND _behaviorOnEnd ) { m_behaviorOnEnd = _behaviorOnEnd; }
        ITF_INLINE PARGEN_ONEND     getBehaviorOnEnd() { return m_behaviorOnEnd; }

        ITF_INLINE void             setRenderPrio(f32 _renderPrio) { m_renderPrio = _renderPrio; }
        ITF_INLINE f32              getRenderPrio() const { return m_renderPrio; }

        ITF_INLINE void         setBoxMin( const Vec2d& _vMin ) { m_genBox.setMin(_vMin); }
        ITF_INLINE const Vec2d& getBoxMin() const { return m_genBox.getMin(); }

        ITF_INLINE void         setBoxMax( const Vec2d& _vMax ) { m_genBox.setMax(_vMax); }
        ITF_INLINE const Vec2d& getBoxMax() const { return m_genBox.getMax(); }

        ITF_INLINE void         setAABBMin( const Vec2d& _vMin ) { m_bounding.setMin(_vMin); }
        ITF_INLINE const Vec2d& getAABBMin() const { return m_bounding.getMin(); }

        ITF_INLINE void         setAABBMax( const Vec2d& _vMax ) { m_bounding.setMax(_vMax); }
        ITF_INLINE const Vec2d& getAABBMax() const { return m_bounding.getMax(); }

        ITF_INLINE void       setUVmodeFlag(UVMODEFLAG _modeFlag) { m_UVmodeFlag = _modeFlag; }
        ITF_INLINE UVMODEFLAG getUVmodeFlag() { return m_UVmodeFlag; }

        ITF_INLINE void             setPivot( const Vec2d& _pivot ) { m_pivot = _pivot; }
        ITF_INLINE const Vec2d&     getPivot() const { return m_pivot; }

        ITF_INLINE void     setGenAngMin( Angle _ang ) { m_genAngMin = _ang; }
        ITF_INLINE Angle    getGenAngMin(	) const { return m_genAngMin; }

        ITF_INLINE void     setGenAngMax( Angle _ang ) { m_genAngMax = _ang; }
        ITF_INLINE Angle    getGenAngMax(	) const { return m_genAngMax; }

        ITF_INLINE void          setDefaultColor( const Color& _color ) { m_defaultColor = _color; }
        ITF_INLINE const Color&  getDefaultColor() const { return m_defaultColor; }
        ITF_INLINE Color&        getDefaultColor() { return m_defaultColor; }

        bbool getDisableLight() const       { return m_disableLight; }
        void setDisableLight(bbool _v)      {m_disableLight = _v;}

        AABB& getGenBox()                   {return m_genBox;}
        AABB& getBoundingBox()              {return m_bounding;}
        AABB& getBoundingBoxWorking()       {return m_boundingWorking;}

        void setWorkingBoundingBox(const AABB& _aabb) {m_boundingWorking = _aabb;}

        void synchronizeBoundingBox() {m_bounding = m_boundingWorking;}

        void setBoundingBox(const AABB& _aabb) {m_bounding = _aabb;}

        /// Phases.
        void setNumberPhase( u32 _nphase );

        ITF_INLINE u32 getNumberPhase( ) const {return m_nbPhase;} 
        void initFromPhase(ITF_Particle* _par, f32 _time, const class ITF_ParticleGenerator* _owner) const;
        void initToNextPhase(ITF_Particle* _par, f32 _time, const class ITF_ParticleGenerator* _owner) const;
        void getDstBlend(ITF_Particle* _par, const class ITF_ParticleGenerator* _gen) const;

        ITF_INLINE void     setPhaseTime( u32 _nphase, f32 _time ) { m_phaseList[_nphase].m_phaseTime = _time; }
        ITF_INLINE f32      getPhaseTime(    u32 _nphase	) const { return m_phaseList[_nphase].m_phaseTime; }

        ITF_INLINE void     setDeltaPhaseTime( u32 _nphase, f32 _time ) { m_phaseList[_nphase].m_deltaphasetime = _time; }
        ITF_INLINE f32      getDeltaPhaseTime(    u32 _nphase	) const { return m_phaseList[_nphase].m_deltaphasetime; }

        ITF_INLINE void     setPhaseAnimStart( u32 _nphase, i32 _v ) { m_phaseList[_nphase].m_animStart = _v; }
        ITF_INLINE i32      getPhaseAnimStart(    u32 _nphase	) const { return m_phaseList[_nphase].m_animStart; }

        ITF_INLINE void     setPhaseAnimEnd( u32 _nphase, i32 _v ) { m_phaseList[_nphase].m_animEnd = _v; }
        ITF_INLINE i32      getPhaseAnimEnd(    u32 _nphase	) const { return m_phaseList[_nphase].m_animEnd; }

        ITF_INLINE void         setPhaseSizeMin( u32 _nphase, Vec2d _sizemin ) { m_phaseList[_nphase].m_sizeMin = _sizemin; }
        ITF_INLINE const Vec2d& getPhaseSizeMin(	u32 _nphase)const  { return m_phaseList[_nphase].m_sizeMin; }

        ITF_INLINE void         setPhaseSizeMax( u32 _nphase, Vec2d _sizemax ) { m_phaseList[_nphase].m_sizeMax = _sizemax; }
        ITF_INLINE const Vec2d& getPhaseSizeMax(	u32 _nphase) const { return m_phaseList[_nphase].m_sizeMax; }

        ITF_INLINE void         setPhaseColorMin( u32 _nphase, const Color& _colormin ) { m_phaseList[_nphase].m_colorMin = _colormin; }
        ITF_INLINE const Color& getPhaseColorMin( u32 _nphase) const { return m_phaseList[_nphase].m_colorMin; }

        ITF_INLINE void         setPhaseColorMax( u32 _nphase, const Color& _colormax ) { m_phaseList[_nphase].m_colorMax = _colormax; }
        ITF_INLINE const Color& getPhaseColorMax( u32 _nphase) const { return m_phaseList[_nphase].m_colorMax; }

        ITF_INLINE void         setPhase(u32 _index, ParPhase & _phase)
        {
            setPhaseTime(_index,_phase.m_phaseTime);
            setPhaseSizeMin(_index,_phase.m_sizeMin);
            setPhaseSizeMax(_index,_phase.m_sizeMax);
            setPhaseColorMin(_index,_phase.m_colorMin);
            setPhaseColorMax(_index,_phase.m_colorMax);
        }

        ITF_INLINE void     setLifeTimeMultiplier( f32 _mult ) { m_lifeTimeMultiplier = (_mult >= 0.0f) ? _mult : 1.0f ;}
        ITF_INLINE f32      getLifeTimeMultiplier() const { return m_lifeTimeMultiplier;}

        ITF_INLINE bbool    getRandomizeDirection() const { return m_randomizeDirection; }
        ITF_INLINE void     setRandomizeDirection(bbool _val) { m_randomizeDirection = _val; }

        ITF_INLINE bbool    isFlip()const  {return m_flipped;}

        ITF_INLINE const Vec2d & getActorTranslationOffset() const { return m_actorTranslationOffset; }

    public:
        Color           m_defaultColor;
        u32				m_maxParticles;
        u32				m_emitParticlesCount;

        Vec3d			m_pos;				// position.
        Vec3d			m_posOffset;    	// positionOffset.
        Vec2d           m_pivot;			// pivot.
        f32 			m_velNorm;  		// velocity.
        f32		        m_velParamAngle;    // velocity angle (Z) set by parameter.
        f32             m_velParamAngleDelta;
        Vec3d			m_vel;				// velocity (set by fxbank)
        Vec3d			m_grav;				// gravitation.
        Vec3d			m_acc;				// wind/acceleration.
        f32				m_velocityVar;      // velocity variations
        f32				m_friction;

        f32             m_depth;            // depth

        f32				m_freq;
        f32				m_freqDelta;

        Angle   		m_initAngle;
        Angle		    m_angleDelta;

        Angle	     	m_angularSpeed;
        Angle   		m_angularSpeedDelta;        

        u32             m_nbPhase;
        ITF_VECTOR<ParPhase> m_phaseList;

        f32             m_renderPrio;

        // Internal modifier
        f32             m_lifeTimeMultiplier;

        /// Generator.
        GFX_MATERIAL        m_material;

        PARGEN_GEN_TYPE     m_genGenType;
        PARGEN_MODE         m_genMode;
        PARGEN_ONEND        m_behaviorOnEnd;


        /// params for gen Type:
        /// rectangle:
        AABB                m_genBox;
        /// bezier:
        f32                 m_genSize; // bezier generation band width
        u32                 m_genSide; // 0 both, 1 up, 2 down.
        f32                 m_genBezierStart;
        f32                 m_genBezierEnd;
        f32                 m_genBezierDensity;

        /// options:
        UVMODEFLAG          m_UVmodeFlag;
        f32                 m_uniformScale;

        ///
        Angle               m_genAngMin;
        Angle               m_genAngMax;

        Vec2d               m_actorTranslationOffset;

        f32                 m_dieFadeTime;
        f32                 m_emitterMaxLifeTime;

        /// splitmode.
        u32                 m_nSplit;
        Angle               m_splitDelta;

        // AnimMeshVertex
        u32                 m_amvID; // cf. ANIM_MANAGER->getAnimMeshVertexIdFromMaterial

        BOOL_COND           m_useMatrix;
        BOOL_COND           m_scaleGenBox;

        bbool               m_disableLight;
        bbool               m_usePhasesColorAndSize;
        bbool               m_useActorTranslation;

        bbool               m_useZAsDepth;
        bbool               m_ForceEmitAtStart; // Force at least one particle emition at startup.
        bbool               m_is2d;
        bbool               m_randomizeDirection;


    private:
        bbool               m_flipped;
    };

} // namespace ITF

#endif // _ITF_PARTICULEGENERATORPARAMETERS_H_
