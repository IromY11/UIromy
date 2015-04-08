#include "precompiled_engine.h"

#ifndef _ITF_MUSICCOMPONENT_H_
#include "engine/actors/components/MusicComponent.h"
#endif // _ITF_MUSICCOMPONENT_H_

#ifdef ITF_SUPPORT_RAKI

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif // _ITF_AIUTILS_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif // _ITF_METRONOMEMANAGER_H_

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif // _ITF_SOUNDMANAGER_H_

#ifndef _ITF_MUSICINSTANCE_H_
#include "engine/sound/Music/MusicInstance.h"
#endif // _ITF_MUSICINSTANCE_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(MusicComponent_Template)
    BEGIN_SERIALIZATION_CHILD(MusicComponent_Template)
        SERIALIZE_OBJECT("musicPartSet",m_musicPartSet);
        SERIALIZE_OBJECT("musicTree",m_musicTree);
        SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
    END_SERIALIZATION()

    MusicComponent_Template::MusicComponent_Template()
    {
    }

    MusicComponent_Template::~MusicComponent_Template()
    {
    }

    bbool MusicComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);

        bOk &= m_musicTree.onTemplateLoaded(m_actorTemplate->getFile());

        MusicTree instanceTree;

        instanceTree.createFromTemplate(&m_musicTree);

        CSerializerLoadInPlace serializer;

        serializer.setFactory(GAMEINTERFACE->getMusicTreeNodeInstanceFactory());
        serializer.Init(&m_instanceData);

        instanceTree.Serialize(&serializer,ESerialize_Data_Save);
        serializer.close();

        return bOk;
    }

    void MusicComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        Super::onTemplateDelete(_hotReload);

        m_musicTree.onTemplateDelete();
        m_instanceData.clear();
    }


    MusicTree* MusicComponent_Template::createTreeInstance() const
    {
        ArchiveMemory archLoad(m_instanceData.getData(),m_instanceData.getSize());
        CSerializerLoadInPlace serializer;

        serializer.setFactory(GAMEINTERFACE->getMusicTreeNodeInstanceFactory());
        serializer.Init(&archLoad);

        MusicTree* ret = serializer.loadInPlace<MusicTree>();

        ret->Serialize(&serializer,ESerialize_Data_Load);

        return ret;
    }

    IMPLEMENT_OBJECT_RTTI(MusicComponent)
    BEGIN_SERIALIZATION_CHILD(MusicComponent)
    END_SERIALIZATION()


    const MusicComponent* MusicComponent::ms_registeredComponent = NULL;

    MusicComponent::MusicComponent()
        :Super()
        , m_initializeMusicDescriptor(bfalse)
        , m_loadedMusicDescriptor(NULL)
    {
    }

    MusicComponent::~MusicComponent()
    {
    }

    void MusicComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        if (SOUND_MANAGER->getInitState())
        {

            if( m_loadedMusicDescriptor )
            {
                m_initializeMusicDescriptor = btrue;
                m_loadedMusicDescriptor->init( m_actor, getTemplate() );
                SOUND_MANAGER->registerMusicDescriptor( m_loadedMusicDescriptor );
            }
        }

    }

    void MusicComponent::onStartDestroy( bbool _hotReload )
{
        Super::onStartDestroy();

        if (SOUND_MANAGER->getInitState())
        {
            if ( m_loadedMusicDescriptor )
            {
                // descriptor has changed, remove-it from descriptor list
                SOUND_MANAGER->unregisterMusicDescriptor(m_loadedMusicDescriptor);

                // destroy the descriptor
                m_loadedMusicDescriptor->destroy();
            }
        }
    }

    void MusicComponent::onResourceLoaded()
    {
        // done in main thread
        Super::onResourceLoaded();

        if ( SOUND_MANAGER->getInitState() && m_initializeMusicDescriptor )
        {            
            if (m_loadedMusicDescriptor)
            {
                m_loadedMusicDescriptor->createMarkerMap();
                m_initializeMusicDescriptor = bfalse;
            }
        }
    }

    
    void MusicComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);
    }

    void MusicComponent::onSceneActive()
    {
        Super::onSceneActive();
        // set the actor has always active and prepare the music descriptor
        AIUtils::setAlwaysActive(m_actor, btrue);
    }

    void MusicComponent::onSceneInactive()
    {
        Super::onSceneInactive();
        AIUtils::setAlwaysActive(m_actor, bfalse);
    }

    void MusicComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        // this should only be a component to load data, we have to do the less possible actions in here relative to sound (no play, stop...)
        
        // to avoid problem, allow only one MusicComponent to register. This is only used to detect that more than one music component are present on the scene
        if (MusicComponent::ms_registeredComponent == NULL)
        {
            ms_registeredComponent = this;
        }
#ifndef ITF_FINAL
        else
        {
            ITF_WARNING_CATEGORY(Sound, NULL, 0, "A MusicComponent is present on actor %s, the map already has a MusicComponent on actor %s", 
                m_actor->getUserFriendly().cStr(), MusicComponent::ms_registeredComponent->GetActor()->getUserFriendly().cStr());
        }
#endif
    }

    void MusicComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if (MusicComponent::ms_registeredComponent == this)
        {
            MusicComponent::ms_registeredComponent = NULL;
        }
    }

    void MusicComponent::validate(bbool &_isComponentValidated) 
    {
        Super::validate(_isComponentValidated);

        MusicDescriptor* musicDesc = SOUND_MANAGER->getMusicDescriptor(getTemplate()->getMusicTreeTemplate().getMetronomeType());

        VALIDATE_COMPONENT_PARAM("Music tree", getTemplate()->getMusicPartSetTemplate().validate(), "is incorrect on MusicComponent of actor %s : error in the part list.", m_actor->getUserFriendly().cStr());

        if (musicDesc)
        {
            VALIDATE_COMPONENT_PARAM("Music tree", musicDesc->validate(&getTemplate()->getMusicPartSetTemplate()), "is incorrect on MusicComponent of actor %s", m_actor->getUserFriendly().cStr());
        }

        VALIDATE_COMPONENT_PARAM("Music tree", getTemplate()->getMusicPartSetTemplate().validate(), "is incorrect on MusicComponent of actor %s : error in the part list.", m_actor->getUserFriendly().cStr());
    }

    void MusicComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        ITF_ASSERT_CRASH( m_loadedMusicDescriptor == NULL, "This should not be possible (component reused without passing by onActorClearComponents ?).");

        m_loadedMusicDescriptor = newAlloc(mId_AudioEngine, MusicDescriptor);
        m_loadedMusicDescriptor->requestTemplateAndResources( m_actor, getTemplate() );
    }

    void MusicComponent::onActorClearComponents()
    {
        if (m_loadedMusicDescriptor)
        {
            m_loadedMusicDescriptor->releaseTemplateAndResources();
            SF_DEL(m_loadedMusicDescriptor);
        }
    }
}
#else
// Dummies !
namespace ITF
{
	///////////////////////////////////////////////////////////////////////////////////////////
	IMPLEMENT_OBJECT_RTTI(MusicComponent_Template)
	BEGIN_SERIALIZATION_CHILD(MusicComponent_Template)
	END_SERIALIZATION()

	MusicComponent_Template::MusicComponent_Template()
	{
	}

	MusicComponent_Template::~MusicComponent_Template()
	{
	}

    IMPLEMENT_OBJECT_RTTI(MusicComponent)
    BEGIN_SERIALIZATION_CHILD(MusicComponent)
    END_SERIALIZATION()


    const MusicComponent* MusicComponent::ms_registeredComponent = NULL;
}

#endif // #ifdef ITF_SUPPORT_RAKI