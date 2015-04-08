#ifndef ITF_CAMERANEARFARFADECOMPONENT_H_
#define ITF_CAMERANEARFARFADECOMPONENT_H_

namespace ITF
{
    class CameraNearFarFadeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CameraNearFarFadeComponent, ActorComponent, 1872005904)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        CameraNearFarFadeComponent();
        virtual ~CameraNearFarFadeComponent();

        void onActorLoaded(Pickable::HotReloadType _hotReload);

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onEvent(Event * _event);

        virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views );

    private:
        f32 m_near;
        f32 m_nearForced;
        f32 m_far;
        f32 m_farForced;
        f32 m_fadeRange;
        f32 m_fadeRangeForced;

        f32 m_interpValue;

        void setViewParams(class View & _view);

        ITF_INLINE const class CameraNearFarFadeComponent_Template* getTemplate() const;
    };


    //---------------------------------------------------------------------------------------------------

    class CameraNearFarFadeComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(CameraNearFarFadeComponent_Template, ActorComponent_Template, 1362628678)
        DECLARE_ACTORCOMPONENT_TEMPLATE(CameraNearFarFadeComponent)
        DECLARE_SERIALIZE()

    public:

        CameraNearFarFadeComponent_Template();
        virtual ~CameraNearFarFadeComponent_Template();

        inline StringID getInterpInput() const { return m_interpInput; }
        inline StringID getNearInput() const { return m_nearInput; }
        inline StringID getFarInput() const { return m_farInput; }
        inline StringID getFadeInput() const { return m_fadeInput; }

    private:
        StringID m_interpInput;
        StringID m_nearInput;
        StringID m_farInput;
        StringID m_fadeInput;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const CameraNearFarFadeComponent_Template* CameraNearFarFadeComponent::getTemplate() const
    {
        return static_cast<const CameraNearFarFadeComponent_Template*>(m_template);
    }
}

#endif // ITF_CAMERANEARFARFADECOMPONENT_H_
