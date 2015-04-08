
#ifndef _ITF_GHOSTMANAGER_H_
#define _ITF_GHOSTMANAGER_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_EVENTLISTENER_H_
#include "engine/events/IEventListener.h"
#endif // #ifndef _ITF_EVENTLISTENER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

namespace ITF
{
#define Ghost_max_players_toRecord  4

    // Mask common part
#define Ghost_mask_Common_Available         1 << 0 
#define Ghost_mask_Common_Pos_x             1 << 1
#define Ghost_mask_Common_Pos_y             1 << 2
#define Ghost_mask_Common_Pos_z             1 << 3
#define Ghost_mask_Common_UserData			1 << 4
#define Ghost_mask_Common_Angle				1 << 5
#define Ghost_mask_CommonStartIndex         6
    // Specific ANIMATION INPUTS
#define Ghost_mask_Inputs_LookingRight      1 << (Ghost_mask_CommonStartIndex + 0)
#define Ghost_mask_Inputs_Scale             1 << (Ghost_mask_CommonStartIndex + 1)
#define Ghost_mask_Inputs_AnimID            1 << (Ghost_mask_CommonStartIndex + 2)
#define Ghost_mask_Inputs_InputsCount       1 << (Ghost_mask_CommonStartIndex + 3)
    // Specific PAD INPUTS
#define Ghost_mask_Pad_LeftJoy_x            1 << (Ghost_mask_CommonStartIndex + 0)
#define Ghost_mask_Pad_LeftJoy_y            1 << (Ghost_mask_CommonStartIndex + 1)
#define Ghost_mask_Pad_IsLookingRight1Frame 1 << (Ghost_mask_CommonStartIndex + 2)
#define Ghost_mask_Pad_StateBegin           Ghost_mask_CommonStartIndex + 3
    // Game mode
#define Ghost_idleMode              1 << 0
#define Ghost_recordMode            1 << 1
#define Ghost_readMode              1 << 2
    // Constants
#define Ghost_HeaderSize            1024
#define Ghost_VERSION               11
#define Ghost_MaxFileCountByMap     50
#define Ghost_MaxFileAutoLoad       10
#define Ghost_RecordInputTypeUnset  0
#define Ghost_AnimInputType         1
#define Ghost_PadInputType          2
#define Ghost_MaxSecondsRecorded    480
    //////////////////////////////////////////////////////////////////////////
    //  class GhostInputInfo

    class GhostInputInfo
    {
    public:
        GhostInputInfo(): type(InputType_F32), id(StringID::Invalid), updated(bfalse) {}
        union InputValue
        {
            u32         u32Val;
            f32         f32Val;
        };

        // Serialized data
        InputValue  val;        // value of the input 

        InputType   type;       // type of input (F32 or U32)   --> only serialized the first frame, it should be constant
        StringID    id;         // input name                   --> only serialized the first frame, it should be constant

        // Used in local only
        bbool       updated;    
    };

    class GhostRecorderInfoBase
    {
    public:
        GhostRecorderInfoBase();
        virtual ~GhostRecorderInfoBase() {}
        virtual u32 getRecorderType() const=0;

        u32     m_mask;             // 32bits mask for all the main data (without inputs)
        bbool   m_bAvailable;       // if the frame is valid or not 
        Vec3d   m_Pos;              // actor position 
        u32		m_userData;			// UserData
        f32		m_angle;			// Angle of the actor
    };

    class GhostRecorderAnimInfo: public GhostRecorderInfoBase
    {
    public:
        GhostRecorderAnimInfo();
        virtual ~GhostRecorderAnimInfo() { if(m_inputInfos) Memory::free(m_inputInfos); }
        virtual u32 getRecorderType() const {return Ghost_AnimInputType;}

        // Serialized data        
        bbool                   m_IsLookingRight;       // flipped or not
        StringID                m_mainAnimId;           // animID 
        Vec2d                   m_Scale;                // actor scale         
        f32                     m_AnimTime;             // Anim time (for CurTime) -> not used, seems 0 is enough .. 

        // Inputs data (serialized)
        u64                     m_inputMaskLower;       // --
        u64                     m_inputMaskUpper;       // 128bits mask        
        u32                     m_inputInfosCount;      // count of m_inputInfos
        GhostInputInfo*         m_inputInfos;           // all animation inputs

