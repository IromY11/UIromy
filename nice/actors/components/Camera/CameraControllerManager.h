#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#define _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_ICAMERACONTROLLER_H_
#include "engine/display/ICameraController.h"
#endif //_ITF_ICAMERACONTROLLER_H_

#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#include "engine/actors/components/Camera/CameraModifierComponent.h"
#endif //_ITF_CAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_CAMERASHAKE_H_
#include "engine/actors/components/Camera/CameraShake.h"
#endif // _ITF_CAMERASHAKE_H_

namespace ITF
{
    class AABB3d;
    class FrustumDesc;

    enum CamIds
    {
        CAMID_MAIN          = 0x0001,
        CAMID_REMOTE        = 0x0002,
       // CAMID_ETC           = 0x0004,
        CAMID_COUNT_MASK    = 0x0003,
    };

    class CameraControllerData 
    {
    public:
        CameraControllerData( ICameraController* _controller )
            : m_controller(_controller)
            , m_finalWeight(0.f)
            , m_destinationWeight(0.f)
            , m_weight(0.f)
            , m_mainPriority(0)
            , m_prevSpeed(Vec3d::Zero)
        {
            m_prevCamData.m_pos = Vec3d::Infinity;
        }

        ICameraController*      m_controller;
        f32                     m_finalWeight;
        f32                     m_destinationWeight;
        f32                     m_weight;

        bbool                   hasPrevData() const { return m_prevCamData.m_pos != Vec3d::Infinity; }
        CameraData              m_prevCamData;
        Vec3d                   m_prevSpeed;
        u32                     m_mainPriority;

        ITF_INLINE const Vec3d& getPosition() const  { return m_prevCamData.m_pos; }
        ITF_INLINE f32          getFocale() const { return m_prevCamData.m_focale; }
    };

    struct SubjectPerform
    {
        SubjectPerform()
            : m_justJumped(bfalse)
            , m_justBounceJumped(bfalse)
            , m_justReceivedPunchUp(bfalse)
        {}

        bbool       m_justJumped;
        bbool       m_justBounceJumped;
        bbool       m_justReceivedPunchUp;
        void        reset() { m_justJumped=bfalse; m_justBounceJumped=bfalse; m_justReceivedPunchUp=bfalse; }
    };

    class CameraControllerSubject 
    {
    public:
        CameraControllerSubject()
            : m_delayToRegister(0.f)
            , m_isPlayer(btrue)
            , m_leadCamera(bfalse)
        {

        }
        ObjectRef   m_ref;
        ObjectRef   m_refOld;
        f32         m_delayToRegister;
        bbool       m_isPlayer;
        bbool       m_leadCamera;
        SubjectPerform m_subjectPerform;
    };

    class CameraControllerManager : public IEventListener
    {
    public:

        struct BindedObject
        {
            ObjectRef   m_objectRef;
            bbool       m_updateInitPos;
            Vec3d       m_offset;
            Vec3d       m_initialOffset;
            Vec3d       m_lastComputedPos;
            bbool       m_ignoreZ;
        };

        template <class T> static void  create(u32 _id)    
        { 
            u32 numInstances = s_CCManagerInstance.size();
            for ( u32 i = 0; i < numInstances; i++ )
            {
                CameraControllerManager* camMgr = s_CCManagerInstance[i];

                if ( camMgr->getId() == _id )
                {
                    SF_DEL(camMgr);
                    s_CCManagerInstance.eraseKeepOrder(i);
                    break;
                }
            }

            CameraControllerManager* pInstance = newAlloc(mId_Gameplay, T());
            ITF_ASSERT_MSG( (_id&(_id-1))==0, "Id camera must be power of 2, use the enum CamIds please" );
            pInstance->m_id = _id;

            s_CCManagerInstance.push_back(pInstance);
        }

