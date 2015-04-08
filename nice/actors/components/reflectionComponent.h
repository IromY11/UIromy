#ifndef ITF_REFLECTIONCOMPONENT_H_
#define ITF_REFLECTIONCOMPONENT_H_

#define ReflectionComponent_CRC ITF_GET_STRINGID_CRC(ReflectionComponent,3596453687)
#define ReflectionComponent_Template_CRC ITF_GET_STRINGID_CRC(ReflectionComponent_Template,994189222)


namespace ITF
{
    class ReflectionComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ReflectionComponent, ActorComponent, ReflectionComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        ReflectionComponent();
        virtual ~ReflectionComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        Update(f32 _deltaTime);
        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views );

    private:
        ITF_INLINE const class ReflectionComponent_Template* getTemplate() const;
        void computeUnitScaledAABB();
    };


    //---------------------------------------------------------------------------------------------------

    class ReflectionComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(ReflectionComponent_Template, ActorComponent_Template, ReflectionComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(ReflectionComponent)
        DECLARE_SERIALIZE()

    public:

        ReflectionComponent_Template();
        virtual ~ReflectionComponent_Template();

    private:
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const ReflectionComponent_Template* ReflectionComponent::getTemplate() const
    {
        return static_cast<const ReflectionComponent_Template*>(m_template);
    }
}

#endif // ITF_REFLECTIONCOMPONENT_H_