        // Used in local only
        bbool                   m_firstFrame;
    };

    class GhostRecorderPadInfo: public GhostRecorderInfoBase
    {
    public:
        GhostRecorderPadInfo();
        virtual u32 getRecorderType() const {return Ghost_PadInputType;}

        // Serialized data        
        Vec2d   m_leftJoy;
        bbool   m_isLookingRightAtFirstFrame;

        // TODO: should be generic
        enum ListenerList
        {
            Attack,
            AttackHold,
            Jump,
            JumpHold,
            Helico,
            HelicoHold,
            Sprint,
            //Block,
            Throw,
            Drop,
            Dodge,
            Leave,
            ListenersCount
        };
        bbool   m_stateListeners[ListenersCount];
    };
    //////////////////////////////////////////////////////////////////////////
    class PlayerRecordedInfo 
    {
    public:
        u32 playerIndex;
        ObjectRef playerRef;
        u32 cursorPosition;
        bbool needToSave;
        u32 ghostIndexDupplicated; // if not dupplicated, U32_INVALID
    };
    //////////////////////////////////////////////////////////////////////////
    class GhostPlayerInfo
    {
    public:
        GhostPlayerInfo():m_RecorderInfo(NULL),m_fAlpha(1.0f) {};
        ~GhostPlayerInfo() { SF_DEL(m_RecorderInfo); }
        f32                     m_fAlpha;
        GhostRecorderInfoBase*  m_RecorderInfo;
    };
    //////////////////////////////////////////////////////////////////////////
    class GhostKeyFrame
    {
    public:
        GhostKeyFrame(): m_keyFrameBufferSize(0),m_keyFrameBuffer(NULL) { }
        ~GhostKeyFrame() 
        {
            if(m_keyFrameBuffer) free( m_keyFrameBuffer );
        }

        ITF_INLINE void add(i32 _value) { m_pos.push_back(_value);}
        void reset() 
        { 
            if(m_keyFrameBuffer)
            {
                free(m_keyFrameBuffer);
                m_keyFrameBuffer = NULL;
            }
            m_pos.clear(); 
        }

        bbool buildBuffer();
        bbool buildKeyFramesList(u8* buffer_, u32 buffersize_);

        ITF_INLINE u32 getSize() const { return m_keyFrameBufferSize; }
        ITF_INLINE u8* getBuffer() const { return m_keyFrameBuffer; }

    private:
        ITF_VECTOR<i32> m_pos;
        u8*  m_keyFrameBuffer;
        u32  m_keyFrameBufferSize;
    };

    //////////////////////////////////////////////////////////////////////////
    class GhostRecorder
    {
    public:
        GhostRecorder();
        ~GhostRecorder();

        void begin(u32 recordType);
        void end();

        void serializeGhostInput(ArchiveMemory& am,GhostInputInfo& info);
        void serializeGhostRecorderAnim(ArchiveMemory& am,GhostRecorderAnimInfo& info,GhostKeyFrame* keyframes=NULL);
        void serializeGhostRecorderPad(ArchiveMemory& am,GhostRecorderPadInfo& info);

        void  resetMask(u32& mask, u32 value) { mask &= (~value); }
        void  setMask(u32& mask, u32 value) { mask |= value; }
        bbool getMask(u32 mask, u32 value) { return ( mask & value ) != 0; }

        void  setMask64(u64& mask, u64 value) { mask |= value; }
        bbool getMask64(u64& mask, u64 value) { return ( mask & value) != 0; }

        void setMask128(u64& maskLower, u64& maskUpper, u32 bit)
        {
            if( bit < 64 )
                maskLower |= (u64(1) << u64(bit) );
            else if ( bit < 128 )
                maskUpper |= (u64(1) << u64(bit - 64) );
        }
        bbool getMask128(u64& maskLower, u64& maskUpper, u32 bit)
        {
            if( bit < 64 )
                return ( maskLower & (u64(1) << u64(bit) )) != 0;
            else if ( bit < 128 )
                return ( maskUpper & (u64(1) << u64(bit - 64) ) ) != 0;
            return bfalse;
        }

