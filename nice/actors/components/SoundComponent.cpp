#include "precompiled_engine.h"


#ifdef ITF_SUPPORT_RAKI

#include "adapters/AudioSerializedData/SoundComponent_common.h"
#include "adapters/AudioSerializedData/SoundDescriptor_common.h"

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/Resource.h"
#endif // _ITF_RESOURCE_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_SOUNDSTREAMED_H_
#include "engine/sound/StreamedSoundResource.h"
#endif // _ITF_SOUNDSTREAMED_H_

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(SoundComponent_Template)
    BEGIN_SERIALIZATION_CHILD(SoundComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("soundList",m_soundList);
    SERIALIZE_MEMBER("defaultSound",m_defaultSound);
    SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
    SERIALIZE_CONTAINER_WITH_FACTORY("musicList", m_musicList, GAMEINTERFACE->getEventFactory());
    SERIALIZE_CONTAINER_WITH_FACTORY("busMixList", m_busMixList, GAMEINTERFACE->getEventFactory());
    END_SERIALIZATION()

        SoundComponent_Template::SoundComponent_Template()
    {
    }

    SoundComponent_Template::~SoundComponent_Template()
    {
        MusicList::iterator musicIt = m_musicList.begin();
        for ( ; musicIt != m_musicList.end() ; musicIt++ )
        {
            SF_DEL(*musicIt);
        }
        m_musicList.clear();

        BusMixList::iterator busMixIt = m_busMixList.begin();
        for ( ; busMixIt != m_busMixList.end() ; busMixIt++ )
        {
            SF_DEL(*busMixIt);
        }
        m_busMixList.clear();
    }

    bbool SoundComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);

        // Load resources assigned on this actor
        for ( u32 i = 0 ; i < m_soundList.size() ; ++i )
        {
#ifndef ITF_FINAL
            m_soundList[i].checkLoopOption();
#endif

            // Call onTemplateLoaded() on descriptors
            m_soundList[i].onTemplateLoaded();

            // Get all resource associated to these sound descriptors and add them to our resourceContainer

            const ITF_VECTOR<Path> sounds = m_soundList[i].getFiles();
            const Resource::ResourceType resourceType = m_soundList[i].isStream() ? Resource::ResourceType_SoundStreamed : Resource::ResourceType_Sound;

            if (m_resources.find(m_soundList[i].getName())==-1)
            {
                ITF_VECTOR<ResourceID>& resourceIDs = m_resources.set(m_soundList[i].getName(),ITF_VECTOR<ResourceID>());

                for ( u32 j = 0 ; j < sounds.size() ; j++ )
                {

                    ResourceID resourceID = m_actorTemplate->getResourceContainer()->addResourceFromFile(resourceType,sounds[j]);
                    if ( resourceID.isValid() )
                    {
                        // add streaming info, before the sound is loaded
                        if (m_soundList[i].isStream())
                        {
                            StreamedSoundResource* soundStreamed = static_cast<StreamedSoundResource*>(resourceID.getResource());
                            soundStreamed->setPrefetched(m_soundList[i].isPrefetched());
                        }

                        resourceIDs.push_back(resourceID);
                    }
                }
            }
            else
            {
                ITF_WARNING_CATEGORY(Sound, NULL, bfalse, "%s : The sound descriptor %s has already been added in the resource array, either the file was included twice, or two sound descriptors have the same name.", m_actorTemplate->getFile().getBasename(), m_soundList[i].getName().getDebugString() );
            }
        }

        // Load extra resources needed from FeedbackFXManager
        // Don't call onTemplateLoaded on these descriptors, nor set isLoadedInPlace flag as it's coming from feedback FX manager
        // and it's already been done.
        FXControllerComponent_Template * FXControllerCmp_Tmpl = m_actorTemplate->GetComponent<FXControllerComponent_Template>();
        if( FXControllerCmp_Tmpl && (FXControllerCmp_Tmpl->getFeedbackTags().size() > 0) ) // nothing to add if we have no feedback tags (can happen at least in edition, with a bad template)
        {
            ITF_VECTOR<const SoundDescriptor_Template* > additonnalDescs;
            FEEDBACKFX_MANAGER->getAdditionnalSoundDescriptors(FXControllerCmp_Tmpl->getFeedbackTags(),additonnalDescs);

            for ( u32 i = 0 ; i < additonnalDescs.size() ; ++i )
            {
                const ITF_VECTOR<Path> sounds = additonnalDescs[i]->getFiles();
                if ( m_resources.find(additonnalDescs[i]->getName())== -1 )
                {
                    ITF_VECTOR<ResourceID>& resourceIDs = m_resources.setNewValue(additonnalDescs[i]->getName(),ITF_VECTOR<ResourceID>());
                    const Resource::ResourceType resourceType = additonnalDescs[i]->isStream() ? Resource::ResourceType_SoundStreamed : Resource::ResourceType_Sound;

                    for ( u32 j = 0 ; j < sounds.size() ; j++ )
                    {
                        ResourceID resourceID = m_actorTemplate->getResourceContainer()->addResourceFromFile(resourceType, sounds[j]);
                        if ( resourceID.isValid() )
                        {
                            resourceIDs.push_back(resourceID);
                        }
                    }
                }
            }
        }

        return bOk;
    }

    void SoundComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        for ( u32 i = 0 ; i < m_soundList.size() ; ++i )
        {
            m_soundList[i].onTemplateDelete();
        }

        for ( u32 i = 0 ; i < m_resources.size() ; ++i )
        {
            const ITF_VECTOR<ResourceID>& resources = m_resources.getAt(i);
            for ( u32 j = 0 ; j < resources.size() ; j++ )
                m_actorTemplate->getResourceContainer()->removeResource(resources[j]);
        }

        m_resources.free();

        Super::onTemplateDelete(_hotReload);
    }

    void SoundComponent_Template::setLoadedInPlace()
    {
        Super::setLoadedInPlace();

        for ( u32 i = 0 ; i < m_soundList.size() ; ++i )
        {
            m_soundList[i].setLoadedInPlace();
        }
    }

    ResourceID SoundComponent_Template::getResourceId(const StringID& _descName, const u32 _index) const
    {
        i32 descIndex = m_resources.find(_descName);

        if ( descIndex != -1 )
        {
            ITF_WARNING_CATEGORY(GPP,NULL,_index < m_resources.getAt(descIndex).size(),"Access out of bound : there is no resourceID available on %s at descIndex %d",_descName.getDebugString(),_index);
            if ( _index < m_resources.getAt(descIndex).size() )
            {
                return m_resources.getAt(descIndex)[_index];
            }
        }

        return ResourceID::Invalid;
    }

    ResourceID SoundComponent_Template::getResourceIdAt(const u32 _index) const
    {
        u32 count = 0;

        for ( u32 i = 0 ; i < m_resources.size() ; ++i )
        {
            if ( _index < (count + m_resources.getAt(i).size()) )
                return m_resources.getAt(i)[_index-count];

            count += m_resources.getAt(i).size();
        }

        return ResourceID::Invalid;
    }


    u32 SoundComponent_Template::getResourceCount() const
    {
        u32 count = 0;

        for ( u32 i = 0 ; i < m_resources.size() ; ++i )
        {
            count += m_resources.getAt(i).size();
        }

        return count;
    }

    IMPLEMENT_OBJECT_RTTI(SoundComponent)
        BEGIN_SERIALIZATION_CHILD(SoundComponent)
        SERIALIZE_CONTAINER_OBJECT("soundList", m_soundListTemplate);
    END_SERIALIZATION()

        void SoundComponent::releaseAllInstances()
    {
        u32 numInstance = m_instances.size();

        for (u32 i = 0; i < numInstance; ++i)
        {
            SOUND_MANAGER->releaseInstanceOnOwnerDestroy(m_instances[i]);
        }

        m_instances.clear();
    }

    SoundComponent::SoundComponent()
        : Super()
        , m_soundInputUpdate(NULL)
        , m_animLight(NULL)
        , m_dieRequested(bfalse)
        , m_cameraPrevPos(Vec3d::Zero)
		, m_isEnable(btrue)
    {
        m_random.SetSeed(u32(uPtr(this)));
        m_random.Reset();
    }

    SoundComponent::~SoundComponent()
    {
        releaseAllInstances();

        MusicList::iterator musicIt = m_musicList.begin();
        for ( ; musicIt != m_musicList.end() ; musicIt++ )
        {
            SF_DEL(*musicIt);
        }
        m_musicList.clear();

        BusMixList::iterator busMixIt = m_busMixList.begin();
        for ( ; busMixIt != m_busMixList.end() ; busMixIt++ )
        {
            SF_DEL(*busMixIt);
        }
        m_busMixList.clear();
    }

    ResourceID SoundComponent::getResourceId(const StringID& _descName, const u32 _index) const
    {
        i32 descIndex = m_resources.find(_descName);

        if ( descIndex != -1 )
        {
            ITF_WARNING_CATEGORY(GPP,NULL,_index < m_resources.getAt(descIndex).size(),"Access out of bound : there is no resourceID available on %s at descIndex %d",_descName.getDebugString(),_index);
            if ( _index < m_resources.getAt(descIndex).size() )
            {
                return m_resources.getAt(descIndex)[_index];
            }
        }

        return getTemplate()->getResourceId(_descName,_index);
    }

    void SoundComponent::onResourceLoaded()
    {
#ifndef ITF_FINAL
        for (ITF_VECTOR<SoundDescriptor>::iterator it = m_soundList.begin(); it != m_soundList.end(); ++it)
        {
            const SoundDescriptor_Template* desc = it->getTemplate();

            for (u32 i=0; i < desc->getFileCount(); ++i)
            {
                if (desc->isStream())
                {
                    const StreamedSoundResource *sound = static_cast<StreamedSoundResource *>(getResourceId(desc->getName(), i).getResource());

                    if (sound && sound->getBinWaveData())
                    {
                        ITF_WARNING_CATEGORY(Sound, NULL, desc->getNumChannels() == sound->getNbChannels(), 
                            "Number of channel in the wave file \"%s\" (%d) is different from the one set in the sound Descriptor \"%s\" (%d)", 
                            sound->getPath().getBasename(), sound->getNbChannels(), desc->getName().getDebugString(), desc->getNumChannels() );
                    }
                }
                else
                {
                    const Sound *sound = static_cast<Sound *>(getResourceId(desc->getName(), i).getResource());

                    if (sound && sound->getWaveData())
                    {
                        ITF_WARNING_CATEGORY(Sound, NULL, desc->getNumChannels() == sound->getNbChannels(), 
                            "Number of channel in the wave file \"%s\" (%d) is different from the one set in the sound Descriptor \"%s\" (%d)", 
                            sound->getPath().getBasename(), sound->getNbChannels(), desc->getName().getDebugString(), desc->getNumChannels() );
                    }
                }
            }
        }
#endif
    }

    void SoundComponent::onUnloadResources()
    {
        releaseAllInstances();
    }

    bbool SoundComponent::keepAlive()
    {
        ///For each instance, if voice is allocated...
        u32 numInstances = m_instances.size();

        for (u32 i = 0; i < numInstances; i++)
        {
            SoundPlayingID handle = m_instances[i];

            if (handle.isValid() && SOUND_MANAGER->isPlaying(handle))
                return btrue;
        }
        return bfalse;
    }

    void SoundComponent::Update( f32 _deltaTime )
    {
        if ( m_soundInputUpdate )
        {
            m_soundInputUpdate->updateSoundInput();
        }

        if ( getTemplate()->getDefaultSound().isValid() && !m_defaultSoundInstance.isValid() )
        {
            m_defaultSoundInstance = playSound(getTemplate()->getDefaultSound());
        }

        Vec3d pos = m_actor->getPos();
        const Vec3d& micPos = View::getGameplayView()->getCamera()->getPos();
        const Vec3d& micSpeed = micPos - m_cameraPrevPos;

        ///For each instance, if voice is allocated...
        u32 numInstances = m_instances.size();
        u32 i = 0;

        while ( i < numInstances )
        {
            if (SOUND_MANAGER->isPlaying(m_instances[i]))
            {
                // set sound position
                if (SOUND_MANAGER->getPlayMode(m_instances[i]) == FXPlayMode_AttachedToBone)
                {
                    if (m_animLight)
                    {
                        m_animLight->getBonePos(SOUND_MANAGER->getBoneIndex(m_instances[i]),pos);
                    }
                }
                else if (SOUND_MANAGER->getPlayMode(m_instances[i]) == FXPlayMode_Position )
                {
                    pos = SOUND_MANAGER->getDesiredPos(m_instances[i]);
                }
                SOUND_MANAGER->setPos(m_instances[i], pos);
                SOUND_MANAGER->setMicPos(m_instances[i], micPos);
                SOUND_MANAGER->setMicSpeed(m_instances[i], micSpeed);
            }
            else
            {
                if(!SOUND_MANAGER->getIsSerial(m_instances[i]))
                {
                    // we don't need to actually do anything to the instance, as it will be destroyed by the sound manager automatically
                    m_instances.removeAtUnordered(i);
                    numInstances--;
                    continue;
                }
            }

            i++;
        }

        m_cameraPrevPos = micPos;
    }


    // Specific add sound
    // Must be called before loadResources
    void SoundComponent::setSoundList(const SafeArray<SoundDescriptor_Template *> & soundDescriptorList)
    {
        ITF_ASSERT(!m_soundListTemplate.size());
        m_soundListTemplate.resize(soundDescriptorList.size());
        u32 count = soundDescriptorList.size();

        for (u32 i=0; i<count; i++)
        {
            BinaryClone(soundDescriptorList[i], &m_soundListTemplate[i]);
            m_soundListTemplate[i].onTemplateLoaded();
        }

        int index = m_soundList.size();
        count = index + m_soundListTemplate.size();
        m_soundList.resize(count);

        for (u32 i=index; i<count; i++)
        {
            SoundDescriptor * desc = &m_soundList[i];
            desc->init(&m_soundListTemplate[i-index],this);
            desc->onActorLoaded(m_actor);
            m_sounds[desc->getTemplate()->getName()] = i;
        }
    }

    void SoundComponent::setSoundPos(const SoundPlayingID _handle, const Vec3d& _pos)
    {
        if (_handle.isValid())
        {
            SOUND_MANAGER->setDesiredPos(_handle, _pos);
            SOUND_MANAGER->setPlayMode(_handle, FXPlayMode_Position);
        }
    }

    void SoundComponent::attachToBone(const SoundPlayingID _handle, u32 _boneIndex)
    {
        if (_handle.isValid())
        {
            SOUND_MANAGER->setBoneIndex(_handle, _boneIndex);
            SOUND_MANAGER->setPlayMode(_handle, FXPlayMode_AttachedToBone);
        }
    }

    void SoundComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animLight = m_actor->GetComponent<AnimLightComponent>();

        m_inputList.resize(getTemplate()->getInputList().size());

        for ( u32 i = 0; i < m_inputList.size(); i++ )
        {
            Input* input = &m_inputList[i];
            const InputDesc& inputDesc = getTemplate()->getInputList()[i];

            input->setId(inputDesc.getId());
            input->setType(inputDesc.getType());

            if ( inputDesc.getType() == InputType_F32 )
            {
                input->setValue(0.f);
            }
            else
            {
                input->setValue(static_cast<u32>(0));
            }
        }

        //FX

        m_soundList.resize(getTemplate()->getSoundList().size() + m_soundListTemplate.size());
        u32 i = 0;
        for (SoundList::const_iterator it = getTemplate()->getSoundList().begin(); it != getTemplate()->getSoundList().end(); ++it)
        {
            SoundDescriptor * desc = &m_soundList[i];
            if ( desc->init(&(*it),this) )
            {
                desc->onActorLoaded(m_actor);
                m_sounds[desc->getTemplate()->getName()] = i;
                ++i;
            }
        }
        for (SoundList::iterator it = m_soundListTemplate.begin(); it != m_soundListTemplate.end(); ++it)
        {
            SoundDescriptor * desc = &m_soundList[i];
            it->onTemplateLoaded();
            // Get all resource associated to these sound descriptors and add them to our resourceContainer
            const ITF_VECTOR<Path> sounds = it->getFiles();
            // already checked in the template
            ITF_ASSERT_MSG( m_resources.find(it->getName())==-1, "%s : Sound descriptor \"%s\" has already been added in the resource array, please check this (it should only be added once)", m_actor->getTemplate()->getFile().getBasename(), it->getName().getDebugString() );

            ITF_VECTOR<ResourceID>& resourceIDs = m_resources.set(it->getName(),ITF_VECTOR<ResourceID>());
            const Resource::ResourceType resourceType = it->isStream() ? Resource::ResourceType_SoundStreamed : Resource::ResourceType_Sound;

            for ( u32 j = 0 ; j < sounds.size() ; j++ )
            {
                ResourceID resourceID = m_actor->getResourceContainer()->addResourceFromFile(resourceType, sounds[j]);
                if ( resourceID.isValid() )
                {
                    resourceIDs.push_back(resourceID);
                }
            }

            if ( desc->init(&(*it),this) )
            {
                desc->onActorLoaded(m_actor);
                m_sounds[desc->getTemplate()->getName()] = i;
                ++i;
            } 
            else
            {
                if( SOUND_MANAGER->getInitState() == btrue) //only assert if sound engine is init
                    ITF_ASSERT(0);
            }
        }

        if (i != m_soundList.size())
        {
            ITF_ASSERT(i < m_soundList.size());
            m_soundList.resize(i);
        }

        // Check Music events
        const MusicList& musicList = getTemplate()->getMusicList();
        m_musicNames.clear();
        m_musicNames.resize(musicList.size(),StringID::Invalid);
        for ( u32 i = 0 ; i < musicList.size() ; i++ )
        {
            const Event* musicEvent = musicList[i];

            const StringID::StringIdValueType musicEventCRC = musicEvent->GetObjectClassCRC();

            ITF_WARNING_CATEGORY(Sound,m_actor,musicEventCRC == EventPlayMusic_CRC || musicEventCRC == EventStopMusic_CRC, "Music List contains an invalid event (it should only be EventPlayMusic or EventStopMusic types)");

            if ( musicEventCRC == EventPlayMusic_CRC
                || musicEventCRC == EventStopMusic_CRC )
            {
                m_musicList.push_back(DYNAMIC_CAST(musicEvent->CreateNewObject(),Event));
                BinaryClone(const_cast<Event*>(musicEvent), m_musicList.back());

                if ( EventPlayMusic * eventPlayMusic = DYNAMIC_CAST(m_musicList.back(),EventPlayMusic) )
                {
                    m_musicNames[i] = eventPlayMusic->getName();
                }
                else if ( EventStopMusic * eventStopMusic = DYNAMIC_CAST(m_musicList.back(),EventStopMusic) )
                {
                    m_musicNames[i] = eventStopMusic->getName();
                }
            }
        }

        // Check busmix events
        const BusMixList& busMixList = getTemplate()->getBusMixList();
        m_busMixNames.clear();
        m_busMixNames.resize(busMixList.size(),StringID::Invalid);
        for ( u32 i = 0 ; i < busMixList.size() ; i++ )
        {
            EventBusMix * eventBusMix = DYNAMIC_CAST(busMixList[i],EventBusMix);

            ITF_WARNING_CATEGORY(Sound,m_actor,eventBusMix, "BusMix List contains an invalid event (it should only be EventBusMix)");

            if ( eventBusMix )
            {
                m_busMixNames[i] = eventBusMix->getName();

                m_busMixList.push_back(DYNAMIC_CAST(busMixList[i]->CreateNewObject(),Event));
                BinaryClone(const_cast<Event*>(busMixList[i]), m_busMixList.back());
                m_busMixList.back()->setSender(m_actor->getRef());
            }
        }

        //Register this component for events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventTrigger_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventDie_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetFloatInput_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetUintInput_CRC,this);

        m_dieRequested = bfalse;
    }

    void SoundComponent::onStartDestroy( bbool _hotReload )
{
        Super::onStartDestroy();

        // Release FeedbackFXManager sound descriptors
        FEEDBACKFX_MANAGER->releaseSoundDescriptors(m_actor);
        m_feedbackSounds.clear();
    }

    SoundPlayingID SoundComponent::playSound(StringID _name, u32 _animBoneIndex, const SoundDescriptor_Template * _soundDesc, const StringID& _customCategory /*= StringID::Invalid*/, bbool _isOwner /*= btrue*/)
    {
        if(!m_isEnable)
            return SoundPlayingID::getInvalidHandle();

        // avoid to play a sound if actor is destroyed at the end of the frame
        if ( m_actor->isDestructionRequested() )
            return SoundPlayingID::getInvalidHandle();

        if( SOUND_MANAGER->getInitState() == bfalse)
            return SoundPlayingID::getInvalidHandle();

        SoundDescriptor * desc = NULL;

        // A sound descriptor has been specified, then check feedback list first to get descriptor
        if ( _soundDesc )
        {
            ITF_MAP<StringID,SoundDescriptor *>::const_iterator feedback_it = m_feedbackSounds.find(_name);

            // A slot has already been taken from FeedbackFXManager ?
            if ( feedback_it != m_feedbackSounds.end() )
            {
                desc = feedback_it->second;
            }
            // Ask a free sound descriptor slot to FeedbackFXManager, and get SoundDescriptor
            else
            {
                desc = FEEDBACKFX_MANAGER->getNewSoundDescriptor( m_actor, _soundDesc, this);
                if ( desc )
                {
                    m_feedbackSounds[_name] = desc;
                }
            }
        }
        else
        {
            ITF_ASSERT(_isOwner);
            SoundMap::const_iterator it = m_sounds.find(_name);
            desc = it != m_sounds.end() ? &m_soundList[it->second] : NULL;
        }

        ITF_WARNING_CATEGORY(Sound, NULL, desc != NULL,"Unknown sound name %s on actor %s <%s>",_name.getDebugString(),m_actor ? m_actor->getUserFriendly().cStr() : "null actor", m_actor ? m_actor->getTemplatePath().toString8().cStr() : "");

        if (desc)
        {
            // set custom category
            desc->setCustomCategory(_customCategory);

            u32 soundCount = desc->getTemplate()->getFileCount();
            u32 soundIndex = 0;
            if(desc->getTemplate()->getParams()->getPlayMode() == PlayMode_Serie)
                desc->reinit();

            if (!soundCount)
                return SoundPlayingID::getInvalidHandle();

            soundIndex = desc->getNextSoundIndex();
            ResourceID soundResID = ResourceID::Invalid;
            if (soundIndex != U32_INVALID)
            {
                if ( _isOwner )
                {
                    soundResID = getResourceId(desc->getTemplate()->getName(),soundIndex);
                }
                else
                {
                    // Get resource ID from resource manager directly as it's a sound from a feedback
                    // for the moment, sounds from resource manager are not streamed (see if usefull to add stream here)
                    if (desc->isStream())
                        soundResID = m_actor->addResource(Resource::ResourceType_SoundStreamed,desc->getTemplate()->getFiles()[soundIndex]);
                    else
                        soundResID = m_actor->addResource(Resource::ResourceType_Sound,desc->getTemplate()->getFiles()[soundIndex]);
                }
            }

            if (!soundResID.isValid())
                return SoundPlayingID::getInvalidHandle();

            SoundPlayingID handle = SoundPlayingID::getInvalidHandle();

            if (desc->isStream())
            {
                StreamedSoundResource *soundStreamed = static_cast<StreamedSoundResource *>(soundResID.getResource());
                if (!soundStreamed)
                    return SoundPlayingID::getInvalidHandle();

                handle = SOUND_MANAGER->acquireInstance(soundStreamed, GetActor()->getRef(), desc);
            }
            else
            {
                Sound *sound = static_cast<Sound *>(soundResID.getResource());
                if (!sound)
                    return SoundPlayingID::getInvalidHandle();

                handle = SOUND_MANAGER->acquireInstance(sound, GetActor()->getRef(), desc);
            }

            if (handle.isValid())
            {

#if !defined(ITF_FINAL)
#if defined(ITF_DEBUG_STRINGID)
                SOUND_MANAGER->debugSetName(handle, GetActor()->getUserFriendly() + "::" + desc->getTemplate()->getName().getDebugString() + "(" + soundResID.getDebugString().cStr() + ")");
#else
                SOUND_MANAGER->debugSetName(handle, GetActor()->getUserFriendly() + "(" + soundResID.getDebugString().cStr() + ")");
#endif
#endif 
                SOUND_MANAGER->setBoneIndex(handle, _animBoneIndex);
                Vec3d pos = m_actor->getPos();
                if (_animBoneIndex != U32_INVALID)
                {
                    if (m_animLight)
                    {
                        m_animLight->getBonePos(_animBoneIndex,pos);
                        SOUND_MANAGER->setPlayMode(handle, FXPlayMode_AttachedToBone);
                    }
                }
                else
                {
                    SOUND_MANAGER->setPlayMode(handle, FXPlayMode_Actor);
                }

                SOUND_MANAGER->setPos(handle, pos);

                if (View::getGameplayView() && View::getGameplayView()->getCamera())
                    SOUND_MANAGER->setMicPos(handle, View::getGameplayView()->getCamera()->getPos());

                SOUND_MANAGER->play(handle);

                m_instances.push_back(handle);

                return handle;
            }
            else if(desc->getTemplate()->getLimitInstanceMode() == LimiterMode_OnlyOnePlaying)//limit case for onlyoneplaying limiter mode ( the sound has already be launched once)
            {
                return SOUND_MANAGER->acquireFirstInstance(desc->getTemplate());
            }

        }

        return SoundPlayingID::getInvalidHandle();
    }

    void SoundComponent::stopSound(SoundPlayingID& _handle, bbool _onlyStopLooping)
    {
        if (_handle.isValid())
        {
            if (!_onlyStopLooping || (_onlyStopLooping && SOUND_MANAGER->isLooping(_handle)))
            {
                SOUND_MANAGER->stop(_handle);
                _handle.invalidate();
            }
        }
    }

    void SoundComponent::stopAll( bbool _onlyStopLooping )
    {
        u32 numInstances = m_instances.size();

        for(u32 index = 0; index < numInstances; ++index)
        {
            stopSound(m_instances[index],_onlyStopLooping);
        }

        m_instances.clear();
    }

    bbool SoundComponent::isPlaying(const SoundPlayingID _handle) const
    {
        if (_handle.isValid())
            return SOUND_MANAGER->isPlaying(_handle);
        else
            return bfalse;
    }

    void SoundComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if (EventTrigger * evt = DYNAMIC_CAST(_event,EventTrigger))
        {
            if (evt->getActivated())
            {
                if (getTemplate()->getDefaultSound().isValid() && !m_defaultSoundInstance.isValid() && !m_dieRequested)
                {
                    m_defaultSoundInstance = playSound(getTemplate()->getDefaultSound());
                }
            }
            else
            {
                if (m_defaultSoundInstance.isValid())
                {
                    stopSound(m_defaultSoundInstance);
                }
            }
        }
        else if (EventDie * eventDie = DYNAMIC_CAST(_event,EventDie))
        {
            m_dieRequested = !eventDie->isDisableOnly();

            stopAll(btrue);
        }
        else if (EventSetFloatInput * onSetInput = DYNAMIC_CAST(_event,EventSetFloatInput))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
        }
        else if (EventSetUintInput * onSetInput = DYNAMIC_CAST(_event,EventSetUintInput))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
        }
    }

    void SoundComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if ( getTemplate()->getDefaultSound().isValid() && !m_defaultSoundInstance.isValid() && !m_dieRequested)
        {
            m_defaultSoundInstance = playSound(getTemplate()->getDefaultSound());
        }
    }

    void SoundComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if (m_defaultSoundInstance.isValid())
        {
            stopSound(m_defaultSoundInstance);
        }

        // Release FeedbackFXManager sound descriptors
        FEEDBACKFX_MANAGER->releaseSoundDescriptors(m_actor);
        m_feedbackSounds.clear();
    }

    f32 SoundComponent::getDecibelVolume(const SoundPlayingID _handle) const
    {
        ITF_ASSERT(_handle.isValid());
        if (_handle.isValid())
            return SOUND_MANAGER->getCurrentDecibelVolume(_handle);
        return -100.0f;
    }

    u32 SoundComponent::getFileCount( const StringID& _soundDescName ) const
    {
        const SoundDescriptor_Template* soundDescriptor_template = getSoundDescriptor_Template(_soundDescName);
        return soundDescriptor_template ? soundDescriptor_template->getFileCount() : 0;
    }

    const SoundDescriptor_Template* SoundComponent::getSoundDescriptor_Template( const StringID& _soundDescriptorTemplateName ) const
    {
        if ( !_soundDescriptorTemplateName.isValid() )
            return NULL;

        const SoundList& soundList = getTemplate()->getSoundList();

        for ( u32 i = 0 ; i < soundList.size() ; i++ )
        {
            if ( soundList[i].getName() == _soundDescriptorTemplateName )
                return &soundList[i];
        }

        return NULL;
    }

    u32 SoundComponent::findInputIndex( StringID _id ) const
    {
        for ( u32 i = 0; i < m_inputList.size(); i++ )
        {
            const Input& input = m_inputList[i];

            if ( input.getId() == _id )
            {
                return i;
            }
        }

        return U32_INVALID;
    }

    Event * SoundComponent::getMusicEvent( const StringID& _name ) const
    {
        for ( u32 i = 0 ; i < m_musicNames.size() ; i++ )
        {
            if ( m_musicNames[i] == _name )
                return m_musicList[i];
        }

        return NULL;
    }

    Event * SoundComponent::getBusMixEvent( const StringID& _name ) const
    {
        for ( u32 i = 0 ; i < m_busMixNames.size() ; i++ )
        {
            if ( m_busMixNames[i] == _name )
                return m_busMixList[i];
        }

        return NULL;
    }

}

#endif