        static void                     destroy();        
        static void                     registerSubject( u32 _mask, ObjectRef _subject, f32 _delayToRegister = 0.f, bbool _isPlayer = btrue, bbool _doTeleport = btrue, bbool _leadCamera = bfalse, bbool _resetIfAlone = btrue );
        static void                     setLeaderSubject( u32 _mask, ObjectRef _subject);
        static void                     unregisterSubject( u32 _mask, ObjectRef _subject);
        static u32                      getIsSubjectRegisteredMask( ObjectRef _subject );
        static void                     swapSubjects( u32 _mask, ObjectRef _subjectRefFrom, ObjectRef _subjectRefTo );
        static CameraControllerManager* getManagerFromId( u32 _id );
        static CameraControllerManager* getManagerFromIndex( u32 _index ) { return s_CCManagerInstance[_index]; }
        static u32                      getManagerCount() { return s_CCManagerInstance.size(); }
        static u32                      getManagerIndexFromId( u32 _id );
        static void                     init( u32 _mask );
        static void                     onSwitchToWorld( u32 _mask, World* _pWorld); 
        static void                     unregisterNPCFromCamera( u32 _mask );
        static void                     unregisterAllSubjects( u32 _mask );
        static void                     setSubjectJustJumped( u32 _mask, ObjectRef _subject );   
        static void                     setSubjectJustReceivedPunchUp( u32 _mask, ObjectRef _subject );
        static void                     setSubjectJustBounceJumped( u32 _mask, ObjectRef _subject );
        static void                     onEndUpdateProcess( u32 _mask, f32 _dt);
        virtual void                    onEndUpdateProcess(f32 /*_dt*/);

        static void                     startCameraSequence ( u32 _mask, const Vec3d& _pos, f32 _blendIn = 0.05f, f32 _depth = F32_INFINITY, bbool _unregisterPlayer = bfalse, f32 _focale = MTH_PIBY4 );
        static void                     stopCameraSequence ( u32 _mask, f32 _blendOut = 0.05f );
        static void                     setCameraSequencePosition ( u32 _mask, const Vec3d& _pos, f32 _depth = F32_INFINITY );
        static void                     resetInGameCamera(u32 _mask);

        static void                     updateRegisteredSubjectStatus( u32 _mask, ObjectRef _ref, bbool _leadCamera );

        static FixedArray<CameraControllerManager*,2>    s_CCManagerInstance;

        CameraControllerManager();
        virtual ~CameraControllerManager();
        
        Camera *                getCamera() const;                       		
        f32     m_screenRatio;
		f32     m_wantedScreenRatio;
        bbool   m_setZoneNeutralDest;

        f32             getTargetZ()    {return m_camData.m_pos.z()-m_camData.m_depth;}
        const AABB&     getAABB() const { return m_screenAABB; }
        bbool           getAABB( const f32 _z, AABB& _aabb ) const;
        bbool           projectAABB( AABB& _aabb, f32 _curZ, f32 _destZ ) const;
        const CameraData& getCamData() const { return m_camData; }
        const Vec3d&    getCamPos() const { return m_camData.m_pos; }
        const Vec3d&    getCamSpeed() const { return m_camSpeedInGame; }
        void            setCamPos(Vec3d& _pos) { m_camData.m_pos = _pos; }
        bbool           updateCamControllers_SubjectList() { return m_updateCamControllers_SubjectList;}
        f32             getDepth() const { return m_camData.m_depth; }
        void            setDefaultDepth(f32 _depth) { m_defaultDepth = _depth; }
        f32             getDefaultDepth() const { return m_defaultDepth; }        
        static void     predictViewFrustum( FrustumDesc& _frustum, const AABB3d& _aabb, f32 _screenRatio, f32 _focale = MTH_PIBY4, f32 _camDistance = 0.f );
        void            getPredictPosition( const AABB& _aabb, f32 _z, Vec3d& _predictPosition );
        void            getScreenCenter( Vec3d& _pos ) const;
        void            getPredictPositionInConstraintAABB( const AABB& _constraintAABB, const f32 _depth, Vec3d& _pos, f32 _focale ) const;
        void            predictCameraDataAfterTeleport( CameraData& _camData, bbool _useLookAtOffset = bfalse ) const;