        void setGhostIndex(u32 ghostindex_) { m_currentGhostIndex = ghostindex_; }
        GhostKeyFrame*  getKeyFrames(u32 playerindex) {return (playerindex<Ghost_max_players_toRecord)?&m_keyframes[playerindex]:NULL;}
    private:
        void        buildCommonMask(ArchiveMemory& am,GhostRecorderInfoBase& info, bbool& canOptimize);

        GhostRecorderInfoBase*  m_prevRecorderInfo;     // We store the last updated frame data
        GhostRecorderInfoBase*  m_lastCompleteInfo;     // We only store the last updated field
        u32                     m_recordedFrame;        // Recorded frame counter
        bbool                   m_useOptim;             // If the recorded frame has to be optimized 
        f64                     m_recordStartTime;      
        GhostKeyFrame           m_keyframes[Ghost_max_players_toRecord];
        u32                     m_currentGhostIndex;

        // Size tracking 
        enum TrackSize
        {
            track_animID=0,
            track_scale,
            track_angle,
            track_userdata,
            track_pos,
            track_inputs,
            track_available,
            track_lookingright,
            track_mask,
            track_inputsCount,
            track_inputMask,
            track_pos_x,
            track_pos_y,
            track_pos_z,
            track_joy_x,
            track_joy_y,
            track_listeners,
            track_count
        };
        u32                 m_dataSize[track_count];
        /// ---------------        
    };

    // Header of a ghost file 
    // we reserve Ghost_HeaderSize for the header, so we can put new data without breaking anything
    class GhostFileHeader
    {
    public:
        GhostFileHeader(): m_version(0), m_score(0)
            , m_enduranceSeed(U32_INVALID)
            , m_enduranceDist(0.f)
            , m_enduranceTime(0.f)
            , m_ghostIntruderType(0)
            , m_ghostUniqueID(0)
            , m_isCompressed(bfalse)
            , m_uncompressedSize(0)
        {
            ITF_MemSet(m_headerBuffer, 0, sizeof(m_headerBuffer));
        }
        ~GhostFileHeader(){}

        //////////////////////////////////////////////////////////////////////////
        // Fill the data buffer from the class data (when you want to write a file)
        bbool   buildHeaderFromData();
        // Fill the data buffer from a file (local file)
        bbool   buildHeaderFromFile( const Path & filePath );
        // Fill the data buffer from a buffer (online data)
        bbool   buildHeaderFromBuffer( u8* buffer, u32 bufferSize );

        //////////////////////////////////////////////////////////////////////////
        // Will return the Max size of the header (see Ghost_HeaderSize)
        static u32 getHeaderSize() ;

        //////////////////////////////////////////////////////////////////////////
        // Returns the current data buffer  (static buffer)
        u8* getHeaderBuffer() { return m_headerBuffer; }

        //////////////////////////////////////////////////////////////////////////
        // @Header data: path of the map
        const Path & getPath() const { return m_path; }
        void setPath( const Path & path ) ; 

        // @Header data: version of the header
        void setVersion(u8 version) { m_version = version; }
        u8 getVersion() const { return m_version; }

        // @Header data
        ITF_INLINE u32 getEnduranceSeed() const { return m_enduranceSeed; }
        ITF_INLINE void setEnduranceSeed( const u32 _value ) { m_enduranceSeed = _value; }
        ITF_INLINE f32 getEnduranceDist() const { return m_enduranceDist; }
        ITF_INLINE void setEnduranceDist( const f32 _value ) { m_enduranceDist = _value; }
        ITF_INLINE f32 getEnduranceTime() const { return m_enduranceTime; }
        ITF_INLINE void setEnduranceTime( const f32 _value ) { m_enduranceTime = _value; }
        ITF_INLINE u32 getScore() const { return m_score; }
        ITF_INLINE void setScore(u32 val) { m_score = val; }

        ITF_INLINE u32 getGhostIntruderType() const { return m_ghostIntruderType; }
        ITF_INLINE void setGhostIntruderType( const u32 _value ) { m_ghostIntruderType = _value; }

        ITF_INLINE const StringID& getGhostCostumeID(u32 _localIndex) const { return (_localIndex<Ghost_max_players_toRecord)?m_ghostCostumeID[_localIndex]:StringID::Invalid; }
        ITF_INLINE void setGhostCostumeID( const StringID & _value, u32 _localIndex ) { if(_localIndex<Ghost_max_players_toRecord) m_ghostCostumeID[_localIndex] = _value; }

