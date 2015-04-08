#ifndef _ITF_KEEPALIVECOMPONENT_H_
#define _ITF_KEEPALIVECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "../actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

#define KeepAliveComponent_CRC ITF_GET_STRINGID_CRC(KeepAliveComponent,4149685283)
    class KeepAliveComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(KeepAliveComponent,ActorComponent,4149685283);

    public:
        DECLARE_SERIALIZE()

        KeepAliveComponent();
        ~KeepAliveComponent();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return bfalse; }
		virtual	bbool needsDraw2DNoScreenRatio() const { return bfalse; }

		virtual void onBecomeActive();
        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void onEvent(Event *_event);
        virtual void  Update(f32 _deltaTime);

    private:

        ITF_INLINE const class KeepAliveComponent_Template*  getTemplate() const;
        bbool updateFade(const f32 _dt);

    private:

        bbool   m_tryDying;
        bbool   m_updateDone;
        bbool   m_disableOnly;

        class GraphicComponent * m_graphicComponent;
        bbool           m_fadeStarted;
        f32             m_fadeTimer;
        f32             m_fadeStartAlpha;
    };

    //-------------------------------------------------------------------------------------
    class KeepAliveComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(KeepAliveComponent_Template,ActorComponent_Template,2568491216);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(KeepAliveComponent);

    public:

        KeepAliveComponent_Template();
        ~KeepAliveComponent_Template() {}

        ITF_INLINE bbool getStartDying() const {return m_startDying;}
        ITF_INLINE bbool getUseFade() const { return m_useFade; }
        ITF_INLINE f32 getFadeDuration() const { return m_fadeDuration; }

    private:

        bbool   m_startDying;
        bbool   m_useFade;
        f32     m_fadeDuration;
    };



    const KeepAliveComponent_Template*  KeepAliveComponent::getTemplate() const {return static_cast<const KeepAliveComponent_Template*>(m_template);}
}

#endif // _ITF_GRAPHICCOMPONENT_H_