        void            resetInGameCamera();
        bbool           isInGameCameraJustReseted() const { return m_isInGameCameraJustReseted; }
        bbool           isCameraJustTeleported() const { return m_isCameraJustTeleported; }

    private:

        void            applyCameraData() const;        

#ifdef ITF_SUPPORT_CHEAT
        bbool       m_isCheatDezoomMax;
        f32         m_cheatZoom;      
#endif //ITF_SUPPORT_CHEAT

        bbool   m_isInGameCameraJustReseted;
        bbool   m_isCameraJustTeleported;

        class CameraSequenceData 
        {
        public:
            CameraSequenceData()
                : m_finalWeight(0)
                , m_destinationWeight(0)
                , m_weight(0)
            {}

            f32      m_finalWeight;
            f32      m_destinationWeight;
            f32      m_weight;
            CameraData m_camData;

            SafeArray<CameraControllerSubject>  m_camSubjectList;

            void    deactivate() { m_finalWeight = 0.f; }
        };

        CameraData  m_camData;
        Vec3d       m_camSpeedInGame;
        AABB        m_screenAABB;
        f32         m_defaultDepth;
        u32         m_id;

        // Mixer        
        f32                     m_rampUpCoeff;
        u32                     m_mainPriority;

        CameraSequenceData                     m_cameraSequence;
        SafeArray<CameraControllerData>     m_camControllerList;
        SafeArray<CameraModifierComponent*> m_camModifierList;
        SafeArray<CameraControllerSubject>  m_camSubjectList, m_camSubjectList_Active;
        
        bbool       m_updateCamSubjectList_Active;
        bbool       m_updateCamControllers_SubjectList;

        void        getAABBFromDepth( AABB& _aabb, f32 _depth ) const;
        void        getAABBFromDepthAndPos( AABB& _aabb, f32 _depth, const Vec3d & _pos, f32 _focal ) const;
        void        updateScreenAABB();
        void        updateCameraControllerSubjectActiveList();        
        bbool       isCameraControllerActive( ICameraController* _controller);
        bbool       cameraControllerHasInfluence( const CameraControllerData& data ) const { return data.m_weight > MTH_EPSILON; } 
        
        void        init();
        void        updateScreenRatio();
        void        onSwitchToWorld(World* _pWorld); 
 
    public:        
        const ICameraController* getInGameCamera() const;
        void        setLeaderSubject( ObjectRef _subject );
        bbool       isCamModifierRegistered(CameraModifierComponent* _camModifier ) const;
        bbool       isCamModifierComponentActive( const CameraModifierComponent* _camModifier ) const;
        void        registerCamModifierComponent ( CameraModifierComponent* _camModifier );
        void        unregisterCamModifierComponent( CameraModifierComponent* _camModifier );
        void        registerCameraController ( ICameraController* _cam );
        void        unregisterCameraController ( ICameraController* _cam );
        void        setMainCameraController ( ICameraController* _mainCam, f32 _blendIn );
        void        removeMainCameraController ( ICameraController* _mainCam, f32 _blendOut );        
        ICameraController* getMainCameraController() ;
        bbool       isMainCameraController( const ICameraController* _cam );        
        bbool       cameraControllerHasInfluence( const ICameraController* _cam ) const;
        CameraControllerData* getCameraControllerData( const ICameraController* _cam );

        void        startCameraSequence ( const Vec3d& _pos, f32 _blendIn = 0.05f, f32 _depth = F32_INFINITY, bbool _unregisterPlayer = bfalse, f32 _focale = MTH_PIBY4 );
        void        stopCameraSequence ( f32 _blendOut = 0.05f );
        void        setCameraSequencePosition ( const Vec3d& _pos, f32 _depth = F32_INFINITY );
        bbool       isCameraSequenceFinished() const { return !isCameraSequenceActive() && m_cameraSequence.m_weight < MTH_EPSILON; }
        bbool       isCameraSequenceActive() const { return m_cameraSequence.m_finalWeight == 1.f; }
           
#ifdef ITF_SUPPORT_CHEAT
        void        setCheatZoom(f32 _increment);
        void        resetCheatZoom();
        f32         getCheatZoom() const { return m_cheatZoom; }
        void        switchCheatDezoomMaxWithPad();
        void        switchCheatDezoomMax();
        bbool       isCheatDezoomMax() const { return m_isCheatDezoomMax; }
        void        setCheatDezoomMax( bbool _value ) { m_isCheatDezoomMax = _value; resetCheatZoom(); }
#endif //ITF_SUPPORT_CHEAT