        // Compression 
        ITF_INLINE bbool getCompressed() const { return m_isCompressed; }
        ITF_INLINE void setCompressed( const bbool _value ) { m_isCompressed = _value; }
        ITF_INLINE u32 getUncompressedSize() const { return m_uncompressedSize; }
        ITF_INLINE void setUncompressedSize( const u32 _value ) { m_uncompressedSize = _value; }

        ITF_INLINE u32 getGhostUniqueID() const { return m_ghostUniqueID; }
        ITF_INLINE void setGhostUniqueID( const u32 _value ) { m_ghostUniqueID = _value; }
        //////////////////////////////////////////////////////////////////////////
        ITF_INLINE bbool  isValid() const { return m_version >= Ghost_VERSION; }
        void   reset();
    protected:
        void        resetBuffer();
        u8          m_headerBuffer[ Ghost_HeaderSize ];
        Path        m_path; 
        u8          m_version;
        static u32  m_magicNumber;
        u32         m_score;
        // TODO! this is RO2-specific!
        u32         m_enduranceSeed;
        f32         m_enduranceDist;
        f32         m_enduranceTime;
        u32         m_ghostIntruderType;
        StringID    m_ghostCostumeID[Ghost_max_players_toRecord];
        u32         m_ghostUniqueID;
        // 
        bbool       m_isCompressed;
        u32         m_uncompressedSize;
    };
    //////////////////////////////////////////////////////////////////////////
    //class GhostManager
    //////////////////////////////////////////////////////////////////////////
    class GhostManager: public IEventListener
    {
    public:
        GhostManager();
        virtual ~GhostManager();

        enum GhostType
        {
            RecordUnset         = Ghost_RecordInputTypeUnset,
            AnimInputBased      = Ghost_AnimInputType,
            PlayerInputBased    = Ghost_PadInputType,
        };

        //////////////////////////////////////////////////////////////////////////
        static void s_createInstance() { ITF_ASSERT(!m_instance); m_instance =  newAlloc(mId_Singleton, GhostManager()); }
        static void s_destroyInstance() { ITF_ASSERT(m_instance); SF_DEL(m_instance); }
        static GhostManager* s_get() { return m_instance; }

        //////////////////////////////////////////////////////////////////////////
        void	init();        

        //////////////////////////////////////////////////////////////////////////
        /// For now, save and load use the current map name, _szFilename is ignored
        bbool   save(const String8 & _szFilename);
        bbool   load(const String8 & _szFilename);

        //////////////////////////////////////////////////////////////////////////
        // Get last valid buffer. Used by Challenge 
        u8*			getLastSavedGhostBuffer() const     { return m_lastSavedGhostBuffer; }
        u32         getLastSavedGhostBufferSize() const { return m_lastSavedGhostBufferSize; }

        //////////////////////////////////////////////////////////////////////////
        // Try to load a ghost from a buffer 
        bbool   load(u8* buffer, u32 bufferSize, const String8& _name, u32 _flags = 0, bbool _spawnASAP = bfalse);

        //////////////////////////////////////////////////////////////////////////
        // Try to load file with the following name: (for multi ghost)
        // MAP_NAME_auto_X.ghost  --> X is between 1 and 10
        bbool   load_auto();

        //////////////////////////////////////////////////////////////////////////
        // To call when you want to start a new session, because if you call several 
        // load() method, the ghost will be push inside a reading list of ghost to display
        void    cleanBeforeLoading();

        //////////////////////////////////////////////////////////////////////////
        // build a filename from a path and an extension , _szSrcName is ignored
        void	buildName(Path& _szDstName,const String8 & _szSrcName, const String8 & _extension);

        //////////////////////////////////////////////////////////////////////////
        // Clear methods 
        void    clear();                    // clear everything recorded
        void    clearRegisteredGhost();     // if you want to reset a hotloaded path / challenge
        void    clearRecordedData();        // if you want to clear the recorded data 
        void    clearLoadedData();          // if you want to clear the loaded data (called by cleanBeforeLoading())
        void    clearChallengeData();       // if you want to clear challenge parameter

        //////////////////////////////////////////////////////////////////////////
        // update stuff 
        void    update(f32 _elapsed);

