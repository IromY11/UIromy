#ifndef _ITF_MOVIEPLAYERCOMPONENT_H_
#define _ITF_MOVIEPLAYERCOMPONENT_H_

#ifndef ITF_MOVIEPLAYER_H_
#include "engine/video/MoviePlayer.h"
#endif //ITF_MOVIEPLAYER_H_

namespace ITF
{

    class MoviePlayerComponent : public ActorComponent, public IMoviePlayerListener
    {
        DECLARE_OBJECT_CHILD_RTTI(MoviePlayerComponent, ActorComponent,57059588);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        MoviePlayerComponent();
        virtual ~MoviePlayerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onFinalizeLoad();
        virtual void onStartDestroy(bbool _hotReload);
        virtual void        onBecomeInactive();
        virtual void        Update( f32 _dt );

        void checkPlayMovie();

        void                playMovie();
        void                playMovieToTexture();
        class TextureDyn *  getTexture(); 
        void                stopMovie(bbool force = bfalse);
        void                pauseMovie();
        void                onEvent(Event * _event);
        ITF_INLINE bbool    isPlayingMovie() const { return m_moviePlayer.isPlayingMovie(); }
        ITF_INLINE f32      getCurrentTime() { return m_moviePlayer.getCurrentTime(); }
        bbool               isPaused() const { return m_moviePlayer.isPaused(); }


    protected:
        ITF_INLINE const class MoviePlayerComponent_Template* getTemplate() const;


        virtual void onMovieStarted() {}
        virtual void onMovieStopped() {}
        virtual void onMoviePaused() {}
        virtual void onFadeInEnded() {}

        MoviePlayer m_moviePlayer;
        u32                         m_currentFrame;
        class TextureGraphicComponent * m_textureComponent;
        GFX_MATERIAL m_material;
        GFX_MATERIAL m_materialReplacement;
        ResourceID m_videoID;
        bbool   m_playRequested;
        
        bbool canPlay() const;
        bbool wantsToPlay() const;
    };


    //---------------------------------------------------------------------------------------------------

    class MoviePlayerComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(MoviePlayerComponent_Template, ActorComponent_Template,3807008301);
        DECLARE_SERIALIZE_VIRTUAL()
        DECLARE_ACTORCOMPONENT_TEMPLATE(MoviePlayerComponent);

    public:

        MoviePlayerComponent_Template() 
            : m_audioTrack(0)
            , m_videoTrack(0)
            , m_autoPlay(btrue)
            , m_fadeInTime(0.0f)
            , m_fadeOutTime(0.0f)
            , m_playFromMemory(bfalse)
            , m_pauseBeforeStop(bfalse)
            , m_playToTexture(bfalse)
            , m_loop(bfalse)
            , m_sound(btrue)
            , m_mainthread(bfalse)
        {
        }
        virtual ~MoviePlayerComponent_Template() {}

        ITF_INLINE const Path&  getVideoPath()  const { return m_video; }
        ITF_INLINE i32          getAudioTrack()  const { return m_audioTrack; }
        ITF_INLINE i32          getVideoTrack()  const { return m_videoTrack; }
        ITF_INLINE const bbool& getAutoPlay()   const { return m_autoPlay; }
        ITF_INLINE const f32&   getFadeInTime() const { return m_fadeInTime; }
        ITF_INLINE const f32&   getFadeOutTime() const { return m_fadeOutTime; }
        ITF_INLINE const bbool&  getPlayFromMemory() const { return m_playFromMemory; }
        ITF_INLINE const bbool&  getPauseBeforeStop() const { return m_pauseBeforeStop; }
        ITF_INLINE bbool        getPlayToTexture() const { return m_playToTexture; }
        ITF_INLINE const GFXMaterialSerializable & getMaterial() const { return m_material; }
        ITF_INLINE bbool        getLoop() const { return m_loop; }
        ITF_INLINE bbool        getSound() const { return m_sound; }
        ITF_INLINE bbool        getRunOnMainThread() const { return m_mainthread; }

    private:

        Path            m_video;
        i32             m_audioTrack;
        i32             m_videoTrack;
        bbool           m_autoPlay;
        f32             m_fadeInTime;
        f32             m_fadeOutTime;
        bbool           m_playFromMemory;
        bbool           m_pauseBeforeStop;
        bbool           m_playToTexture;
        bbool           m_loop;
        bbool           m_sound;
        bbool           m_mainthread;
        GFXMaterialSerializable m_material;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const MoviePlayerComponent_Template* MoviePlayerComponent::getTemplate() const
    {
        return static_cast<const MoviePlayerComponent_Template*>(m_template);
    }
};

#endif //_ITF_MOVIEPLAYERCOMPONENT_H_