        void        removeInvalidSubjects    (   );                         
        bbool       isSubjectRegistered     (   ObjectRef _subject ) const { return getSubjectIndex(_subject ) != -1; }
        u32         getSubjectListCount     (   );
        u32         getSubjectPlayerListCount(  );
        void        getSubjectByIndex       (   u32 _index, CameraControllerSubject& _subject);
        i32         getSubjectIndex         (   ObjectRef _subject ) const;        
        bbool       isSubjectJustJumped     (   ObjectRef _subject  );                
        bbool       isSubjectJustBounceJumped     (   ObjectRef _subject  );
        SubjectPerform getSubjectPerform( ObjectRef _subject );
        bbool       hasLeadCameraSubject() const;
        void        teleportCameraControllers(   );
        void        onEvent( Event* _event );   // IEventListener
        void        onMapLoaded();
        f32         getScreenRatio          (   ) const { return m_screenRatio; }
        f32         getWantedScreenRatio    (   ) const { return m_wantedScreenRatio; }
        void        backToGame();

        void                        bindObject( Pickable * _pickable, bbool _updateInitPos = bfalse, bbool _ignoreZ = bfalse);
        void                        unbindObject( Pickable * _pickable );
        bbool                       isObjectBound( Pickable * _pickable ) const;
        BindedObject*               getBindObject( Pickable * _pickable );
        ITF_INLINE const Vec3d &    getBindCameraMoveDelta() const { return m_bindCameraMoveDelta; }


        const SafeArray<CameraControllerData>&      getCameraControllerList() const { return m_camControllerList; }
        const SafeArray<CameraModifierComponent*>&  getCameraModifierList() const { return m_camModifierList; }
        const SafeArray<CameraControllerSubject>&   getCameraSubjectList() const { return m_camSubjectList_Active; }
		const CameraModifierComponent*              getCameraModifierComponent( const AABB& _aabb, f32 _depth, bbool _withConstraint = bfalse) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// request position change; request is dispatched to controllers, or applied on next update()
        /// @param _pos the new position
        static void        teleport                ( u32 _mask, const Vec3d& _pos);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// add shake movement
        ///@param _name shake definition to use (cf GameConfig/camerashakeconfig.isg)
        static void addShake( u32 _mask, const StringID& _name);
        /// reset shake movement
		static void resetShake( u32 _mask );
		static bbool getShakeConfig( u32 _mask, const StringID& _name, CameraShake &_shake );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// request immediat position change
        /// @param _pos the new position
        static void preApplyCameraPos( u32 _mask, const Vec3d& _pos);

#ifdef ITF_SUPPORT_DEBUGFEATURE
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to debug info flag
        bbool       m_debugDrawInfo;
        bbool       isDebugDrawInfo             (   ){ return m_debugDrawInfo; }
        void        switchDebugDrawInfo         (   ){ m_debugDrawInfo = !m_debugDrawInfo; }  
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to show 4/3 screen flag
        enum DebugDrawScreen4By3Type
        {
            None,
            Opaque,
            Transparent,
            Count
        };
        DebugDrawScreen4By3Type                     m_debugDrawScreen4By3;
        bbool       isDebugDrawScreen4By3           (   ){ return m_debugDrawScreen4By3 != None; }
        void        setDebugDrawScreen4By3          (   DebugDrawScreen4By3Type _type ) { m_debugDrawScreen4By3 = _type; } 
        const DebugDrawScreen4By3Type& getDebugDrawScreen4By3() const { return m_debugDrawScreen4By3; }            
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to shake debug draw flag
        bbool       m_shakeDebugDraw;
        bbool       isShakeDebugDraw        (   ){ return m_shakeDebugDraw; }
        void        switchShakeDebugDraw    (   ){ m_shakeDebugDraw = !m_shakeDebugDraw; }  
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to modifiers debugdraw flag
        bbool       m_debugDrawModifiers;
        bbool       isDebugDrawModifiers             (   ){ return m_debugDrawModifiers; }
        void        switchDebugDrawModifiers         (   ){ m_debugDrawModifiers = !m_debugDrawModifiers; } 
#endif // ITF_SUPPORT_DEBUGFEATURE

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Pause all cameras (eg for gameplay purpose). This is different from global pause
        static void        pauseMode           ( u32 _mask, bbool _pause);       
        bbool       isPauseMode         (             ) { return m_pauseMode;}        
        