        //////////////////////////////////////////////////////////////////////////
        // update ghost visibility
        void	setVisible(u32 _index,bbool _visible);

        //////////////////////////////////////////////////////////////////////////
        // destroy a player to record (invalidate the ref)
        void    destroyPlayerGhost();

        //////////////////////////////////////////////////////////////////////////
        // Start/Stop a record and register a player to record
        void    toggleRecording(GhostType ghostTypeToRecord = RecordUnset /* when you want to stop, we don't care about this parameter */);
        void    registerNewPlayerToRecord(const ObjectRef & actorRef, u32 playerIndex );
        bbool   isRecording() const { return m_bIsRecording ;}
        bbool   isGhostVisible() const { return m_bViewMode; }
        bbool   isGhostPrefetched() const { return m_bIsPrefetched; }
        void    toggleReading();
        void    prefetchGhostBeforeReading();
        void    updateReadingGhostList(u32 frame=0);
        void    getInactiveGhostIndexList(ITF_VECTOR<u32> & _list);

        // Return the current count of visible ghost
        u32     getReadingGhostListSize();
        // Return the count of ghost (visible or not)
        u32		getReadingGhostListFullSize() const { return m_TrackPlayer.size(); }
        u32     getCurrentReadingFrame();
        void	setGhostCursor(u32 _ghostIndex, u32 _cursor);

        //////////////////////////////////////////////////////////////////////////
        // Pause actions 
        ITF_INLINE void     setIsRecordingPaused(bbool val) { m_bIsRecordingPaused = val; }
        ITF_INLINE bbool    getIsRecordingPaused() const { return m_bIsRecordingPaused; }

        ITF_INLINE bbool    getIsReadingPaused() const { return m_bIsReadingPaused; }
        ITF_INLINE void     setIsReadingPaused( const bbool _value ) { m_bIsReadingPaused = _value; }

        //////////////////////////////////////////////////////////////////////////
        // An option to active an auto save when the ghost manager component is destroyed
        ITF_INLINE bbool    getSaveRecordingOnDeath() const { return m_bSaveRecordingOnDeath; }
        ITF_INLINE void     setSaveRecordingOnDeath( const bbool _value ) { m_bSaveRecordingOnDeath = _value; }

        //////////////////////////////////////////////////////////////////////////
        // Hide all the ghosts
        void    clearVisibility();

        //////////////////////////////////////////////////////////////////////////
        // Drag&Drop a .ghost file 
        void    setDragAndDropGhost(const Path& ghostPath, bbool askToLoadMap = btrue);
        typedef void (*onPendingMapAdded)(const Path & mapPath, void* pParam);
        void    registerOnPendingMapAdded(onPendingMapAdded callback, void* callbackParam) { m_callbackPendingMapAdded= callback; m_callbackParam= callbackParam; }

        //////////////////////////////////////////////////////////////////////////
        // Hot loading of a ghost from a buffer 
        void    setGhostFromChallengeBegin();
        bbool   setGhostFromChallenge(u8* _buffer, u32 _bufferSize, const String8& _name, bbool _askToLoadMap = bfalse, u32 _flags = 0, bbool _spawnASAP = bfalse);
        bbool   isGhostNameAlreadyRegistered(const String8& _name) const ;

        //////////////////////////////////////////////////////////////////////////
        // Ref of component manager + ghost template
        virtual bbool spawnGhost( const Vec3d& _pos, u32 _ghostIndex ) {return bfalse;}
        virtual bbool unSpawnGhost() {return bfalse;}
        virtual u32 getSpawnedGhostCount() const { return 0; }

        //////////////////////////////////////////////////////////////////////////
        // When a ghost is displaying, call this method to get all the info you need
        // The displayed frame is updated each frame 
        GhostRecorderInfoBase*  getCurrentInfo(u32 _playerIndex, bbool _onlyIfVisible = btrue);
        u32						getGhostCursorByUserData(u32 _ghostIndex, u32 _data);
        u32						getGhostCursor(u32 _ghostIndex);
        void                    setGhostColor(const Color & _color) { m_ghostColorApply = _color; }
        const Color &           getGhostColor() const { return m_ghostColorApply; }
        ITF_INLINE Color        getGhostFogColor() const { return m_ghostFogColorApply; }
        ITF_INLINE void         setGhostFogColor( const Color _value ) { m_ghostFogColorApply = _value; }
        ITF_INLINE Color        getGhostFactorColor() const { return m_ghostFactorColorApply; }
        ITF_INLINE void         setGhostFactorColor( const Color _value ) { m_ghostFactorColorApply = _value; }

