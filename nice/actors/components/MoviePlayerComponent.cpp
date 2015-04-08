#include "precompiled_engine.h"

#ifndef _ITF_MOVIEPLAYERCOMPONENT_H_
#include "engine/actors/components/MoviePlayerComponent.h"
#endif //_ITF_MOVIEPLAYERCOMPONENT_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(MoviePlayerComponent_Template)

    BEGIN_SERIALIZATION_CHILD(MoviePlayerComponent_Template)
        SERIALIZE_MEMBER("video", m_video);
        SERIALIZE_MEMBER("audioTrack", m_audioTrack);
        SERIALIZE_MEMBER("videoTrack", m_videoTrack);
        SERIALIZE_MEMBER("autoPlay", m_autoPlay);
        SERIALIZE_MEMBER("fadeInTime", m_fadeInTime);
        SERIALIZE_MEMBER("fadeOutTime", m_fadeOutTime);
        SERIALIZE_MEMBER("playFromMemory", m_playFromMemory);
        SERIALIZE_MEMBER("playToTexture", m_playToTexture);
        SERIALIZE_MEMBER("loop", m_loop);
        SERIALIZE_MEMBER("sound", m_sound);
        SERIALIZE_MEMBER("mainthread", m_mainthread);
        //SERIALIZE_MEMBER("pauseBeforeStop", m_pauseBeforeStop);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(MoviePlayerComponent)

    BEGIN_SERIALIZATION_CHILD(MoviePlayerComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(MoviePlayerComponent) 
    VALIDATE_PARAM("", !getTemplate()->getPlayToTexture() || m_textureComponent, "Need a texture component with play to texture");
    END_VALIDATE_COMPONENT()

    MoviePlayerComponent::MoviePlayerComponent()
    : m_currentFrame(U32_INVALID)
    , m_textureComponent(NULL)
    , m_playRequested(bfalse)
    {
        m_moviePlayer.setListener(this);
    }

    MoviePlayerComponent::~MoviePlayerComponent()
    {

    }

    void MoviePlayerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_moviePlayer.setFadeInTime(getTemplate()->getFadeInTime());
        m_moviePlayer.setFadeOutTime(getTemplate()->getFadeOutTime());
        m_moviePlayer.setLoop(getTemplate()->getLoop());
        m_moviePlayer.setSound(getTemplate()->getSound());
        m_moviePlayer.setRunOnMainThread(getTemplate()->getRunOnMainThread());

        m_textureComponent = m_actor->GetComponent<TextureGraphicComponent>();
        m_material = getTemplate()->getMaterial();
        if (m_textureComponent && m_textureComponent->getMaterial())
            m_materialReplacement = *m_textureComponent->getMaterial();

        if (!getTemplate()->getVideoPath().isEmpty())
        {
            if(getTemplate()->getPlayFromMemory())
                m_videoID = m_actor->addResource(Resource::ResourceType_Video, getTemplate()->getVideoPath());
            else
                m_videoID = m_actor->addResource(Resource::ResourceType_VideoStreamed, getTemplate()->getVideoPath());
        }


    }

    void MoviePlayerComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        // Register events
        EVENTMANAGER_REGISTER_EVENT_LISTENER(EventPlayMovie_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventPlayMovie_CRC,this);
    }

    void MoviePlayerComponent::onStartDestroy( bbool _hotReload )
    {
        Super::onStartDestroy(_hotReload);

        EVENTMANAGER->unregisterEvent(EventPlayMovie_CRC, this);
    }

    void MoviePlayerComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        stopMovie();
    }

    void MoviePlayerComponent::Update( f32 _dt )
    {
        if ( m_currentFrame == CURRENTFRAME )
            return;
        
        m_currentFrame = CURRENTFRAME;

        Super::Update(_dt);
        
        if (m_moviePlayer.isPlayingMovie())
        {
            m_playRequested = bfalse;
            m_moviePlayer.update(_dt);
        }
        else
        {
            if (wantsToPlay() && canPlay())
                playMovie();
        }

        if(m_textureComponent && getTemplate()->getPlayToTexture())
        {
            //set texture
            if(getTexture())
            {
                if(m_material.getTexture() != getTexture())
                {
                    m_material.getTextureSet().setTextureResID(getTexture());
                    m_textureComponent->setMaterial(m_material);
                }
            }
            else
            {
                if(!m_moviePlayer.getLoop()) // if looping, keep last video frame, else replace to default material.
                {
                    if(m_material.getTexture() != m_materialReplacement.getTexture())
                    {
                        m_material.getTextureSet().setTextureResID(m_materialReplacement.getTexture());
                        m_textureComponent->setMaterial(m_material);
                    }
                }
            }
        }
    }

    void MoviePlayerComponent::playMovie()
    {
        if (m_playRequested)
            m_playRequested = bfalse;

        if (getTemplate()->getPlayToTexture() && m_textureComponent)
        {
            m_moviePlayer.playToTexture(getTemplate()->getVideoPath(), getTemplate()->getAudioTrack(), getTemplate()->getVideoTrack(), getTemplate()->getPlayFromMemory(), m_videoID);
        }
        else
        {
            m_moviePlayer.play(getTemplate()->getVideoPath(), getTemplate()->getAudioTrack(), getTemplate()->getVideoTrack(), getTemplate()->getPlayFromMemory(), m_videoID);
        }
    }

    void MoviePlayerComponent::stopMovie(bbool force)
    {
        if( getTemplate()->getPauseBeforeStop() && !force)
        {
            m_moviePlayer.pause();                
        }
        else
        {
            m_moviePlayer.stop();
        }
    }

    void MoviePlayerComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);


        if (EventPlayMovie* playMovieEvent = DYNAMIC_CAST(_event,EventPlayMovie))
        {
            if(playMovieEvent->getPlay())
                m_playRequested = btrue;
            else
                stopMovie(btrue);
        }
    }

    class TextureDyn * MoviePlayerComponent::getTexture()
    {
        return m_moviePlayer.getTexture();
    }

    void MoviePlayerComponent::checkPlayMovie()
    {
        Resource* pResource = (Resource*)m_videoID.getResource();
        if(pResource && !pResource->isPhysicallyLoaded())
            return;

        if ( !GAMEMANAGER->isLoadingVisible() && getTemplate()->getAutoPlay() && !m_moviePlayer.getMoviePlayed() )
            playMovie();
    }

    bbool MoviePlayerComponent::canPlay() const
    {
        Resource* pResource = (Resource*)m_videoID.getResource();
        if(pResource && !pResource->isPhysicallyLoaded())
            return bfalse;

        if ( GAMEMANAGER->isLoadingVisible())
            return bfalse;

        return btrue;

    }

    bbool MoviePlayerComponent::wantsToPlay() const
    {
        if (getTemplate()->getAutoPlay() && !m_moviePlayer.getMoviePlayed())
            return btrue;
        if (getTemplate()->getLoop() && !m_moviePlayer.isPlayingMovie())
            return btrue;
        if (m_playRequested)
            return btrue;

        return bfalse;
    }
};
