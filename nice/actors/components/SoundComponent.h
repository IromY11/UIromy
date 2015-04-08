#ifndef _ITF_SOUNDCOMPONENT_H_
#define _ITF_SOUNDCOMPONENT_H_

#if defined(ITF_SUPPORT_RAKI)

#include "core/container/vector.h"

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_SEEDER_H_
#include "core/Seeder.h"
#endif //_ITF_SEEDER_H_

#include "core/container/KeyArray.h"

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"


namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
#define SoundComponent_CRC ITF_GET_STRINGID_CRC(SoundComponent,2111333436)
    typedef ITF_VECTOR<SoundDescriptor_Template> SoundList;
    typedef ITF_VECTOR<Event*> MusicList;
    typedef ITF_VECTOR<Event*> BusMixList;
    typedef KeyArray< ITF_VECTOR<ResourceID> > ResourceMap;

    class SoundComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SoundComponent, ActorComponent,2111333436);

    public:
        DECLARE_SERIALIZE()

        SoundComponent();
        ~SoundComponent();
        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return bfalse; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        virtual bbool   needsDraw2DNoScreenRatio() const { return bfalse; }
        virtual bbool   keepAlive();

        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void onStartDestroy(bbool _hotReload);
        virtual void    Update( f32 _deltaTime );
        virtual void    onEvent(Event * _event);
        virtual void    onResourceLoaded();
        virtual void    onUnloadResources();
        virtual void    onBecomeInactive();
        virtual void    onBecomeActive();

        SoundPlayingID     playSound( StringID _name, u32 _animBoneIndex = U32_INVALID, const SoundDescriptor_Template * _soundDesc = NULL, const StringID& _customCategory = StringID::Invalid, bbool _isOwner = btrue);
        void            stopSound(SoundPlayingID& _handle, bbool _onlyStopLooping = bfalse);
        void            stopAll( bbool _onlyStopLooping = bfalse );
        bbool           isPlaying(const SoundPlayingID _handle) const;
        f32             getDecibelVolume(const SoundPlayingID _handle) const;

        template <typename T> void  setInput(StringID _inputName, T _value );
        template <typename T> void  setInputFromIndex(u32 _index, T _value );
        const InputContainer &      getInputs() const { return m_inputList; }
        void                        setUpdateSoundInput( IUpdateSoundInputCriteria* _update ) { m_soundInputUpdate = _update; }
        u32                         getNumInputs() const { return m_inputList.size();}
        u32                         findInputIndex( StringID _id ) const;
#ifdef ITF_DEBUG_STRINGID
        const char*                 getInputName( u32 _index ) const { return m_inputList[_index].getName(); }
#endif
        InputType                   getInputType( u32 _index ) const { return m_inputList[_index].getType(); }

        template <typename T> void  getInputValue( u32 _index, T& _val ) const
        {
            const Input& input = m_inputList[_index];
            input.getValue(_val);
        }

        void setSoundList(const SafeArray<SoundDescriptor_Template *> & soundDescriptorList);
        void setSoundPos(const SoundPlayingID _handle, const Vec3d& _pos);
        void attachToBone(const SoundPlayingID _handle, u32 _boneIndex);
        ITF_VECTOR<SoundDescriptor> & getSoundList() { return m_soundList; }
        u32 getResourceCount() const;
        ResourceID getResourceIdAt(const u32 _index) const;

        // returns the number of sound files that the given sound descriptor template has
        u32                         getFileCount(const StringID& _soundDescriptorTemplateName) const;
        ResourceID                  getResourceId(const StringID& _descName, const u32 _index) const;
        const SoundDescriptor_Template* getSoundDescriptor_Template( const StringID& _soundDescName ) const;

        Event *                     getMusicEvent( const StringID& _name ) const;
        Event *                     getBusMixEvent( const StringID& _name ) const;

	
		bbool						getIsEnable() const { return m_isEnable; }
		void						setIsEnable(bbool _val) { m_isEnable = _val; }

    private:
        ITF_INLINE const class SoundComponent_Template* getTemplate() const;
        void                        releaseAllInstances();

        class AnimLightComponent*   m_animLight;
        ITF_VECTOR<SoundPlayingID>     m_instances;

        Seeder                      m_random;
        SoundPlayingID                 m_defaultSoundInstance;

        Vec3d                       m_cameraPrevPos;

        IUpdateSoundInputCriteria*  m_soundInputUpdate;

        bbool                       m_dieRequested;

        InputContainer              m_inputList;

        SoundList                   m_soundListTemplate;
        ITF_VECTOR<SoundDescriptor> m_soundList;
        ResourceMap                 m_resources;

        typedef ITF_MAP<StringID,u32> SoundMap;
        SoundMap m_sounds;

        // Used to lock slots on feedbackFXManager
        ITF_MAP<StringID,SoundDescriptor *> m_feedbackSounds;

        MusicList                   m_musicList;
        ITF_VECTOR<StringID>        m_musicNames;
        BusMixList                  m_busMixList;
        ITF_VECTOR<StringID>        m_busMixNames;

		bbool						m_isEnable;
	};


    template <typename T>
    ITF_INLINE void SoundComponent::setInput(StringID _inputName, T _value )
    {
        u32 numInputs = m_inputList.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            Input& input = m_inputList[i];

            if (input.getId() == _inputName)
            {
                input.setValue(_value);
                break;
            }
        }
    }

    template <typename T>
    ITF_INLINE void SoundComponent::setInputFromIndex( u32 _index, T _value )
    {
        m_inputList[_index].setValue(_value);
    }

#define SoundComponent_Template_CRC ITF_GET_STRINGID_CRC(SoundComponent_Template, 3645729875)
    class SoundComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(SoundComponent_Template, ActorComponent_Template,SoundComponent_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(SoundComponent);

    public:

        SoundComponent_Template();
        ~SoundComponent_Template();
        virtual bbool onTemplateLoaded( bbool _hotReload );
        virtual void  onTemplateDelete( bbool _hotReload );
        virtual void  setLoadedInPlace();

        ITF_INLINE const SoundList & getSoundList() const { return m_soundList; }
        ITF_INLINE StringID         getDefaultSound() const { return m_defaultSound; }
        ITF_INLINE const ITF_VECTOR<InputDesc>& getInputList() const { return m_inputList; }
        ResourceID      getResourceId(const StringID& _descName, const u32 _index) const ;
        u32             getResourceCount() const;
        ResourceID      getResourceIdAt(const u32 _index) const;

        const MusicList&    getMusicList()  const { return m_musicList; }
        const BusMixList&   getBusMixList() const { return m_busMixList; }

    private:
        SoundList                   m_soundList;
        StringID                    m_defaultSound;
        ITF_VECTOR <InputDesc>      m_inputList;
        // Used to match sound descriptors
        ResourceMap                 m_resources;

        MusicList                   m_musicList;
        BusMixList                  m_busMixList;
    };

    ITF_INLINE const class SoundComponent_Template* SoundComponent::getTemplate() const
    {
        return static_cast<const class SoundComponent_Template*>(m_template);
    }

    // INLINED
    ITF_INLINE u32 SoundComponent::getResourceCount() const { return getTemplate()->getResourceCount(); }
    ITF_INLINE ResourceID SoundComponent::getResourceIdAt(const u32 _index) const { return getTemplate()->getResourceIdAt(_index); }
}

#endif

#endif // _ITF_SOUNDCOMPONENT_H_