        void        setIgnoreShakeCamFromCageBehaviour( bbool _ignoreShake ) { m_ignoreShakeFromCageBehaviour = _ignoreShake; }
        bbool       getMapDirection         ( Vec2d& _mapDir, const Vec3d& _pos );

        virtual void    registerSubject         (   ObjectRef _subject, f32 _delayToRegister = 0.f, bbool _isPlayer = btrue, bbool _doTeleport = btrue, bbool _leadCamera = bfalse, bbool _resetIfAlone = btrue );
        virtual void    unregisterSubject       (   ObjectRef _subject);
        

    protected :

        void        updateRegisteredSubjectStatus(ObjectRef _ref, bbool _leadCamera);
        bbool       hasMask( u32 _mask ) const;
        u32         getId() const { return m_id; }

        virtual void setTeleported();

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void drawDebug();
#endif //ITF_SUPPORT_DEBUGFEATURE

    private:
        bbool       isMainCameraControllerData( const CameraControllerData& _data) const { return _data.m_finalWeight == 1.f; }
        void        resetTempValues();
        void        setCameraData( f32 _dt );
        void        updateControllers( f32 _dt );
        void        updateBindedObjects();
        void        resetControllerSubject();
        void        updateShake(f32 _dt);
        CameraControllerData*      getMainCameraControllerData();
        void        restorePreviousMainCameraController( const CameraControllerData* _pCurrentMainData, f32 _blend );
		void        addShake(const StringID& _name);
		bbool		getShakeConfig( const StringID& _name, CameraShake &_shake );
        void        resetShake() { m_shake = CameraShake(); m_shakeCountdown = 0.0f; }

        void        swapSubjects            (   ObjectRef _subjectRefFrom, ObjectRef _subjectRefTo );        
        void        teleport                (   const Vec3d& _pos);
        void        preApplyCameraPos       (   const Vec3d& _pos);
        void        pauseMode               ( bbool _pause) {m_pauseMode = _pause;}
        void        unregisterNPCFromCamera (   );
        void        unregisterAllSubjects   (   );
        void        setSubjectJustJumped    (   ObjectRef _subject );
        void        setSubjectJustReceivedPunchUp  (   ObjectRef _subject );
        void        setSubjectJustBounceJumped    (   ObjectRef _subject );

        void processEventRegisterSubject( class EventRegisterCameraSubject* eventRegister );
        
        void        updateCameraSequenceWeight();        
        void        CameraSequenceTeleport();
        
        bbool       m_pauseMode;

        f32         m_autoRampUpCoeff_LengthSqr;
        bbool       m_autoRampUpCoeff_IsActive;        
        void        updateAutoRampUpCoeff( const CameraData& _data );
        void        activeAutoRampUpCoeff( bbool _isActive );                

        // shake
        bbool m_ignoreShake;
        bbool m_ignoreShakeFromCageBehaviour;

        f32 m_shakeCountdown;
        CameraShake m_shake;
        const CameraShakeConfig_Template* m_shakeConfig;

        SafeArray<BindedObject>     m_bindedObjects;
        Vec3d                       m_bindLastCameraPos;
        Vec3d                       m_bindCameraMoveDelta;
    };
}
#endif //_ITF_CAMERACONTROLLERMANAGER_H_
