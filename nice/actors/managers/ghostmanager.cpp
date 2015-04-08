#include "precompiled_engine.h"

#ifndef _ITF_GHOSTMANAGER_H_
#include "engine/actors/managers/ghostmanager.h"
#endif //_ITF_GHOSTMANAGER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/Actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_COMPRESS_H_
#include "engine/compression/compress.h"
#endif //_ITF_COMPRESS_H_

#ifndef _ITF_TASKMANAGER__H_
#include "engine/taskmanager/TaskManager.h"
#endif //_ITF_TASKMANAGER__H_

#ifndef _ITF_CHEATMANAGER_H_
#include "gameplay/managers/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_FILE_H_
# include "core/file/File.h"
#endif //_ITF_FILE_H_

namespace ITF
{
 
//////////////////////////////////////////////////////////////////////////
u8* GhostManager::m_lastSavedGhostBuffer = NULL;
u32 GhostManager::m_lastSavedGhostBufferSize = 0;
GhostManager* GhostManager::m_instance = NULL;

GhostManager::GhostManager():
      m_bIsRecording(bfalse)
    , m_bIsRecordingPaused(bfalse)
    , m_bIsReadingPaused(bfalse)
    , m_bSaveRecordingOnDeath(bfalse)
    , m_uCurrentRecordPosition(0)
    , m_bViewMode(0)
    , m_justSavedTime(0.0)
    , m_justSaved(bfalse)
    , m_justLoaded(bfalse)
    , m_justLoadedFailed(bfalse)
    , m_justLoadedTime(0.0)
    , m_CurrentGhostCountReading(0)
    , m_CurrentGhostCountRecording(0)
    , m_globalMode(0)
    , m_globalModeChangedTime(0)
    , m_callbackPendingMapAdded(NULL)
    , m_callbackParam(NULL)
    , m_hotLoadedJustSet(bfalse)
    , m_hotLoadedChallenge(bfalse)
    , m_ghostColorApply(COLOR_WHITE)
    , m_ghostFogColorApply(Color(0.0f,1.0f,1.0f,1.0f))
    , m_ghostFactorColorApply(COLOR_WHITE)
    , m_recordInputType(RecordUnset)
    , m_bIsPrefetched(bfalse)
    , m_startReadingFrame(0)
    , m_frameCounter(0)
	, m_savingState(Sav_Idle)
	, m_loadingState(Load_Idle)
	, m_toggleReadingASAP(bfalse)
	, m_muteSound(btrue)
	, m_stopRecordOnCheckpointLoading(btrue)
{
    activeGlobalMode( Ghost_readMode, btrue );
    activeGlobalMode( Ghost_recordMode, btrue );
    m_globalModeChangedTime = 0;

	Synchronize::createCriticalSection(&m_taskCriticalSection);

    m_loadedHeader.reset();
    m_savedHeader.reset();
}

#define Ghost_path      "GameSave/SaveGhost/"

GhostManager::~GhostManager()
{
   clear();
   clearLoadedData();
   freeStaticBuffer();

   Synchronize::destroyCriticalSection(&m_taskCriticalSection);
}

void GhostManager::init()
{    
    m_szDirectory = Ghost_path;
    m_szDirectory.setDirectory();
#ifdef ITF_WINDOWS
    Directory::create(m_szDirectory);
#endif // ITF_WINDOWS

    EVENTMANAGER_REGISTER_EVENT_LISTENER(EventGhostTrigger_CRC, this)

    internalInit();
}

void GhostManager::clearLoadedData()
{
    u32 ghostCount = m_TrackPlayer.size();
    for (u32 indexGhost = 0;indexGhost<ghostCount;++indexGhost)
    {
        vGhost &ghost = m_TrackPlayer[indexGhost]->ghost;
        if(!m_TrackPlayer[indexGhost]->isDupplicated)
        {            
        for (vGhost::iterator iter = ghost.begin();
            iter != ghost.end();++iter)
        {
            SF_DEL((*iter));
        }
        }
        ghost.clear();
        SF_DEL(m_TrackPlayer[indexGhost]);
    }
    m_TrackPlayer.clear();
    m_TrackPlayerHeader.clear();
    unSpawnGhost();
    m_bIsPrefetched = bfalse;
}
void GhostManager::clearRecordedData()
{
    u32 ghostCount = m_TrackRecorder.size();
    for (u32 indexGhost = 0;indexGhost<ghostCount;++indexGhost)
    {
        vGhost &ghost = m_TrackRecorder[indexGhost];
        for (vGhost::iterator iter = ghost.begin();
            iter != ghost.end();++iter)
        {
            SF_DEL((*iter));
        }
        ghost.clear();
        //SF_DEL(m_TrackRecorder[indexGhost]);
    }
    m_TrackRecorder.clear();
    m_playersRecorded.clear();
}

void GhostManager::clear()
{
    destroyPlayerGhost();
    clearVisibility();

    stopRecording();

    m_bSaveRecordingOnDeath = bfalse;
    m_bViewMode = bfalse;

	if(m_savingState==Sav_Idle)
		clearRecordedData();    

    if(!m_hotLoadedChallenge)
        clearLoadedData();
}

void GhostManager::clearChallengeData()
{
    m_hotLoadedPath.clear();
    m_hotLoadedChallenge = bfalse;

    clear();
}

void GhostManager::clearRegisteredGhost()
{
    if(!m_hotLoadedJustSet)
    {
        m_hotLoadedPath.clear();
        m_hotLoadedChallenge = bfalse;
    }
    else
        m_hotLoadedJustSet = bfalse;    
}

void GhostManager::destroyPlayerGhost()
{    
    m_playersGhost.clear();

    resetAllGhosts();
}

void GhostManager::startRecording(u32 playerCount)
{
    m_bIsRecording              = btrue;
    m_bIsRecordingPaused        = bfalse;
    m_uCurrentRecordPosition    = 0;

    clearRecordedData();
    m_CurrentGhostCountRecording = 0;

    m_justSaved = bfalse;
    m_justLoaded = bfalse;
}

void GhostManager::stopRecording()
{
    m_bIsRecording              = bfalse;
}

#define INIT_TRACK_SIZE u32 tmp_Size = 0;
#define BEGIN_TRACK_SIZE tmp_Size = am.getSize() ;  
#define END_TRACK_SIZE(x)  m_dataSize[x] += (am.getSize() - tmp_Size);  

void GhostRecorder::serializeGhostInput(ArchiveMemory& am,GhostInputInfo& info)
{    
    if( m_recordedFrame == 0 )
	{
        info.id.serialize(am);

		if(am.isReading())
		{
			u32 t ;
			am.serialize(t);
			ITF_ASSERT(t==1||t==0);
			info.type = (t==0) ? InputType_F32 : InputType_U32;
		} 
    else 
		{
			u32 t = info.type == InputType_F32 ? 0 : 1;
			am.serialize(t);
		}
	}
	ITF_ASSERT(info.type==InputType_F32||info.type==InputType_U32);
    if(info.type == InputType_F32)
        am.serialize(info.val.f32Val);
    else
        am.serialize(info.val.u32Val);
}

void GhostRecorder::serializeGhostRecorderAnim(ArchiveMemory& am,GhostRecorderAnimInfo& info,GhostKeyFrame* keyframes)
{
    info.m_mask = 0;
    INIT_TRACK_SIZE

    bbool canOptimize = (m_recordedFrame % 120) != 0 && m_useOptim;
    
    // push a key frame in the header
    if(!canOptimize && keyframes)
    {
        i32 currentPos = am.getSeekPos(); 
        keyframes->add(currentPos);
    }

    GhostRecorderAnimInfo* m_prevRecorderAnimInfo = (static_cast< GhostRecorderAnimInfo* > (m_prevRecorderInfo));    
    GhostRecorderAnimInfo* m_lastCompleteAnimInfo = (static_cast< GhostRecorderAnimInfo* > (m_lastCompleteInfo));    
         
    if (!am.isReading())
    {
        if( canOptimize )
        {
            // build mask
            if ( m_prevRecorderAnimInfo->m_bAvailable != info.m_bAvailable )
            {
                setMask(info.m_mask, Ghost_mask_Common_Available);
            }

            if ( m_prevRecorderAnimInfo->m_IsLookingRight != info.m_IsLookingRight )
            {
                setMask(info.m_mask, Ghost_mask_Inputs_LookingRight);
            }

            if ( m_prevRecorderAnimInfo->m_mainAnimId.GetValue() != info.m_mainAnimId.GetValue() )
            {
                setMask(info.m_mask, Ghost_mask_Inputs_AnimID);
            }

            if ( m_prevRecorderAnimInfo->m_Pos.x() != info.m_Pos.x() )
            {
                setMask(info.m_mask, Ghost_mask_Common_Pos_x);
            }
            if ( m_prevRecorderAnimInfo->m_Pos.y() != info.m_Pos.y() )
            {
                setMask(info.m_mask, Ghost_mask_Common_Pos_y);
            }
            if ( m_prevRecorderAnimInfo->m_Pos.z() != info.m_Pos.z() )
            {
                setMask(info.m_mask, Ghost_mask_Common_Pos_z);
            }

            if ( m_prevRecorderAnimInfo->m_Scale != info.m_Scale )
            {
                setMask(info.m_mask, Ghost_mask_Inputs_Scale);
            }

            if ( m_prevRecorderAnimInfo->m_inputInfosCount != info.m_inputInfosCount )
            {
                setMask(info.m_mask, Ghost_mask_Inputs_InputsCount);
            }

			if ( m_prevRecorderAnimInfo->m_userData != info.m_userData )
			{
				setMask(info.m_mask, Ghost_mask_Common_UserData);
			}

			if ( m_prevRecorderAnimInfo->m_angle != info.m_angle )
			{
				setMask(info.m_mask, Ghost_mask_Common_Angle);
        } 
        } 
        else
        {
            setMask(info.m_mask, Ghost_mask_Inputs_AnimID);
            setMask(info.m_mask, Ghost_mask_Common_Available);
            setMask(info.m_mask, Ghost_mask_Inputs_LookingRight);
            setMask(info.m_mask, Ghost_mask_Common_Pos_x);
            setMask(info.m_mask, Ghost_mask_Common_Pos_y);
            setMask(info.m_mask, Ghost_mask_Common_Pos_z);
            setMask(info.m_mask, Ghost_mask_Inputs_Scale);
            setMask(info.m_mask, Ghost_mask_Inputs_InputsCount);
			setMask(info.m_mask, Ghost_mask_Common_UserData);
			setMask(info.m_mask, Ghost_mask_Common_Angle);
        }
    }    
    
    // start serialization
    BEGIN_TRACK_SIZE
    am.serialize(info.m_mask);
    END_TRACK_SIZE(track_mask)

    if ( getMask(info.m_mask, Ghost_mask_Common_Available) )
    {
        BEGIN_TRACK_SIZE
        am.serialize(info.m_bAvailable);
        m_lastCompleteAnimInfo->m_bAvailable = info.m_bAvailable;
        END_TRACK_SIZE(track_available)
    } else if(am.isReading())
    {
        info.m_bAvailable = m_lastCompleteAnimInfo->m_bAvailable;
    }
    
    if ( getMask(info.m_mask, Ghost_mask_Inputs_LookingRight) )
    {
        BEGIN_TRACK_SIZE
        am.serialize(info.m_IsLookingRight);
        m_lastCompleteAnimInfo->m_IsLookingRight = info.m_IsLookingRight;
        END_TRACK_SIZE(track_lookingright)
    } else if(am.isReading())
    {
        info.m_IsLookingRight = m_lastCompleteAnimInfo->m_IsLookingRight;
    }

    if ( getMask(info.m_mask, Ghost_mask_Inputs_AnimID) )
    {
        BEGIN_TRACK_SIZE
        info.m_mainAnimId.serialize(am);
        m_lastCompleteAnimInfo->m_mainAnimId = info.m_mainAnimId;
        END_TRACK_SIZE(track_animID)
    } else if(am.isReading())
    {
        info.m_mainAnimId = m_lastCompleteAnimInfo->m_mainAnimId;
    }

    if ( getMask(info.m_mask, Ghost_mask_Common_Pos_x) )
    {
        BEGIN_TRACK_SIZE
        am.serialize(info.m_Pos.x());
        m_lastCompleteAnimInfo->m_Pos.x() = info.m_Pos.x();
        END_TRACK_SIZE(track_pos_x)
    } else if(am.isReading())
    {
        info.m_Pos.x() = m_lastCompleteAnimInfo->m_Pos.x();
    }
    
    if ( getMask(info.m_mask, Ghost_mask_Common_Pos_y) )
    {
        BEGIN_TRACK_SIZE
        am.serialize(info.m_Pos.y());
        m_lastCompleteAnimInfo->m_Pos.y() = info.m_Pos.y();
        END_TRACK_SIZE(track_pos_y)
    } else if(am.isReading())
    {
        info.m_Pos.y() = m_lastCompleteAnimInfo->m_Pos.y();
    }

    if ( getMask(info.m_mask, Ghost_mask_Common_Pos_z) )
    {
        BEGIN_TRACK_SIZE
        am.serialize(info.m_Pos.z());
        m_lastCompleteAnimInfo->m_Pos.z() = info.m_Pos.z();
        END_TRACK_SIZE(track_pos_z)
    } else if(am.isReading())
    {
        info.m_Pos.z() = m_lastCompleteAnimInfo->m_Pos.z();
    }


    if ( getMask(info.m_mask, Ghost_mask_Inputs_Scale) )
    {
        BEGIN_TRACK_SIZE
        info.m_Scale.serialize(am);
        m_lastCompleteAnimInfo->m_Scale = info.m_Scale;
        END_TRACK_SIZE(track_scale) 
    } else if(am.isReading())
    {
        info.m_Scale = m_lastCompleteAnimInfo->m_Scale;
    }


	if ( getMask(info.m_mask, Ghost_mask_Common_UserData) )
	{
		BEGIN_TRACK_SIZE		
		am.serialize(info.m_userData);
		m_lastCompleteAnimInfo->m_userData = info.m_userData;
		END_TRACK_SIZE(track_userdata) 
	} else if(am.isReading())
	{
		info.m_userData = m_lastCompleteAnimInfo->m_userData;
	}
    
	if ( getMask(info.m_mask, Ghost_mask_Common_Angle) )
	{
		BEGIN_TRACK_SIZE		
		am.serialize(info.m_angle);
		m_lastCompleteAnimInfo->m_angle = info.m_angle;
		END_TRACK_SIZE(track_angle) 
	} else if(am.isReading())
	{
		info.m_angle = m_lastCompleteAnimInfo->m_angle;
	}
    
    // Begin input serialization 
    if ( getMask(info.m_mask, Ghost_mask_Inputs_InputsCount) )
    {
        BEGIN_TRACK_SIZE
        am.serialize(info.m_inputInfosCount);
        m_lastCompleteAnimInfo->m_inputInfosCount = info.m_inputInfosCount;
        END_TRACK_SIZE(track_inputsCount)
    } else if(am.isReading())
    {
        info.m_inputInfosCount = m_lastCompleteAnimInfo->m_inputInfosCount;
    }

    if (am.isReading())
    {
        if ( m_recordedFrame > 0 && info.m_inputInfosCount != m_prevRecorderAnimInfo->m_inputInfosCount )
        {
            ITF_ASSERT_MSG ( 0, "Data Buffer seems incorrect" );
            am.clear();

            info.m_inputInfos = NULL;
            info.m_inputInfosCount = 0;
            return ; 
        }

        info.m_inputInfos = (GhostInputInfo*)Memory::malloc(sizeof(GhostInputInfo)*info.m_inputInfosCount);

        // We have a model to fill the animInfo
        if( m_recordedFrame > 0 )
        {         
            for(u32 i = 0; i < info.m_inputInfosCount; i++)
            { 
                info.m_inputInfos[i].id     = m_prevRecorderAnimInfo->m_inputInfos[i].id;
                info.m_inputInfos[i].type   = m_prevRecorderAnimInfo->m_inputInfos[i].type;
            }
        }
    }
    
    BEGIN_TRACK_SIZE
    info.m_inputMaskLower = 0;
    info.m_inputMaskUpper = 0;
    if ( !am.isReading() )
    {
        for(u32 i = 0; i < info.m_inputInfosCount; i++)
        {        
            if(canOptimize)
            {
                if ( m_prevRecorderAnimInfo->m_inputInfos[i].type == InputType_F32)
                {
                    info.m_inputInfos[i].updated = ( m_prevRecorderAnimInfo->m_inputInfos[i].val.f32Val
                        != info.m_inputInfos[i].val.f32Val );
                }
                else
                {
                    ITF_ASSERT(m_prevRecorderAnimInfo->m_inputInfos[i].type == InputType_U32);
                    info.m_inputInfos[i].updated = ( m_prevRecorderAnimInfo->m_inputInfos[i].val.u32Val
                        != info.m_inputInfos[i].val.u32Val );
                }
            }
            else
            {
                info.m_inputInfos[i].updated = btrue;
            }    
         
            if ( info.m_inputInfos[i].updated )
            {
                setMask128(info.m_inputMaskLower, info.m_inputMaskUpper, i);
            }
        }
    }

    BEGIN_TRACK_SIZE
    am.serialize(info.m_inputMaskLower);
    am.serialize(info.m_inputMaskUpper);
    END_TRACK_SIZE(track_inputMask)

    for(u32 i = 0; i < info.m_inputInfosCount; i++)
    {
        if ( getMask128(info.m_inputMaskLower, info.m_inputMaskUpper, i) )
        {
            serializeGhostInput(am, info.m_inputInfos[i]);
            if(m_lastCompleteAnimInfo->m_inputInfos==NULL)
            {
                m_lastCompleteAnimInfo->m_inputInfos = (GhostInputInfo*)Memory::malloc(sizeof(GhostInputInfo)*info.m_inputInfosCount);
            }
            m_lastCompleteAnimInfo->m_inputInfos[i].id = info.m_inputInfos[i].id;
            m_lastCompleteAnimInfo->m_inputInfos[i].type = info.m_inputInfos[i].type;
            m_lastCompleteAnimInfo->m_inputInfos[i].val = info.m_inputInfos[i].val;
        }
        else if (am.isReading())
        {
            info.m_inputInfos[i].id = m_lastCompleteAnimInfo->m_inputInfos[i].id;
            info.m_inputInfos[i].type = m_lastCompleteAnimInfo->m_inputInfos[i].type;
            info.m_inputInfos[i].val = m_lastCompleteAnimInfo->m_inputInfos[i].val;
        }
    }
    END_TRACK_SIZE(track_inputs)

    // end of serialization 
    // save current value
    m_prevRecorderAnimInfo->m_bAvailable = info.m_bAvailable;
    m_prevRecorderAnimInfo->m_IsLookingRight = info.m_IsLookingRight;
    m_prevRecorderAnimInfo->m_mainAnimId = info.m_mainAnimId;
    m_prevRecorderAnimInfo->m_Pos = info.m_Pos;
    m_prevRecorderAnimInfo->m_Scale = info.m_Scale;
    m_prevRecorderAnimInfo->m_angle = info.m_angle;
    if(m_prevRecorderAnimInfo->m_inputInfos==NULL)
    {
        m_prevRecorderAnimInfo->m_inputInfos = (GhostInputInfo*)Memory::malloc(sizeof(GhostInputInfo)*info.m_inputInfosCount);
    }
    ITF_Memcpy(m_prevRecorderAnimInfo->m_inputInfos, info.m_inputInfos, sizeof(GhostInputInfo)*info.m_inputInfosCount);
    m_prevRecorderAnimInfo->m_inputInfosCount = info.m_inputInfosCount; // should be constant now

    m_recordedFrame++;
}

void GhostRecorder::begin(u32 recordType)
{
    for ( u32 i = 0; i < track_count; i++ )
        m_dataSize[i] = 0;

    m_recordedFrame = 0;
    m_recordStartTime = SYSTEM_ADAPTER->getTime();

    for ( u32 i = 0; i < Ghost_max_players_toRecord; i++ )
        m_keyframes[i].reset();

    // Check and cast internal struture
    if(m_prevRecorderInfo && m_prevRecorderInfo->getRecorderType() != Ghost_AnimInputType)
    {
        SF_DEL(m_prevRecorderInfo);
    }

    if(m_prevRecorderInfo == NULL)
    {
        m_prevRecorderInfo = newAlloc(mId_Ghost, GhostRecorderAnimInfo);
    }

    if(m_lastCompleteInfo && m_lastCompleteInfo->getRecorderType() != Ghost_AnimInputType)
    {
        SF_DEL(m_lastCompleteInfo);
    }

    if(m_lastCompleteInfo == NULL)
    {
        m_lastCompleteInfo = newAlloc(mId_Ghost, GhostRecorderAnimInfo);
    }
}

GhostRecorder::GhostRecorder() : 
      m_recordedFrame(0)
    , m_useOptim(btrue)
    , m_recordStartTime(0.0)
    , m_prevRecorderInfo(NULL)
    , m_lastCompleteInfo(NULL)
    , m_currentGhostIndex(0)
{
    for ( u32 i = 0; i < track_count; i++ )
        m_dataSize[i] = 0;
}

GhostRecorder::~GhostRecorder()
{
    SF_DEL(m_prevRecorderInfo);
    SF_DEL(m_lastCompleteInfo);
}

void GhostRecorder::end()
{
}

#define SERIALIZE_DATA(x,mask,track)\
{\
    if ( getMask(info.m_mask, mask) ) \
    {\
        BEGIN_TRACK_SIZE\
        am.serialize(info.x); \
        m_lastCompleteAnimInfo->x = info.x;\
        END_TRACK_SIZE(track)\
    }\
    else if(am.isReading())\
    {\
        info.x = m_lastCompleteAnimInfo->x;\
    }\
}

#define SERIALIZE_DATA_ALWAYS(x)\
{\
    am.serialize(info.x); \
    m_lastCompleteAnimInfo->x = info.x;\
}

void GhostRecorder::buildCommonMask(ArchiveMemory& am,GhostRecorderInfoBase& info, bbool& canOptimize)
{
    info.m_mask = 0;
    canOptimize = (m_recordedFrame % 60) != 0 && m_useOptim;
    if (!am.isReading())
    {
        if( canOptimize )
        {
            // build mask
            if ( m_prevRecorderInfo->m_bAvailable != info.m_bAvailable )
            {
                setMask(info.m_mask, Ghost_mask_Common_Available);
            }
            if ( m_prevRecorderInfo->m_Pos.x() != info.m_Pos.x() )
            {
                setMask(info.m_mask, Ghost_mask_Common_Pos_x);
            }
            if ( m_prevRecorderInfo->m_Pos.y() != info.m_Pos.y() )
            {
                setMask(info.m_mask, Ghost_mask_Common_Pos_y);
            }
            if ( m_prevRecorderInfo->m_Pos.z() != info.m_Pos.z() )
            {
                setMask(info.m_mask, Ghost_mask_Common_Pos_z);
            }
        } 
        else
        {
            setMask(info.m_mask, Ghost_mask_Common_Available);
            setMask(info.m_mask, Ghost_mask_Common_Pos_x);
            setMask(info.m_mask, Ghost_mask_Common_Pos_y);
            setMask(info.m_mask, Ghost_mask_Common_Pos_z);
        }
    }    
}

void GhostRecorder::serializeGhostRecorderPad( ArchiveMemory& am,GhostRecorderPadInfo& info)
{
    INIT_TRACK_SIZE

    bbool canOptimize = bfalse;
    buildCommonMask(am, info, canOptimize);

    GhostRecorderPadInfo* m_prevRecorderAnimInfo = (static_cast< GhostRecorderPadInfo* > (m_prevRecorderInfo));    
    GhostRecorderPadInfo* m_lastCompleteAnimInfo = (static_cast< GhostRecorderPadInfo* > (m_lastCompleteInfo));    

    // TODO: optimize for PAD info, and Remove Common info (built in buildCommonMask)
    if (!am.isReading())
    {
        if( canOptimize )
        {
            // build mask
            if ( m_prevRecorderAnimInfo->m_bAvailable != info.m_bAvailable )
            {
                setMask(info.m_mask, Ghost_mask_Common_Available);
            }

            if ( m_prevRecorderAnimInfo->m_Pos.x() != info.m_Pos.x() )
            {
                setMask(info.m_mask, Ghost_mask_Common_Pos_x);
            }
            if ( m_prevRecorderAnimInfo->m_Pos.y() != info.m_Pos.y() )
            {
                setMask(info.m_mask, Ghost_mask_Common_Pos_y);
            }
            if ( m_prevRecorderAnimInfo->m_Pos.z() != info.m_Pos.z() )
            {
                setMask(info.m_mask, Ghost_mask_Common_Pos_z);
            }
        } 
        else
        {
            setMask(info.m_mask, Ghost_mask_Common_Available);
            setMask(info.m_mask, Ghost_mask_Common_Pos_x);
            setMask(info.m_mask, Ghost_mask_Common_Pos_y);
            setMask(info.m_mask, Ghost_mask_Common_Pos_z);
        }
    }    

    // start serialization
    BEGIN_TRACK_SIZE
    am.serialize(info.m_mask);
    END_TRACK_SIZE(track_mask)

    SERIALIZE_DATA(m_bAvailable, Ghost_mask_Common_Available, track_available)
    SERIALIZE_DATA(m_Pos.x(), Ghost_mask_Common_Pos_x, track_pos_x)
    SERIALIZE_DATA(m_Pos.y(), Ghost_mask_Common_Pos_y, track_pos_y)
    SERIALIZE_DATA(m_Pos.z(), Ghost_mask_Common_Pos_z, track_pos_z)

    // Pad serialiaziation
    SERIALIZE_DATA_ALWAYS(m_leftJoy.x())
    SERIALIZE_DATA_ALWAYS(m_leftJoy.y())
    //SERIALIZE_DATA(m_leftJoy.x(), Ghost_mask_Pad_LeftJoy_x, track_joy_x)
    //SERIALIZE_DATA(m_leftJoy.y(), Ghost_mask_Pad_LeftJoy_y, track_joy_y)

    SERIALIZE_DATA_ALWAYS(m_isLookingRightAtFirstFrame)

    BEGIN_TRACK_SIZE
    for(u32 i = 0; i < GhostRecorderPadInfo::ListenersCount; i++)
    {
        SERIALIZE_DATA_ALWAYS(m_stateListeners[i]);
        m_prevRecorderAnimInfo->m_stateListeners[i] = info.m_stateListeners[i];
    }
    END_TRACK_SIZE(track_listeners)

    m_prevRecorderAnimInfo->m_bAvailable = info.m_bAvailable;
    m_prevRecorderAnimInfo->m_Pos = info.m_Pos;
    m_prevRecorderAnimInfo->m_leftJoy = info.m_leftJoy;
    m_prevRecorderAnimInfo->m_isLookingRightAtFirstFrame = info.m_isLookingRightAtFirstFrame;
    m_recordedFrame++;
}


void GhostManager::externalSerialize(ArchiveMemory& am, GhostRecorderAnimInfo& info, GhostRecorderAnimInfo& infoModel )
{
    m_recorder.begin(Ghost_AnimInputType);
    
    ArchiveMemory amModel;
    m_recorder.serializeGhostRecorderAnim(amModel,infoModel);
    m_recorder.serializeGhostRecorderAnim(am,info);
}


void GhostManager::serializeGhost(ArchiveMemory& am, GhostType ghostType)
{    
    if (am.isReading())
    {
        u32 currentGhostCount = 0;
        am.serialize(currentGhostCount);

        u32 ghostTypeU32 ;
        am.serialize(ghostTypeU32);

        if ( ghostTypeU32 !=  Ghost_RecordInputTypeUnset && ghostTypeU32 != Ghost_AnimInputType && ghostTypeU32 != Ghost_PadInputType)
        {
            ITF_ASSERT_MSG (0, " Ghost archive corrupted, invalid ghost type ");
            return ; 
        }

        m_recorder.begin(ghostTypeU32);
        m_CurrentGhostCountReading += currentGhostCount;

        for (u32 indexGhost = 0;indexGhost<currentGhostCount;++indexGhost)  
        {
            GhostReader* reader = newAlloc(mId_Ghost, GhostReader);
            reader->cursor = 0;
            reader->visible = bfalse;
            reader->over = bfalse;
            reader->localIndex = indexGhost;
            reader->spwaned = bfalse;
            reader->needspawn = bfalse;
            reader->isDupplicated = bfalse;

            u32 size = 0;
            am.serialize(size);
            reader->ghost.clear();

            ghostType = (GhostType)ghostTypeU32;
			u32 lastUserData = U32_INVALID;

            for (u32 index = 0;index<size;++index)
            {
                if( ghostType == Ghost_AnimInputType )
                {
                    GhostRecorderAnimInfo* pRecordingInfo = newAlloc(mId_Ghost, GhostRecorderAnimInfo);
                    m_recorder.serializeGhostRecorderAnim(am,*pRecordingInfo,&reader->keyFrames);

					if(pRecordingInfo->m_userData!=lastUserData)
					{
						reader->userData[pRecordingInfo->m_userData] = index;
						lastUserData = pRecordingInfo->m_userData;
					}

                    reader->ghost.push_back(pRecordingInfo);

                } else if ( ghostType == Ghost_PadInputType )
                {
                    GhostRecorderPadInfo* pRecordingInfo = newAlloc(mId_Ghost, GhostRecorderPadInfo);
                    m_recorder.serializeGhostRecorderPad(am,*pRecordingInfo);
                    reader->ghost.push_back(pRecordingInfo);
                }
            }

            m_TrackPlayer.push_back(reader);
            m_TrackPlayerHeader.push_back(m_loadedHeader);
        }
    }
    else
    {
        u32 ghostCount = m_TrackRecorder.size();
		u32 ghostToSaveCount = 0;
		for (u32 i = 0; i < m_playersRecorded.size(); i++)
		{
			if ( m_playersRecorded[i].needToSave )
				ghostToSaveCount++;
		}

        am.serialize(ghostToSaveCount);

        u32 ghostTypeU32 = (u32)ghostType;
        am.serialize(ghostTypeU32);

        m_recorder.begin(ghostTypeU32);

        for (u32 indexGhost = 0;indexGhost<ghostCount;++indexGhost)
        {
			// We don't want to record all the ghost 
			if( m_playersRecorded.size() > indexGhost )
			{
				if (!m_playersRecorded[indexGhost].needToSave)
					continue;
			}

            m_recorder.setGhostIndex(indexGhost);

            vGhost &ghost = m_TrackRecorder[indexGhost];
            u32 size = ghost.size();
            am.serialize(size);

            for (vGhost::iterator iter = ghost.begin();
                iter != ghost.end();++iter)
            {
                if ( ghostType == AnimInputBased )
                {
                    GhostRecorderAnimInfo* info = (GhostRecorderAnimInfo*)(*iter);
                    m_recorder.serializeGhostRecorderAnim(am,*info);
                }
                else if ( ghostType == PlayerInputBased )
                {
                    GhostRecorderPadInfo* info = (GhostRecorderPadInfo*)(*iter);
                    m_recorder.serializeGhostRecorderPad(am,*info);
                }
            }
        }
    }
    m_recorder.end();
}

void GhostManager::addGhostPlayer(Actor* _actor)
{
    if ( _actor )
    {
        _actor->setAngle(_actor->getAngle());
        _actor->enable();
        
        m_playersGhost.push_back( _actor->getRef());
    }
}
//////////////////////////////////////////////////////////////////////////
void  GhostManager::buildName(Path& _szDstName,const String8& _szSrcName, const String8 & _extension)
{
    _szDstName.appendPath( m_szDirectory );           
    _szDstName.append( getCurrentMapName().cStr() );        

    if(!_extension.isEmpty()) 
        _szDstName.append( _extension );

    _szDstName.append( ".ghost" );
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::freeStaticBuffer()
{
    if (m_lastSavedGhostBuffer)
    {
        Memory::free( m_lastSavedGhostBuffer );
        m_lastSavedGhostBuffer = NULL;
        m_lastSavedGhostBufferSize = 0;
    }
}
//////////////////////////////////////////////////////////////////////////
bbool GhostManager::save(const String8& _szFilename)
{
    if (!m_bIsRecording)
        return bfalse;
    
    m_lastGhostSaved.clear();
    m_savedHeader.reset();
        
    // Allow other class to modify the header
	u32 ivalid = 0;
    for (u32 i = 0; i < m_playersRecorded.size(); i++)
    {
        EventGhostOnSaving eventOnSaving(&m_savedHeader, m_playersRecorded[i].playerRef, i); 
        EVENTMANAGER->broadcastEvent(&eventOnSaving);
		
        if ( Player* player = GAMEMANAGER->getPlayerFromActor(m_playersRecorded[i].playerRef) )
        {
			m_savedHeader.setGhostCostumeID( player->getCurrentPlayerIDInfo(), ivalid );
        }

		m_playersRecorded[i].needToSave = eventOnSaving.getSaveGhost();
		if ( eventOnSaving.getSaveGhost() )
			ivalid ++;
    }

    // The following members cant be changed
    m_savedHeader.setVersion(Ghost_VERSION);
    m_savedHeader.setPath(getCurrentPath());
            

	TaskManager::get()->pushTask(task_SaveGhost, onTask_SaveGhostFinish, this, "SaveGhost");
	m_savingState = Sav_Saving;
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::cleanBeforeLoading()
{
    clearLoadedData();    
    m_CurrentGhostCountReading = 0;
    m_lastGhostLoaded.clear();    
}
//////////////////////////////////////////////////////////////////////////
bbool GhostManager::load_auto()
{
    if(!m_hotLoadedPath.isEmpty())
        return btrue; 

#ifdef ITF_WINDOWS
    u32 fileLoadedCount = 0 ;
    for ( u32 fileCount = 1 ; fileCount <= Ghost_MaxFileAutoLoad; fileCount ++ )
    {
        m_hotLoadedPath.clear();
        String8 extend;
        extend.setTextFormat("_auto_%d", fileCount);
        buildName(m_hotLoadedPath, "", extend);        

        if ( FILEMANAGER->fileExists( m_hotLoadedPath ) )
        {
            if ( load ( "" ) ) 
            {
                fileLoadedCount ++;
            }
        }
    }    
#endif //ITF_WINDOWS

    m_hotLoadedPath.clear();
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
bbool GhostManager::load(const String8& _szFilename)
{    
    m_lastGhostLoaded.clear();    
    // if m_hotLoadedPath is filled, use it 
    if(!m_hotLoadedPath.isEmpty())
        m_lastGhostLoaded = m_hotLoadedPath;
    else
        buildName(m_lastGhostLoaded,_szFilename,"_load");

    File* ghostFile = FILEMANAGER->openFile(m_lastGhostLoaded,ITF_FILE_ATTR_READ);
    if (ghostFile)
    { 
        u32 fileSize    = (u32) ghostFile->getLength();
        if(!fileSize)
            return bfalse;

        u8* pBuffer     = new u8[fileSize];

        ghostFile->read(pBuffer,fileSize);
        FILEMANAGER->closeFile(ghostFile);
        
        bbool res = load(pBuffer, fileSize, "");
        
        SF_DEL_ARRAY(pBuffer);
        return res;
    }

    m_justLoadedFailed = btrue;
    m_justLoaded = btrue;
    m_justLoadedTime = SYSTEM_ADAPTER->getTime();

    return bfalse;
}
//////////////////////////////////////////////////////////////////////////
i32 GhostManager::task_LoadGhost(void* pParam)
{
	GhostManager* pGhostManager = (GhostManager*) pParam;
		
	Synchronize::enterCriticalSection(&pGhostManager->m_taskCriticalSection);
	LoadingInfo loadingInfo = pGhostManager->m_waitingLoadingInfo.front();
	pGhostManager->m_waitingLoadingInfo.pop_front();
	Synchronize::leaveCriticalSection(&pGhostManager->m_taskCriticalSection);

	u8* buffer = loadingInfo.m_buffer;
	u32 bufferSize = loadingInfo.m_bufferSize;
	u32 ghostCount = pGhostManager->m_TrackPlayer.size();

	if (!pGhostManager->m_loadedHeader.buildHeaderFromBuffer(buffer, bufferSize)
		|| !pGhostManager->m_loadedHeader.isValid())
	{		
		return 0;
	}

	bbool serialationDone = bfalse;

	if ( pGhostManager->m_loadedHeader.getCompressed() )
	{
		u8* bufferuncompressed = (u8*) Memory::mallocCategory( pGhostManager->m_loadedHeader.getUncompressedSize() ,MemoryId::mId_Ghost);
		if ( Compress::uncompressBuffer(bufferuncompressed, pGhostManager->m_loadedHeader.getUncompressedSize(), 
					buffer+GhostFileHeader::getHeaderSize(), 
					bufferSize-GhostFileHeader::getHeaderSize()) > 0 )
		{
			ArchiveMemory rd(bufferuncompressed, pGhostManager->m_loadedHeader.getUncompressedSize());
			pGhostManager->serializeGhost(rd, AnimInputBased);
			serialationDone = btrue;
		}
		Memory::free(bufferuncompressed);
	}

	if ( !serialationDone )
	{        
		ArchiveMemory rd(buffer+GhostFileHeader::getHeaderSize(),bufferSize-GhostFileHeader::getHeaderSize());
		pGhostManager->serializeGhost(rd, AnimInputBased);
	}

	for(u32 n=ghostCount; n<pGhostManager->m_TrackPlayer.size(); ++n)
	{
		GhostReader* reader = pGhostManager->m_TrackPlayer[n];        
		reader->name = loadingInfo.m_ghostName;
        reader->flags = loadingInfo.m_ghostFlags;
        reader->needspawn = loadingInfo.m_needSpawn;
	}

	Memory::free(loadingInfo.m_buffer);
	return 1;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::onTask_LoadGhostFinish (void* pParam, i32 _result)
{
	GhostManager* pGhostManager = (GhostManager*) pParam;

	if ( _result > 0 )
	{
		EventGhostOnLoaded eventOnLoaded(&pGhostManager->m_loadedHeader, bfalse);
		EVENTMANAGER->broadcastEvent(&eventOnLoaded);

		pGhostManager->m_justLoadedFailed = bfalse;

		if(pGhostManager->m_toggleReadingASAP)
		pGhostManager->startReading();

	} else
	{
		pGhostManager->m_justLoadedFailed = btrue;		
	}

	// if we have task .. just do it
	Synchronize::enterCriticalSection(&pGhostManager->m_taskCriticalSection);
	u32 waitingTask = pGhostManager->m_waitingLoadingInfo.size();
	Synchronize::leaveCriticalSection(&pGhostManager->m_taskCriticalSection);
		
	if(waitingTask > 0)
	{
		TaskManager::get()->pushTask( task_LoadGhost, onTask_LoadGhostFinish, pParam, "LoadGhost2");
	}
	else
	{
		pGhostManager->m_loadingState = Load_Idle;
	pGhostManager->m_justLoaded = btrue;
	pGhostManager->m_justLoadedTime = SYSTEM_ADAPTER->getTime();
	}
}
//////////////////////////////////////////////////////////////////////////
bbool GhostManager::load(u8* buffer, u32 bufferSize, const String8& _name, u32 _flags, bbool _spawnASAP /*= bfalse*/)
{
	LoadingInfo loadingInfo;
    loadingInfo.m_buffer = (u8*)Memory::mallocCategory(bufferSize,MemoryId::mId_Ghost );
    ITF_Memcpy(loadingInfo.m_buffer, buffer, bufferSize);

	loadingInfo.m_bufferSize		= bufferSize;
	loadingInfo.m_ghostName			= _name;
	loadingInfo.m_currentGhostCount	= m_TrackPlayer.size();
    loadingInfo.m_needSpawn         = _spawnASAP;
    loadingInfo.m_ghostFlags        = _flags;

	Synchronize::enterCriticalSection(&m_taskCriticalSection);
	m_waitingLoadingInfo.push_back( loadingInfo );
	Synchronize::leaveCriticalSection(&m_taskCriticalSection);

	if(m_loadingState == Load_Idle)
	{
		TaskManager::get()->pushTask( task_LoadGhost, onTask_LoadGhostFinish, this, "LoadGhost");
	m_loadingState = Load_Loading;
	}

    return btrue;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::update(f32 _elapsed)
{
    update_recording();
    update_reading();
    update_display();
    apply_display();

    internalUpdate();
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::update_reading()
{
    if (!m_bViewMode || m_bIsReadingPaused)
        return;

#ifdef ITF_SUPPORT_CHEAT
	if(CHEATMANAGER->getPause())
		return;
#endif //ITF_SUPPORT_CHEAT

    bbool spawnNewGhost = bfalse;
    //////////////////////////////////////////////////////////////////////////
    // Reading mode, update the reading cursor
    for (u32 i = 0; i < m_TrackPlayer.size(); i++)
    {        
        GhostReader* reader = m_TrackPlayer[i];
        
        if(!reader->spwaned && reader->needspawn)
        {
            spawnNewGhost = btrue;
        }

        if (reader->visible)
        {
			EventGhostOnReadingFrame eventOnReadingFrame(i);
			EVENTMANAGER->broadcastEvent(&eventOnReadingFrame);

			if ( eventOnReadingFrame.getCanUpdateCursor() )
				reader->cursor++;			

            if (reader->cursor == reader->ghost.size())
            {
                setVisible(i, bfalse);
                reader->over = btrue;

                EventGhostOnReadingFinish eventOnReadingFinish (i);
                EVENTMANAGER->broadcastEvent(&eventOnReadingFinish);
            }
        }
    }

    if(spawnNewGhost)
    {
        spawnGhostForReading(getCurrentReadingFrame());
    }

    m_frameCounter ++;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::update_recording()
{
    if (!m_bIsRecording || m_bIsRecordingPaused)
        return ;
    
#ifdef ITF_SUPPORT_CHEAT
	if(CHEATMANAGER->getPause())
		return;
#endif //ITF_SUPPORT_CHEAT

    for (u32 i = 0; i < m_playersRecorded.size(); i++)
    {
        const PlayerRecordedInfo & info = m_playersRecorded[i];        
        if(info.playerRef.isValid())
        {
            Actor* p = (Actor *)(GETOBJECT(info.playerRef));
            if (p && p->isPhysicalReady())
            {
                GhostRecorderInfoBase* pRecordingInfo = NULL;
                if ( m_recordInputType == Ghost_AnimInputType )
                {
                    pRecordingInfo = newAlloc(mId_Ghost, GhostRecorderAnimInfo);                    
					((GhostRecorderAnimInfo*)(pRecordingInfo))->m_Scale = p->getScale();
                }
                else if ( m_recordInputType == Ghost_PadInputType )
                {
                    pRecordingInfo = newAlloc(mId_Ghost, GhostRecorderPadInfo);
                }

                ITF_ASSERT ( pRecordingInfo != NULL );
                if ( pRecordingInfo )
                {
                    pRecordingInfo->m_bAvailable = btrue;
                    p->onRecordGhost(pRecordingInfo);
					pRecordingInfo->m_angle = p->getAngle();
					
					EventGhostOnRecordingFrame	onRecordingFrame(i);
					EVENTMANAGER->broadcastEvent(&onRecordingFrame);

					pRecordingInfo->m_userData = onRecordingFrame.getUserData();
                    m_TrackRecorder[i].push_back( pRecordingInfo );

                    if( m_TrackPlayer.size() > info.ghostIndexDupplicated )
                    {
                        GhostReader* reader = m_TrackPlayer[info.ghostIndexDupplicated];
                        reader->ghost.push_back( pRecordingInfo );
                }
            }
        }
    }
    }
	
    m_uCurrentRecordPosition++;//May we will need a current pos by player ghost

	u32 maxFrames = Ghost_MaxSecondsRecorded * 60;
	if ( maxFrames < m_uCurrentRecordPosition )
	{
		stopRecording();
	}
}
//////////////////////////////////////////////////////////////////////////
// Display info update
void GhostManager::update_display()
{    
    if(m_justLoaded && SYSTEM_ADAPTER->getTime() - m_justLoadedTime > 5.0 )
        m_justLoaded = bfalse;    

    if(m_justSaved && SYSTEM_ADAPTER->getTime() - m_justSavedTime > 7.0 )
        m_justSaved = bfalse;     

    if ( m_globalModeChangedTime != 0 && SYSTEM_ADAPTER->getTime() - m_globalModeChangedTime > 7.0)
        m_globalModeChangedTime = 0;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::toggleRecording(GhostType ghostTypeToRecord)
{
    if (m_bIsRecording)
    {
        if ( !SYSTEM_ADAPTER->exitRequested() )
            save("");
        stopRecording();
    }
    else
    {
        ITF_ASSERT(ghostTypeToRecord!=RecordUnset);
        m_recordInputType = ghostTypeToRecord;
        startRecording();
    }
}
//////////////////////////////////////////////////////////////////////////
void  GhostManager::toggleReading()
{
    if ( isGhostVisible() )
    {
        clear( );
    }
    else
    {
        startReading();
    }
}
//////////////////////////////////////////////////////////////////////////
void  GhostManager::clearVisibility()
{
    for ( u32 i = 0 ; i < m_TrackPlayer.size(); i++ )
        m_TrackPlayer[i]->visible = bfalse;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::setVisible(u32 _index,bbool _visible)
{
    if (_index<m_TrackPlayer.size())
    {
        //check if there is a ghost available
        m_TrackPlayer[_index]->visible = _visible;
    }    
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::apply_display()
{
#ifdef ITF_SUPPORT_EDITOR
    if(m_bIsRecording)
    {
        static String8 str; 
        str.setTextFormat("[GHOST] Recording, frame %d %s", m_uCurrentRecordPosition, m_bIsRecordingPaused ? " [PAUSED] " : "" );
        GFX_ADAPTER->drawDBGText(str, Color(1.f,1.f,0.f,0.f), 15.0f, 15.0f);
    }    
    if(m_justSaved)
    {
        static String8 str;    
        m_justLoaded = bfalse;
        str.setTextFormat("[GHOST] File saved \"%s/%s\"", Ghost_path, m_lastGhostSaved.getBasename());
        GFX_ADAPTER->drawDBGText(str, Color(1.f,0.f,1.f,0.f), 15.0f, 30.0f);
    }
    if(m_justLoaded)
    {
        m_justSaved = bfalse;
        static String8 str; 
        if ( m_justLoadedFailed )
        {
            str.setTextFormat("[GHOST] Loading failed \"%s/%s\"", Ghost_path, m_lastGhostLoaded.getBasename());
            GFX_ADAPTER->drawDBGText(str, Color(1.f,1.f,0.f,0.f), 15.0f, 30.0f);
        }
        else
        {
            if(m_lastGhostLoaded.isEmpty())
                str.setTextFormat("[GHOST] Buffer loaded ");
            else
            str.setTextFormat("[GHOST] File loaded \"%s/%s\"", Ghost_path, m_lastGhostLoaded.getBasename());
            GFX_ADAPTER->drawDBGText(str, Color(1.f,0.f,1.f,0.f), 15.0f, 30.0f);
        }
        
    }
    if(m_globalModeChangedTime!=0)
    {
        String8 modeName("");
        if ( isGlobalModeActive( Ghost_idleMode ) )
            modeName = "Inactif";
        else
        {
            modeName += " Reading=";
            if( isGlobalModeActive( Ghost_readMode ) ) 
                modeName += "[ON]";
            else 
                modeName += "[OFF]";

            modeName += " Recording=";
            if( isGlobalModeActive( Ghost_recordMode ) )
                modeName += "[ON]";
            else 
                modeName += "[OFF]";
        }
        

        static String8 str; 
        str.setTextFormat("[GHOST] Mode changed: %s", modeName.cStr());
        GFX_ADAPTER->drawDBGText(str, Color(1.f,0.f,1.f,0.f), 15.0f, 45.0f);
    }
#endif //!ITF_SUPPORT_EDITOR
}
//////////////////////////////////////////////////////////////////////////
GhostRecorderInfoBase* GhostManager::getCurrentInfo( u32 _playerIndex, bbool _onlyIfVisible )
{
    if(m_TrackPlayer.size() <= _playerIndex)
    {
        ITF_ASSERT_MSG(0,"bad ghost index");
        return NULL;
    }
    
    if (_onlyIfVisible && !m_bViewMode)
        return NULL;

    GhostReader* reader = m_TrackPlayer[_playerIndex];

    if ((_onlyIfVisible && !reader->visible) || reader->over)
        return NULL;

    u32 cursor = reader->cursor;

    if(cursor>=reader->ghost.size())
        return NULL;

    if(!reader->ghost[cursor])
    {
        ITF_ASSERT_MSG(0,"bad ghost data");
        return NULL;
    }

    if (!reader->ghost[cursor]->m_bAvailable)
        return NULL;

    return reader->ghost[cursor];
}
//////////////////////////////////////////////////////////////////////////
bbool GhostManager::isGlobalModeActive( u32 mode ) const
{
    return ( m_globalMode & mode ) != 0;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::activeGlobalMode( u32 mode, bbool isActive )
{
    if( isActive )
        m_globalMode |= mode;
    else
        m_globalMode &= (~mode);

    if( mode == Ghost_idleMode && isActive )
    {
        activeGlobalMode( Ghost_recordMode, bfalse );
        activeGlobalMode( Ghost_readMode, bfalse );
        clear();
    }

    if(SYSTEM_ADAPTER)
        m_globalModeChangedTime = SYSTEM_ADAPTER->getTime();
}
//////////////////////////////////////////////////////////////////////////
bbool GhostManager::isGhostNameAlreadyRegistered(const String8& _name) const 
{
	for (u32 i = 0; i < m_TrackPlayer.size(); i++)
	{
		GhostReader* reader	= m_TrackPlayer[i];
		if ( reader->name == _name )
		{
			return btrue;
		}
	}
	return bfalse;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::setGhostFromChallengeBegin()
{
    cleanBeforeLoading();
}
//////////////////////////////////////////////////////////////////////////
bbool GhostManager::setGhostFromChallenge(u8* _buffer, u32 _bufferSize, const String8& _name, bbool _askToLoadMap, u32 _flags, bbool _spawnASAP/* = bfalse*/)
{
    if ( load ( _buffer, _bufferSize, _name, _flags, _spawnASAP ) )
    {
        m_hotLoadedChallenge = btrue;
        m_hotLoadedPath.clear();
        m_hotLoadedJustSet = btrue;
        onHotLoadedGhostMap(_askToLoadMap);
        return btrue;
    }    
    return bfalse;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::setDragAndDropGhost( const Path& ghostPath, bbool askToLoadMap )
{
    // Get path
    if ( m_loadedHeader.buildHeaderFromFile(ghostPath) )
    {       
        m_hotLoadedPath = ghostPath;
        m_hotLoadedJustSet = btrue;
        m_hotLoadedChallenge = bfalse;
        onHotLoadedGhostMap(askToLoadMap);
    }
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::onHotLoadedGhostMap(bbool askToLoadMap)
{
    // BroadCast event
    EventGhostOnLoaded eventOnLoaded(&m_loadedHeader, btrue);
    EVENTMANAGER->broadcastEvent(&eventOnLoaded);

    // If we need to reload the map
    if(askToLoadMap && m_callbackPendingMapAdded)
        m_callbackPendingMapAdded(m_loadedHeader.getPath(), m_callbackParam);
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::onEvent( Event* _event )
{
    if (EventGhostTrigger * evt = DYNAMIC_CAST(_event,EventGhostTrigger))
    {
        if(m_playersGhost.find(evt->getActivator())!=-1)
            return;

        // Toggle Recording 
        if ( isGlobalModeActive( Ghost_recordMode ) )
        {                   
            toggleRecording( (GhostType)evt->getRecordType() );
			m_stopRecordOnCheckpointLoading = evt->getStopRecordOnCheckPointLoading();
        }

        // Toggle ghost to load
        if ( isGlobalModeActive( Ghost_readMode ) )
        {
            // if the ghost is currently playing stop and clear it 
            if ( isGhostVisible() )
            {
                clear();
            }
            else
            {
                bbool isLoaded = btrue;
                if ( !m_hotLoadedChallenge )
                {
                    cleanBeforeLoading();
                    // Check for auto loaded file + hotloaded file
#ifdef ITF_WINDOWS
                    isLoaded = load("") || load_auto();
					if( isLoaded )
						m_toggleReadingASAP = btrue;                    
#endif //ITF_WINDOWS
                }
				else 
                {                    
                    startReading();
                }
            }
        }        
    }
}
//////////////////////////////////////////////////////////////////////////
u32 GhostManager::getCurrentReadingFrame()
{
    if( m_bViewMode )
        return ( m_frameCounter );
    return 0;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::updateReadingGhostList(u32 frame)
{
        spawnGhostForReading(frame);
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::resetAllGhosts()
{
    for ( u32 i = 0 ; i < m_TrackPlayer.size(); i ++)
    {           
        m_TrackPlayer[i]->spwaned = bfalse;
        m_TrackPlayer[i]->over = bfalse;
    }
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::setGhostCursor(u32 _ghostIndex, u32 _cursor)
{
	if(_cursor!= U32_INVALID && m_TrackPlayer.size()>_ghostIndex)
	{
		GhostReader* reader = m_TrackPlayer[_ghostIndex];
		reader->cursor = _cursor;
	}
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::spawnGhostForReading(u32 frame)
{
    for ( u32 i = 0 ; i < m_TrackPlayer.size(); i ++)
    {           
        // already spawned?
        if( m_TrackPlayer[i]->spwaned )
            continue; 

        // be sure to set the cursor to the current frame 
        m_TrackPlayer[i]->cursor = frame;

        // info base can be null if the ghost is already finished
        if(GhostRecorderInfoBase* ghostInfo = getCurrentInfo(i, bfalse))
        {
            if ( spawnGhost(ghostInfo->m_Pos, i) )
        {
            m_TrackPlayer[i]->spwaned = btrue;            
        }        
    }
    }
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::prefetchGhostBeforeReading()
{
    spawnGhostForReading();
    m_bIsPrefetched = btrue;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::startReading( u32 frameStart /*= 0*/ )
{
	m_toggleReadingASAP = bfalse;
    m_bViewMode = btrue;
    m_bIsReadingPaused = bfalse;
    m_startReadingFrame = CURRENTFRAME;
    m_frameCounter = 0;

    if(!m_bIsPrefetched)
    {
        m_playersGhost.clear();
        resetAllGhosts();
        spawnGhostForReading(frameStart);
    }
}
//////////////////////////////////////////////////////////////////////////
u32 GhostManager::getGhostIndexFromGhostActor( const ActorRef & actorRef )
{
    for ( u32 i = 0; i < m_playersGhost.size(); i++ )
    {
        if ( m_playersGhost[i] == actorRef )
        {
            return i;
        }
    }
    return U32_INVALID;
}
//////////////////////////////////////////////////////////////////////////
GhostFileHeader* GhostManager::getHeaderFromGhostIndex( u32 ghostIndex )
{
    if (ghostIndex < m_TrackPlayerHeader.size())
    {
        return &m_TrackPlayerHeader[ghostIndex];
    }
    return NULL;
}
//////////////////////////////////////////////////////////////////////////
u32 GhostManager::getPlayerRecordingCount() const
{
    u32 count = 0;
    for( u32 i = 0; i < Ghost_max_players_toRecord; i++)
    {
        if(m_playersRecorded[i].playerRef.isValid())
            count ++;
    }
    return count;
}

void GhostManager::registerNewPlayerToRecord( const ObjectRef & actorRef, u32 playerIndex )
{
    for(u32 i = 0; i< m_playersRecorded.size(); i++)
    {
        if(m_playersRecorded[i].playerIndex == playerIndex)
        {
            ITF_ASSERT_MSG(0, "Already registered");
            return; 
        }
    }

    PlayerRecordedInfo info;
    info.playerIndex = playerIndex;
    info.playerRef = actorRef;
    info.cursorPosition = 0;
	info.needToSave		= btrue;
    info.ghostIndexDupplicated = U32_INVALID;

    m_playersRecorded.push_back(info);

    vGhost instanceGhost;
    m_TrackRecorder.push_back(instanceGhost);
    m_CurrentGhostCountRecording = m_TrackRecorder.size();

    ITF_ASSERT( m_TrackRecorder.size() == m_playersRecorded.size() );
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::getInactiveGhostIndexList( ITF_VECTOR<u32> & _list )
{
    for ( u32 i = 0; i < m_TrackPlayer.size(); i++)
    {
        if(!m_TrackPlayer[i]->visible && !m_TrackPlayer[i]->over)
        {
            _list.push_back(i);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
u32 GhostManager::getReadingGhostListSize()
{
    u32 count = 0;
    for ( u32 i = 0; i < m_TrackPlayer.size(); i++)
    {
        if(m_TrackPlayer[i]->visible &&  !m_TrackPlayer[i]->over)
        {
            count ++;
        }
    }
    return count;
}
//////////////////////////////////////////////////////////////////////////
u32 GhostManager::getLocalGhostIndexFromGhostIndex( u32 ghostIndex )
{
    if(ghostIndex<m_TrackPlayer.size())
    {
        return m_TrackPlayer[ghostIndex]->localIndex;
    }
    ITF_ASSERT(0);
    return U32_INVALID;
}
//////////////////////////////////////////////////////////////////////////
u32 GhostManager::getGhostIndexFromPlayer( const ActorRef & actorRef )
{
    for ( u32 i = 0; i < m_TrackPlayer.size(); i++)
    {
        if(m_TrackPlayer[i]->player == actorRef)
        {
            return i;
        }
    }    
    return U32_INVALID;
}
//////////////////////////////////////////////////////////////////////////
void GhostManager::setPlayerRef( u32 _ghostIndex, const ActorRef & _actorRef )
{
    if(_ghostIndex<m_TrackPlayer.size())
        m_TrackPlayer[_ghostIndex]->player = _actorRef;
}
//////////////////////////////////////////////////////////////////////////
i32   GhostManager::task_SaveGhost (void* pParam) 
{
	GhostManager* pGhostManager = (GhostManager*) pParam;

#ifdef ITF_WINDOWS
	// on PC, save a file 
	pGhostManager->buildName(pGhostManager->m_lastGhostSaved, "", "_sav");
	u32 count = 0;

	// Find a free name 
	while (FILEMANAGER->fileExists( pGhostManager->m_lastGhostSaved ))
	{
		count++;

		if( count == Ghost_MaxFileCountByMap )
		{
			count = 0; // will exit the while()
		}

		pGhostManager->m_lastGhostSaved.clear();
		String8 extend;
		extend.setTextFormat("_sav_%d", count);
		pGhostManager->buildName(pGhostManager->m_lastGhostSaved, "", extend);        

		if(count == 0)
			break;
	}

	// Now open/create the file
	File* ghostFile = FILEMANAGER->openFile(pGhostManager->m_lastGhostSaved,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
	if (ghostFile)
	{
#endif // ITF_WINDOWS 


		// Prepare data to serialize
		ArchiveMemory amw;
		pGhostManager->serializeGhost(amw, pGhostManager->m_recordInputType);

		// Warning: here we have to create a temp buffer to give the full buffer to the event 
		// if the ghost size grows up in the futur, maybe we should do different 
		pGhostManager->freeStaticBuffer();        
        m_lastSavedGhostBuffer = (u8*)Memory::mallocCategory( amw.getSize() + pGhostManager->m_savedHeader.getHeaderSize(),MemoryId::mId_Ghost );


		u32 outLen = Compress::compressBuffer(m_lastSavedGhostBuffer + pGhostManager->m_savedHeader.getHeaderSize(), amw.getSize(), amw.getData(), amw.getSize());
		if ( outLen == 0 ) // compression failed
		{
			m_lastSavedGhostBufferSize  = amw.getSize() + pGhostManager->m_savedHeader.getHeaderSize();
			pGhostManager->m_savedHeader.setCompressed(bfalse);

			ITF_Memcpy( m_lastSavedGhostBuffer + pGhostManager->m_savedHeader.getHeaderSize(), amw.getData(), amw.getSize() );
			LOG("COMPRESSED_FAILED: %i, UNCOMPRESSED: %d", outLen, amw.getSize());
		} else
		{
			m_lastSavedGhostBufferSize  = outLen + pGhostManager->m_savedHeader.getHeaderSize();
			pGhostManager->m_savedHeader.setCompressed(btrue);
			pGhostManager->m_savedHeader.setUncompressedSize(amw.getSize());
			f32 byteByFrame = (f32)outLen / (f32)pGhostManager->m_uCurrentRecordPosition;
			LOG("COMPRESSED: %d, UNCOMPRESSED: %d. Byte by frame compressed: %.4f", outLen, amw.getSize(), byteByFrame);
		}        

		LOG("TOTAL SIZE OF THE GHOST: %d", m_lastSavedGhostBufferSize);

		// And serialize header 
		pGhostManager->m_savedHeader.buildHeaderFromData();

		// Copy header + keyframes
		ITF_Memcpy( m_lastSavedGhostBuffer, pGhostManager->m_savedHeader.getHeaderBuffer(), pGhostManager->m_savedHeader.getHeaderSize() );


#ifdef ITF_WINDOWS        
		ghostFile->write(m_lastSavedGhostBuffer, m_lastSavedGhostBufferSize);
		FILEMANAGER->closeFile(ghostFile);
#endif // ITF_WINDOWS


#ifdef ITF_WINDOWS
	}
#endif //#ifdef ITF_WINDOWS

	return 1;
}
//////////////////////////////////////////////////////////////////////////
void  GhostManager::onTask_SaveGhostFinish (void* pParam, i32 _result) 
{
    if(_result == TaskManager::TaskAborted)
        return;

	GhostManager* pGhostManager = (GhostManager*) pParam;

	EventGhostOnSaved eventOnSaved( &pGhostManager->m_savedHeader, m_lastSavedGhostBuffer, m_lastSavedGhostBufferSize );
	EVENTMANAGER->broadcastEvent(&eventOnSaved);
	pGhostManager->m_savingState = Sav_Idle;

	LOG("onTask_SaveGhostFinish() ");
}
//////////////////////////////////////////////////////////////////////////
u32 GhostManager::getGhostCursorByUserData( u32 _ghostIndex, u32 _data )
{
	if(_ghostIndex<m_TrackPlayer.size())
	{
        ITF_MAP<u32,u32>::const_iterator it = m_TrackPlayer[_ghostIndex]->userData.find( _data );
        if(it == m_TrackPlayer[_ghostIndex]->userData.end())
            return U32_INVALID;            
		return it->second;
	}
	return U32_INVALID;
}
//////////////////////////////////////////////////////////////////////////
u32 GhostManager::getGhostCursor( u32 _ghostIndex )
{
    if(_ghostIndex<m_TrackPlayer.size())
    {
        return m_TrackPlayer[_ghostIndex]->cursor;
	}
	return U32_INVALID;
}
//////////////////////////////////////////////////////////////////////////
u32 GhostManager::addEmptyGhost(const StringID & _costumeId)
{
    GhostReader* reader = newAlloc(mId_Ghost, GhostReader);
    reader->cursor = 0;
    reader->visible = bfalse;
    reader->over = bfalse;
    reader->localIndex = 0;
    reader->spwaned = bfalse;
    reader->needspawn = btrue;
    reader->isDupplicated = btrue;
    reader->ghost.clear();

    GhostFileHeader header;
    header.setVersion(Ghost_VERSION);
    header.setGhostIntruderType(2);
    header.setGhostCostumeID(_costumeId, 0);

    m_TrackPlayer.push_back(reader);
    m_TrackPlayerHeader.push_back(header);

    return m_TrackPlayer.size() - 1;
}
//////////////////////////////////////////////////////////////////////////
PlayerRecordedInfo* GhostManager::getPlayerRecordedInfo( u32 _playerIndex )
{
    for(u32 i = 0; i < m_playersRecorded.size(); i++)
    {
        PlayerRecordedInfo* info = &m_playersRecorded[i];
        if ( info->playerIndex == _playerIndex )
        {
            return info;
        }
    }
    return NULL;
}
//////////////////////////////////////////////////////////////////////////
bbool GhostFileHeader::buildHeaderFromBuffer( u8* buffer, u32 bufferSize )
{
    if(bufferSize<getHeaderSize())
    {
        ITF_ASSERT_MSG(0, "Invalid Ghost file");
        return bfalse;
    }

    // First is version 
    ArchiveMemory am( buffer, bufferSize );
    am.serialize(m_version);

    if(m_version!=Ghost_VERSION)
    {
        ITF_ASSERT_MSG(0, "Invalid Ghost version");
        return bfalse;
    }

    am.serialize(m_magicNumber);
    m_path.serialize(am);
    am.serialize(m_score);
    am.serialize(m_enduranceSeed);
    am.serialize(m_enduranceDist);
    am.serialize(m_enduranceTime);
    am.serialize(m_ghostIntruderType);
    for(u32 i = 0; i < Ghost_max_players_toRecord; i++)
        m_ghostCostumeID[i].serialize(am);
    am.serialize(m_ghostUniqueID);
    am.serialize(m_isCompressed);
    am.serialize(m_uncompressedSize);
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
bbool GhostFileHeader::buildHeaderFromFile( const Path & filePath )
{
    File* ghostFile = FILEMANAGER->openFile(filePath,ITF_FILE_ATTR_READ);
    if (ghostFile)
    {
        u32 lenght = (u32) ghostFile->getLength();
        if(lenght<getHeaderSize())
        {
            ITF_ASSERT_MSG(0, "Invalid Ghost file");
            FILEMANAGER->closeFile(ghostFile); 
            return bfalse;
        }

        reset();
        ghostFile->read(m_headerBuffer,getHeaderSize());
        FILEMANAGER->closeFile(ghostFile); 

        return buildHeaderFromBuffer(m_headerBuffer, getHeaderSize());
    }
    return bfalse;
}
//////////////////////////////////////////////////////////////////////////
u32 GhostFileHeader::getHeaderSize() 
{
    return Ghost_HeaderSize;
}
//////////////////////////////////////////////////////////////////////////
bbool GhostFileHeader::buildHeaderFromData()
{    
    ArchiveMemory am;
    am.serialize(m_version);
    am.serialize(m_magicNumber);
    m_path.serialize(am);
    if((u32)am.getSize()>getHeaderSize())
    {
        ITF_ASSERT(0);
        return bfalse;
    }
    am.serialize(m_score);
    am.serialize(m_enduranceSeed);
    am.serialize(m_enduranceDist);
    am.serialize(m_enduranceTime);
    am.serialize(m_ghostIntruderType);
    for(u32 i = 0; i < Ghost_max_players_toRecord; i++)
        m_ghostCostumeID[i].serialize(am);
    am.serialize(m_ghostUniqueID);
    am.serialize(m_isCompressed);
    am.serialize(m_uncompressedSize);
    resetBuffer();
    ITF_Memcpy(m_headerBuffer, am.getData(), am.getSize());
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
void GhostFileHeader::resetBuffer()
{
    ITF_MemSet(m_headerBuffer, 0, getHeaderSize());
}
//////////////////////////////////////////////////////////////////////////
void GhostFileHeader::setPath( const Path & path )
{
    m_path = path;
}
//////////////////////////////////////////////////////////////////////////
void GhostFileHeader::reset()
{
    resetBuffer();

    m_version = 0;
    m_path.clear();
    m_score = 0;
    m_enduranceSeed = U32_INVALID;
    m_enduranceDist = 0.f;
    m_enduranceTime = 0.f;
    m_ghostIntruderType = 0;
}
//////////////////////////////////////////////////////////////////////////
u32 GhostFileHeader::m_magicNumber = 4276992687;    // 0xFEEDBAAF

//////////////////////////////////////////////////////////////////////////
GhostRecorderInfoBase::GhostRecorderInfoBase():
      m_mask(0)
    , m_bAvailable(bfalse)
	, m_Pos(0, 0, 0)
	, m_userData(0)
	, m_angle(0)
{

}
//////////////////////////////////////////////////////////////////////////
GhostRecorderAnimInfo::GhostRecorderAnimInfo() :
      m_inputInfosCount(0)
    , m_inputInfos(NULL)
    , m_firstFrame(btrue)
    , m_IsLookingRight(btrue)
    , m_Scale(1,1)    
    , m_inputMaskLower(0)
    , m_inputMaskUpper(0)
    , m_AnimTime(0)
{
}
//////////////////////////////////////////////////////////////////////////

GhostRecorderPadInfo::GhostRecorderPadInfo():      
      m_leftJoy(Vec2d::Zero)
    , m_isLookingRightAtFirstFrame(btrue)
    , GhostRecorderInfoBase()
{
    for(u32 i = 0; i < ListenersCount; i++)
        m_stateListeners[i] = bfalse;
}

//////////////////////////////////////////////////////////////////////////
bbool GhostKeyFrame::buildBuffer()
{
    ArchiveMemory am;
    u32 keycount = m_pos.size();
    am.serialize(keycount);
    for (u32 i = 0 ; i < m_pos.size(); i++)
    {
        am.serialize( m_pos[i] );
    }

    if(m_keyFrameBuffer)
        Memory::free(m_keyFrameBuffer);

    m_keyFrameBuffer = (u8*) Memory::mallocCategory(am.getSize(),MemoryId::mId_Ghost);
    {
        ITF_Memcpy(m_keyFrameBuffer, am.getData(), am.getSize());
    }

    return btrue;
}
//////////////////////////////////////////////////////////////////////////
bbool GhostKeyFrame::buildKeyFramesList( u8* buffer_, u32 buffersize_ )
{
    if(buffer_ == NULL || buffersize_ == 0)
        return bfalse;

    ArchiveMemory am(buffer_, buffersize_);
    u32 keyCount;
    am.serialize(keyCount);

    if(keyCount > Ghost_MaxSecondsRecorded)
        return bfalse;

    for(u32 i = 0; i < keyCount; i++)
    {
        i32 val ;
        am.serialize(val);
        add(val);
    }

    return btrue;
}

}