        //////////////////////////////////////////////////////////////////////////
        u32 addEmptyGhost(const StringID & _costumeId);

        //////////////////////////////////////////////////////////////////////////
        // To get the internal ghost index from a spwned ghost 
        u32     getGhostIndexFromGhostActor(const ActorRef & actorRef);
        u32     getGhostIndexFromPlayer(const ActorRef & actorRef);
        void    addGhostPlayer(Actor* _actor);
        void    setPlayerRef(u32 _ghostIndex, const ActorRef & _actorRef);
        ITF_INLINE Actor * getPlayerGhost(u32 _ghostIndex) { ITF_ASSERT_CRASH(_ghostIndex < m_playersGhost.size(), " invalid ghost index"); return (Actor *)(GETOBJECT(m_playersGhost[_ghostIndex]));}
        ITF_INLINE Actor * getPlayerActor(u32 _ghostIndex) { ITF_ASSERT_CRASH(_ghostIndex < m_TrackPlayer.size(), " invalid ghost index" ); return (Actor *)(GETOBJECT(m_TrackPlayer[_ghostIndex]->player));}
        ITF_INLINE const String8& getPlayerName(u32 _ghostIndex) const { ITF_ASSERT_MSG(_ghostIndex < m_TrackPlayer.size(), " invalid ghost index" ); if(_ghostIndex < m_TrackPlayer.size()) return m_TrackPlayer[_ghostIndex]->name; return String8::emptyString; }
        ITF_INLINE u32 getPlayerFlags(u32 _ghostIndex) const { ITF_ASSERT_MSG(_ghostIndex < m_TrackPlayer.size(), " invalid ghost index" ); if(_ghostIndex < m_TrackPlayer.size()) return m_TrackPlayer[_ghostIndex]->flags; return 0; }

        //////////////////////////////////////////////////////////////////////////
        // External access to ghost serialization 
        void    externalSerialize(ArchiveMemory& amw, GhostRecorderAnimInfo& info, GhostRecorderAnimInfo& infoModel);

        //////////////////////////////////////////////////////////////////////////
        // To active/inactive the recording/reading 
        void    activeGlobalMode(u32 mode, bbool isActive);
        bbool   isGlobalModeActive(u32 mode) const ;

        //////////////////////////////////////////////////////////////////////////
        // From IEventListener
        virtual void   onEvent( Event* _event );

        //////////////////////////////////////////////////////////////////////////
        // Get last loaded/saved header 
        const GhostFileHeader & getSavedHeader() const { return m_savedHeader; }
        const GhostFileHeader & getLoadedHeader() const { return m_loadedHeader; }
        GhostFileHeader* getHeaderFromGhostIndex(u32 ghostIndex);
        u32 getLocalGhostIndexFromGhostIndex(u32 ghostIndex);

        //////////////////////////////////////////////////////////////////////////
        virtual void onWorldLoaded(class World* _pWorld){}
        virtual void onCheckpointLoaded(){}
        virtual void onMapClosed(){}

        //////////////////////////////////////////////////////////////////////////
        bbool getMuteSound() const { return m_muteSound; }
        void  setMuteSound(bbool _val) { m_muteSound = _val; onMuteSoundUpdated(); }

        virtual void onMuteSoundUpdated() {}

        //////////////////////////////////////////////////////////////////////////
        PlayerRecordedInfo* getPlayerRecordedInfo(u32 _playerIndex) ; 

    private:
        // To load a map or not after a hotloaded path set 
        void onHotLoadedGhostMap(bbool askToLoadMap);

        // Update details
        void    update_reading();
        void    update_recording();
        void    update_display();

        // Debug info 
        void    apply_display();

        // Read/record stuff
        void    startReading(u32 frameStart = 0);
        void    startRecording(u32 playerCount = 1);
        void    stopRecording();
        void    resetAllGhosts();
        void    spawnGhostForReading(u32 frame=0);

        // Serialization stuff
        void    serializeGhost(ArchiveMemory& amw, GhostType ghostType = RecordUnset /* for reading */);

        u32     getPlayerRecordingCount() const;

        // Internal stuff for init/destroy
        virtual void internalInit(){}
        virtual void destroyInternal(){}
        virtual void internalUpdate(){}


        ITF_VECTOR<PlayerRecordedInfo> m_playersRecorded;
        ITF_VECTOR<ObjectRef> m_playersGhost;
        typedef ITF_VECTOR<GhostRecorderInfoBase*> vGhost;
        ITF_VECTOR<vGhost>	m_TrackRecorder;


        struct GhostReader
        {
            vGhost ghost; 
            u32 cursor;
            bbool visible;
            bbool over;
            bbool spwaned;
            bbool needspawn;
            bbool isDupplicated;
            u32 localIndex;
            ActorRef player;
            String8 name;
            u32 flags;
            GhostKeyFrame keyFrames;
            ITF_MAP<u32,u32> userData;
        };

        ITF_VECTOR<GhostReader*>	    m_TrackPlayer;
        ITF_VECTOR<GhostFileHeader>	    m_TrackPlayerHeader;

        GhostRecorder   m_recorder;

        u32             m_globalMode;
        f64             m_globalModeChangedTime;

        bbool   m_bIsRecording;
        bbool   m_bIsRecordingPaused;
        bbool   m_bIsReadingPaused;
        bbool   m_bIsPrefetched;
        bbool   m_bSaveRecordingOnDeath;
        bbool   m_bViewMode;
        u32     m_uCurrentRecordPosition;
        u32     m_CurrentGhostCountReading;
        u32     m_CurrentGhostCountRecording;

        Path    m_szDirectory;
        Path    m_lastGhostSaved;
        Path    m_lastGhostLoaded;

        bbool   m_justSaved;
        f64     m_justSavedTime;

        bbool   m_justLoaded;
        bbool   m_justLoadedFailed;
        f64     m_justLoadedTime;

        bbool   m_isReading;
        u32     m_startReadingFrame;
        u32     m_frameCounter;
        Path    m_hotLoadedPath;
        bbool   m_hotLoadedJustSet; // used to know if we just loaded the map ... For now I don't know how to do in a better way
        bbool   m_hotLoadedChallenge;
        bbool	m_toggleReadingASAP;
        bbool	m_muteSound;

        Color   m_ghostColorApply;
        Color   m_ghostFogColorApply;        
        Color   m_ghostFactorColorApply;        

        onPendingMapAdded       m_callbackPendingMapAdded;
        void*                   m_callbackParam;

        GhostFileHeader         m_savedHeader;
        GhostFileHeader         m_loadedHeader;

        void   freeStaticBuffer();
        static u8*              m_lastSavedGhostBuffer;
        static u32              m_lastSavedGhostBufferSize;

        GhostType               m_recordInputType;


        static i32  task_SaveGhost (void* pParam) ;
        static i32	task_LoadGhost (void* pParam) ;
        static void onTask_SaveGhostFinish (void* pParam, i32 _result) ;
        static void onTask_LoadGhostFinish (void* pParam, i32 _result) ;

        enum SavingState
        {
            Sav_Idle,
            Sav_Saving
        };
        enum LoadingState
        {
            Load_Idle,
            Load_Loading
        };

        SavingState				m_savingState;
        LoadingState			m_loadingState;

        struct LoadingInfo
        {
            LoadingInfo():m_buffer(NULL), m_bufferSize(0), m_currentGhostCount(0),m_needSpawn(bfalse), m_ghostFlags(0) {}

            String8 m_ghostName;
            u8* m_buffer;
            u32 m_bufferSize;
            u32 m_currentGhostCount;
            bbool m_needSpawn;
            u32 m_ghostFlags;
        };

        ITF_LIST<LoadingInfo>			m_waitingLoadingInfo;
        ITF_THREAD_CRITICAL_SECTION		m_taskCriticalSection;

    protected:
        static GhostManager*    m_instance;

        bbool	m_stopRecordOnCheckpointLoading;

        virtual String8 getCurrentMapName() { return ""; }
        virtual const Path & getCurrentPath() { return Path::EmptyPath; }
    };

#define GHOSTMANAGER        GhostManager::s_get()
}


#endif // _ITF_GHOSTMANAGER_H_
